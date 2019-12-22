//
// Created by Martin Miralles-Cordal on 12/22/2019.
//

#pragma once

#include <iterator>
#include <ranges>

#include <bits/algo_support.hpp>

namespace std::ranges {

namespace detail {

struct fill_n_fn {
    template <typename T, typename O> requires output_iterator<O, const T&>
    constexpr O
    operator()(O first, iter_difference_t<O> n, const T& value) const
    {
        for (iter_difference_t<O> i{0}; i < n; ++i, ++first) {
            *first = value;
        }
        return first;
    }
};

} // namespace detail

__DEF_NIEBLOID(detail::fill_n_fn, fill_n)

} // namespace std::ranges
