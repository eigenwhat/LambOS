//
// Created by Martin Miralles-Cordal on 9/4/2013.
//

#pragma once

#include <arch/i386/cpu/GlobalDescriptorTable.hpp>
#include <arch/i386/cpu/InterruptDescriptorTable.hpp>
#include <cpu/CPU.hpp>

#include <cstddef>
#include <cstdint>

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
    GlobalDescriptorTable _gdt{};
    InterruptDescriptorTable _idt{};
    TaskStateSegment _tss{};
};