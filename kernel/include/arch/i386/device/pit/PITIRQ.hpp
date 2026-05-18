//
// Created by Martin Miralles-Cordal on 12/22/2019.
//

#pragma once

#include <arch/i386/cpu/X86.hpp>
#include <system/asm.h>
#include <arch/i386/X86Kernel.hpp>

#define KBD_DEVICE    0x60
#define KEY_PENDING   0x64

class PITIRQ : public InterruptServiceRoutine
{
  public:
    /**
     * Installs an ISR for the PS2 keyboard into the CPU.
     * @param cpu The CPU to install into.
     * @param keyboard The PS2Keyboard driver object to send events to.
     */
    static void install(X86::CPU &cpu)
    {
        cpu.idt()->setISR(InterruptNumber::kPITIRQ, new PITIRQ());
        cpu.unmaskIRQ(0);
    }

    virtual void operator()(RegisterTable &)
    {
        endOfInterrupt();
    }
};
