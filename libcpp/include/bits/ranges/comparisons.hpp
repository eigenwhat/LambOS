//
// Created by Martin Miralles-Cordal on 12/23/2019.
//

#pragma once

#include <concepts>
#include <functional>

#include <bits/ranges/concepts.hpp>

namespace std::ranges {

// [range.comparisons]

struct equal_to {
    template <typename T, typename U> requires equality_comparable_with<T, U>
    constexpr auto operator()(T&& t, U&& u) const -> bool
    {
        return std::equal_to<>{}(std::forward<T>(t), std::forward<U>(u));
    }

    using is_transparent = std::true_type;
};

struct not_equal_to {
    template <typename T, typename U>
    constexpr auto operator()(T&& t, U&& u) const
            -> std::enable_if_t<equality_comparable_with<T, U>, bool>
    {
        return !ranges::equal_to{}(std::forward<T>(t), std::forward<U>(u));
    }

    using is_transparent = std::true_type;
};

struct less {
    template <typename T, typename U>
    constexpr auto operator()(T&& t, U&& u) const
            -> std::enable_if_t<totally_ordered_with<T, U>, bool>
    {
        return std::less<>{}(std::forward<T>(t), std::forward<U>(u));
    }

    using is_transparent = std::true_type;
};

struct greater {
    template <typename T, typename U>
    constexpr auto operator()(T&& t, U&& u) const
            -> std::enable_if_t<totally_ordered_with<T, U>, bool>
    {
        return ranges::less{}(std::forward<U>(u), std::forward<T>(t));
    }

    using is_transparent = std::true_type;
};

struct greater_equal {
    template <typename T, typename U>
    constexpr auto operator()(T&& t, U&& u) const
            -> std::enable_if_t<totally_ordered_with<T, U>, bool>
    {
        return !ranges::less{}(std::forward<T>(t), std::forward<U>(u));
    }

    using is_transparent = std::true_type;
};

struct less_equal {
    template <typename T, typename U>
    constexpr auto operator()(T&& t, U&& u) const
            -> std::enable_if_t<totally_ordered_with<T, U>, bool>
    {
        return !ranges::less{}(std::forward<U>(u), std::forward<T>(t));
    }

    using is_transparent = std::true_type;
};

} // namespace std::ranges