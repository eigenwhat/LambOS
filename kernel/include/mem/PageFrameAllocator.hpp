//
// Created by Martin Miralles-Cordal on 8/26/2013.
//

#pragma once

#include <mem/PageTable.hpp>
#include <mem/UniquePtr.hpp>

#include <util/BitSet.hpp>

constexpr std::uint32_t const kPagesInBitmap = 1048576;

typedef uint32_t PageFrame;

class PageFrameAllocator
{
  public:
    struct Bitmap
    {
        void loadMemoryMap(uint32_t mmapAddr, uint32_t mmapLength);
        bool usableAndFree(uint32_t frameIndex) { return usable[frameIndex] && !used[frameIndex]; }

        sys::BitSet<kPagesInBitmap> usable;
        sys::BitSet<kPagesInBitmap> used;
    };

    PageFrameAllocator() : _bitmap{}, _lastAllocFrame{0} {}
    void loadMemoryMap(uint32_t mmapAddr, uint32_t mmapLength) { _bitmap.loadMemoryMap(mmapAddr, mmapLength); }

    PageFrame alloc();
    void free(PageFrame frame);
    void markFrameUsable(PageFrame frame, bool usable);
    bool requestFrame(PageFrame frame);

  private:
    uint32_t _frameToIndex(PageFrame frame) { return (frame & k4KPageAddressMask) / 0x1000; }
    PageFrame _indexToFrame(uint32_t index) { return index * 0x1000; }

    Bitmap _bitmap;
    PageFrame _lastAllocFrame;
};
