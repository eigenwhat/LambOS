#pragma once

#include <util/Collection.hpp>

/**
 * A Stack is a Collection that stores its elements by order of insertion.
 * Objects come out of the Stack in last in, first out (LIFO) order.
 * @tparam T The type of object the Stack contains.
 */
template<typename T>
class Stack : public virtual Collection<T>
{
  public:
    /**
     * Adds an element to the top of the Stack.
     * @param obj The object to add.
     * @return `true` if an object was added. `false` otherwise.
     */
    virtual bool push(const T &obj) = 0;

    /**
     * Adds an element to the top of the Stack.
     * @param obj The object to add.
     * @return `true` if an object was added. `false` otherwise.
     */
    virtual bool push(T &&obj) = 0;

    /**
     * Removes an element from the top of the Stack.
     * @return The object. If the Stack is empty, the return value is undefined.
     */
    virtual T pop() = 0;

    /**
     * Returns the element at the top of the Stack without removing it.
     * @return The object. If the Stack is empty, the return value is undefined.
     */
    virtual const T &peek() const = 0;

    /**
     * Adds an element to the top of the Stack. Equivalent to push(T).
     * @param obj The object to add.
     * @return `true` if an object was added. `false` otherwise.
     */
    bool insert(const T &obj) override { return push(obj); }
};
