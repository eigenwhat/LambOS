//
// Created by Martin Miralles-Cordal on 05/23/2026.
//

#pragma once

#include <type_traits>


namespace std {

template <typename T, typename... Args, typename = decltype(::new(std::declval<void*>()) T(std::declval<Args>()...))>
[[gnu::always_inline]] 
constexpr T* construct_at(T* location, Args&&... _Args)
{
    return ::new (static_cast<void*>(location)) T(std::forward<Args>(_Args)...);
}

} // namespace std
