//
// Created by Martin Miralles-Cordal on 01/01/2020.
//

#pragma once

#include <math/Prefix.hpp>

#include <cstddef>

namespace sys {
inline namespace mem_unit_literals {

constexpr inline std::size_t operator""_KiB(unsigned long long v) { return kibi<std::size_t>(v); }
constexpr inline std::size_t operator""_KiB(long double v) { return kibi<std::size_t>(v); }
constexpr inline std::size_t operator""_MiB(unsigned long long v) { return mebi<std::size_t>(v); }
constexpr inline std::size_t operator""_MiB(long double v) { return mebi<std::size_t>(v); }
constexpr inline std::size_t operator""_GiB(unsigned long long v) { return gibi<std::size_t>(v); }
constexpr inline std::size_t operator""_GiB(long double v) { return gibi<std::size_t>(v); }
constexpr inline std::size_t operator""_TiB(unsigned long long v) { return tebi<std::size_t>(v); }
constexpr inline std::size_t operator""_TiB(long double v) { return tebi<std::size_t>(v); }

} // namespace mem_unit_literals
} // namespace sys
