//
// Created by Martin Miralles-Cordal on 8/26/2013.
//

#include <mem/PageFrameAllocator.hpp>
#include <arch/i386/cpu/multiboot.h>
#include <Kernel.hpp>

#include <io/Print.hpp>

#include <cstdio>
#include <cstring>

using namespace sys::literals;

namespace {

constexpr auto frame_to_index(PageFrame frame) { return (frame & k4KPageAddressMask) / 0x1000; }
constexpr PageFrame index_to_frame(uint32_t index) { return index * 0x1000; }

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
        sys::print("page_offset=%@,num_pages=%@\n", page_offset, num_pages);
        if (mmap->type == 1) {
            for (auto i = std::size_t(page_offset); i < page_offset + num_pages; ++i) {
                usable[i] = true;
            }
        }
    }
}

void PageFrameAllocator::markFrameUsable(PageFrame frame, bool usable)
{
    uint32_t const frameNumber = frame_to_index(frame);
    _bitmap.usable[frameNumber] = usable;
}

bool PageFrameAllocator::requestFrame(PageFrame frame)
{
    return requestFrameIndex(frame_to_index(frame));
}


bool PageFrameAllocator::requestFrameIndex(std::size_t index)
{
    bool retval = _bitmap.usableAndFree(index);
    if (retval) {
        _bitmap.used[index] = true;
    }

    return retval;
}

PageFrame PageFrameAllocator::alloc(std::size_t numberOfFrames)
{
    if (numberOfFrames == 0) { return std::numeric_limits<PageFrame>::max(); }

    uint32_t const startIndex = frame_to_index(_lastAllocFrame);

    auto const findFrames = [=, this](uint32_t i) -> sys::Maybe<PageFrame> {
        // printf("checking bitmap idx %d\n", i);
        bool found = true;
        for (auto offset = 0_sz; offset < numberOfFrames; ++offset) {
            found &= _bitmap.usableAndFree(i + offset);
        }

        if (found) {
            // puts("got 'em");
            PageFrame const result = index_to_frame(i);
            for (auto offset = 0_sz; offset < numberOfFrames; ++offset) {
                _bitmap.used[i + offset] = true;
            }
            _lastAllocFrame = index_to_frame(i + numberOfFrames - 1);
            return result;
        }
        else {
            return sys::Nothing;
        }
    };
    // start checking from the last place we allocated since the next page is likely available
    for (uint32_t i = startIndex; i < kPagesInBitmap; ++i) {
        auto result = findFrames(i);
        if (result) {
            return *result;
        }
    }

    // we didn't find anything yet, so check the rest
    for (uint32_t i = 0; i < startIndex; ++i) {
        auto result = findFrames(i);
        if (result) {
            return *result;
        }
    }

    // out of memory
    kernel->panic("Unable to allocate page frame. Out of memory.");

    return 0;
}

void PageFrameAllocator::free(PageFrame frame, std::size_t numberOfFrames)
{
    auto const index = frame_to_index(frame);
    for (auto i = 0_sz; i < numberOfFrames; ++i) {
        _bitmap.used[index + i] = false;
    }
}
