//
// Created by Martin Miralles-Cordal on 8/26/2013.
//

#include <mem/PageFrameAllocator.hpp>
#include <arch/i386/cpu/multiboot.h>
#include <Kernel.hpp>

#include <cstdio>
#include <cstring>

namespace {

constexpr auto frame_to_index(auto frame) { return (frame & k4KPageAddressMask) / 0x1000; }

multiboot_memory_map_t * next(multiboot_memory_map_t *mmap)
{
    return reinterpret_cast<multiboot_memory_map_t *>(uint32_t(mmap) + mmap->size + sizeof(mmap->size));
}

}

void PageFrameAllocator::Bitmap::loadMemoryMap(uint32_t mmapAddr, uint32_t mmapLength)
{
    auto const end = (multiboot_memory_map_t *)(mmapAddr + mmapLength);
    for (auto *mmap = (multiboot_memory_map_t *)mmapAddr; mmap < end; mmap = next(mmap))
    {
        uint64_t page_offset = mmap->addr / 0x1000;
        uint64_t num_pages = mmap->len / 0x1000;
        printf("page_offset=%d,num_pages=%d\n", page_offset, num_pages);
        if (mmap->type == 1) {
            for (auto i = std::size_t(page_offset); i < page_offset + num_pages; ++i) {
                usable[i] = true;
            }
        }
    }
}

void PageFrameAllocator::markFrameUsable(PageFrame frame, bool usable)
{
    uint32_t const frameNumber = _frameToIndex(frame);
    _bitmap.usable[frameNumber] = usable;
}

bool PageFrameAllocator::requestFrame(PageFrame frame)
{
    uint32_t const index = _frameToIndex(frame);
    bool retval = _bitmap.usableAndFree(index);
    if (retval) {
        _bitmap.used[index] = true;
    }

    return retval;
}

PageFrame PageFrameAllocator::alloc()
{
    uint32_t startIndex = _frameToIndex(_lastAllocFrame);
    // start checking from the last place we allocated since the next page is likely available
    for (uint32_t i = startIndex + 1; i < kPagesInBitmap; ++i) {
        // printf("checking bitmap idx %d\n", i);
        if (_bitmap.usableAndFree(i)) {
            // puts("got 'em");
            _lastAllocFrame = _indexToFrame(i);
            _bitmap.used[i] = true;
            return _lastAllocFrame;
        }
    }

    // we didn't find anything yet, so check the rest
    for (uint32_t i = 0; i < startIndex; ++i) {
        if (_bitmap.usableAndFree(i)) {
            _lastAllocFrame = _indexToFrame(i);
            _bitmap.used[i] = true;
            return _lastAllocFrame;
        }
    }

    // out of memory
    kernel->panic("Unable to allocate page frame. Out of memory.");

    return 0;
}

void PageFrameAllocator::free(PageFrame frame)
{
    auto const index = _frameToIndex(frame);
    _bitmap.used[index] = false;
}
