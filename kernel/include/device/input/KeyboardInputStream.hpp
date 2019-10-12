#pragma once

#include <util/RingBuffer.hpp>
#include <io/InputStream.hpp>
#include <device/input/Keyboard.hpp>

#include <stdint.h>
#include <cstddef>

class KeyboardInputStream : public InputStream
{
public:
    KeyboardInputStream(Keyboard *keyboard) : _keyboard(keyboard) {}
    virtual int read();

private:
    ArcPtr<Keyboard> _keyboard;
};
