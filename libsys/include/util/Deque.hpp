#pragma once

#include <util/Stack.hpp>
#include <util/Queue.hpp>

/**
 * A Deque is a Queue and a Stack. A Queuestack, if you will. To avoid any
 * potential association with a certain MS Paint Adventure, we'll stick to the
 * Java Collections Framework's name for the structure (pronounced like "deck").
 * @tparam T The type of object the Deque contains.
 */
template <typename T> class Deque : public virtual Stack<T>
                                  , public virtual Queue<T>
{
  public:
    /**
     * Adds an element to the back of the Deque. Equivalent to enqueue(T).
     * @param obj The object to add.
     * @return `true` if an object was added. `false` otherwise.
     */
    bool insert(const T &obj) override { return this->enqueue(obj); }

    /**
     * Removes an element from the back of the Deque.
     * @return The object. If the Deque is empty, the return value is undefined.
     */
    virtual T unqueue() = 0;

    /**
     * Returns the element at the back of the Deque without removing it.
     * @return The object. If the Deque is empty, the return value is undefined.
     */
    virtual const T &peekBack() const = 0;
};
