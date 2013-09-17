#pragma once
#include <cpu/X86CPU.hpp>
#include "Keyboard.hpp"
#include <util/RingBuffer.hpp>

class PS2Keyboard : public Keyboard
{
public:
    PS2Keyboard(X86CPU &cpu);
    virtual KeyEvent read();
    friend class KeyboardISR;
private:
    void pushScanCode(uint8_t code);
    RingBuffer<int> _buffer;
    X86CPU &_cpu;
};
