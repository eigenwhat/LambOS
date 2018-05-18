#pragma once
#include <io/OutputStream.hpp>

class FilterOutputStream : public OutputStream
{
public:
    FilterOutputStream(OutputStream &out) : _out(out) {}

    void close() override { _out.close(); }

    void flush() override { _out.flush(); }

    void write(uint8_t const *bytes, size_t bytesToWrite) override
    {
        for(; bytesToWrite--; _out.write(*bytes++));
    }

    void write(uint8_t byte) override { _out.write(byte); }

protected:
    OutputStream &_out;
};
