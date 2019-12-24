//
// Created by Martin Miralles-Cordal on 12/24/2019.
//

#pragma once

#include <concepts>
#include <iterator>

#include <bits/algo_support.hpp>
#include <bits/memory/concepts.hpp>
#include <bits/ranges/concepts.hpp>

namespace std {

template <destructible T> void destroy_at(T* location) noexcept { location->~T(); }

namespace detail {

struct destroy_fn
{
    template <nothrow_input_iterator I, nothrow_sentinel_for<I> S> requires destructible<iter_value_t<I>>
    I operator()(I first, S last) const noexcept { return destroy_fn::impl(std::move(first), std::move(last)); }

    template <nothrow_input_range Rng> requires destructible<iter_value_t<ranges::iterator_t<Rng>>>
    ranges::safe_iterator_t<Rng> operator()(Rng&& rng) const noexcept { return destroy_fn::impl(begin(rng), end(rng)); }

  private:
    template <typename I, typename S>
    static I impl(I first, S last) noexcept
    {
        for (; first != last; ++first) { destroy_at(std::addressof(*first)); }
        return first;
    }
};

}

__DEF_NIEBLOID(detail::destroy_fn, destroy)

namespace detail {

struct destroy_n_fn
{
    template <typename I>
    std::enable_if_t<
        nothrow_input_iterator<I> &&
        destructible<iter_value_t<I>>, I>
    operator()(I first, iter_difference_t<I> n) const noexcept
    {
        return destroy(make_counted_iterator(std::move(first), n), default_sentinel).base();
    }
};

} // namespace detail

__DEF_NIEBLOID(detail::destroy_n_fn, destroy_n)

} // namespace std
