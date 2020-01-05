//
// Created by Martin Miralles-Cordal on 01/05/2020.
//

#pragma once

#include <io/OutputStream.hpp>

#include <cstdio>

namespace sys {

class StandardOutput final : public OutputStream
{
  public:
    static StandardOutput & Get() { static StandardOutput s_out; return s_out; }
    void write(std::byte byte) override { putchar(static_cast<int>(byte)); }
};

} // libsys namespace
