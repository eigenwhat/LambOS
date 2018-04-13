#pragma once

#include <arch/i386/cpu/X86CPU.hpp>
#include <device/input/Keyboard.hpp>
#include <util/RingBuffer.hpp>

class PS2Keyboard : public Keyboard
{
public:
    PS2Keyboard(X86CPU &cpu);

    virtual KeyEvent read();

    virtual bool keyIsPressed(KeyCode key) { return _keysPressed[key]; }

    friend class KeyboardISR;

private:
    void pushScanCode(uint8_t code);

    RingBuffer<int> _buffer;
    bool _keysPressed[256];
    X86CPU &_cpu;
};
