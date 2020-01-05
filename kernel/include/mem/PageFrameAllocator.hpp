//
// Created by Martin Miralles-Cordal on 8/26/2013.
//

#pragma once

#include <cstddef>
#include <stdint.h>
#include <mem/PageTable.hpp>

#define PAGES_IN_BITMAP 1048576

typedef uint32_t PageFrame;

class PageFrameAllocator;

template <typename Hook>
concept PageFrameInitializationHook = requires(Hook hook, PageFrameAllocator *allocator) { hook(allocator); };

class PageFrameAllocator
{
public:
    PageFrameAllocator(uint32_t mmap_addr, uint32_t mmap_length);

    template <PageFrameInitializationHook Func>
    PageFrameAllocator(uint32_t mmap_addr, uint32_t mmap_length, Func &&hook)
        : PageFrameAllocator{mmap_addr, mmap_length} { hook(this); }

    PageFrame alloc();
    void free(PageFrame frame);
    void markFrameUsable(PageFrame frame, bool usable);
    bool requestFrame(PageFrame frame);
    bool frameIsUsable(PageFrame frame) { return _frameIsUsable(_frameToIndex(frame)); }
    bool frameIsFree(PageFrame frame) { return _frameIsFree(_frameToIndex(frame)); }
private:
    uint32_t _frameToIndex(PageFrame frame) { return (frame & k4KPageAddressMask) / 0x1000; }
    PageFrame _indexToFrame(uint32_t index) { return index * 0x1000; }
    bool _frameIsUsable(uint32_t frameIndex) { return !(_bitmapUsable[frameIndex/8] & (1 << (frameIndex % 8))); }
    bool _frameIsFree(uint32_t frameIndex) { return _frameIsUsable(frameIndex) && !(_bitmapFree[frameIndex/8] & (1 << (frameIndex % 8))); }
    uint8_t _bitmapUsable[PAGES_IN_BITMAP/8];
    uint8_t _bitmapFree[PAGES_IN_BITMAP/8];
    PageFrame _lastAllocFrame;
};
