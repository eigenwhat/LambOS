#pragma once

#include <stdint.h>
#include <stddef.h>
#include <cpu/X86CPU.hpp>
#include <util/RingBuffer.hpp>
#include <io/InputStream.hpp>

class KeyboardInputStream : public InputStream
{
public:
    KeyboardInputStream(X86CPU &cpu);
    virtual int read();
    friend class KeyboardISR;
private:
    void pushScanCode(uint8_t code);
    RingBuffer<uint8_t> _buffer;
    X86CPU &_cpu;
};
