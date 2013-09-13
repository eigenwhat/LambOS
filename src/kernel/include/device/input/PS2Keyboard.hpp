#pragma once

#include <stdint.h>
#include <stddef.h>
#include <cpu/X86CPU.hpp>
#include <util/RingBuffer.hpp>

class PS2Keyboard
{
public:
    PS2Keyboard(X86CPU &cpu);
    void pushScanCode(uint8_t code);
private:
    RingBuffer<uint8_t> _buffer;
};
