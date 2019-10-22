//
// Created by Martin Miralles-Cordal on 10/21/19.
//

#pragma once

#include <bits/traits.hpp>
#include <limits.h>
#include <float.h>

#define _LIBCPP_SIGNED(_Type) ((_Type)(-1) < (_Type)(0))
#define _LIBCPP_DIGITS(_Type) (sizeof(_Type) * CHAR_BIT - _LIBCPP_SIGNED(_Type))

#define _LIBCPP_FLOATING(_Tp, _FltV, _DblV, _LDblV) (std::__are_same_v<_Tp, float>  ? _FltV \
                                                   : std::__are_same_v<_Tp, double> ? _DblV \
                                                                                    : _LDblV)


// You should not need to define any macros below this point.

// The fraction 643/2136 approximates log10(2) to 7 significant digits.
#define _LIBCPP_DIGITS10_B(T,B) (_LIBCPP_DIGITS(T) * 643L / 2136)

#define _LIBCPP_DIGITS10(T) _LIBCPP_DIGITS10_B (T, sizeof(T) * __CHAR_BIT__)

#define _LIBCPP_MAX_DIGITS10(T) (2 + (T) * 643L / 2136)
