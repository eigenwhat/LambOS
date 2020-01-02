//
// Created by Martin Miralles-Cordal on 12/07/2019.
// Copyright (c) 2019 Martin Miralles-Cordal. All rights reserved.
//

#pragma once

#include <util/meta/Base.hpp>

#include <algorithm>
#include <initializer_list>
#include <tuple>
#include <type_traits>
#include <utility>

namespace sys {

using size = std::make_signed_t<std::size_t>;
template<size Value> using ct_size = std::integral_constant<size, Value>;

/**
 * Performs a compile-time for loop (i.e. a loop unrolling) over the given interval.
 *
 * This performs the compile-time unrolled equivalent of:
 *
 * @code
 * for (auto i = Start; i < End; ++i)
 * {
 *   f(i);
 * }
 * @endcode
 *
 * @tparam Start The starting index.
 * @tparam End The end index (exclusive).
 * @tparam F The function type.
 * @param f The function which will serve as the loop body.
 *          Takes the current index (as integral_constant) as a parameter.
 */
template <size Start, size End, class F>
[[gnu::always_inline]] inline constexpr void template_for(F&& f);

/**
 * @brief Allows nested constexpr for loops including symmetrizing over loops.
 *
 * @note All upper bounds are exclusive (end check is `i < upperBound`, not `i <= upperBound`)
 *
 * The `loop_bounds` class is used for specifying the lower and upper bounds for a single loop.
 * Symmetrizing over loops is supported via the `inner_loop_bounds_low` and `inner_loop_bounds_high` classes.
 *
 * @see loop_bounds inner_loop_bounds_low inner_loop_bounds_high
 */
template <class Bounds0, class Bounds1, class... Bounds, class F>
[[gnu::always_inline]] inline constexpr void template_for(F&& f);

/**
 * Performs a compile-time for loop (i.e. a loop unrolling) over the given interval.
 *
 * This performs the compile-time unrolled equivalent of:
 *
 * @code
 * for (auto i = Bounds::kBegin; i < Bounds::kEnd; ++i)
 * {
 *   f();
 * }
 * @endcode
 *
 * @tparam Bounds The loop bounds.
 * @tparam F The function type.
 * @param f The function which will serve as the loop body.
 *          Takes the current index (as integral_constant) as a parameter.
 */
template <class Bounds, class F>
[[gnu::always_inline]] inline constexpr void template_for(F&& f);

/**
 * @brief Specifies the bounds [Lower, Upper) in a template_for loop.
 *
 * @see template_for inner_loop_bounds_low inner_loop_bounds_high
 *
 * @tparam Lower The lower bound. (inclusive)
 * @tparam Upper The upper bound. (exclusive)
 */
template <size Lower, size Upper>
struct loop_bounds {
  static constexpr const size kLower = Lower;
  static constexpr const size kUpper = Upper;
  static constexpr const size kSize = kUpper - kLower;
};

/**
 * @brief The loop bounds for an inner loop which goes from `Lower` to `I + Offset`.
 *
 * This is equivalent to loops of the form:
 *
 * @code
 * for (size i = 0; i < outerLoopEnd; ++i)
 * {
 *   for (size j = lower; j < i + offset; ++j)
 *   {
 *     // ...
 *   }
 * }
 * @endcode
 *
 * @see template_for loop_bounds inner_loop_bounds_high
 *
 * @tparam I The current index of the outer loop to symmetrize over.
 * @tparam Lower The lower bound of the loop.
 * @tparam Offset [optional] The offset to apply to I for setting the upper bound of the loop. (Default = 0)
 */
template <size I, size Lower, size Offset = 0>
struct inner_loop_bounds_low {};

/**
 * @brief The loop bounds for an inner loop which goes from `I` to `Upper`.
 *
 * This is equivalent to loops of the form:
 *
 * @code
 * for (size i = 0; i < outerLoopEnd; ++i)
 * {
 *   for (size j = i; j < upper; ++j)
 *   {
 *     // ...
 *   }
 * }
 * @endcode
 *
 * @see template_for loop_bounds inner_loop_bounds_low
 *
 * @tparam I The current index of the outer loop being symmetrized over.
 * @tparam Upper The upper bound of the loop.
 */
template <size I, size Upper>
struct inner_loop_bounds_high {};

namespace for_constexpr_detail {

template<size N> using make_indexes = std::make_integer_sequence<size, std::max(size(1), N)>;
template<size... Ns> using indexes = std::integer_sequence<size, Ns...>;

// Base case
template <size Lower, size... Is, class F, class... Constants>
[[gnu::always_inline]] inline constexpr void impl(F&& f, indexes<Is...>, Constants&&... v)
{
  (void)std::initializer_list<int>{((void)f(v..., ct_size<Is + Lower>{}), 0)...};
}

// Cases of second last loop
template <size Lower, size NextLower, size NextUpper, size... Is, typename F, typename...Constants>
[[gnu::always_inline]] inline constexpr void
impl(F&& f, loop_bounds<NextLower, NextUpper>, indexes<Is...>, Constants&&... v)
{
  static_assert(NextUpper - NextLower >= 0, "template_for does not support bounds where Start > End.");
  (void)std::initializer_list<int>{
      ((void)impl<NextLower>(std::forward<F>(f), make_indexes<NextUpper - NextLower>{}, v..., ct_size<Is + Lower>{}),
       0)...};
}

template <size Lower, size NextIndex, size NextUpper, size... Is, class F, class... Constants>
[[gnu::always_inline]] inline constexpr void impl(F&& f, inner_loop_bounds_high<NextIndex, NextUpper>, indexes<Is...>, Constants&&... v)
{
  #define LIBSYS_NEXT_LOWER std::get<NextIndex>(std::make_tuple(Constants::value..., Is + Lower))
  #define LIBSYS_BOUNDS_SIZE NextUpper - LIBSYS_NEXT_LOWER

  static_assert(((LIBSYS_BOUNDS_SIZE >= 0) && ...), "template_for does not support bounds where Start > End.");
  (void)std::initializer_list<int>{
      ((void)impl<LIBSYS_NEXT_LOWER>(std::forward<F>(f), make_indexes<LIBSYS_BOUNDS_SIZE>{}, v..., ct_size<Is + Lower>{}),
      0)...};

  #undef LIBSYS_BOUNDS_SIZE
  #undef LIBSYS_NEXT_LOWER
}

template <size Lower, size NextIndex, size NextLower, size NextOffset, size... Is, class F, class... Constants>
[[gnu::always_inline]] inline constexpr void
impl(F&& f, inner_loop_bounds_low<NextIndex, NextLower, NextOffset>, indexes<Is...>, Constants&&... v)
{
  #define LIBSYS_BOUNDS_SIZE std::get<NextIndex>(std::make_tuple(Constants::value..., Is + Lower)) + NextOffset - NextLower

  static_assert(((LIBSYS_BOUNDS_SIZE >= 0) && ...), "template_for does not support bounds where Start > End.");

  (void)std::initializer_list<int>{
      ((void)impl<NextLower>(std::forward<F>(f), make_indexes<LIBSYS_BOUNDS_SIZE>{}, v..., ct_size<Is + Lower>{}), 0)...};

  #undef LIBSYS_BOUNDS_SIZE
}

// Handle cases of more than two nested loops
template <size Lower, class Bounds1, class... Bounds, size NextLower,
          size NextUpper, size... Is, class F, class... Constants>
[[gnu::always_inline]] inline constexpr void impl(F&& f, loop_bounds<NextLower, NextUpper>, indexes<Is...>, Constants&&... v)
{
  static_assert(NextUpper - NextLower >= 0, "template_for does not support bounds where Start > End.");

  (void)std::initializer_list<int>{
      ((void)impl<NextLower, Bounds...>(std::forward<F>(f), Bounds1{}, make_indexes<NextUpper - NextLower>{}, v...,
                                        ct_size<Is + Lower>{}), 0)...};
}

template <size Lower, class Bounds1, class... Bounds, size NextIndex,
          size NextUpper, size... Is, class F, class... Constants>
[[gnu::always_inline]] inline constexpr void impl(F&& f, inner_loop_bounds_high<NextIndex, NextUpper>, indexes<Is...>, Constants&&... v)
{
  #define LIBSYS_NEXT_LOWER std::get<NextIndex>(std::make_tuple(Constants::value..., Is + Lower))
  #define LIBSYS_BOUNDS_SIZE NextUpper - LIBSYS_NEXT_LOWER

  static_assert(((LIBSYS_BOUNDS_SIZE >= 0) && ...), "template_for does not support bounds where Start > End.");

  (void)std::initializer_list<int>{
      ((void)impl<LIBSYS_NEXT_LOWER>(std::forward<F>(f), Bounds1{}, make_indexes<LIBSYS_BOUNDS_SIZE>{}, v...,
                                  ct_size<Is + Lower>{}), 0)...};

  #undef LIBSYS_BOUNDS_SIZE
  #undef LIBSYS_NEXT_LOWER
}

template <size Lower, class Bounds1, class... Bounds, size NextIndex, size NextLower,
          size NextOffset, size... Is, class F, class... Constants>
[[gnu::always_inline]] inline constexpr void
impl(F&& f, inner_loop_bounds_low<NextIndex, NextLower, NextOffset>, indexes<Is...>, Constants&&... v)
{
  #define LIBSYS_BOUNDS_SIZE std::get<NextIndex>(std::make_tuple(Constants::value..., Is + Lower)) + NextOffset - NextLower

  static_assert(((LIBSYS_BOUNDS_SIZE >= 0) && ...), "template_for does not support bounds where Start > End.");

  (void)std::initializer_list<int>{((void)impl<NextLower>(std::forward<F>(f), Bounds1{}, make_indexes<LIBSYS_BOUNDS_SIZE>{},
                                                          v..., ct_size<Is + Lower>{}), 0)...};

  #undef LIBSYS_BOUNDS_SIZE
}

} // namespace for_constexpr_detail

// single loop
template <size Start, size End, class F>
[[gnu::always_inline]] inline constexpr void template_for(F&& f) { template_for<loop_bounds<Start, End>>(f); }

// base case
template <class Bounds, class F>
[[gnu::always_inline]] inline constexpr void template_for(F&& f)
{
  static_assert(Bounds::kSize >= 0, "template_for does not support bounds where Start > End.");
  for_constexpr_detail::impl<Bounds::kLower>(std::forward<F>(f), for_constexpr_detail::make_indexes<Bounds::kSize>{});
}

// 2+ loops
template <class Bounds0, class Bounds1, class... Bounds, class F>
[[gnu::always_inline]] inline constexpr void template_for(F&& f)
{
  static_assert(Bounds0::kSize >= 0, "template_for does not support bounds where Start > End.");
  for_constexpr_detail::impl<Bounds0::kLower, Bounds...>(std::forward<F>(f), Bounds1{},
                                                         for_constexpr_detail::make_indexes<Bounds0::kSize>{});
}

} // namespace sys
