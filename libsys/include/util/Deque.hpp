#pragma once

#include <util/Stack.hpp>
#include <util/Queue.hpp>

/**
 * A Deque is a Queue and a Stack. A Queuestack, if you will. To avoid any
 * potential association with a certain MS Paint Adventure, we'll stick to the
 * Java Collections Framework's name for the structure (pronounced like "deck").
 * @tparam T The type of object the Deque contains.
 */
template <typename T>
concept Deque = Stack<T> && Queue<T> && requires(T a)
{
    /**
     * Removes an element from the back of the Deque.
     * @return The object. If the Deque is empty, the return value is undefined.
     */
    { a.popBack() } -> Same<typename T::ValueType>;

    /**
     * Returns the element at the back of the Deque without removing it.
     * @return The object. If the Deque is empty, the return value is undefined.
     */
    { a.peekBack() } -> Same<typename T::ValueType const &>;
};
