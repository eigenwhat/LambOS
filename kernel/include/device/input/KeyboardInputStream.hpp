#pragma once

#include <util/RingBuffer.hpp>
#include <io/InputStream.hpp>
#include <device/input/Keyboard.hpp>

#include <cstdint>
#include <cstddef>

class KeyboardInputStream : public InputStream
{
public:
    KeyboardInputStream(Keyboard *keyboard) : _keyboard(keyboard) {}
    Byte read() override;

private:
    ArcPtr<Keyboard> _keyboard;
};
