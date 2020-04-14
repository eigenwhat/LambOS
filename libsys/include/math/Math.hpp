//
// Created by Martin Miralles-Cordal on 02/24/2020.
//

#pragma once

#include <Defines.hpp>
#include <math/Prefix.hpp>

#include <concepts>

namespace sys {

template <std::integral I> constexpr bool is_positive(I value) { return value > I{0}; }

template <std::unsigned_integral U> constexpr bool is_negative(U) { return false; }
template <std::signed_integral S> constexpr bool is_negative(S value) { return value < S{0}; }

template <std::unsigned_integral U> constexpr int sign(U value) { return value > U{0} ? 1 : 0; }
template <std::signed_integral S> constexpr int sign(S value) { return value > S{0} ? 1 : (value < S{0} ? -1 : 0); }

template <std::unsigned_integral U> constexpr U abs(U value) { return value; }
template <std::signed_integral S> constexpr S abs(S value) { return is_negative(value) ? -value : value; }

template <std::unsigned_integral T, std::unsigned_integral B>
constexpr std::common_type_t<T, B> div_ceil(T dvd, B dvs)
{
    using R = std::common_type_t<T, B>;
    R const quot = dvd / dvs;
    R const rem = dvd % dvs;
    return quot + (rem > 0);
}

template <std::integral T, std::integral B> requires std::signed_integral<T> || std::signed_integral<B>
constexpr std::common_type_t<T, B> div_ceil(T dvd, B dvs)
{
    using R = std::common_type_t<T, B>;
    bool const same_sign = (is_negative(dvd) && is_negative(dvs)) || (is_positive(dvd) && is_positive(dvs));
    if (same_sign) {
        R const quot = dvd / dvs;
        R const rem = dvd % dvs;
        return quot + (rem > 0);
    }
    else {
        return dvd / dvs;
    }
}

} // namespace sys
