// nanorange/algorithm/copy.hpp
//
// Copyright (c) 2018 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bits/ranges/concepts.hpp>

namespace std::ranges {

template <typename I, typename O>
struct copy_result
{
    [[no_unique_address]] I in;
    [[no_unique_address]] O out;

    template <typename I2, typename O2> requires convertible_to<const I&, I2> && convertible_to<const O&, O2>
    constexpr operator copy_result<I2, O2>() const & { return {in, out}; }

    template <typename I2, typename O2> requires convertible_to<I, I2> && convertible_to<O, O2>
    constexpr operator copy_result<I2, O2>() && { return {std::move(in), std::move(out)}; }
};

namespace detail {

struct copy_fn
{
  public:
    template <input_iterator I, sentinel_for<I> S, weakly_incrementable O> requires indirectly_copyable<I, O>
    copy_result<I, O> operator()(I first, S last, O result) const
    {
        return copy_fn::impl(std::move(first), std::move(last), std::move(result), __priority_tag<1>{});
    }

    template <input_range Rng, weakly_incrementable O> requires indirectly_copyable<iterator_t<Rng>, O>
    constexpr copy_result<safe_iterator_t<Rng>, O>
    operator()(Rng&& rng, O result) const
    {
        return copy_fn::impl(begin(rng), end(rng), std::move(result), __priority_tag<1>{});
    }

  private:
    // If we know the distance between first and last, we can use that
    // information to (potentially) allow better codegen
    template <typename I, sized_sentinel_for<I> S, typename O>
    static constexpr copy_result<I, O> impl(I first, S last, O result, __priority_tag<1>)
    {
        const auto dist = last - first;

        for (iter_difference_t<I> i = 0; i < dist; ++i) {
            *result = *first;
            ++first;
            ++result;
        }

        return {std::move(first), std::move(result)};
    }

    template <typename I, typename S, typename O>
    static constexpr copy_result<I, O> impl(I first, S last, O result, __priority_tag<0>)
    {
        while (first != last) {
            *result = *first;
            ++first;
            ++result;
        }

        return {std::move(first), std::move(result)};
    }
};

} // namespace detail

__DEF_NIEBLOID(detail::copy_fn, copy)

template <typename I, typename O>
using copy_n_result = copy_result<I, O>;

namespace detail {

struct copy_n_fn
{
    template <input_iterator I, weakly_incrementable O> requires indirectly_copyable<I, O>
    constexpr copy_n_result<I, O> operator()(I first, iter_difference_t<I> n, O result) const
    {
        for (iter_difference_t<I> i{}; i < n; i++) {
            *result = *first;
            ++first;
            ++result;
        }

        return {std::move(first), std::move(result)};
    }
};

} // namespace detail

__DEF_NIEBLOID(detail::copy_n_fn, copy_n)

template <typename I, typename O>
using copy_if_result = copy_result<I, O>;

namespace detail {

struct copy_if_fn {
private:
    template <typename I, typename S, typename O, typename Pred, typename Proj>
    static constexpr copy_if_result<I, O> impl(I first, S last, O result, Pred pred, Proj proj)
    {
        while (first != last) {
            if (invoke(pred, invoke(proj, *first))) {
                *result = *first;
                ++result;
            }
            ++first;
        }

        return {std::move(first), std::move(result)};
    }

public:
    template <input_iterator I, sentinel_for<I> S, weakly_incrementable O, typename Proj = identity, typename Pred>
    requires indirect_unary_predicate<Pred, projected<I, Proj>> && indirectly_copyable<I, O>
    constexpr copy_if_result<I, O> operator()(I first, S last, O result, Pred pred, Proj proj = Proj{}) const
    {
        return copy_if_fn::impl(std::move(first), std::move(last),
                                std::move(result), std::move(pred),
                                std::move(proj));
    }

    template <input_range Rng, weakly_incrementable O, typename Proj = identity, typename Pred>
    requires indirect_unary_predicate<Pred, projected<iterator_t<Rng>, Proj>>
    constexpr copy_if_result<safe_iterator_t<Rng>, O>
    operator()(Rng&& rng, O result, Pred pred, Proj proj = Proj{}) const
    {
        return copy_if_fn::impl(begin(rng), end(rng),
                                std::move(result), std::move(pred),
                                std::move(proj));
    }
};

} // namespace detail

__DEF_NIEBLOID(detail::copy_if_fn, copy_if)

template <typename I, typename O>
using copy_backward_result = copy_result<I, O>;

namespace detail {

struct copy_backward_fn
{
  public:
    template <bidirectional_iterator I1, sentinel_for<I1> S1, bidirectional_iterator I2>
    requires indirectly_copyable<I1, I2>
    constexpr copy_backward_result<I1, I2> operator()(I1 first, S1 last, I2 result) const
    {
        return copy_backward_fn::impl(std::move(first), std::move(last),
                                      std::move(result));
    }

    template <bidirectional_range Rng, bidirectional_iterator I> requires indirectly_copyable<iterator_t<Rng>, I>
    constexpr copy_backward_result<safe_iterator_t<Rng>, I>
    operator()(Rng&& rng, I result) const { return copy_backward_fn::impl(begin(rng), end(rng), std::move(result)); }

  private:
    template <typename I1, typename S1, typename I2>
    static constexpr copy_backward_result<I1, I2>
    impl(I1 first, S1 last, I2 result)
    {
        I1 last_it = next(first, std::move(last));
        I1 it = last_it;

        while (it != first) {
            *--result = *--it;
        }

        return {std::move(last_it), std::move(result)};
    }
};

} // namespace detail

__DEF_NIEBLOID(detail::copy_backward_fn, copy_backward)

} // namespace std::ranges
