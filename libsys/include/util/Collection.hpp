#pragma once

#include <Object.hpp>

template<typename T>
class Collection : public virtual Object
{
public:
    virtual bool isEmpty() const = 0;
    virtual size_t size() const = 0;
    virtual bool insert(T object) = 0;
    virtual bool remove(T object) = 0;
};
