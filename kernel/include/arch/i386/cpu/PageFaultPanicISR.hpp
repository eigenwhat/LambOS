//
// Created by Martin Miralles-Cordal on 5/10/2018.
//

#pragma once

#include <arch/i386/cpu/InterruptDescriptorTable.hpp>
#include <Kernel.hpp>
#include <io/debug.h>
#include <cstdio>

class PageFaultISR : public InterruptServiceRoutine
{
  public:
    virtual void operator()(RegisterTable &registers) override
    {
        magicBreakpoint();
        char message[128];
        uint32_t cr2;
        asm volatile ("\t movl %%cr2, %0" : "=rm"(cr2));
        sprintf(message, "Page Fault: %x/%s%s%s%s%s",
                cr2,
                registers.err_code & 0x01 ? "Protection/" : "",
                registers.err_code & 0x02 ? "Write access/" : "",
                registers.err_code & 0x04 ? "in User mode/" : "in Kernel mode/",
                registers.err_code & 0x08 ? "RSVD bits set/" : "",
                registers.err_code & 0x10 ? "NX violation/" : "");
        kernel->panic(message);
    }
};
