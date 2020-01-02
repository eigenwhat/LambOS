//
// Created by Martin Miralles-Cordal on 9/1/2013.
//

#pragma once

#include <io/OutputStream.hpp>

namespace sys {

class FilterOutputStream : public OutputStream
{
public:
    FilterOutputStream(OutputStream &out) : _out(out) {}

    void close() override { _out.close(); }
    void flush() override { _out.flush(); }
    void write(std::byte byte) override { _out.write(byte); }
    using OutputStream::write;

protected:
    OutputStream &_out;
};

} // libsys namespace
