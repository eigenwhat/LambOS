//
// Created by Martin Miralles-Cordal on 12/22/2019.
//

#pragma once

#include <iterator>

namespace std::ranges {

using std::begin;
using std::cbegin;
using std::rbegin;
using std::crbegin;
using std::end;
using std::cend;
using std::rend;
using std::crend;

struct dangling
{
    constexpr dangling() noexcept = default;
    template <typename... Args> constexpr dangling(Args&&...) noexcept {}
};

template<typename T>
concept range = requires(T& t) { ranges::begin(t); ranges::end(t); };

template <ranges::range T>
using iterator_t = decltype(ranges::begin(declval<T&>()));

template <ranges::range T>
using sentinel_t = decltype(ranges::end(declval<T&>()));

template <ranges::range T>
using range_difference_t = iter_difference_t<ranges::iterator_t<T>>;

template <ranges::range T>
using range_value_t = iter_value_t<ranges::iterator_t<T>>;

template <ranges::range T>
using range_reference_t = iter_reference_t<ranges::iterator_t<T>>;

template <ranges::range T>
using range_rvalue_reference_t = iter_rvalue_reference_t<ranges::iterator_t<T>>;

template<class R, class T>
concept output_range = range<R> && output_iterator<iterator_t<R>, T>;

template<ranges::range R> inline constexpr bool enable_safe_range = false;

template<class R>
concept safe_range = ranges::range<R>
                     && (is_lvalue_reference_v<R> || ranges::enable_safe_range<remove_cvref_t<R>>);

template<ranges::range R>
using safe_iterator_t = conditional_t<ranges::safe_range<R>, ranges::iterator_t<R>, ranges::dangling>;


} // namespace std::ranges