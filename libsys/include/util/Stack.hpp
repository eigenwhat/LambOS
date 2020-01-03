#pragma once

#include <util/Collection.hpp>

namespace sys {

template <typename T>
concept Stack = DynamicCollection<T> && requires(T a)
{
    /**
    * Adds an element to the top of the Stack.
    * @param obj The object to add.
    * @return `true` if an object was added. `false` otherwise.
    */
    { a.push(std::declval<typename T::value_type>()) } -> std::same_as<bool>;

    /**
     * Removes an element from the top of the Stack.
     * @return The object. If the Stack is empty, the return value is undefined.
     */
    { a.pop() } -> std::same_as<typename T::value_type>;

    /**
     * Returns the element at the top of the Stack without removing it.
     * @return The object. If the Stack is empty, the return value is undefined.
     */
    { a.peek() } -> std::same_as<typename T::value_type const &>;
};

} // namespace concepts
