#pragma once

#include <Object.hpp>

template<typename T>
class Collection : public virtual Object
{
public:
    virtual bool isEmpty() = 0;
    virtual size_t size() = 0;
};
