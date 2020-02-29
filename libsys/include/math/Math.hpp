//
// Created by Martin Miralles-Cordal on 02/24/2020.
//

#pragma once

#include <Defines.hpp>
#include <math/Prefix.hpp>

#include <concepts>

namespace sys {

template <std::unsigned_integral I>
constexpr I div_ceil(I dvd, I dvs)
{
    I const quot = dvd / dvs;
    I const rem = dvd % dvs;
    return quot + (rem > 0);
}

template <std::signed_integral I>
constexpr I div_ceil(I dvd, I dvs)
{
    bool const same_sign = (dvd < 0 && dvs < 0) || (dvd > 0 && dvs > 0);
    if (same_sign) {
        using UI = std::make_unsigned_t<I>;
        UI const uquot = div_ceil<UI>(static_cast<UI>(dvd), static_cast<UI>(dvs));
        return static_cast<I>(uquot);
    }
    else {
        return dvd / dvs;
    }
}

} // namespace sys
