//
// Created by Martin Miralles-Cordal on 9/1/2013.
//

#pragma once

#include <Object.hpp>

#include <cstddef>
#include <cstdint>

namespace _ns_LIBSYS {

class OutputStream : public Object
{
  public:
    virtual void close() {}
    virtual void flush() {}
    void write(std::byte const *bytes, size_t bytesToWrite) { for (; bytesToWrite--; write(*bytes++)); }
    void write(char const *bytes, size_t bytesToWrite) { write((std::byte const *)(bytes), bytesToWrite); }
    virtual void write(std::byte byte) = 0;
    void write(char c) { write(std::byte(c)); }
};

} // libsys namespace
