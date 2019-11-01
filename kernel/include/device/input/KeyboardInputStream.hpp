//
// Created by Martin Miralles-Cordal on 9/12/2013.
//

#pragma once

#include <util/RingBuffer.hpp>
#include <io/InputStream.hpp>
#include <device/input/Keyboard.hpp>

#include <cstdint>
#include <cstddef>

class KeyboardInputStream : public sys::InputStream
{
  public:
    KeyboardInputStream(sys::ArcPtr<Keyboard> keyboard) : _keyboard(std::move(keyboard)) {}
    Byte read() override;

  private:
    sys::ArcPtr<Keyboard> _keyboard;
};
