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

    PageFrameAllocator() = default;
    PageFrameAllocator(uint32_t mmapAddr, uint32_t mmapLength) { _bitmap.loadMemoryMap(mmapAddr, mmapLength); }

    void loadMemoryMap(uint32_t mmapAddr, uint32_t mmapLength) { _bitmap.loadMemoryMap(mmapAddr, mmapLength); }

    PageFrame alloc();
    void free(PageFrame frame);
    void markFrameUsable(PageFrame frame, bool usable);
    bool requestFrame(PageFrame frame);
    bool requestFrameIndex(std::size_t index);

  private:
    Bitmap _bitmap{};
    PageFrame _lastAllocFrame{0};
};
