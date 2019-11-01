//
// Created by Martin Miralles-Cordal on 10/5/19.
//

#pragma once

#include <Defines.hpp>

namespace _ns_LIBSYS {

struct Void
{
    constexpr bool operator==(const Void&) const { return true; }
    constexpr bool operator<=(const Void&) const { return true; }
    constexpr bool operator>=(const Void&) const { return true; }
    constexpr bool operator<(const Void&)  const { return false; }
    constexpr bool operator!=(const Void&) const { return false; }
    constexpr bool operator>(const Void&)  const { return false; }
};

[[maybe_unused]] constexpr const Void Nothing;

template <typename T> struct nonvoid { using type = T; };
template <> struct nonvoid<void> { using type = Void; };

/**
 * Gives back the given type T unless "T == void" whereby it resolves to Void.
 *
 * This is useful when you would otherwise have to specialize a template because
 * you cannot declare instances of or otherwise use void like any other type.
 */
template <typename T> using regular_t = typename nonvoid<T>::type;

} // libsys namespace
