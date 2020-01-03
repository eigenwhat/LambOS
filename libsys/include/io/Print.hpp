//
// Created by Martin Miralles-Cordal on 01/02/2020.
//

#pragma once

#include <io/OutputStream.hpp>
#include <util/String.hpp>
#include <util/TypeTraits.hpp>

#include <concepts>
#include <cstdlib>
#include <tuple>

namespace sys {

template <typename T>
void print(OutputStream &out, T &&data)
{
    if constexpr (is_any_of_v<std::decay_t<T>, char, unsigned char, signed char, std::byte>) {
        out.write(data);
    }
    else if constexpr (std::convertible_to<std::decay_t<T>, char const *>) {
        char const *it = data;
        while (char currLetter = *it++) { print(out, currLetter); }
    }
    else if constexpr (std::same_as<std::decay_t<T>, String>) {
        print(out, data.cstr());
    }
    else if constexpr (std::same_as<std::decay_t<T>, bool>) {
        print(out, data ? "true" : "false");
    }
    else if constexpr (std::integral<std::decay_t<T>> || std::convertible_to<std::decay_t<T>, void *>) {
        print(out, to_string(data));
    }
    else {
        static_assert(!std::is_same_v<T, T>);
    }
}

template <typename H, typename ...Ts>
void print(OutputStream &out, char const *format, H &&first, Ts &&...rest)
{
    auto const inputs = std::make_tuple(std::forward<H>(first), std::forward<Ts>(rest)...);
    size_t currentInput = 0;

    while (format[0] != '\0') {
        if (!(format[0] == '%' && format[1] == '@')) {
            out.write(*format++);
        }
        else {
            format += 2;
            template_for<0, type_count<H, Ts...>>([&](auto idx_constant) {
                constexpr auto index = decltype(idx_constant)::value;
                if (currentInput == index) {
                    print(out, std::get<index>(inputs));
                }
            });
            ++currentInput;
        }
    }
}

template <typename H, typename ...Ts>
void println(OutputStream &out, char const *format, H &&first, Ts &&...rest)
{
    print(out, format, std::forward<H>(first), std::forward<Ts>(rest)...);
    out.write('\n');
}

} // libsys namespace
