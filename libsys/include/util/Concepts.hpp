//
// Created by Martin Miralles-Cordal on 10/6/19.
//

#pragma once

#include <Defines.hpp>

#include <concepts>
#include <utility>

namespace sys {

template <typename T> concept copy_mutable = std::copy_constructible<T> && std::copy_assignable<T>;
template <typename T> concept move_mutable = std::move_constructible<T> && std::move_assignable<T>;

template <typename T> concept pointer = std::is_pointer_v<T>;
template <typename T> concept referenceable = std::is_object_v<T> || std::is_reference_v<T>;
template <typename T> concept dereferenceable = requires(T a) { {*a} -> referenceable; };

template <typename T, typename V>
concept insertable = requires(T a, V &&v)
{
    /**
     * Adds the given object to this one, growing its size.
     *
     * @param object The object to add.
     * @return `true` if this object changed. `false` otherwise.
     */
    { a.insert(std::forward<V>(v)) } -> std::same_as<bool>;
};

template <typename T>
concept clearable = requires(T a)
{
    /**
     * Clears this object's internal storage, whatever that means.
     *
     * @return `true` if this object's state changed, `false` otherwise.
     */
    { a.clear() } -> std::same_as<bool>;
};

template <typename T>
concept sized = requires(T a)
{
    /**
     * Returns whether or not this object is empty (size() == 0).
     * @return `true` if empty, `false` otherwise.
     */
    { a.isEmpty() } -> std::same_as<bool>;

    /**
     * The size of this object (number of elements, bytes, count, etc.).
     * @return A size_t equal to the size of this object.
     */
    { a.size() } -> std::same_as<size_t>;
};

} // namespace sys
