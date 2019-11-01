//
// Created by Martin Miralles-Cordal on 9/1/2013.
//

#pragma once

#include <device/display/Console.hpp>
#include <io/OutputStream.hpp>

class ConsoleOutputStream : public sys::OutputStream
{
  public:
    ConsoleOutputStream(Console &term) : _console(term) {}
    void write(std::byte byte) override;

  private:
    Console &_console;
};
