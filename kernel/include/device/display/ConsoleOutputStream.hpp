#pragma once

#include <device/display/Console.hpp>
#include <io/OutputStream.hpp>

class ConsoleOutputStream : public OutputStream
{
public:
    ConsoleOutputStream(Console &term) : _console(term) {}
    virtual void write(uint8_t byte) { _console.putChar((char) byte); }

private:
    Console &_console;
};
