#pragma once

#include <cstddef>
#include <cstdint>

#define k4KPageAddressMask 0xFFFFF000
#define k4MPageAddressMask 0xFFC00000
#define kPageFlagsMask 0x00000FFF

enum PageEntryFlag : std::uint32_t {
    kPresentBit = 0x00000001,
    kReadWriteBit = 0x00000002,
    kSupervisorBit = 0x00000004,
    kWriteThroughBit = 0x00000008,
    kCacheDisabledBit = 0x00000010,
    kAccessedBit = 0x00000020,
    kDirtyBit = 0x00000040,
    kPageSizeBit = 0x00000080,
    kGlobalBit = 0x00000100,
};

class PageTable;

class PageEntry {
public:
    PageEntry(std::uint32_t address) : _entry(address & k4KPageAddressMask) {}
    void unsetFlag(PageEntryFlag flag) { _entry &= ~flag; }
    void setFlag(PageEntryFlag flag) { _entry |= flag; }
    bool getFlag(PageEntryFlag flag) { return (bool)(_entry & flag); }
    void setFlags(std::uint32_t flags) { _entry = (_entry & k4KPageAddressMask) | (flags & kPageFlagsMask); }
    std::uint32_t address() { return _entry & k4KPageAddressMask; }
    std::uint32_t flags() { return _entry & kPageFlagsMask; }
    friend class PageTable;
private:
    std::uint32_t _entry;
};

class PageTable
{
  public:
    /**
     * Creates a representation of a page table.
     * @param tableAddress The virtual address where the table resides if paging
     *                     is enabled, physical otherwise.
     */
    PageTable(std::uint32_t *tableAddress) : _tableAddress(tableAddress) {}

    /**
     * Clears all entries in the table.
     * This method should be called when creating a new page table.
     */
    void clear();

    /** The addressable location of the page table in memory. */
    std::uint32_t *address() const { return _tableAddress; }

    /**
     * Returns the PageEntry at the given index.
     * @param index The index of the entry.
     * @return The entry.
     */
    PageEntry entryAtIndex(uint16_t index) const;

    /**
     * Sets the PageEntry at the given index.
     * @param index The index to set/replace.
     * @param entry The entry to set.
     */
    void setEntry(uint16_t index, PageEntry entry);

    /** Installs the PageTable as the active page directory. */
    void install();

  private:
    std::uint32_t *_tableAddress;
};
