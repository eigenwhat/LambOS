//
// Created by Martin Miralles-Cordal on 05/23/2026.
//

#pragma once

#include <type_traits>

namespace std {

template <typename T>
constexpr remove_reference_t<T>&& move(T&& a) { return static_cast<remove_reference_t<T>&&>(a); }

template<typename T>
constexpr T && forward(remove_reference_t<T>& t) noexcept { return static_cast<T &&>(t); }

template<typename T>
constexpr T && forward(remove_reference_t<T>&& t) noexcept { return static_cast<T &&>(t); }

} // namespace std
