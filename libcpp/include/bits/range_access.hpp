//
// Created by Martin Miralles-Cordal on 12/22/2019.
//

#pragma once

#include <bits/iterator/concepts.hpp>
#include <bits/iterator/reverse_iterator.hpp>

#include <initializer_list>

namespace std {

template<typename _T> inline constexpr auto begin(_T& rng) { return rng.begin(); }
template<typename _T> inline constexpr auto begin(const _T& rng) { return rng.begin(); }

template<typename _T> inline constexpr auto end(_T& rng) { return rng.end(); }
template<typename _T> inline constexpr auto end(const _T& rng) { return rng.end(); }

template<typename _T, size_t _Nm> inline constexpr _T* begin(_T (&__arr)[_Nm]) { return __arr; }
template<typename _T, size_t _Nm> inline constexpr _T* end(_T (&__arr)[_Nm]) { return __arr + _Nm; }

template<typename _T>
inline constexpr auto cbegin(const _T& rng) noexcept(noexcept(std::begin(rng))) { return std::begin(rng); }
template<typename _T>
inline constexpr auto cend(const _T& rng) noexcept(noexcept(std::end(rng))) { return std::end(rng); }

template<typename _T> inline constexpr auto rbegin(_T& rng) { return rng.rbegin(); }
template<typename _T> inline constexpr auto rbegin(const _T& rng) { return rng.rbegin(); }

template<typename _T> inline constexpr auto rend(_T& rng) { return rng.rend(); }
template<typename _T> inline constexpr auto rend(const _T& rng) { return rng.rend(); }

template<class T, size_t N> constexpr reverse_iterator<T*> rbegin(T (&a)[N]) { return reverse_iterator<T*>(a + N); }
template<class T, size_t N> constexpr reverse_iterator<T*> rend(T (&a)[N]) { return reverse_iterator<T*>(a); }
template<class E> constexpr reverse_iterator<const E*> rbegin(initializer_list<E> il) { return reverse_iterator<const E*>(il.end()); }
template<class E> constexpr reverse_iterator<const E*> rend(initializer_list<E> il) { return reverse_iterator<const E*>(il.begin()); }
template<class C> constexpr auto crbegin(const C& c) { return std::rbegin(c); }
template<class C> constexpr auto crend(const C& c) { std::rend(c); }

template<class C> constexpr auto size(const C& c) { return c.size(); }
template<class T, size_t N> constexpr size_t size(const T (&)[N]) noexcept { return N; }

template<class C> constexpr auto ssize(const C& c)
{
    using __ResultT = common_type_t<ptrdiff_t, make_signed_t<decltype(c.size())>>;
    return static_cast<__ResultT>(c.size());
}
template<class T, ptrdiff_t N> constexpr ptrdiff_t ssize(const T (&)[size_t(N)]) noexcept { return N; }

template<class C> [[nodiscard]] constexpr auto empty(const C& c) { return c.empty(); }
template<class T, size_t N> [[nodiscard]] constexpr bool empty(const T (&)[N]) noexcept { return false; }
template<class E> [[nodiscard]] constexpr bool empty(initializer_list<E> il) noexcept { return il.size() == 0; }

template<class C> constexpr auto data(C& c) { return c.data(); }
template<class C> constexpr auto data(const C& c) { return c.data(); }
template<class T, size_t N> constexpr T* data(T (&a)[N]) noexcept { return a; }
template<class E> constexpr const E* data(initializer_list<E> il) noexcept { return il.begin(); }

} // namespace std
