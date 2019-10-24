#pragma once
#include <io/OutputStream.hpp>

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
