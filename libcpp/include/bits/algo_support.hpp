//
// Created by Martin Miralles-Cordal on 12/22/2019.
//

#pragma once

#define __DEF_NIEBLOID(type, name)      \
    inline namespace function_objects { \
    inline constexpr type name{};       \
    }

namespace std {

template <std::size_t I> struct __priority_tag : __priority_tag<I - 1> {};
template <> struct __priority_tag<0> {};

}
