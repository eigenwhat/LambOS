#pragma once

#include <util/Collection.hpp>

template<typename T>
class Queue : public virtual Collection<T>
{
public:
    virtual void enqueue(T obj) = 0;
    virtual T dequeue() = 0;
    virtual T peek() = 0;
};
