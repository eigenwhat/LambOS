//
// Created by Martin Miralles-Cordal on 01/02/2020.
//

#pragma once

#include <io/OutputStream.hpp>
#include <io/StandardOutput.hpp>
#include <util/Compare.hpp>
#include <util/String.hpp>
#include <util/TypeTraits.hpp>

#include <concepts>
#include <cstdlib>
#include <tuple>

namespace sys {

template <typename... Ts>
void debug_print(char const *format, Ts&&...args);
template <typename... Ts>
void debug_println(char const *format, Ts&&...args);

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
    else if constexpr (std::integral<std::decay_t<T>> || pointer<std::decay_t<T>>) {
        char buf[64];
        char const *str = to_cstr(data, buf);
        print(out, str);
    }
    else {
        static_assert(!std::is_same_v<T, T>);
    }
}

template <typename H, typename ...Ts>
void print(OutputStream &out, char const *format, H &&first, Ts &&...rest)
{
    static constexpr auto has_format_spec = [](char const *s) -> bool { return s[0] == '%' && s[1] == Any<'@','x','p'>; };

    auto const inputs = std::make_tuple(std::forward<H>(first), std::forward<Ts>(rest)...);
    size_t currentInput = 0;

    while (format[0] != '\0') {
        if (!(has_format_spec(format))) {
            out.write(*format++);
        }
        else {
            char const formatSpecifier = format[1];
            std::advance(format, 2);
            template_for<0, type_count<H, Ts...>>([&](auto idx_constant) {
                constexpr auto index = decltype(idx_constant)::value;
                using type = nth_type_t<index, H, Ts...>;
                if (currentInput == index) {
                    if constexpr (std::integral<type> || pointer<type>) {
                        if (formatSpecifier == 'x' || formatSpecifier == 'p') {
                            print(out, reinterpret_cast<void const *>(std::get<index>(inputs)));
                            return;
                        }
                    }

                    print(out, std::get<index>(inputs));
                }
            });
            ++currentInput;
        }
    }
}

template <typename ...Ts>
void println(OutputStream &out, char const *format, Ts &&...rest)
{
    print(out, format, std::forward<Ts>(rest)...);
    out.write('\n');
}

template <typename T> void print(T &&data) { print(StandardOutput::Get(), std::forward<T>(data)); }

template <typename ...Ts>
void print(char const *format, Ts &&...rest) { print(StandardOutput::Get(), format, std::forward<Ts>(rest)...); }

template <typename ...Ts>
void println(char const *format, Ts &&...rest) { println(StandardOutput::Get(), format, std::forward<Ts>(rest)...); }

} // libsys namespace
