#pragma once

#include <stddef.h>
#include <stdint.h>

#define k4KPageAddressMask 0xFFFFF000
#define k4MPageAddressMask 0xFFC00000
#define kPageFlagsMask 0x00000FFF

typedef enum page_flag {
    kPresentBit = 0xFFFFFFFE,
    kReadWriteBit = 0xFFFFFFFD,
    kSupervisorBit = 0xFFFFFFFB,
    kWriteThroughBit = 0xFFFFFFF7,
    kCacheDisabledBit = 0xFFFFFFEF,
    kAccessedBit = 0xFFFFFFDF,
    kDirtyBit = 0xFFFFFFBF,
    kPageSizeBit = 0xFFFFFF7F,
    kGlobalBit = 0xFFFFFEFF,
} PageEntryFlag;

class PageTable;

class PageEntry {
public:
    PageEntry() : _entry(0) {}
    void setPageTableAddress(uint32_t address) { _entry = (address & k4KPageAddressMask) | (_entry & kPageFlagsMask); }
    void unsetFlag(PageEntryFlag flag) { _entry &= flag; }
    void setFlag(PageEntryFlag flag) { _entry |= ~flag; }
    bool getFlag(PageEntryFlag flag) { return (bool)(_entry & ~flag); }
private:
    uint32_t _entry;
};

class PageTable {
    
};

class PageDirectory {
public:
    PageDirectory() : _pageDirectoryAddress(0) {}
    PageDirectory(uint32_t *directoryAddress) : _pageDirectoryAddress(directoryAddress) {}
    void install();
private:
	uint32_t *_pageDirectoryAddress;
};
