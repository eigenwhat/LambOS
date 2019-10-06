#pragma once

#include <type_traits>

template <typename T>
using IfReference = std::enable_if_t<std::is_reference<T>::value>;

template <typename T>
using IfValue = std::enable_if_t<!std::is_reference<T>::value>;

template <typename T>
using IfCopyConstructible = std::enable_if_t<std::is_copy_constructible<T>::value>;

template <typename T>
using IfDefaultConstructible = std::enable_if_t<std::is_default_constructible<T>::value>;

template <typename T>
using IfMoveConstructible = std::enable_if_t<std::is_move_constructible<T>::value>;


/**
 * Conditionally adds or removes a const qualifier to or from a type.
 * @tparam Type The type to operate on
 * @tparam Const If true, add const. If false, remove const.
 */
template <typename Type, bool Const = true>
using const_t = typename std::conditional_t<Const, std::add_const_t<Type>, std::remove_const_t<Type>>;

/**
 * Conditionally adds or removes a const qualifier to or from a type, then evaluates to a pointer to that type.
 * @tparam Type The type to operate on.
 * @tparam Const If true, add const. If false, remove const.
 */
template <typename Type, bool Const = true>
using const_ptr_t = std::add_pointer_t<const_t<std::remove_reference_t<Type>, Const>>;

/**
 * Conditionally adds or removes a const qualifier to or from a type, then evaluates to a reference to that type.
 * @tparam Type The type to operate on.
 * @tparam Const If true, add const. If false, remove const.
 */
template <typename Type, bool Const = true>
using const_lvref_t = std::add_lvalue_reference_t<const_t<std::remove_reference_t<Type>, Const>>;

/**
 * The value type of T. (T minus all pointer and reference levels)
 * @tparam T The type to operate on.
 */
template <typename T>
using value_t = const_t<std::decay_t<T>, std::is_const<T>::value>;

/**
 * The lvalue reference type of T.
 * @tparam T The type to operate on.
 */
template <typename T>
using lvref_t = std::add_lvalue_reference_t<std::remove_reference_t<T>>;

/**
 * The non-const rvalue reference type of T.
 * @tparam T The type to operate on.
 */
template <typename T>
using rvref_t = std::add_rvalue_reference_t<std::remove_reference_t<std::remove_const_t<T>>>;

/**
 * The const lvalue reference type of T.
 * @tparam T The type to operate on.
 */
template <typename T> using cref_t = const_lvref_t<T, true>;
