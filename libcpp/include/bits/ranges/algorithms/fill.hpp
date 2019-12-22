//
// Created by Martin Miralles-Cordal on 12/22/2019.
//

#pragma once

#include <iterator>

#include <bits/ranges/concepts.hpp>
#include <bits/algo_support.hpp>

namespace std::ranges {

namespace detail {

struct fill_fn {
  private:
    template <typename T, typename O, typename S>
    requires std::output_iterator<O, T const&> && std::sentinel_for<S, O>
    static constexpr O impl(O first, S last, const T& value)
    {
        for (; first != last; ++first) { *first = value; }
        return first;
    }

  public:
    template <typename T, typename O, typename S> requires std::output_iterator<O, const T&> && std::sentinel_for<S, O>
    constexpr O operator()(O first, S last, T const &value) const
    {
        return fill_fn::impl(std::move(first), std::move(last), value);
    }

    template <typename T, typename Rng> requires output_range<Rng, const T&>
    constexpr safe_iterator_t<Rng> operator()(Rng&& rng, T const &value) const
    {
        return fill_fn::impl(ranges::begin(rng), ranges::end(rng), value);
    }
};

} // namespace detail

__DEF_NIEBLOID(detail::fill_fn, fill)

} // namespace std::ranges
