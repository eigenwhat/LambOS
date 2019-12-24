//
// Created by Martin Miralles-Cordal on 12/23/2019.
//

#pragma once

#include <iterator>

#include <bits/ranges/comparisons.hpp>
#include <bits/ranges/concepts.hpp>
#include <bits/ranges/identity.hpp>
#include <bits/algo_support.hpp>

namespace std::ranges {

namespace detail {

struct find_if_fn {
  private:
    friend struct find_fn;
    friend struct find_if_not_fn;

    template <typename I, typename S, typename Pred, typename Proj>
    static constexpr I impl(I first, S last, Pred& pred, Proj& proj)
    {
        while (first != last) {
            if (invoke(pred, invoke(proj, *first))) { return first; }
            ++first;
        }
        return first;
    }

  public:
    template <input_iterator I, typename S, typename Proj = identity, typename Pred>
    requires sentinel_for<S, I> && indirect_unary_predicate<Pred, projected<I, Proj>>
    constexpr I operator()(I first, S last, Pred pred, Proj proj = Proj{}) const
    {
        return find_if_fn::impl(std::move(first), std::move(last), pred, proj);
    }

    template <input_range Rng, typename Proj = identity, typename Pred>
    requires indirect_unary_predicate<Pred, projected<iterator_t<Rng>, Proj>>
    constexpr safe_iterator_t<Rng> operator()(Rng&& rng, Pred pred, Proj proj = Proj{}) const
    {
        return find_if_fn::impl(begin(rng), end(rng), pred, proj);
    }
};

} // namespace detail

__DEF_NIEBLOID(detail::find_if_fn, find_if)

namespace detail {

struct find_fn {
    template <input_iterator I, sentinel_for<I> S, typename T, typename Proj = identity>
    requires sentinel_for<S, I> && indirect_relation<ranges::equal_to, projected<I, Proj>, const T*>
    constexpr I operator()(I first, S last, const T& value, Proj proj = Proj{}) const
    {
        const auto pred = [&value] (const auto& t) { return t == value; };
        return find_if_fn::impl(std::move(first), std::move(last), pred, proj);
    }

    template <input_range Rng, typename T, typename Proj = identity>
    requires indirect_relation<ranges::equal_to, projected<iterator_t<Rng>, Proj>, const T*>
    constexpr safe_iterator_t<Rng> operator()(Rng&& rng, const T& value, Proj proj = Proj{}) const
    {
        const auto pred = [&value] (const auto& t) { return t == value; };
        return find_if_fn::impl(begin(rng), end(rng), pred, proj);
    }
};
} // namespace detail

__DEF_NIEBLOID(detail::find_fn, find)

namespace detail {

struct find_if_not_fn {
  private:
    template <typename Pred> struct not_pred
    {
        Pred& p;
        template <typename T> constexpr bool operator()(T&& t) const { return !invoke(p, std::forward<T>(t)); }
    };

  public:
    template <input_iterator I, sentinel_for<I> S, typename Proj = identity, typename Pred>
    requires indirect_unary_predicate<Pred, projected<I, Proj>>
    constexpr I operator()(I first, S last, Pred pred, Proj proj = Proj{}) const
    {
        const auto find_if_pred = not_pred<Pred>{pred};
        return find_if_fn::impl(std::move(first), std::move(last), find_if_pred, proj);
    }

    template <input_range Rng, typename Proj = identity, typename Pred>
    requires indirect_unary_predicate<Pred, projected<iterator_t<Rng>, Proj>>
    constexpr safe_iterator_t<Rng> operator()(Rng&& rng, Pred pred, Proj proj = Proj{}) const
    {
        const auto find_if_pred = not_pred<Pred>{pred};
        return find_if_fn::impl(begin(rng), end(rng), find_if_pred, proj);
    }
};

} // namespace detail

__DEF_NIEBLOID(detail::find_if_not_fn, find_if_not)

} // namespace std::ranges
