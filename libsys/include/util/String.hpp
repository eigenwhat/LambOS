//
// Created by Martin Miralles-Cordal on 5/3/2018.
//

#pragma once

#include <util/BasicString.hpp>

#include <algorithm>
#include <concepts>

namespace sys {

/** A char string. */
using String = BasicString<char>;

template <typename T> struct Hasher<BasicString<T>>
{
    size_t operator()(String const &strObj) const
    {
        size_t hash = 5381;
        int c;
        T const *str = strObj.cstr();
        while ((c = *str++)) {
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
        }

        return hash;
    }
};

namespace string_impl {

template <std::unsigned_integral Int>
void append_string(String &s, Int num, unsigned int base)
{
    base = std::clamp(base, 2u, 32u);

    String revstr(sizeof(Int) * 8u);
    do {
        auto const mod = num % base;
        if(mod < 10) { revstr.append(static_cast<char>('0' + mod)); }
        else { revstr.append(static_cast<char>('A' + mod - 10)); }
        num /= base;
    } while (num != 0);

    s.append(revstr.rbegin(), revstr.rend());
}

template <std::signed_integral Int>
void append_string(String &s, Int num, unsigned int base)
{
    if (base == 10 && num < 0) {
        s.append('-');
        num = -num;
    }

    append_string(s, static_cast<std::make_unsigned_t<Int>>(num), base);
}

} // namespace string_impl

template <std::integral Int> String to_string(Int num, unsigned int base)
{
    String s;
    base = std::clamp(base, 2u, 32u);

    if (base == 2) { s.append("0b"); }
    else if (base == 8) { s.append("0o"); }
    else if (base == 10) {}
    else if (base == 16) { s.append("0x"); }
    else {
        s.append('b');
        string_impl::append_string(s, base, 10);
        s.append('_');
    }

    string_impl::append_string(s, num, base);
    return s;
}

template <std::integral Int> String to_string(Int num) { return to_string(num, 10); }
template <typename T> String to_string(T *ptr) { return to_string(reinterpret_cast<std::uintptr_t>(ptr), 16); }

} // libsys namespace
