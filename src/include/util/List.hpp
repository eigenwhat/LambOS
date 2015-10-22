#pragma once

template<typename T>
class List : public virtual Queue<T>
{
public:
    virtual T& operator[](size_t x) = 0;
};
