//
// Created by Martin Miralles-Cordal on 5/3/2018.
//

#pragma once

#include <util/BasicString.hpp>

namespace _ns_LIBSYS {

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

} // libsys namespace
