// Copyright (c) 2018 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bits/memory/destroy.hpp>

namespace std {

namespace detail {

struct uninitialized_value_construct_fn
{
    template <nothrow_forward_iterator I, nothrow_sentinel_for<I> S> requires default_constructible<iter_value_t<I>>
    I operator()(I first, S last) const
    {
        return uninitialized_value_construct_fn::impl(
                std::move(first), std::move(last));
    }

    template <nothrow_forward_range Rng> requires default_constructible<iter_value_t<ranges::iterator_t<Rng>>>
    ranges::safe_iterator_t<Rng> operator()(Rng&& rng) const
    {
        return uninitialized_value_construct_fn::impl(
                begin(rng), end(rng));
    }

  private:
    template <typename I, typename S>
    static I impl(I first, S last)
    {
        I it = first;
        for (; it != last; ++it) { new (detail::voidify(*it)) std::remove_reference_t<iter_reference_t<I>>(); }
        return it;
    }
};

}

__DEF_NIEBLOID(detail::uninitialized_value_construct_fn, uninitialized_value_construct)

namespace detail {

struct uninitialized_value_construct_n_fn {
    template <nothrow_forward_iterator I> requires default_constructible<iter_value_t<I>>
    I operator()(I first, iter_difference_t<I> n) const
    {
        return uninitialized_value_construct(make_counted_iterator(std::move(first), n), default_sentinel).base();
    }

};

}

__DEF_NIEBLOID(detail::uninitialized_value_construct_n_fn, uninitialized_value_construct_n)

} // namespace std
