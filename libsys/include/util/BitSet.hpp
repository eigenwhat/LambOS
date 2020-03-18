//
// Created by Martin Miralles-Cordal on 02/24/2020.
//

#pragma once

#include <math/Math.hpp>
#include <cstddef>
#include <cstring>
#include <limits>

namespace sys {

template <std::size_t SizeBits>
class BitSet {
    using StoreUnit = unsigned int;
    static constexpr inline std::size_t kBPU = 8 * sizeof(StoreUnit);

    class ProxyBit
    {
      public:
        constexpr ProxyBit(BitSet *parent, std::size_t index) : _parent{parent}, _index{index} {}
        [[nodiscard]] constexpr operator bool() const { return _parent->test(_index); }
        constexpr ProxyBit& operator=(bool b) { b ? _parent->set(_index) : _parent->unset(_index); return *this; }

      private:
        BitSet *_parent;
        std::size_t _index;
    };

  public:
    constexpr BitSet() : BitSet(false) {}
    constexpr explicit BitSet(bool initial) { std::memset(_bits, initial ? 0xFF : 0, size()); }

    [[nodiscard]]
    constexpr bool test(std::size_t bit) const { return (unitForBit(bit) & bitmask(bit)) != StoreUnit{0}; }

    [[nodiscard]]
    constexpr bool operator[](std::size_t bit) const { return test(bit); }

    [[nodiscard]]
    constexpr ProxyBit operator[](std::size_t bit) { return ProxyBit{this, bit}; }

    constexpr bool set(std::size_t bit)
    {
        bool const prev = test(bit);
        unitForBit(bit) = unitForBit(bit) | bitmask(bit);
        return prev;
    }

    constexpr bool unset(std::size_t bit)
    {
        bool const prev = test(bit);
        unitForBit(bit) = unitForBit(bit) & StoreUnit(~bitmask(bit));
        return prev;
    }

    [[nodiscard]] constexpr std::size_t size() const { return sizeof(_bits); }

  private:
    [[nodiscard]]
    constexpr StoreUnit bitmask(std::size_t bit) const { return static_cast<StoreUnit>(1 << (bit % kBPU)); }

    [[nodiscard]] constexpr StoreUnit& unitForBit(std::size_t bit) { return _bits[bit/kBPU]; }
    [[nodiscard]] constexpr StoreUnit unitForBit(std::size_t bit) const { return _bits[bit/kBPU]; }
    StoreUnit _bits[div_ceil(SizeBits, kBPU)];
};

} // namespace sys