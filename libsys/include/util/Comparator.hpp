//
// Created by Martin Miralles-Cordal on 11/01/2019.
//

#pragma once

#include <Defines.hpp>

#include <cstring>

namespace _ns_LIBSYS {

/**
 * Function object for performing equality comparisons. Unless specialised,
 * invokes operator== on type T.
 */
template<typename T, typename U = T>
constexpr auto EqualTo = [](const T &lhs, const U &rhs) { return lhs == rhs; };

template<>
constexpr auto EqualTo<char *> = [](char const *lhs, char const *rhs) { return lhs == rhs || !strcmp(lhs, rhs); };
template<> constexpr auto EqualTo<char const *> = EqualTo<char *>;

} // libsys namespace
