//
// Created by Martin Miralles-Cordal on 01/01/2020.
//

#pragma once

#include <concepts>

namespace sys {

template <typename T> concept Arithmetic = std::is_arithmetic_v<T>;


template <typename T> constexpr inline T centi(Arithmetic auto v) { return T(v) / T(100); }
template <typename T> constexpr inline T deci (Arithmetic auto v) { return T(v) / T(10); }
template <typename T> constexpr inline T deka (Arithmetic auto v) { return T(v * 10); }
template <typename T> constexpr inline T hecto(Arithmetic auto v) { return T(v * 100); }
template <typename T> constexpr inline T kilo (Arithmetic auto v) { return T(v * 1000); }
template <typename T> constexpr inline T mega (Arithmetic auto v) { return kilo<T>(v * 1000); }
template <typename T> constexpr inline T giga (Arithmetic auto v) { return mega<T>(v * 1000); }
template <typename T> constexpr inline T tera (Arithmetic auto v) { return giga<T>(v * 1000); }
template <typename T> constexpr inline T peta (Arithmetic auto v) { return tera<T>(v * 1000); }
template <typename T> constexpr inline T exa  (Arithmetic auto v) { return peta<T>(v * 1000); }
template <typename T> constexpr inline T zetta(Arithmetic auto v) { return exa<T>(v * 1000); }
template <typename T> constexpr inline T yotta(Arithmetic auto v) { return zetta<T>(v * 1000); }

template <typename T> constexpr inline T kibi(Arithmetic auto v) { return T(v * 1024); }
template <typename T> constexpr inline T mebi(Arithmetic auto v) { return kibi<T>(v) * 1024; }
template <typename T> constexpr inline T gibi(Arithmetic auto v) { return mebi<T>(v) * 1024; }
template <typename T> constexpr inline T tebi(Arithmetic auto v) { return gibi<T>(v) * 1024; }
template <typename T> constexpr inline T pebi(Arithmetic auto v) { return tebi<T>(v) * 1024; }
template <typename T> constexpr inline T exbi(Arithmetic auto v) { return pebi<T>(v) * 1024; }
template <typename T> constexpr inline T zebi(Arithmetic auto v) { return exbi<T>(v) * 1024; }
template <typename T> constexpr inline T yobi(Arithmetic auto v) { return zebi<T>(v) * 1024; }

} // namespace sys
