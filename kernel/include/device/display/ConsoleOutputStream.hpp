#pragma once

#include <device/display/Console.hpp>
#include <io/OutputStream.hpp>

class ConsoleOutputStream : public OutputStream
{
public:
    ConsoleOutputStream(Console &term) : _console(term) {}
    void write(std::byte byte) override;

private:
    Console &_console;
};
