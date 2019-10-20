#pragma once

#include <cstddef>
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
    virtual void maskIRQ(unsigned int IRQ);
    virtual void unmaskIRQ(unsigned int IRQ);

  private:
    GlobalDescriptorTable _gdt;
    InterruptDescriptorTable _idt;
    TaskStateSegment _tss;
};
