#pragma once

template<typename T>
class List : public virtual Queue
{
public:
    virtual T& operator[](size_t x) = 0;
};
