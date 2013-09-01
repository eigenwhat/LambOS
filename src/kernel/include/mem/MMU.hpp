#pragma once
#include "PageFrameAllocator.hpp"
#include "PageTable.hpp"

class MMU
{
public:
    MMU(uint32_t mmap_addr, uint32_t mmap_length);
private:
    PageFrameAllocator _pageFrameAllocator;
    PageTable _pageDirectory;
};
