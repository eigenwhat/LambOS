#pragma once
#include "PageFrameAllocator.hpp"
#include "PageTable.hpp"

/** Abstraction for the X86 memory management unit. Implements paging. */
class MMU
{
public:
    MMU(uint32_t mmap_addr, uint32_t mmap_length);

    /**
     * Allocates contiguous pages of memory.
     * @param numberOfPages The number of pages to allocate.
     * @return the start of the contiguous allocated memory.
     */
    void *palloc(size_t numberOfPages);

    /**
     * Attempts to allocate a number of pages at the given address.
     * @param virtualAddress
     * @param numberOfPages
     * @return
     */
    void *palloc(void *virtualAddress, size_t numberOfPages);

    /**
     * Frees a page-aligned block of memory.
     * @param startOfMemoryRange The start of the memory block to free.
     * @param numberOfPages The length of the memory block in pages. Defaults to 1.
     * @return -1 if an error occurred, 0 otherwise.
     */
    int pfree(void *startOfMemoryRange, size_t numberOfPages = 1);

    /** Installs the managed page table. */
    void install();

private:
    PageTable getOrCreateTable(uint16_t directoryIndex);
    PageTable tableForAddress(void *virtualAddress);
    PageEntry pageForAddress(void *virtualAddress);

    void allocatePages(void *address, size_t numberOfPages);
    void _flush();

    PageFrameAllocator _pageFrameAllocator;
    PageTable _pageDirectory;
};
