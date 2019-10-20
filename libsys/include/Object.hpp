#pragma once

#include <Memory.hpp>

class Object : public IntrusiveRefCount
{
  public:
    virtual ~Object() = default;
};
