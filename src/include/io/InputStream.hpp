#pragma once

#include <Object.hpp>

class InputStream : public Object
{
public:
    virtual size_t available() { return 0; }
    virtual void close() {}
    virtual void mark(size_t /*readLimit*/) {}
    virtual bool markSupported() { return false; }
    virtual int read() = 0;
    virtual int read(uint8_t *bytes, size_t bytesToRead)
    {
        for (; bytesToRead--; *bytes++ = read());

        return (int) bytesToRead;
    }

    virtual void reset() {}
};
