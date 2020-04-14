//
// Created by Martin Miralles-Cordal on 01/01/2020.
//

#pragma once

#include <mem/PageTable.hpp>

#include <mem/Units.hpp>
#include <util/Array.hpp>
#include <util/TypeTraits.hpp>
#include <system/asm.h>

namespace X86 {

using namespace sys::mem_unit_literals;
constexpr std::size_t const kPageFrameSize = 4_KiB;
constexpr std::size_t const kPageTableSize = 4_KiB;
constexpr std::size_t const kPageDirectorySize = kPageTableSize * 1024;

class PageDirectory
{
    sys::Array<PageTable, 1024> tables_;
    sys::Array<PageEntry, 1024> tableFrames_;
    std::uintptr_t physicalAddress_;

  public:
    struct Index
    {
        std::uint16_t directory;
        std::uint16_t page;

        Index() = default;
        explicit Index(void *vaddr) : Index{reinterpret_cast<std::uintptr_t>(vaddr)} {}
        Index(std::uint16_t pde, std::uint16_t pte) : directory{pde}, page{pte} {}
        explicit Index(std::uintptr_t vaddr)
                : directory{static_cast<std::uint16_t>(vaddr >> 22u)}
                , page{static_cast<std::uint16_t>((vaddr >> 12u) & 0x03FFu)} {}
    };

    struct Element
    {
        PageTable table;
        PageEntry entry;
    };

    template <bool Const>
    class EntryProxy
    {
        sys::const_t<PageDirectory, Const> *parent_;
        std::size_t index_;

      public:
        EntryProxy(sys::const_t<PageDirectory, Const> *parent, size_t index) : parent_(parent), index_(index) {}
        [[nodiscard]] PageTable table() const { return parent_->tables_[index_]; }
        [[nodiscard]] std::uintptr_t frame() const { return parent_->tableFrames_[index_].address(); }
        [[nodiscard]] PageEntry entry() const { return parent_->tableFrames_[index_]; }

        [[nodiscard]] bool getFlag(PageEntryFlag flag) const { return entry().getFlag(flag); }
        [[nodiscard]] bool present() const { return getFlag(kPresentBit); }

        auto operator[](std::size_t idx) { return table()[idx]; }

        EntryProxy& operator=(Element const& e)
        {
            static_assert(Const == false);
            parent_->set(index_, e);
            return *this;
        }
    };

    class PageProxy;

    PageDirectory()  : physicalAddress_{reinterpret_cast<std::uintptr_t>(tableFrames_.data())} {}
    explicit PageDirectory(std::uintptr_t physicalAddress) : physicalAddress_{physicalAddress} {}

    PageDirectory& set(std::size_t pde, Element const& e)
    {
        tables_[pde] = e.table;
        tableFrames_[pde] = e.entry;
        invlpg(e.entry.address());
        return *this;
    }

    auto elemAt(std::size_t pde) const { return Element{tables_[pde], tableFrames_[pde]}; }
    auto operator[](std::size_t pde) const { return EntryProxy<true>{this, pde}; }
    auto operator[](std::size_t pde) { return EntryProxy<false>{this, pde}; }

    PageProxy operator[](Index idx) const;

    [[nodiscard]] auto physicalAddress() const { return physicalAddress_; }

    void install();

    void clear();
};

class PageDirectory::PageProxy
{
    PageTable parent_;
    std::uint16_t index_;

  public:
    PageProxy(PageTable parent, std::uint16_t index) : parent_(parent), index_(index) {}
    [[nodiscard]] auto get() const { return parent_.entryAtIndex(index_); }
    PageProxy& operator=(PageEntry e) { parent_.setEntry(index_, e); return *this; }
};

inline auto PageDirectory::operator[](Index idx) const -> PageProxy { return {tables_[idx.directory], idx.page}; }

} //namespace X86
