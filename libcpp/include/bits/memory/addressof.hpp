//
// Created by Martin Miralles-Cordal on 05/23/2026.
//

#pragma once

namespace std {

template<typename T> constexpr T *addressof(T &r) noexcept { return __builtin_addressof(r); }
template<typename T> const T *addressof(const T&&) = delete;

} // namespace std
