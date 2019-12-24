//
// Created by Martin Miralles-Cordal on 12/24/2019.
//

#pragma once

#include <bits/ranges/concepts.hpp>

namespace std {

template<typename I>
concept nothrow_input_iterator = input_iterator<I> && is_lvalue_reference_v<iter_reference_t<I>>
                                 && same_as<remove_cvref_t<iter_reference_t<I>>, iter_value_t<I>>;

template<typename S, typename I>
concept nothrow_sentinel_for = sentinel_for<S, I>;

template<typename R>
concept nothrow_input_range = ranges::range<R>
                              && nothrow_input_iterator<ranges::iterator_t<R>>
                              && nothrow_sentinel_for<ranges::sentinel_t<R>, ranges::iterator_t<R>>;

template<typename I>
concept nothrow_forward_iterator = nothrow_input_iterator<I> && forward_iterator<I> && nothrow_sentinel_for<I, I>;

template<typename R>
concept nothrow_forward_range = nothrow_input_range<R> && nothrow_forward_iterator<ranges::iterator_t<R>>;

namespace detail {
template <typename T>
void* voidify(T& ptr) noexcept { return const_cast<void*>(static_cast<const volatile void*>(std::addressof(ptr))); }
}

} // namespace std