//
// Created by Martin Miralles-Cordal on 10/6/2019.
//

#pragma once

#include <util/Concepts.hpp>

#include <algorithm>
#include <utility>

namespace sys {

/** Fixed-length array object where the size is known at compile time. Basically std::array. */
template<typename T, std::size_t Size>
struct Array
{
    static_assert(Size > 0, "Array must be nonzero size!");
    using value_type = T;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = value_type*;
    using const_iterator = const value_type*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    constexpr void fill(const value_type& u) { std::fill_n(begin(), size(), u); }
    constexpr void swap(Array& other) noexcept(std::is_nothrow_swappable<T>::value) { std::swap_ranges(begin(), end(), other.begin()); }

    [[nodiscard]] constexpr size_type size() const noexcept { return Size; }
    [[nodiscard]] constexpr size_type max_size() const noexcept { return Size; }
    [[nodiscard]] constexpr bool empty() const noexcept { return size() == 0; }

    [[nodiscard]] constexpr reference operator[](size_type n) noexcept { return _elems[n]; }
    [[nodiscard]] constexpr const_reference operator[](size_type n) const noexcept { return _elems[n]; }

    [[nodiscard]] constexpr reference front() noexcept { return *begin(); }
    [[nodiscard]] constexpr const_reference front() const noexcept { return _elems[0]; }
    [[nodiscard]] constexpr reference back() noexcept { return *(end() - 1); }
    [[nodiscard]] constexpr const_reference back() const noexcept { return _elems[Size - 1]; }

    [[nodiscard]] constexpr pointer data() noexcept { return _elems; }
    [[nodiscard]] constexpr const_pointer data() const noexcept { return _elems; }

    [[nodiscard]] constexpr iterator begin() noexcept { return iterator(data()); }
    [[nodiscard]] constexpr const_iterator begin() const noexcept { return const_iterator(data()); }
    [[nodiscard]] constexpr iterator end() noexcept { return iterator(data() + Size); }
    [[nodiscard]] constexpr const_iterator end() const noexcept { return const_iterator(data() + Size); }
    [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return const_iterator(data()); }
    [[nodiscard]] constexpr const_iterator cend() const noexcept { return const_iterator(data() + Size); }

    [[nodiscard]] constexpr friend bool operator==(Array const &lhs, Array const &rhs) { return std::equal(lhs.begin(), lhs.end(), rhs.begin()); }
    [[nodiscard]] constexpr friend bool operator!=(Array const &lhs, Array const &rhs) { return !(lhs == rhs); }
    [[nodiscard]] constexpr friend bool operator<(Array const &lhs, Array const &rhs) { return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()); }
    [[nodiscard]] constexpr friend bool operator>(Array const &lhs, Array const &rhs) { return rhs < lhs; }
    [[nodiscard]] constexpr friend bool operator<=(Array const &lhs, Array const &rhs) { return !(lhs > rhs); }
    [[nodiscard]] constexpr friend bool operator>=(Array const &lhs, Array const &rhs) { return !(lhs < rhs); }

    T _elems[Size];
};

template<typename T, std::same_as<T>... Ts> Array(T, Ts...) -> Array<T, 1 + sizeof...(Ts)>;

template<std::swappable T, std::size_t Size>
constexpr inline void swap(Array<T, Size> &a, Array<T, Size> &b) noexcept(noexcept(a.swap(b))) { a.swap(b); }

} // libsys namespace
