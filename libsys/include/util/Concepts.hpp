//
// Created by Martin Miralles-Cordal on 10/6/19.
//

#pragma once

#include <type_traits>

template <typename T, typename U>
concept Same = std::is_same<T, U>::value;

template <class T>
concept Integral = std::is_integral<T>::value;
template <class T>
concept SignedIntegral = Integral<T> && std::is_signed<T>::value;
template <class T>
concept UnsignedIntegral = Integral<T> && !SignedIntegral<T>;


template <typename T, typename V>
concept Insertable = requires(T a, V &&v)
{
    /**
     * Adds the given object to this one, growing its size.
     *
     * @param object The object to add.
     * @return `true` if this object changed. `false` otherwise.
     */
    { a.insert(std::forward<V>(v)) } -> Same<bool>;
};

template <typename T>
concept Clearable = requires(T a)
{
    /**
     * Clears this object's internal storage, whatever that means.
     *
     * @return `true` if this object's state changed, `false` otherwise.
     */
    { a.clear() } -> Same<bool>;
};

template <typename T>
concept Sized = requires(T a)
{
    /**
     * Returns whether or not this object is empty (size() == 0).
     * @return `true` if empty, `false` otherwise.
     */
    { a.isEmpty() } -> Same<bool>;

    /**
     * The size of this object (number of elements, bytes, count, etc.).
     * @return A size_t equal to the size of this object.
     */
    { a.size() } -> Same<size_t>;
};
