#pragma once

#include <type_traits>

template <typename T>
using IsReference = std::enable_if_t<std::is_reference<T>::value>;

template <typename T>
using IsValue = std::enable_if_t<!std::is_reference<T>::value>;

template <typename T>
using IsCopyConstructible = std::enable_if_t<std::is_copy_constructible<T>::value>;

template <typename T>
using IsDefaultConstructible = std::enable_if_t<std::is_default_constructible<T>::value>;

template <typename T>
using IsMoveConstructible = std::enable_if_t<std::is_move_constructible<T>::value>;
