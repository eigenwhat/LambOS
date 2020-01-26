//
// Created by Martin Miralles-Cordal on 5/10/2018.
//

#pragma once

#include <arch/i386/cpu/InterruptDescriptorTable.hpp>
#include <Kernel.hpp>

#include <io/Format.hpp>
#include <system/Debug.hpp>

#include <cstdio>

class PageFaultISR : public InterruptServiceRoutine
{
  public:
    virtual void operator()(RegisterTable &registers) override
    {
        DEBUG_BREAK();
        uint32_t cr2;
        asm volatile ("\t movl %%cr2, %0" : "=rm"(cr2));
        auto message = sys::format("Page Fault: %x/%@%@%@%@%@",
                                   uint32_t{cr2},
                                   registers.err_code & 0x01 ? "Protection/" : "",
                                   registers.err_code & 0x02 ? "Write access/" : "",
                                   registers.err_code & 0x04 ? "in User mode/" : "in Kernel mode/",
                                   registers.err_code & 0x08 ? "RSVD bits set/" : "",
                                   registers.err_code & 0x10 ? "NX violation/" : "");
        kernel->panic(message.cstr());
    }
};
