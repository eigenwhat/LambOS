#pragma once
#include <io/OutputStream.hpp>

class FilterOutputStream : public OutputStream
{
public:
    FilterOutputStream(OutputStream &out) : _out(out) {}
    virtual void close() { _out.close(); }
    virtual void flush() { _out.flush(); }
    virtual void write(uint8_t *bytes, size_t bytesToWrite) { for(; bytesToWrite--; _out.write(*bytes++)); }
    virtual void write(uint8_t byte) { _out.write(byte); }
protected:
    OutputStream &_out;
};
