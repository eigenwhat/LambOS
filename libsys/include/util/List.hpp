#pragma once

#include <util/Deque.hpp>

/**
 * A List is, well, a list of things. Like Queues and Stacks, List elements are
 * ordered. Unlike them, elements are indexed and thus supports random access.
 * @tparam T The type of object the List contains.
 */
template<typename T>
class List : public virtual Deque<T>
{
  public:
    /**
     * Removes an element from the front of the List.
     * @return The object. If the List is empty, the return value is undefined.
     */
    virtual T pop() = 0;

    /**
     * Returns the element at the top of the List without removing it.
     * @return The object. If the List is empty, the return value is undefined.
     */
    virtual const T &peek() const = 0;

    /**
     * Returns the element at the given index.
     * @note Runtime complexity is implementation specific. O(1) not guaranteed!
     * @param idx The index of the object.
     * @return The object at that index. If the index is out of bounds, the
     *         return value is undefined.
     */
    virtual T& operator[](size_t idx) const = 0;
};
