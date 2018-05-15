#include <Kernel.hpp>
#include <stdlib.h>
#include <cstdio>
#include <new>
#include <mem/MMU.hpp>
#include <mem/PageFrameAllocator.hpp>
#include <mem/PageTable.hpp>
#include <sys/asm.h>

#define kPDESelfMapIndex 1023
#define kVGAPage (0xB8000/0x1000)

//==========================================================
// Externs
//==========================================================
extern uint32_t kernel_end;
extern uint32_t readonly_end;
extern "C" int log_task(char const *printstr, int success);
extern "C" int log_test(char const *printstr, int success);

namespace {

//==========================================================
// Prototypes
//==========================================================
PageTable PageTableForDirectoryIndex(int index);

}

//==========================================================
// Page Frame Allocator init hook
//==========================================================
struct MMUPFAHook : PageFrameInitializationHook
{
    // lock frames in low memory, mark kernel frames as used
    virtual void operator()(PageFrameAllocator *allocator)
    {
        uint32_t i = 0;
        for (; i < 0x100000; i += 0x1000) {
            allocator->markFrameUsable(i, false);
        }

        uint32_t kernel_end_addr = (uint32_t) & kernel_end;

        for (; i <= kernel_end_addr; i += 0x1000) {
            if (!allocator->requestFrame(i)) {
                kernel->panic("Page allocation error: unable to reserve kernel memory frames.");
            }
        }
    }
};

uint8_t hook1[sizeof(MMUPFAHook)];

//===========================================================
// MMU Public Methods
//===========================================================

MMU::MMU(uint32_t mmap_addr, uint32_t mmap_length)
        : _pageFrameAllocator(mmap_addr, mmap_length, new(hook1) MMUPFAHook),
          _pageDirectory((uint32_t *) _pageFrameAllocator.alloc())
{
    _pageDirectory.clear();
    PageTable firstTable((uint32_t * )(_pageFrameAllocator.alloc()));
    firstTable.clear();

    // identity map current used address space
    uint32_t readOnlyEnd = (uint32_t) & readonly_end / 0x1000;
    uint32_t lastUsedPage = (uint32_t) firstTable.address() / 0x1000;

    if (lastUsedPage >= 0x400) {
        kernel->panic("Kernel spans past first page table.");
    }

    uint32_t address = 0;
    for (uint32_t i = 0; i <= lastUsedPage; ++i) {
        PageEntry entry(address);
        entry.setFlag(kPresentBit);
        // make sure pages for read only data are marked read only
        if (i > readOnlyEnd || i == kVGAPage) {
            entry.setFlag(kReadWriteBit);
        }
        firstTable.setEntry(i, entry);
        address += 0x1000;
    }

    // Install table for first 4MB
    PageEntry entry((uint32_t) firstTable.address());
    entry.setFlag(kPresentBit);
    entry.setFlag(kReadWriteBit);
    _pageDirectory.setEntry(0, entry);

    // Set last PDE to PD itself
    PageEntry directory((uint32_t) _pageDirectory.address());
    directory.setFlag(kPresentBit);
    directory.setFlag(kReadWriteBit);
    _pageDirectory.setEntry(kPDESelfMapIndex, directory);
}

void MMU::install()
{
    _pageDirectory.install();
}

void *MMU::palloc(size_t numberOfPages)
{
    size_t contiguousFoundPages = 0;
    size_t retpde = 0, retpte = 0;
    void *retval = nullptr;
    for (size_t pde = 0; pde < 1024 && contiguousFoundPages != numberOfPages; ++pde) {
        PageTable table = getOrCreateTable(pde);

        // look through page table
        for (size_t pte = 0; pte < 1024; ++pte) {
            PageEntry entry = table.entryAtIndex(pte);
            if (!entry.getFlag(kPresentBit)) { // found a page that isn't present (is available)
                if (contiguousFoundPages == 0) { // this is the first available page, set our return pointer to it
                    retpde = pde;
                    retpte = pte;
                }

                ++contiguousFoundPages;
                if (contiguousFoundPages == numberOfPages) { // done
                    retval = (void *) ((retpde << 22) | (retpte << 12));
                    break;
                }
            } else if (contiguousFoundPages) { // current page is used, reset our contiguous page count
                retval = nullptr;
                contiguousFoundPages = 0;
            }
        }
    }

    if (retval) { // if we succeeded in finding space
        allocatePages(retval, numberOfPages);
    }

    return retval;
}

void *MMU::palloc(void *virtualAddress, size_t numberOfPages)
{
    uintptr_t address = reinterpret_cast<uintptr_t>(virtualAddress);
    if (address & 0xFFF) {
        return nullptr; // address is not page aligned
    }

    for (size_t i = 0; i < numberOfPages; ++i) {
        auto page = pageForAddress(reinterpret_cast<void*>(address));
        if (page.getFlag(kPresentBit)) {
            virtualAddress = nullptr; // can't allocate, not enough space
            break;
        }

        address += 0x1000; // increment to next page
    }

    if (virtualAddress) {
        allocatePages(virtualAddress, numberOfPages);
    }

    return virtualAddress;
}

int MMU::pfree(void *startOfMemoryRange, size_t numberOfPages)
{
    uint32_t virtualAddress = (uint32_t) startOfMemoryRange;

    if (!numberOfPages) return -1;

    while (numberOfPages--) {
        uint32_t pdeIndex = virtualAddress >> 22;
        uint32_t pteIndex = virtualAddress >> 12 & 0x03FF;
        PageTable table = PageTableForDirectoryIndex(pdeIndex);
        PageEntry pte = table.entryAtIndex(pteIndex);
        _pageFrameAllocator.free(pte.address());
        table.setEntry(pteIndex, PageEntry(0));

        virtualAddress += 0x1000;
    }

    _flush();

    return 0;
}

//===========================================================
// MMU Private methods
//===========================================================

PageTable MMU::getOrCreateTable(uint16_t directoryIndex)
{
    if (directoryIndex > 1023) {
        kernel->panic("MMU::getOrCreateTable: invalid directoryIndex");
    }

    PageEntry pde = _pageDirectory.entryAtIndex(directoryIndex);
    PageTable table{PageTableForDirectoryIndex(directoryIndex)};
    if (!pde.getFlag(kPresentBit)) { // no page table here, create one
        pde = PageEntry(_pageFrameAllocator.alloc());
        pde.setFlags(kPresentBit | kReadWriteBit);
        _pageDirectory.setEntry(directoryIndex, pde);
        table.clear();
        _flush();
    }

    return table;
}

PageTable MMU::tableForAddress(void *virtualAddress)
{
    uintptr_t address = reinterpret_cast<uintptr_t>(virtualAddress);
    uint32_t pdeIndex = address >> 22;
    return getOrCreateTable(pdeIndex);
}

PageEntry MMU::pageForAddress(void *virtualAddress)
{
    auto table = tableForAddress(virtualAddress);
    uintptr_t address = reinterpret_cast<uintptr_t>(virtualAddress);
    uint32_t pteIndex = address >> 12 & 0x03FF;
    return table.entryAtIndex(pteIndex);
}


void MMU::allocatePages(void *address, size_t numberOfPages)
{
    uintptr_t virtualAddress = reinterpret_cast<uintptr_t>(address);
    uint32_t currpde = virtualAddress >> 22;
    uint32_t currpte = virtualAddress >> 12 & 0x03FF;
    size_t pagesLeft = numberOfPages;
    while (pagesLeft > 0) { // map the pages
        PageTable table = PageTableForDirectoryIndex(currpde);
        PageEntry entry = PageEntry(_pageFrameAllocator.alloc());
        entry.setFlags(kPresentBit | kReadWriteBit);
        table.setEntry(currpte, entry);

        ++currpte;
        --pagesLeft;

        // move to next PDE if needed
        if (currpte > 1023) {
            ++currpde;
            currpte = 0;
        }
    }

    _flush();
}

void MMU::_flush()
{
    asm volatile(
            "movl %cr3, %eax;"
            "movl %eax, %cr3"
            );
}

//===========================================================
// Utility methods
//===========================================================
namespace {

// Pulls the page table from the self-mapped page directory.
// Trust me, you need to use this. Don't use the address from the page directory
// entry. That's the physical page frame!
PageTable PageTableForDirectoryIndex(int index)
{
    uint32_t *addr = (uint32_t *) 0xFFC00000 + (0x400 * index);
    return PageTable(addr);
}

}


