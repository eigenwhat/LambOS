//
// Created by Martin Miralles-Cordal on 01/02/2020.
//

#pragma once

#include <type_traits>

namespace sys {

/**
 * Conditionally adds or removes a const qualifier to or from a type.
 * @tparam Type The type to operate on
 * @tparam Const If true, add const. If false, remove const.
 */
template <typename Type, bool Const = true>
using const_t = std::conditional_t<Const, std::add_const_t<Type>, std::remove_const_t<Type>>;

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
 * Conditionally adds or removes a const qualifier to or from a type, then evaluates to a reference to that type.
 * @tparam Type The type to operate on.
 * @tparam Const If true, add const. If false, remove const.
 */
template <typename Type, bool Const = true>
using const_rvref_t = std::add_rvalue_reference_t<const_t<std::remove_reference_t<Type>, Const>>;

/**
 * The value type of T. (T minus all pointer and reference levels)
 * @tparam T The type to operate on.
 */
template <typename T>
using value_t = const_t<std::decay_t<T>, std::is_const_v<T>>;

/**
 * The lvalue reference type of T.
 * @tparam T The type to operate on.
 */
template <typename T>
using lvref_t = const_lvref_t<T, false>;

/**
 * The non-const rvalue reference type of T.
 * @tparam T The type to operate on.
 */
template <typename T>
using rvref_t = const_rvref_t<T, false>;

/**
 * The const lvalue reference type of T.
 * @tparam T The type to operate on.
 */
template <typename T> using cref_t = const_lvref_t<T, true>;


/**
 * The const lvalue reference type of T.
 * @tparam T The type to operate on.
 */
template <typename T> using crvref_t = const_rvref_t<T, true>;

} // namespace sys
