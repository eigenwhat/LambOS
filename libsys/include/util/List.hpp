#pragma once

#include <util/Deque.hpp>
#include <util/Iterable.hpp>

namespace concepts {
/**
 * A List is, well, a list of things. Like Queues and Stacks, List elements are
 * ordered. Unlike them, elements are indexed and thus supports random access.
 * @tparam T The type of object the List contains.
 */
template<typename T>
concept List = Deque<T> && Iterable<T> && requires(T const &a, size_t idx)
{
    /**
     * Returns the element at the given index.
     * @note Runtime complexity is implementation specific. O(1) not guaranteed!
     * @param idx The index of the object.
     * @return The object at that index. If the index is out of bounds, the
     *         return value is undefined.
     */
    { a[idx] } -> Same<typename T::ValueType> &;
};

} // namespace concepts
