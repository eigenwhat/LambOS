//
// Created by Martin Miralles-Cordal on 5/6/2018.
//

#pragma once

#include <util/Collection.hpp>
#include <util/Iterator.hpp>

namespace sys {
/**
 * An Iterable Collection is a Collection that supports traversal via special
 * pointer objects called {@ref Iterator}s.
 */
template <typename T>
concept Iterable = Collection<T> && requires(T a)
{
    typename T::iterator;
    requires iterator<typename T::iterator>;

    typename T::const_iterator;
    requires iterator<typename T::const_iterator>;

    /**
     * Retrieves an Iterator set to the frontmost element of the Collection. If
     * the Collection is empty, begin() == end().
     * @return An Iterator pointing to the frontmost element of the Collection.
     */
    { static_cast<const T&>(a).begin() } -> std::same_as<typename T::const_iterator>;
    { a.begin() } -> std::same_as<typename T::iterator>;

    /**
     * Retrieves an Iterator marking the end of the Collection. It should *NOT*
     * point to any element in the Collection, but instead act as a placeholder
     * which can be compared against to determine when iteration has completed.
     * @return An Iterator with a special value signifying the end of the
     *         Collection. The return value from dereferencing it is undefined.
     */
    { static_cast<const T&>(a).end() } -> std::same_as<typename T::const_iterator>;
    { a.end() } -> std::same_as<typename T::iterator>;
};

} // namespace concepts
