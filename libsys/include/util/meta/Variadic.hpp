//
// Created by Martin Miralles-Cordal on 12/07/2019.
// Copyright (c) 2019 Martin Miralles-Cordal. All rights reserved.
//

#pragma once

#include <util/meta/Base.hpp>

namespace sys {

//
// Parameter packs and tuples
//

/** nth type in a template parameter pack. */
template <std::size_t N, typename... Ts> struct nth_type;
template <typename T, typename... Ts> struct nth_type<0, T, Ts...> : std::type_identity<T> {};
template <std::size_t N, typename T, typename... Ts> struct nth_type<N, T, Ts...> : nth_type<N-1, Ts...> {};

template <std::size_t N, typename... Types> using nth_type_t = typename nth_type<N, Types...>::type;

/** Number of types in a template parameter pack. */
template <typename...> constexpr inline std::size_t const type_count = 0;
template <typename T>  constexpr inline std::size_t const type_count<T> = 1;
template <typename T, typename... Ts> constexpr inline std::size_t const type_count<T, Ts...> = 1 + type_count<Ts...>;


template <typename... Ts> constexpr inline std::size_t pack_index_npos = type_count<Ts...>;

/** Index of a type within a template parameter pack. */
template <typename, typename...> constexpr inline std::size_t pack_index = 0;
template <typename T, typename...Rest> constexpr inline std::size_t const pack_index<T, T, Rest...> = 0;
template <typename T, typename T1, typename... Ts>
constexpr inline std::size_t const pack_index<T, T1, Ts...> = 1 + pack_index<T, Ts...>;

} // namespace sys
