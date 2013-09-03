#pragma once
#include "PageFrameAllocator.hpp"
#include "PageTable.hpp"

class MMU
{
public:
    MMU(uint32_t mmap_addr, uint32_t mmap_length);
    void *palloc(size_t numberOfPages = 1);
    int pfree(void *startOfMemoryRange, size_t numberOfPages = 1);
    void install();
private:
	void _flush() { asm volatile("movl %0, %%cr3" : : "r"(_pageDirectory.address())); }
    PageFrameAllocator _pageFrameAllocator;
    PageTable _pageDirectory;
};
