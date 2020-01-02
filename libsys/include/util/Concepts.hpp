//
// Created by Martin Miralles-Cordal on 10/6/19.
//

#pragma once

#include <Defines.hpp>

#include <concepts>
#include <utility>

namespace sys::concepts {

template <typename T> concept CopyMutable = std::copy_constructible<T> && std::copy_assignable<T>;
template <typename T> concept MoveMutable = std::move_constructible<T> && std::move_assignable<T>;

template <typename T> concept Pointer = std::is_pointer_v<T>;

template <typename T> concept Dereferencable = requires(T a) { *a; };

template<typename I>
concept WeaklyIncrementable = std::default_constructible<I> && std::movable<I> && requires(I i) {
    { ++i } -> std::same_as<I&>;
    i++;
};

template <typename I> concept Incrementable = std::regular<I> && WeaklyIncrementable<I>
                                            && requires(I i) { { i++ } -> std::same_as<I>; };

template <typename T, typename V>
concept Insertable = requires(T a, V &&v)
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
concept Clearable = requires(T a)
{
    /**
     * Clears this object's internal storage, whatever that means.
     *
     * @return `true` if this object's state changed, `false` otherwise.
     */
    { a.clear() } -> std::same_as<bool>;
};

template <typename T>
concept Sized = requires(T a)
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

} // namespace concepts
