//
// Created by Martin Miralles-Cordal on 11/01/2019.
//

#pragma once

#include <Defines.hpp>

#include <concepts>
#include <cstring>

namespace sys {

namespace compare_detail {

template <auto... Options> struct all_of_impl {};
template <typename T, auto... Options>
constexpr bool operator==(T const &lhs, all_of_impl<Options...>) { return ((lhs == Options) && ...); }
template <typename T, auto... Options>
constexpr bool operator!=(T const &lhs, all_of_impl<Options...> rhs) { return !(lhs == rhs); }
template <typename T, auto... Options>
constexpr bool operator<=(T const &lhs, all_of_impl<Options...>) { return ((lhs <= Options) && ...); }
template <typename T, auto... Options>
constexpr bool operator>=(T const &lhs, all_of_impl<Options...>) { return ((lhs >= Options) && ...); }
template <typename T, auto... Options>
constexpr bool operator< (T const &lhs, all_of_impl<Options...>) { return ((lhs < Options) && ...); }
template <typename T, auto... Options>
constexpr bool operator> (T const &lhs, all_of_impl<Options...>) { return ((lhs > Options) && ...); }

template <auto... Options> struct any_of_impl {};
template <typename T, auto... Options>
constexpr bool operator==(T const &lhs, any_of_impl<Options...>) { return ((lhs == Options) || ...); }
template <typename T, auto... Options>
constexpr bool operator!=(T const &lhs, any_of_impl<Options...> rhs) { return !(lhs == rhs); }
template <typename T, auto... Options>
constexpr bool operator<=(T const &lhs, any_of_impl<Options...>) { return ((lhs <= Options) || ...); }
template <typename T, auto... Options>
constexpr bool operator>=(T const &lhs, any_of_impl<Options...>) { return ((lhs >= Options) || ...); }
template <typename T, auto... Options>
constexpr bool operator<(T const &lhs, any_of_impl<Options...>) { return ((lhs < Options) || ...); }
template <typename T, auto... Options>
constexpr bool operator>(T const &lhs, any_of_impl<Options...>) { return ((lhs > Options) || ...); }

} // namespace compare_detail

template <auto... Options> constexpr inline auto Any = compare_detail::any_of_impl<Options...>{};
template <auto... Options> constexpr inline auto All = compare_detail::all_of_impl<Options...>{};

static_assert(1 == Any<1, 2, 3>);
static_assert(1 != Any<2, 3, 4>);
static_assert((1 != Any<1, 2, 3>) == false);
static_assert(1 < Any<0, 1, 2, 3>);
static_assert(1 > Any<0, 1, 2, 3>);
static_assert(1 < Any<1, 2, 3>);
static_assert((1 > Any<1, 2, 3>) == false);
static_assert((1 < Any<0, 1>) == false);
static_assert(1 > Any<0, 1>);
static_assert(1 <= Any<0, 1, 2, 3>);
static_assert(1 >= Any<0, 1, 2, 3>);
static_assert((1 <= Any<-1, 0>) == false);
static_assert(1 >= Any<-1, 0>);
static_assert(1 <= Any<2, 3>);
static_assert((1 >= Any<2, 3>) == false);

static_assert(1 == All<1, 1u, 1ul, 1ull>);
static_assert((1 != All<1, 1u, 1ul, 1ull>) == false);
static_assert(1 != All<1, 2, 3, 4>);
static_assert(1 != All<2, 3, 4>);
static_assert(1 < All<2, 3, 4>);
static_assert(1 <= All<2, 3, 4>);
static_assert((1 >= All<2, 3, 4>) == false);
static_assert((1 > All<2, 3, 4>) == false);
static_assert((1 < All<-2, 3, 4>) == false);
static_assert((1 <= All<-2, 3, 4>) == false);
static_assert((1 >= All<-2, 3, 4>) == false);
static_assert((1 > All<-2, 3, 4>) == false);

} // libsys namespace
