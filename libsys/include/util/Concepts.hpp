//
// Created by Martin Miralles-Cordal on 10/6/19.
//

#pragma once

#include <type_traits>

template <typename T, typename U>
concept Same = std::is_same<T, U>::value;

template <typename T> concept Swappable = std::is_swappable_v<T>;
template <typename A, typename B> concept SwappableWith = std::is_swappable_with_v<A, B>;
template <typename A, typename B> concept Convertible = std::is_convertible<A, B>::value;

template <typename T, typename...Args> concept Constructible = std::is_constructible<T, Args...>::value;
template <typename T, typename U> concept Assignable = std::is_assignable<T, U>::value;
template <typename T> concept DefaultConstructible = std::is_default_constructible_v<T>;
template <typename T> concept CopyConstructible = std::is_copy_constructible_v<T>;
template <typename T> concept CopyAssignable = std::is_copy_assignable_v<T>;
template <typename T> concept CopyMutable = CopyConstructible<T> && CopyAssignable<T>;
template <typename T> concept MoveConstructible = std::is_move_constructible_v<T>;
template <typename T> concept MoveAssignable = std::is_move_assignable_v<T>;
template <typename T> concept MoveMutable = MoveConstructible<T> && MoveAssignable<T>;
template <typename T> concept Destructible = std::is_destructible_v<T>;

//template <typename T, typename...Args> concept TriviallyConstructible = std::is_trivially_constructible_v<T, Args...>;
//template <typename T, typename U> concept TriviallyAssignable = std::is_trivially_assignable_v<T, U>;
//template <typename T> concept TriviallyDefaultConstructible = std::is_trivially_default_constructible_v<T>;
//template <typename T> concept TriviallyCopyConstructible = std::is_trivially_copy_constructible_v<T>;
//template <typename T> concept TriviallyCopyAssignable = std::is_trivially_copy_assignable_v<T>;
//template <typename T> concept TriviallyCopyable = std::is_trivially_copyable_v<T>;
//template <typename T> concept TriviallyMoveConstructible = std::is_trivially_move_constructible_v<T>;
//template <typename T> concept TriviallyMoveAssignable = std::is_trivially_move_assignable_v<T>;
template <typename T> concept TriviallyDestructible = std::is_trivially_destructible_v<T>;

template <typename T, typename...Args> concept NothrowConstructible = std::is_nothrow_constructible_v<T, Args...>;
template <typename T, typename U> concept NothrowAssignable = std::is_nothrow_assignable_v<T, U>;
template <typename T> concept NothrowDefaultConstructible = std::is_nothrow_default_constructible_v<T>;
template <typename T> concept NothrowCopyConstructible = std::is_nothrow_copy_constructible_v<T>;
template <typename T> concept NothrowCopyAssignable = std::is_nothrow_copy_assignable_v<T>;
template <typename T> concept NothrowMoveConstructible = std::is_nothrow_move_constructible_v<T>;
template <typename T> concept NothrowMoveAssignable = std::is_nothrow_move_assignable_v<T>;
//template <typename T> concept NothrowDestructible = std::is_nothrow_destructible_v<T>;

template <typename T> concept Pointer = std::is_pointer_v<T>;
template <typename T, typename U=T> concept EqualityComparable = requires(T a, U b) { { a == b} -> Convertible<bool>; };

template <typename T> concept Integral = std::is_integral<T>::value;
template <typename T> concept SignedIntegral = Integral<T> && std::is_signed<T>::value;
template <typename T> concept UnsignedIntegral = Integral<T> && !SignedIntegral<T>;


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
