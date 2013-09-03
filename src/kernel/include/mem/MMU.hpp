#pragma once
#include "PageFrameAllocator.hpp"
#include "PageTable.hpp"

class MMU
{
public:
    MMU(uint32_t mmap_addr, uint32_t mmap_length);
    void *palloc(size_t numberOfPages);
    int pfree(void *startOfMemoryRange, size_t numberOfPages);
    void install();
private:
    PageFrameAllocator _pageFrameAllocator;
    PageTable _pageDirectory;
};
