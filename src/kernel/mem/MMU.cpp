#include <Kernel.hpp>
#include <stdlib.h>
#include <new>
#include "MMU.hpp"
#include "PageFrameAllocator.hpp"
#include "PageTable.hpp"

extern uint32_t kernel_end;
extern "C" int log_task(const char *printstr, int success);
extern "C" int log_test(const char *printstr, int success);

struct MMUPFAHook : PageFrameInitializationHook
{
    virtual void operator()(PageFrameAllocator *allocator)
    {
        for(int i = 0; i < 0x100000; i += 0x1000) {
            allocator->markFrameUsable(i, false);
        }

        uint32_t kernel_end_addr = (uint32_t) &kernel_end;

        for(uint32_t i = 0x100000; i <= kernel_end_addr; i += 0x1000) {
            if(!allocator->requestFrame(i)) {
                kernel->panic("Page allocation error: unable to reserve kernel memory frames.");
            }
        }
    }
};

struct PFATestHook : PageFrameInitializationHook
{
    virtual void operator()(PageFrameAllocator *allocator)
    {
        bool foundFreeFrame = false;

        for(int i = 0; i < 0x100000; i += 0x1000) {
            foundFreeFrame = allocator->frameIsUsable(i);
            if(foundFreeFrame) {
                break;
            }
        }

        log_test(" > Testing if first 1MB is not fully reserved...", foundFreeFrame);

        for(int i = 0; i < 0x100000; i += 0x1000) {
            allocator->markFrameUsable(i, false);
        }

        log_task(" > Marking first 1MB as unusable for allocation...", true);

        foundFreeFrame = false;

        for(int i = 0; i < 0x100000; i += 0x1000) {
            foundFreeFrame = allocator->frameIsUsable(i);
            if(foundFreeFrame) {
                char num[33];
                itoa(i, num, 10);
                kernel->out()->writeString(num);
                kernel->out()->writeString("\n");
                break;
            }
        }

        log_test(" > Testing if first 1MB is reserved...", !foundFreeFrame) &&
        log_test(" > Requesting 0x100000 (free) for first time...", allocator->requestFrame(0x100000)) &&
        log_test(" > Requesting 0x100000 (used), pass if denied...", !allocator->requestFrame(0x100000)) &&
        log_task(" > Freeing 0x100000...", (allocator->free(0x100000), true)) &&
        log_test(" > Requesting 0x100000 (free) again...", allocator->requestFrame(0x100000)) &&
        log_task(" > Freeing 0x100000...", (allocator->free(0x100000), true)) &&
        log_test(" > Calling alloc()...", allocator->alloc() == 0x100000) &&
        log_test(" > Verifying page is marked as used...", !allocator->frameIsFree(0x100000)) &&
        log_test(" > Calling alloc() again, checking if new page is returned...", allocator->alloc() == 0x101000) &&
        log_task(" > Freeing 0x100000...", (allocator->free(0x100000), true)) &&
        log_test(" > Verifying page is marked as free...", allocator->frameIsFree(0x100000)) &&
        log_test(" > Calling alloc() again, checking if new page is returned...", allocator->alloc() == 0x102000);
    }
};

uint8_t hook1[sizeof(MMUPFAHook)];
uint8_t hook2[sizeof(PFATestHook)];

MMU::MMU(uint32_t mmap_addr, uint32_t mmap_length)
    : _pageFrameAllocator(mmap_addr, mmap_length)
{
    MMUPFAHook hook;
    hook(&_pageFrameAllocator);
    _pageDirectory = PageTable((uint32_t*)_pageFrameAllocator.alloc());
    char str[33];
    itoa((uint32_t)_pageDirectory.address(), str, 16);
    kernel->out()->writeString(str);
    PageTable firstTable((uint32_t*)(_pageFrameAllocator.alloc()));
    uint32_t address = 0;

    // identity map current used address space
    int lastUsedPage = (uint32_t)firstTable.address() / 0x1000;
    for(int i = 0; i < lastUsedPage; ++i) {
        PageEntry entry(address);
        entry.setFlag(kPresentBit);
        entry.setFlag(kReadWriteBit);
        firstTable.setEntry(i, entry);
        address += 0x1000;
    }

    // Install table for first 4MB
    PageEntry entry((uint32_t)firstTable.address());
    entry.setFlag(kPresentBit);
    entry.setFlag(kReadWriteBit);
    _pageDirectory.setEntry(0, entry);

    // Set last PDE to PD itself
    PageEntry directory((uint32_t)_pageDirectory.address());
    directory.setFlag(kPresentBit);
    directory.setFlag(kReadWriteBit);
    _pageDirectory.setEntry(1023, directory);

    _pageDirectory.install();
}
