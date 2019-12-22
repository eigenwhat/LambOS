//
// Created by Martin Miralles-Cordal on 12/22/2019.
//

#pragma once

#include <iterator>
#include <ranges>

#include <bits/algo_support.hpp>
#include <bits/invoke.hpp>

namespace std::ranges {

namespace detail {

struct generate_n_fn {
    template <typename O, typename F> requires input_or_output_iterator<O> && copy_constructible<F>
                                             && invocable<F&> && writable<O, invoke_result_t<F&>>
    constexpr O operator()(O first, iter_difference_t<O> n, F gen) const
    {
        for (iter_difference_t<O> i{0}; i < n; ++i, ++first) {
            *first = gen();
        }

        return first;
    }
};

} // namespace detail

__DEF_NIEBLOID(detail::generate_n_fn, generate_n)

} // namespace std::ranges
