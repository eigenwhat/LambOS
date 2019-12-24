//
// Created by Martin Miralles-Cordal on 12/24/2019.
//

#pragma once

#include <bits/memory/destroy.hpp>

namespace std {

namespace detail {

struct uninitialized_fill_fn
{
    template <nothrow_forward_iterator I, nothrow_sentinel_for<I> S, typename T>
    requires constructible_from<iter_value_t<I>, const T&>
    I operator()(I first, S last, const T& x) const
    {
        return uninitialized_fill_fn::impl(std::move(first), std::move(last), x);
    }

    template <nothrow_forward_range Rng, typename T>
    requires constructible_from<iter_value_t<ranges::iterator_t<Rng>>, const T&>
    ranges::safe_iterator_t<Rng> operator()(Rng&& rng, const T& x) const
    {
        return uninitialized_fill_fn::impl(begin(rng), end(rng), x);
    }

  private:
    friend struct uninitialized_fill_n_fn;

    template <typename I, typename S, typename T>
    static I impl(I first, S last, const T& x)
    {
        I it = first;
        for (; it != last; ++it) { new (detail::voidify(*it)) std::remove_reference_t<iter_reference_t<I>>(x); }
        return it;
    }
};

}

__DEF_NIEBLOID(detail::uninitialized_fill_fn, uninitialized_fill)

namespace detail {

struct uninitialized_fill_n_fn {
    template <nothrow_forward_iterator I, typename T> requires constructible_from<iter_value_t<I>, const T&>
    I operator()(I first, iter_difference_t<I> n, const T& x) const
    {
        return uninitialized_fill_fn::impl(make_counted_iterator(std::move(first), n), default_sentinel, x).base();
    }
};

}

__DEF_NIEBLOID(detail::uninitialized_fill_n_fn, uninitialized_fill_n)

} // namespace std
