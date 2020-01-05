//
// Created by Martin Miralles-Cordal on 01/05/2020.
//

#pragma once

#include <io/Print.hpp>
#include <io/StringStream.hpp>

namespace sys {

template <typename ...Ts> String format(char const *format, Ts &&...args)
{
    StringStream s;
    print(s, format, std::forward<Ts>(args)...);
    return s.string();
}

} // namespace sys
