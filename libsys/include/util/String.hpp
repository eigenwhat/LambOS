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

inline int clamp_base(unsigned int base) { return static_cast<int>(std::clamp(base, 2u, 32u)); }

} // namespace string_impl

inline char * to_cstr(int num, char *buf, unsigned int base = 10u)
{ return itoa(num, buf, string_impl::clamp_base(base)); }
inline char * to_cstr(long num, char *buf, unsigned int base = 10u)
{ return ltoa(num, buf, string_impl::clamp_base(base)); }
inline char * to_cstr(long long num, char *buf, unsigned int base = 10u)
{ return lltoa(num, buf, string_impl::clamp_base(base)); }
inline char * to_cstr(unsigned int num, char *buf, unsigned int base = 10u)
{ return uitoa(num, buf, string_impl::clamp_base(base)); }
inline char * to_cstr(unsigned long num, char *buf, unsigned int base = 10u)
{ return ultoa(num, buf, string_impl::clamp_base(base)); }
inline char * to_cstr(unsigned long long num, char *buf, unsigned int base = 10u)
{ return ulltoa(num, buf, string_impl::clamp_base(base)); }

template <typename T> char * to_cstr(T *ptr, char *buf) { return to_cstr(reinterpret_cast<std::uintptr_t>(ptr), buf, 16); }

template <std::integral Int> String to_string(Int num, unsigned int base)
{
    using string_impl::append_string;
    base = std::clamp(base, 2u, 32u);

    if (base == 2) {
        String s(2 + sizeof(Int) * 8u);
        s.append("0b");
        append_string(s, num, base);
        return s;
    }
    else if (base == 8) {
        String s(2 + sizeof(Int) * 3u);
        s.append("0o");
        append_string(s, num, base);
        return s;
    }
    else if (base == 10) {
        String s(sizeof(Int) * 3u);
        append_string(s, num, base);
        return s;
    }
    else if (base == 16) {
        String s(2 + (sizeof(Int) * 2u));
        s.append("0x");
        append_string(s, num, base);
        return s;
    }
    else {
        String s(sizeof(Int) * 8u);
        s.append('b');
        append_string(s, base, 10);
        s.append('_');
        append_string(s, num, base);
        return s;
    }
}

template <std::integral Int> String to_string(Int num) { return to_string(num, 10); }
template <typename T> String to_string(T *ptr) { return to_string(reinterpret_cast<std::uintptr_t>(ptr), 16); }

} // libsys namespace
