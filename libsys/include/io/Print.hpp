//
// Created by Martin Miralles-Cordal on 01/02/2020.
//

#pragma once

#include <io/OutputStream.hpp>
#include <util/TypeTraits.hpp>

#include <concepts>
#include <cstdlib>
#include <tuple>

namespace sys {

namespace printstream_detail {
inline void xtoa(bool num, char *intstr, int base)               { itoa(num, intstr, base); }
inline void xtoa(std::int8_t num, char *intstr, int base)        { itoa(num, intstr, base); }
inline void xtoa(std::int16_t num, char *intstr, int base)       { itoa(num, intstr, base); }
inline void xtoa(int num, char *intstr, int base)                { itoa(num, intstr, base); }
inline void xtoa(long int num, char *intstr, int base)           { ltoa(num, intstr, base); }
inline void xtoa(long long int num, char *intstr, int base)      { lltoa(num, intstr, base); }
inline void xtoa(std::uint8_t num, char *intstr, int base)       { itoa(num, intstr, base); }
inline void xtoa(std::uint16_t num, char *intstr, int base)      { itoa(num, intstr, base); }
inline void xtoa(unsigned int num, char *intstr, int base)       { uitoa(num, intstr, base); }
inline void xtoa(unsigned long num, char *intstr, int base)      { ultoa(num, intstr, base); }
inline void xtoa(unsigned long long num, char *intstr, int base) { ulltoa(num, intstr, base); }
}


template <typename T>
void print(OutputStream &out, T &&data)
{
    if constexpr (is_any_of_v<std::decay_t<T>, char, std::byte>) {
        out.write(data);
    }
    else if constexpr (std::convertible_to<std::decay_t<T>, char const *>) {
        char const *it = data;
        while (char currLetter = *it++) { print(out, currLetter); }
    }
    else if constexpr (std::same_as<std::decay_t<T>, bool>) {
        print(out, data ? "true" : "false");
    }
    else if constexpr (std::integral<std::decay_t<T>>) {
        char intstr[33];
        printstream_detail::xtoa(data, intstr, 10);
        print(out, intstr);
    }
    else if constexpr (std::convertible_to<std::decay_t<T>, void *>) {
        char intstr[33];
        printstream_detail::xtoa(reinterpret_cast<std::uintptr_t>(data), intstr, 16);
        print(out, intstr);
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

} // libsys namespace
