//
// Created by Martin Miralles-Cordal on 8/26/2013.
//

#pragma once

#include <mem/PageTable.hpp>
#include <mem/UniquePtr.hpp>

constexpr std::uint32_t const kPagesInBitmap = 1048576;

typedef uint32_t PageFrame;

class PageFrameAllocator
{
  public:
    struct Bitmap
    {
        void loadMemoryMap(uint32_t mmapAddr, uint32_t mmapLength);
        uint8_t usable[kPagesInBitmap/8];
        uint8_t free[kPagesInBitmap/8];
    };

    PageFrameAllocator() : _bitmap{}, _lastAllocFrame{0} {}
    void loadMemoryMap(uint32_t mmapAddr, uint32_t mmapLength) { _bitmap.loadMemoryMap(mmapAddr, mmapLength); }

    PageFrame alloc();
    void free(PageFrame frame);
    void markFrameUsable(PageFrame frame, bool usable);
    bool requestFrame(PageFrame frame);
    bool frameIsUsable(PageFrame frame) { return _frameIsUsable(_frameToIndex(frame)); }
    bool frameIsFree(PageFrame frame) { return _frameIsFree(_frameToIndex(frame)); }

  private:
    uint32_t _frameToIndex(PageFrame frame) { return (frame & k4KPageAddressMask) / 0x1000; }
    PageFrame _indexToFrame(uint32_t index) { return index * 0x1000; }
    bool _frameIsUsable(uint32_t frameIndex) { return !(_bitmap.usable[frameIndex/8] & (1 << (frameIndex % 8))); }
    bool _frameIsFree(uint32_t frameIndex) { return _frameIsUsable(frameIndex) && !(_bitmap.free[frameIndex/8] & (1 << (frameIndex % 8))); }

    Bitmap _bitmap;
    PageFrame _lastAllocFrame;
};
