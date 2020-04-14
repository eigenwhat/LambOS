//
// Created by Martin Miralles-Cordal on 11/1/2019.
//

#pragma once

#include <cstddef>
#include <cstdint>

namespace sys {

enum class Arch { x86, x86_64, Unknown };

#ifdef __amd64__
constexpr inline bool is_amd64 = true;
#else
constexpr inline bool is_amd64 = false;
#endif

#ifdef __i386__
constexpr inline bool is_i386 = true;
#else
constexpr inline bool is_i386 = false;
#endif

constexpr inline Arch kSystemArchitecture = is_i386  ? Arch::x86
                                          : is_amd64 ? Arch::x86_64
                                          : Arch::Unknown;

inline namespace literals {

[[nodiscard]]
consteval std::size_t
operator""_sz (unsigned long long int x) { return static_cast<std::size_t>(x); }

} // namespace literals

} // namespace sys
