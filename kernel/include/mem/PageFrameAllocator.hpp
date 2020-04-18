//
// Created by Martin Miralles-Cordal on 8/26/2013.
//

#pragma once

#include <mem/PageTable.hpp>
#include <mem/UniquePtr.hpp>

#include <util/BitSet.hpp>

constexpr std::uint32_t const kPagesInBitmap = 1048576;
constexpr std::uint32_t const kFrameSize = 4096;

using PageFrame = std::uintptr_t;

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

    template <typename T>
    void * alloc()
    {
        constexpr auto framesNeeded = sys::div_ceil(sizeof(T), kFrameSize);
        return reinterpret_cast<void *>(alloc(framesNeeded));
    }

    PageFrame alloc(std::size_t numberOfFrames);
    void free(PageFrame frame, std::size_t numberOfFrames = 1);
    void markFrameUsable(PageFrame frame, bool usable);
    bool requestFrame(PageFrame frame);
    bool requestFrameIndex(std::size_t index);

  private:
    Bitmap _bitmap{};
    PageFrame _lastAllocFrame{0};
};
