//
// Created by Martin Miralles-Cordal on 11/1/2019.
//

#pragma once

#include <cstddef>
#include <cstdint>

namespace sys {

inline namespace literals {

[[nodiscard]]
consteval std::size_t
operator""_sz (unsigned long long int x) { return static_cast<std::size_t>(x); }

} // namespace literals

} // namespace sys
