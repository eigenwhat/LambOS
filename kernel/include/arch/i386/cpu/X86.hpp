//
// Created by Martin Miralles-Cordal on 9/4/2013.
//

#pragma once

#include <cstddef>
#include <cstdint>

#include <cpu/CPU.hpp>
#include "GlobalDescriptorTable.hpp"
#include "InterruptDescriptorTable.hpp"

class X86
{
  public:
    X86();

    void install();
    InterruptDescriptorTable *idt() { return &_idt; }
    void enableInterrupts();
    void disableInterrupts() { asm volatile ("cli"); }
    void maskIRQ(unsigned int IRQ);
    void unmaskIRQ(unsigned int IRQ);

  private:
    GlobalDescriptorTable _gdt;
    InterruptDescriptorTable _idt;
    TaskStateSegment _tss;
};
