#include <Kernel.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <new>
#include <mem/MMU.hpp>
#include <mem/PageFrameAllocator.hpp>
#include <mem/PageTable.hpp>
#include <sys/asm.h>

#define kPDESelfMapIndex 1023

//==========================================================
// Externs
//==========================================================
extern uint32_t kernel_end;
extern uint32_t readonly_end;
extern "C" int log_task(const char *printstr, int success);
extern "C" int log_test(const char *printstr, int success);

//==========================================================
// Prototypes
//==========================================================
PageTable PageTableForDirectoryIndex(int index);

//==========================================================
// Page Frame Allocator init hook
//==========================================================
struct MMUPFAHook : PageFrameInitializationHook
{
    // lock frames in low memory, mark kernel frames as used
    virtual void operator()(PageFrameAllocator *allocator)
    {
        uint32_t i = 0;
        for(; i < 0x100000; i += 0x1000) {
            allocator->markFrameUsable(i, false);
        }

        uint32_t kernel_end_addr = (uint32_t) &kernel_end;

        for(; i <= kernel_end_addr; i += 0x1000) {
            if(!allocator->requestFrame(i)) {
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
: _pageFrameAllocator(mmap_addr, mmap_length, new (hook1) MMUPFAHook),
    _pageDirectory((uint32_t*)_pageFrameAllocator.alloc())
{
    _pageDirectory.clear();
    PageTable firstTable((uint32_t*)(_pageFrameAllocator.alloc()));

    // identity map current used address space
    uint32_t highMemoryBegin = 0x100000/0x1000;
    uint32_t readOnlyEnd = (uint32_t)&readonly_end / 0x1000;
    uint32_t lastUsedPage = (uint32_t)firstTable.address() / 0x1000;

    if(lastUsedPage >= 0x400) {
        kernel->panic("Kernel spans past first page table.");
    }

    uint32_t address = 0;
    for(uint32_t i = 0; i <= lastUsedPage; ++i) {
        PageEntry entry(address);
        entry.setFlag(kPresentBit);
        // make sure pages for read only data are marked read only
        if(i < highMemoryBegin || i > readOnlyEnd) {
            entry.setFlag(kReadWriteBit);
        }
        firstTable.setEntry(i, entry);
        address += 0x1000;
    }

    // Install table for first 4MB
    PageEntry entry((uint32_t)firstTable.address());
    entry.setFlag(kPresentBit);
    entry.setFlag(kReadWriteBit);
    _pageDirectory.setEntry(0, entry);

    // Set last PDE to PD itself
    PageEntry directory((uint32_t)_pageDirectory.address());
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
    uint32_t pde;
    uint32_t pte;
    void *retval = NULL;
    size_t contiguousFoundPages = 0;
    for(pde = 0; pde < 1024 && contiguousFoundPages != numberOfPages; ++pde) {
        PageEntry directoryEntry = _pageDirectory.entryAtIndex(pde);
        PageTable table = PageTableForDirectoryIndex(pde);
        
        if(!directoryEntry.getFlag(kPresentBit)) { // no page table here, create one
            directoryEntry = PageEntry(_pageFrameAllocator.alloc());
            directoryEntry.setFlags(kPresentBit | kReadWriteBit);
            _pageDirectory.setEntry(pde, directoryEntry);
            _flush();
            table.clear();
            _flush();
        }

        // look through page table
        for (pte = 0; pte < 1024; ++pte) {
            PageEntry entry = table.entryAtIndex(pte);
            if (!entry.getFlag(kPresentBit)) { // found a page that isn't present (is available)
                if(contiguousFoundPages == 0) { // this is the first available page, set our return pointer to it
                    retval = (void*) ((pde << 22) | (pte << 12));
                }

                ++contiguousFoundPages;
                // we got our pages, let's mark them present
                if(contiguousFoundPages == numberOfPages) {
                    for(size_t k = pte; k > pte - numberOfPages; --k) {
                        entry = PageEntry(_pageFrameAllocator.alloc());
                        entry.setFlags(kPresentBit | kReadWriteBit);
                        table.setEntry(k, entry);
                    }
                    break;
                }
            } else if (retval && contiguousFoundPages) { // current page is used, reset our contiguous page count
                contiguousFoundPages = 0;
                retval = NULL;
            }
        }
    }

    _flush();
    
    return retval;
}

int MMU::pfree(void *startOfMemoryRange, size_t numberOfPages)
{
    uint32_t virtualAddress = (uint32_t)startOfMemoryRange;

    if(!numberOfPages) return -1;

    while(numberOfPages--) {
        uint32_t pdeIndex = virtualAddress >> 22;
        uint32_t pteIndex = virtualAddress >> 12 & 0x03FF;
        PageEntry pde = _pageDirectory.entryAtIndex(pdeIndex);
        PageTable table((uint32_t*)pde.address());
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

PageTable PageTableForDirectoryIndex(int index)
{
    uint32_t *addr = (uint32_t *)0xFFC00000 + (0x400 * index);
    return PageTable(addr);
}

