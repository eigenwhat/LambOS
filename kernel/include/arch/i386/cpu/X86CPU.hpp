#pragma once

#include <stddef.h>
#include <stdint.h>

#include <cpu/CPU.hpp>
#include "GlobalDescriptorTable.hpp"
#include "InterruptDescriptorTable.hpp"

class X86CPU : public CPU
{
public:
    X86CPU();

    virtual void install();

    InterruptDescriptorTable *idt() { return &_idt; }

    virtual void enableInterrupts();

    virtual void disableInterrupts() { asm volatile ("cli"); }

    virtual void maskIRQ(unsigned char IRQ);

    virtual void unmaskIRQ(unsigned char IRQ);

private:
    GlobalDescriptorTable _gdt;
    InterruptDescriptorTable _idt;
    TaskStateSegment _tss;
};
