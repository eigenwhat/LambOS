#include <string.h>
#include "PageFrameAllocator.hpp"
#include <cpu/multiboot.h>
#include <Kernel.hpp>

#define FRAME_TO_INDEX(frame) (((frame) & k4KPageAddressMask) / 0x1000)

PageFrameAllocator::PageFrameAllocator(uint32_t mmap_addr, uint32_t mmap_length, PageFrameInitializationHook *hook) 
    : _lastAllocFrame(0)
{
    memset(_bitmapUsable, 0xFF, PAGES_IN_BITMAP/8);
    memset(_bitmapFree, 0, PAGES_IN_BITMAP/8);
    multiboot_memory_map_t *mmap;
    for (mmap = (multiboot_memory_map_t *) mmap_addr; 
        (uint32_t) mmap < mmap_addr + mmap_length;
        mmap = (multiboot_memory_map_t *) ((uint32_t) mmap + mmap->size + sizeof (mmap->size)))
    {
        uint32_t page_offset = mmap->addr/0x1000;
        uint32_t num_pages = mmap->len/0x1000;
        if(mmap->type == 1) {
            for(uint32_t i = page_offset; i < page_offset + num_pages; ++i) {
                int bitmap_index = i/8;
                _bitmapUsable[bitmap_index] &= ~(1 << (i % 8));
            }
        }
    }

    if(hook != NULL) {
        (*hook)(this);
    }
}

void PageFrameAllocator::markFrameUsable(PageFrame frame, bool usable)
{
    uint32_t frameNumber = _frameToIndex(frame);
    uint32_t index = frameNumber/8;
    uint8_t bitNumber = frameNumber % 8;
    _bitmapUsable[index] &= ~(1 << bitNumber);
    _bitmapUsable[index] |= (!usable) << bitNumber;
}

bool PageFrameAllocator::requestFrame(PageFrame frame)
{
    int index = _frameToIndex(frame);
    bool retval = _frameIsFree(index);
    if(retval) {
        _bitmapFree[index/8] |= 1 << (index % 8);
    }

    return retval;
}

PageFrame PageFrameAllocator::alloc()
{
    int startIndex = _frameToIndex(_lastAllocFrame);

    // start checking from the last place we allocated since the next page is likely available
    for(int i = startIndex; i < PAGES_IN_BITMAP; ++i) {
        if(_frameIsUsable(i) && _frameIsFree(i)) {
            _lastAllocFrame = _indexToFrame(i);
            _bitmapFree[i/8] |= 1 << (i % 8);
            return _lastAllocFrame;
        }
    }

    // we didn't find anything yet, so check the rest
    for(int i = 0; i < startIndex; ++i) {
        if(_frameIsUsable(i) && _frameIsFree(i)) {
            _lastAllocFrame = _indexToFrame(i);
            _bitmapFree[i/8] |= 1 << (i % 8);
            return _lastAllocFrame;
        }
    }

    // out of memory
    kernel->panic("Unable to allocate page frame. Out of memory.");

    return 0;
}

void PageFrameAllocator::free(PageFrame frame)
{
    int index = _frameToIndex(frame);
    _bitmapFree[index/8] &= ~(1 << (index % 8));
}
