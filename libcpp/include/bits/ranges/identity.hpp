//
// Created by Martin Miralles-Cordal on 12/23/2019.
//

#pragma once

#include <type_traits>
#include <utility>

namespace std::ranges {

struct identity
{
    template <typename T> constexpr T&& operator()(T&& t) const noexcept { return std::forward<T>(t); }
    using is_transparent = std::true_type;
};

} // namespace std::ranges