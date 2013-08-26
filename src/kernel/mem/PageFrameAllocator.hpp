#pragma once

#include <stddef.h>
#include <stdint.h>

#define PAGES_IN_BITMAP 1048576

class PageFrameAllocator
{
public:
    PageFrameAllocator(uint32_t mmap_addr, uint32_t mmap_length);
    bool frameIsAvailable(uint32_t frameIndex) { return !(_bitmap[frameIndex/8] & (1 << (frameIndex % 8))); }
private:
    uint8_t _bitmap[PAGES_IN_BITMAP/8];
};
