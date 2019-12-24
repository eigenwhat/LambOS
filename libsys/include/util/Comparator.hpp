//
// Created by Martin Miralles-Cordal on 11/01/2019.
//

#pragma once

#include <Defines.hpp>

#include <concepts>
#include <cstring>

namespace _ns_LIBSYS {

template <typename Cmp, typename T>
concept Comparator = requires (Cmp const &cmp, T const &a, T const &b) {
    cmp.equal(a, b) -> std::boolean;
    cmp.three_way_compare(a, b) ->
};

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
