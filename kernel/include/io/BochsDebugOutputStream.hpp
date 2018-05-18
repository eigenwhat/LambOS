#pragma once

#include <arch/i386/sys/asm.h>
#include <io/OutputStream.hpp>

class BochsDebugOutputStream : public OutputStream
{
public:
    virtual void write(uint8_t byte) { outb(0xe9, byte); }
};
