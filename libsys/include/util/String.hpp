//
// Created by Martin Miralles-Cordal on 5/3/2018.
//

#pragma once

#include <util/BasicString.hpp>

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

template <std::integral Int> String to_string(Int num)
{
    String s(sizeof(Int) * 8u);
    xtoa(num, s.cstr(), 10);
    return s;
}

} // libsys namespace
