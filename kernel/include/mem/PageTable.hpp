//
// Created by Martin Miralles-Cordal on 8/17/2013.
//

#pragma once

#include <cstddef>
#include <cstdint>

constexpr std::uint32_t k4KPageAddressMask = 0xFFFFF000;
constexpr std::uint32_t k4MPageAddressMask = 0xFFC00000;
constexpr std::uint32_t kPageFlagsMask = 0x00000FFF;

enum PageEntryFlag : std::uint32_t {
    kPresentBit       = 0b00000001,
    kReadWriteBit     = 0b00000010,
    kSupervisorBit    = 0b00000100,
    kWriteThroughBit  = 0b00001000,
    kCacheDisabledBit = 0b00010000,
    kAccessedBit      = 0b00100000,
    kDirtyBit         = 0b01000000,
    kPageSizeBit      = kDirtyBit,
    kGlobalBit        = 0b10000000,
};

class PageTable;

class PageEntry
{
  public:
    PageEntry() = default;
    explicit PageEntry(std::uintptr_t address) : _entry(address & k4KPageAddressMask) {}
    PageEntry(std::uintptr_t entry, int) : _entry(entry) {}

    [[nodiscard]] bool getFlag(PageEntryFlag flag) const { return (bool)(_entry & flag); }
    void unsetFlag(PageEntryFlag flag) { _entry &= ~flag; }
    void setFlag(PageEntryFlag flag) { _entry |= flag; }
    void setFlags(std::uintptr_t flags) { _entry = (_entry & k4KPageAddressMask) | (flags & kPageFlagsMask); }

    [[nodiscard]] std::uintptr_t address() const { return _entry & k4KPageAddressMask; }
    [[nodiscard]] std::uintptr_t flags() const { return _entry & kPageFlagsMask; }
    [[nodiscard]] std::uintptr_t entry() const { return _entry; }

  private:
    std::uintptr_t _entry;
};

static_assert(sizeof(PageEntry) == sizeof(std::uint32_t));

class PageTable
{
  public:
    PageTable() : _tableAddress{nullptr} {}

    explicit PageTable(PageEntry e) : _tableAddress{reinterpret_cast<PageEntry *>(e.address())} {}

    /**
     * Creates a representation of a page table.
     * @param tableAddress The virtual address where the table resides if paging
     *                     is enabled, physical otherwise.
     */
    PageTable(std::uint32_t tableAddress) : _tableAddress{reinterpret_cast<PageEntry *>(tableAddress)} {}
    PageTable(void *tableAddress) : _tableAddress{static_cast<PageEntry *>(tableAddress)} {}

    /**
     * Clears all entries in the table.
     * This method should be called when creating a new page table.
     */
    void clear();

    /** The addressable location of the page table in memory. */
    [[nodiscard]] std::uint32_t * address() const { return (std::uint32_t *)_tableAddress; }

    /**
     * Returns the PageEntry at the given index.
     * @param index The index of the entry.
     * @return The entry.
     */
    [[nodiscard]] PageEntry entryAtIndex(uint16_t index) const { return _tableAddress[index]; }
    [[nodiscard]] PageEntry operator[](uint16_t index) const { return _tableAddress[index]; }

    /**
     * Sets the PageEntry at the given index.
     * @param index The index to set/replace.
     * @param entry The entry to set.
     */
    void setEntry(uint16_t index, PageEntry entry)
    {
        _tableAddress[index] = entry;
        invalidatePage(entry.address());
    }

    /** Installs the PageTable as the active page directory. */
    void install();

    explicit operator bool() const { return _tableAddress != nullptr; }

  private:
    static void invalidatePage(uintptr_t m);
    PageEntry *_tableAddress;
};
