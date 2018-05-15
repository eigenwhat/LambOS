#pragma once

#include <stddef.h>
#include <stdint.h>

#define k4KPageAddressMask 0xFFFFF000
#define k4MPageAddressMask 0xFFC00000
#define kPageFlagsMask 0x00000FFF

typedef enum page_flag {
    kPresentBit = 0x00000001,
    kReadWriteBit = 0x00000002,
    kSupervisorBit = 0x00000004,
    kWriteThroughBit = 0x00000008,
    kCacheDisabledBit = 0x00000010,
    kAccessedBit = 0x00000020,
    kDirtyBit = 0x00000040,
    kPageSizeBit = 0x00000080,
    kGlobalBit = 0x00000100,
} PageEntryFlag;

class PageTable;

class PageEntry {
public:
    PageEntry(uint32_t address) : _entry(address & k4KPageAddressMask) {}
    void unsetFlag(PageEntryFlag flag) { _entry &= ~flag; }
    void setFlag(PageEntryFlag flag) { _entry |= flag; }
    bool getFlag(PageEntryFlag flag) { return (bool)(_entry & flag); }
    void setFlags(uint32_t flags) { _entry = (_entry & k4KPageAddressMask) | (flags & kPageFlagsMask); }
    uint32_t address() { return _entry & k4KPageAddressMask; }
    uint32_t flags() { return _entry & kPageFlagsMask; }
    friend class PageTable;
private:
    uint32_t _entry;
};

class PageTable {
public:
    PageTable(uint32_t *tableAddress) : _tableAddress(tableAddress) {}
    void clear();
    uint32_t *address() const { return _tableAddress; }
    PageEntry entryAtIndex(uint16_t index) const;
    void setEntry(uint16_t index, PageEntry entry);
    void install();
private:
    uint32_t *_tableAddress;
};
