#include <string.h>
#include "PageFrameAllocator.hpp"
#include "../cpu/multiboot.h"

PageFrameAllocator::PageFrameAllocator(uint32_t mmap_addr, uint32_t mmap_length)
{
    memset(_bitmap, 0xFF, PAGES_IN_BITMAP/8);
    multiboot_memory_map_t *mmap;
    for (mmap = (multiboot_memory_map_t *) mmap_addr; 
            (uint32_t) mmap < mmap_addr + mmap_length;
            mmap = (multiboot_memory_map_t *) ((uint32_t) mmap + mmap->size + sizeof (mmap->size)))
        {
            uint32_t page_offset = mmap->addr/0x1000;
            uint32_t num_pages = mmap->len/0x1000;
            if(mmap->type == 1 && mmap->addr >= 0x100000) {
                for(int i = page_offset; i < page_offset + num_pages; ++i) {
                    int bitmap_index = i/8;
                    _bitmap[bitmap_index] &= ~(1 << (i % 8));
                }
            }
        }
}
