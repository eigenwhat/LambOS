#pragma once

#include <util/Collection.hpp>

template<typename T>
class Queue : public virtual Collection<T>
{
public:
    virtual bool insert(T obj) { return enqueue(obj); }
    virtual bool enqueue(T obj) = 0;
    virtual T dequeue() = 0;
    virtual T peek() const = 0;
};
