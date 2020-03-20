//
// Created by Martin Miralles-Cordal on 8/28/2013.
//

#include <arch/i386/mem/MMU.hpp>

#include <arch/i386/mem/Paging.hpp>
#include <system/asm.h>
#include <mem/PageFrameAllocator.hpp>
#include <mem/PageTable.hpp>
#include <Kernel.hpp>

#include <system/Debug.hpp>

#include <cstdio>
#include <cstring>
#include <new>

namespace {

constexpr std::uint32_t const kVGAPage{0xB8000 / 0x1000};
constexpr std::uint32_t const kPDESelfMapIndex{1023};

using X86PageTable = std::uint32_t[0x400];
X86PageTable * const kPageDirectoryAddress = (X86PageTable *)(0xFFC00000);

}

//==========================================================
// Externs
//==========================================================
extern uint32_t kernel_end;
extern uint32_t readonly_end;

//==========================================================
// Utility methods
//==========================================================
namespace {

// Pulls the page table from the self-mapped page directory.
// Trust me, you need to use this. Don't use the address from the page directory
// entry. That's the physical page frame!
PageTable PageTableForDirectoryIndex(uint32_t index) { return PageTable(kPageDirectoryAddress + index); }

} // anonymous namespace

//===========================================================
// MMU Public Methods
//===========================================================

PageTable MMU::cloneDirectory(AddressSpace src)
{
    void *page = palloc(src, 1);
    std::memset(page, 0, X86::kPageTableSize);
    return {page};
}

MMU::MMU(uint32_t mmap_addr, uint32_t mmap_length) : _pageFrameAllocator{}
{
    _pageFrameAllocator.loadMemoryMap(mmap_addr, mmap_length);

    // mark kernel frames as unusable (i.e. taken)
    uint32_t i = 0;
    for (; i < 0x100000; i += 0x1000) {
        _pageFrameAllocator.markFrameUsable(i, false);
    }

    auto kernel_end_addr = (uint32_t)&kernel_end;

    for (; i <= kernel_end_addr; i += 0x1000) {
        if (!_pageFrameAllocator.requestFrame(i)) {
            kernel->panic("Page allocation error: unable to reserve kernel memory frames.");
        }
    }
}

void MMU::install(AddressSpace addressSpace)
{
    std::uint32_t readOnlyEnd = std::uint32_t(&readonly_end) / 0x1000;
    addressSpace.clear();

    // identity map current used address space
    std::uint32_t lastUsedFrame = readOnlyEnd;
    uint32_t address = 0;
    uint16_t tableDirectoryIndex = 0;

    auto const addToDirectory = [&](uint16_t i, PageTable t) {
        // Install finished table
        PageEntry entry((uint32_t)t.address());
        entry.setFlag(kPresentBit);
        entry.setFlag(kReadWriteBit);
        addressSpace.setEntry(i, entry);
    };

    auto const createNextTable = [&] {
        PageTable newTable{_pageFrameAllocator.alloc()};
        newTable.clear();
        const auto tableFrameIndex = std::uint32_t(newTable.address()) / 0x1000;
        lastUsedFrame = std::max(lastUsedFrame, tableFrameIndex);
        return newTable;
    };

    PageTable table;
    for (std::size_t frame = 0; frame <= lastUsedFrame; ++frame, address += 0x1000)
    {
        if (frame % 0x400 == 0) {
            if (table) { addToDirectory(tableDirectoryIndex++, table); }
            table = createNextTable();
        }

        PageEntry entry(address);
        entry.setFlag(kPresentBit);
        // make sure pages for read only data are marked read only
        if (frame > readOnlyEnd || frame == kVGAPage) {
            entry.setFlag(kReadWriteBit);
        }

        auto const index = static_cast<uint16_t>(frame % 0x400);
        table.setEntry(index, entry);
    }

    // install final one.
    if (table) { addToDirectory(tableDirectoryIndex, table); }

    // Set last PDE to PD itself
    addToDirectory(kPDESelfMapIndex, addressSpace);
    addressSpace.install();
}

void *MMU::palloc(AddressSpace addressSpace, size_t numberOfPages)
{
    size_t contiguousFoundPages = 0;
    uintptr_t retpde = 0, retpte = 0;
    void *retval = nullptr;
    for (uint16_t pde = 0; pde < 1024 && contiguousFoundPages != numberOfPages; ++pde) {
        PageTable table = getOrCreateTable(addressSpace, pde);

        // look through page table
        for (uint16_t pte = 0; pte < 1024; ++pte) {
            PageEntry entry = table.entryAtIndex(pte);
            if (!entry.getFlag(kPresentBit)) { // found a page that isn't present (is available)
                if (contiguousFoundPages == 0) { // this is the first available page, set our return pointer to it
                    retpde = pde;
                    retpte = pte;
                }

                ++contiguousFoundPages;
                if (contiguousFoundPages == numberOfPages) { // done
                    retval = (void *) ((retpde << 22u) | (retpte << 12u));
                    break;
                }
            } else if (contiguousFoundPages) { // current page is used, reset our contiguous page count
                retval = nullptr;
                contiguousFoundPages = 0;
            }
        }
    }

    if (retval) { // if we succeeded in finding space
        allocatePages(addressSpace, retval, numberOfPages);
    }

    return retval;
}

void *MMU::palloc(AddressSpace addressSpace, void *virtualAddress, size_t numberOfPages)
{
    auto address = reinterpret_cast<uintptr_t>(virtualAddress);
    if (address & 0xFFF) {
        return nullptr; // address is not page aligned
    }

    for (size_t i = 0; i < numberOfPages; ++i) {
        auto page = pageForAddress(addressSpace, reinterpret_cast<void*>(address));
        if (page.getFlag(kPresentBit)) {
            virtualAddress = nullptr; // can't allocate, not enough space
            break;
        }

        address += 0x1000; // increment to next page
    }

    if (virtualAddress) {
        allocatePages(addressSpace, virtualAddress, numberOfPages);
    }

    return virtualAddress;
}

int MMU::pfree([[maybe_unused]] AddressSpace addressSpace, void *startOfMemoryRange, size_t numberOfPages)
{
    uint32_t virtualAddress = (uint32_t) startOfMemoryRange;

    if (!numberOfPages) return -1;

    while (numberOfPages--) {
        uint32_t pdeIndex = virtualAddress >> 22u;
        uint16_t pteIndex = virtualAddress >> 12u & 0x03FF;
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

PageTable MMU::getOrCreateTable(AddressSpace addressSpace, uint16_t directoryIndex)
{
    if (directoryIndex > 1023) {
        kernel->panic("MMU::getOrCreateTable: invalid directoryIndex");
    }

    PageEntry pde = addressSpace.entryAtIndex(directoryIndex);
    PageTable table{PageTableForDirectoryIndex(directoryIndex)};
    if (!pde.getFlag(kPresentBit)) { // no page table here, create one
        pde = PageEntry(_pageFrameAllocator.alloc());
        pde.setFlags(kPresentBit | kReadWriteBit);
        addressSpace.setEntry(directoryIndex, pde);
        table.clear();
        _flush();
    }

    return table;
}

PageTable MMU::tableForAddress(AddressSpace addressSpace, void *virtualAddress)
{
    auto address = reinterpret_cast<uintptr_t>(virtualAddress);
    auto pdeIndex = uint16_t(address >> 22u);
    return getOrCreateTable(addressSpace, pdeIndex);
}

PageEntry MMU::pageForAddress(AddressSpace addressSpace, void *virtualAddress)
{
    auto table = tableForAddress(addressSpace, virtualAddress);
    uintptr_t address = reinterpret_cast<uintptr_t>(virtualAddress);
    uint16_t pteIndex = address >> 12u & 0x03FFu;
    return table.entryAtIndex(pteIndex);
}


void MMU::allocatePages([[maybe_unused]] AddressSpace addressSpace, void *address, size_t numberOfPages)
{
    auto virtualAddress = reinterpret_cast<uintptr_t>(address);
    uint32_t currpde = virtualAddress >> 22u;
    uint16_t currpte = virtualAddress >> 12u & 0x03FFu;
    size_t pagesLeft = numberOfPages;
    while (pagesLeft > 0) { // map the pages
        PageTable table = PageTableForDirectoryIndex(currpde);
        PageEntry entry{_pageFrameAllocator.alloc()};
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

