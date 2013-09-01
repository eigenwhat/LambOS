#include <Kernel.hpp>
#include <stdlib.h>
#include <new>
#include <mem/MMU.hpp>
#include <mem/PageFrameAllocator.hpp>
#include <mem/PageTable.hpp>

extern uint32_t kernel_end;
extern uint32_t readonly_end;
extern "C" int log_task(const char *printstr, int success);
extern "C" int log_test(const char *printstr, int success);

struct MMUPFAHook : PageFrameInitializationHook
{
    virtual void operator()(PageFrameAllocator *allocator)
    {
        uint32_t i = 0;
        for(; i < 0x100000; i += 0x1000) {
            allocator->markFrameUsable(i, false);
        }

        uint32_t kernel_end_addr = (uint32_t) &kernel_end;

        for(; i <= kernel_end_addr; i += 0x1000) {
            if(!allocator->requestFrame(i)) {
                kernel->panic("Page allocation error: unable to reserve kernel memory frames.");
            }
        }
    }
};

uint8_t hook1[sizeof(MMUPFAHook)];

MMU::MMU(uint32_t mmap_addr, uint32_t mmap_length)
    : _pageFrameAllocator(mmap_addr, mmap_length, new (hook1) MMUPFAHook),
      _pageDirectory((uint32_t*)_pageFrameAllocator.alloc())
{
    PageTable firstTable((uint32_t*)(_pageFrameAllocator.alloc()));
    uint32_t address = 0;

    // identity map current used address space
    uint32_t highMemoryBegin = 0x100000/0x1000;
    uint32_t readOnlyEnd = (uint32_t)&readonly_end / 0x1000;
    uint32_t lastUsedPage = (uint32_t)firstTable.address() / 0x1000;

    for(uint32_t i = 0; i < lastUsedPage; ++i) {
        PageEntry entry(address);
        entry.setFlag(kPresentBit);
        // make sure pages for read only data are marked read only
        if(i < highMemoryBegin || i > readOnlyEnd) {
            entry.setFlag(kReadWriteBit);
        }
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
