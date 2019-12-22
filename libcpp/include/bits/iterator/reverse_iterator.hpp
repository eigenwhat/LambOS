//
// Created by Martin Miralles-Cordal on 12/22/2019.
//

#pragma once

#include <compare>
#include <type_traits>

#include <bits/iterator/types.hpp>

namespace std {

template<typename _It>
class reverse_iterator : public iterator<typename iterator_traits<_It>::iterator_category,
                                         typename iterator_traits<_It>::value_type,
                                         typename iterator_traits<_It>::difference_type,
                                         typename iterator_traits<_It>::pointer,
                                         typename iterator_traits<_It>::reference>
{
  public:
    using iterator_type     =  _It;
    using iterator_category = typename iterator_traits<_It>::iterator_category;
    using value_type        = typename iterator_traits<_It>::value_type;
    using difference_type   = typename iterator_traits<_It>::difference_type;
    using pointer           = typename iterator_traits<_It>::pointer;
    using reference         = typename iterator_traits<_It>::reference;

    constexpr reverse_iterator() : current() {}

    explicit constexpr reverse_iterator(iterator_type __rhs) : current(__rhs) {}
    constexpr reverse_iterator(const reverse_iterator& __rhs) = default;
    reverse_iterator& operator=(const reverse_iterator&) = default;

    template<typename _OtherIt>
    constexpr reverse_iterator(const reverse_iterator<_OtherIt>& __rhs) : current(__rhs.base()) {}

    constexpr iterator_type base() const { return current; }

    constexpr reference operator*() const { iterator_type __tmp = current; return *--__tmp; }

    constexpr pointer operator->() const
    requires is_pointer_v<_It> || requires(const _It __i) { __i.operator->(); }
    {
        _It __tmp = current;
        --__tmp;
        return __as_ptr(__tmp);
    }

    constexpr reverse_iterator& operator++() { --current; return *this; }
    constexpr reverse_iterator operator++(int)
    {
        reverse_iterator __tmp = *this;
        --current;
        return __tmp;
    }

    constexpr reverse_iterator& operator--() { ++current; return *this; }
    constexpr reverse_iterator operator--(int)
    {
        reverse_iterator __tmp = *this;
        ++current;
        return __tmp;
    }

    constexpr reverse_iterator operator+(difference_type __rhs) const { return reverse_iterator(current - __rhs); }
    constexpr reverse_iterator& operator+=(difference_type __rhs) { current -= __rhs; return *this; }
    constexpr reverse_iterator operator-(difference_type __rhs) const { return reverse_iterator(current + __rhs); }
    constexpr reverse_iterator& operator-=(difference_type __rhs) { current += __rhs; return *this; }
    
    constexpr reference operator[](difference_type __rhs) const { return *(*this + __rhs); }

    constexpr bool operator==(reverse_iterator const &) const noexcept = default;
    constexpr auto operator<=>(reverse_iterator const &) const noexcept = default;

    template<typename _OtherIt>
    constexpr bool operator==(reverse_iterator<_OtherIt> const &__rhs) const noexcept
    {
        return current == __rhs.base();
    }

    template<typename _OtherIt>
    constexpr auto operator<=>(reverse_iterator<_OtherIt> const &__rhs) const noexcept
    {
        return current <=> __rhs.base();
    }

  protected:
    _It current;

  private:
    template<typename _T> static constexpr _T* __as_ptr(_T* __p) noexcept { return __p; }
    template<typename _T>
    static constexpr pointer __as_ptr(_T __v) noexcept(noexcept(__v.operator->())) { return __v.operator->(); }
};

template<typename _ItL, typename _ItR>
inline constexpr auto operator-(const reverse_iterator<_ItL>& __lhs, const reverse_iterator<_ItR>& __rhs)
{ return __rhs.base() - __lhs.base(); }

template<typename _It>
inline constexpr reverse_iterator<_It>
operator+(typename reverse_iterator<_It>::difference_type __n, const reverse_iterator<_It>& __lhs)
{ return reverse_iterator<_It>(__lhs.base() - __n); }

template<typename _It>
inline constexpr reverse_iterator<_It> make_reverse_iterator(_It __i) { return reverse_iterator<_It>(__i); }

//template<typename _It1, typename _It2> requires (!sized_sentinel_for<_It1, _It2>)
//inline constexpr bool disable_sized_sentinel_for<reverse_iterator<_It1>, reverse_iterator<_It2>> = true;

} // namespace std
