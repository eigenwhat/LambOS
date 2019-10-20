//
// Created by Martin Miralles-Cordal on 8/26/2013.
//

#include <mem/PageFrameAllocator.hpp>
#include <arch/i386/cpu/multiboot.h>
#include <Kernel.hpp>
#include <cstdio>
#include <cstring>

#define FRAME_TO_INDEX(frame) (((frame) & k4KPageAddressMask) / 0x1000)

PageFrameAllocator::PageFrameAllocator(uint32_t mmap_addr, uint32_t mmap_length)
        : _lastAllocFrame(0)
{
    memset(_bitmapUsable, 0xFF, PAGES_IN_BITMAP / 8);
    memset(_bitmapFree, 0, PAGES_IN_BITMAP / 8);
    multiboot_memory_map_t *mmap;
    for (mmap = (multiboot_memory_map_t *) mmap_addr;
         (uint32_t) mmap < mmap_addr + mmap_length;
         mmap = (multiboot_memory_map_t * )((uint32_t) mmap + mmap->size + sizeof(mmap->size))) {
        uint64_t page_offset = mmap->addr / 0x1000;
        uint64_t num_pages = mmap->len / 0x1000;
        printf("page_offset=%d,num_pages=%d\n", page_offset, num_pages);
        if (mmap->type == 1) {
            for (uint64_t i = page_offset; i < page_offset + num_pages; ++i) {
                uint64_t bitmap_index = i / 8;
                _bitmapUsable[bitmap_index] &= static_cast<std::uint8_t>(~(1u << (i % 8)));
            }
        }
    }
}

void PageFrameAllocator::markFrameUsable(PageFrame frame, bool usable)
{
    uint32_t frameNumber = _frameToIndex(frame);
    uint32_t index = frameNumber / 8;
    uint8_t bitNumber = frameNumber % 8;
    _bitmapUsable[index] &= std::uint8_t(~(1u << bitNumber));
    _bitmapUsable[index] |= std::uint8_t((!usable) << bitNumber);
}

bool PageFrameAllocator::requestFrame(PageFrame frame)
{
    uint32_t index = _frameToIndex(frame);
    bool retval = _frameIsFree(index);
    if (retval) {
        _bitmapFree[index / 8] |= std::uint8_t(1u << (index % 8));
    }

    return retval;
}

PageFrame PageFrameAllocator::alloc()
{
    uint32_t startIndex = _frameToIndex(_lastAllocFrame);
    // start checking from the last place we allocated since the next page is likely available
    for (uint32_t i = startIndex + 1; i < PAGES_IN_BITMAP; ++i) {
        // printf("checking bitmap idx %d\n", i);
        if (_frameIsUsable(i) && _frameIsFree(i)) {
            // puts("got 'em");
            _lastAllocFrame = _indexToFrame(i);
            _bitmapFree[i / 8] |= std::uint8_t(1u << (i % 8));
            return _lastAllocFrame;
        }
    }

    // we didn't find anything yet, so check the rest
    for (uint32_t i = 0; i < startIndex; ++i) {
        if (_frameIsUsable(i) && _frameIsFree(i)) {
            _lastAllocFrame = _indexToFrame(i);
            _bitmapFree[i / 8] |= std::uint8_t(1 << (i % 8));
            return _lastAllocFrame;
        }
    }

    // out of memory
    kernel->panic("Unable to allocate page frame. Out of memory.");

    return 0;
}

void PageFrameAllocator::free(PageFrame frame)
{
    auto index = _frameToIndex(frame);
    _bitmapFree[index / 8] &= std::uint8_t(~(1u << (index % 8)));
}
