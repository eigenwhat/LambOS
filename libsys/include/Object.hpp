#pragma once

#include <Memory.hpp>

namespace _ns_LIBSYS {

class Object : public IntrusiveRefCount
{
  public:
    virtual ~Object() = default;
};

} // libsys namespace
