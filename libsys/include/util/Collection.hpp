#pragma once

#include <Object.hpp>

#include <cstddef>

#include <util/Concepts.hpp>

/**
 * The root template interface in the collections class hierarchy. Collections,
 * simply put, are groups of objects. Collections may vary by their internal
 * structure and the rules surrounding how the objects in the Collection are
 * inserted, maintained, and removed. (e.g. Are duplicate objects allowed? Is it
 * ordered or unordered? Can you add/remove objects anywhere, or only in certain
 * positions?)
 * @tparam T The type of object the Collection contains.
 */
template <typename T>
concept Collection = Sized<T> && requires(T a) { typename T::ValueType; };

template <typename T>
concept InsertableCollection = Collection<T> && Insertable<T, T::ValueType>;

template <typename T>
concept DynamicCollection = InsertableCollection<T> && Clearable<T> && requires(T a)
{
    /**
    * Removes the object from the Collection, reducing its size.
    *
    * @param object The object to remove.
    * @return `true` if the Collection changed. `false` otherwise.
    */
    { a.remove(std::declval<typename T::ValueType>()) } -> Same<bool>;
};

#define STR(x) #x
#define ASSERT_IS_2(TYPE, MSG) static_assert(Collection<TYPE<Object>>, MSG)
#define ASSERT_IS_COLLECTION(TYPE) ASSERT_IS_2(TYPE, "'" #TYPE "' is not a Collection.");
