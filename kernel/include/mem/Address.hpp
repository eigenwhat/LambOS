//
// Created by Martin Miralles-Cordal on 05/20/2026.
//

#pragma once

#include <compare>
#include <cstdint>

namespace X86 {
struct VirtualAddressParts
{
    std::uint16_t pdeIndex{0};
    std::uint16_t pteIndex{0};
    std::uint16_t pageOffset{0};
};
}

struct PhysicalAddressTag;
struct VirtualAddressTag;

template <typename Tag>
class AddressValue
{
  public:
    template <typename T>
    static AddressValue fromPtr(T *pointer)
    {
        return AddressValue{reinterpret_cast<std::uintptr_t>(pointer)};
    }

    [[nodiscard]]
    static constexpr
    auto fromParts(std::uint16_t pdeIndex, std::uint16_t pteIndex = 0, std::uint16_t pageOffset = 0)
        requires std::same_as<Tag, VirtualAddressTag>
    {
        return AddressValue{(static_cast<std::uintptr_t>(pdeIndex) << 22u)
                            | (static_cast<std::uintptr_t>(pteIndex) << 12u)
                            | pageOffset};
    }

    constexpr AddressValue() = default;
    constexpr explicit AddressValue(std::uintptr_t rawAddress) : rawAddress_(rawAddress) {}

    constexpr AddressValue& operator=(std::nullptr_t) noexcept
    {
        rawAddress_ = 0;
        return *this;
    }


    [[nodiscard]]
    constexpr X86::VirtualAddressParts decompose() const noexcept
        requires std::same_as<Tag, VirtualAddressTag>
    {
        return {
            .pdeIndex   = static_cast<std::uint16_t>(raw() >> 22u),
            .pteIndex   = static_cast<std::uint16_t>((raw() >> 12u) & 0x03FFu),
            .pageOffset = static_cast<std::uint16_t>(raw() & 0xFFFu),
        };
    }

    [[nodiscard]] constexpr std::uintptr_t raw() const noexcept { return rawAddress_; }

    template <typename T = void>
    [[nodiscard]] T *asPtr() const
    {
        return reinterpret_cast<T *>(rawAddress_);
    }

    constexpr explicit operator bool() const noexcept { return rawAddress_ != 0; }

    constexpr AddressValue operator+(std::uintptr_t offset) const noexcept
    {
        return AddressValue{rawAddress_ + offset};
    }
    constexpr AddressValue operator-(std::uintptr_t offset) const noexcept
    {
        return AddressValue{rawAddress_ - offset};
    }

    constexpr AddressValue& operator+=(std::uintptr_t offset) noexcept
    {
        rawAddress_ += offset;
        return *this;
    }
    constexpr AddressValue& operator-=(std::uintptr_t offset) noexcept
    {
        rawAddress_ -= offset;
        return *this;
    }

    constexpr std::uintptr_t operator-(AddressValue other) const noexcept
    {
        return rawAddress_ - other.rawAddress_;
    }

    constexpr bool operator==(AddressValue const& other) const noexcept = default;
    constexpr auto operator<=>(AddressValue const& other) const noexcept = default;

  private:
    std::uintptr_t rawAddress_ = 0;
};

using PhysicalAddress = AddressValue<PhysicalAddressTag>;
using VirtualAddress = AddressValue<VirtualAddressTag>;

constexpr PhysicalAddress operator""_phys(unsigned long long int rawAddress)
{
    return PhysicalAddress{static_cast<std::uintptr_t>(rawAddress)};
}

constexpr VirtualAddress operator""_vaddr(unsigned long long int rawAddress)
{
    return VirtualAddress{static_cast<std::uintptr_t>(rawAddress)};
}
