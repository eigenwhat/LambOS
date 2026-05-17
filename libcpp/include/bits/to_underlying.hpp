//
// Created by Martin Miralles-Cordal on 5/16/2026.
//

#pragma once

#include <type_traits>

namespace std {

template <class Enum>
constexpr underlying_type_t<Enum> to_underlying(Enum __e) noexcept
{
    return static_cast<underlying_type_t<Enum>>(__e);
}

} // namespace std
