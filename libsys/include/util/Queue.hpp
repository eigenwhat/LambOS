#pragma once

#include <util/Collection.hpp>

/**
 * A Queue is a Collection that stores its elements by order of insertion.
 * Objects come out of the Queue in first in, first out (FIFO) order.
 * @tparam T The type of object the Queue contains.
 */
template<typename T>
class Queue : public virtual Object
{
  public:
    using ValueType = T;

    /**
     * Returns whether or not the Queue is empty.
     * @return `true` if empty, `false` otherwise.
     */
    virtual bool isEmpty() const = 0;

    /**
     * The number of elements in the Queue.
     * @return A size_t equal to the number of elements.
     */
    virtual size_t size() const = 0;

    /**
     * Adds an element to the back of the Queue. Equivalent to enqueue(T).
     * @param obj The object to add.
     * @return `true` if an object was added. `false` otherwise.
     */
    virtual bool insert(T obj) { return enqueue(std::move(obj)); }

    /**
     * Removes all objects from the Queue.
     *
     * @return `true` if the Queue changed, `false` otherwise.
     */
    virtual bool clear() = 0;

    /**
     * Adds an element to the back of the Queue.
     * @param obj The object to add.
     * @return `true` if an object was added. `false` otherwise.
     */
    virtual bool enqueue(const T &obj) = 0;

    /**
     * Adds an element to the back of the Queue.
     * @param obj The object to add.
     * @return `true` if an object was added. `false` otherwise.
     */
    virtual bool enqueue(T &&obj) = 0;

    /**
     * Removes an element from the front of the queue.
     * @return The object. If the queue is empty, the return value is undefined.
     */
    virtual T pop() = 0;

    /**
     * Returns the element at the front of the queue without removing it.
     * @return The object. If the queue is empty, the return value is undefined.
     */
    virtual const T &peek() const = 0;
};
