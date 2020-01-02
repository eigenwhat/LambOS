//
// Created by Martin Miralles-Cordal on 01/02/2020.
//

#pragma once

#include <concepts>

namespace sys {

template <typename T, typename ...Ts> using all_same   = std::conjunction<std::is_same<T, Ts>...>;
template <typename T, typename ...Ts> using is_any_of  = std::disjunction<std::is_same<T, Ts>...>;
template <typename T, typename ...Ts> using is_none_of = std::negation<is_any_of<T, Ts...>>;
template <typename T, typename ...Ts> using any_are    = is_any_of<T, Ts...>;
template <typename T, typename ...Ts> using none_are   = is_none_of<T, Ts...>;

template <typename T, typename ...Ts> constexpr inline bool all_same_v   = all_same<T, Ts...>::value;
template <typename T, typename ...Ts> constexpr inline bool is_any_of_v  = is_any_of<T, Ts...>::value;
template <typename T, typename ...Ts> constexpr inline bool is_none_of_v = is_none_of<T, Ts...>::value;
template <typename T, typename ...Ts> constexpr inline bool any_are_v    = any_are<T, Ts...>::value;
template <typename T, typename ...Ts> constexpr inline bool none_are_v   = none_are<T, Ts...>::value;

template <typename T, typename ...Ts> concept OneOf = (std::same_as<T, Ts> || ...);
template <typename T, typename ...Ts> concept DifferentFrom = (!std::same_as<T, Ts> && ...);

} // namespace sys