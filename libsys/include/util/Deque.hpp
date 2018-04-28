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
};
