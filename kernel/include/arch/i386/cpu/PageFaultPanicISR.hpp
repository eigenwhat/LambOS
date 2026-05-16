//
// Created by Martin Miralles-Cordal on 5/10/2018.
//

#pragma once

#include <arch/i386/cpu/InterruptDescriptorTable.hpp>
#include <Kernel.hpp>

#include <io/Format.hpp>
#include <system/Debug.hpp>

#include <cstdio>

#define PAGE_FAULT_FORMAT "Page Fault: cr2=%x eip=%x/%@%@%@%@%@",                          \
                          cr2,                                                             \
                          static_cast<uint32_t>(registers.eip),                            \
                          registers.err_code & 0x01 ? "Protection/" : "",                  \
                          registers.err_code & 0x02 ? "Write access/" : "",                \
                          registers.err_code & 0x04 ? "in User mode/" : "in Kernel mode/", \
                          registers.err_code & 0x08 ? "RSVD bits set/" : "",               \
                          registers.err_code & 0x10 ? "NX violation/" : ""

class PageFaultISR : public InterruptServiceRoutine
{
  public:
    virtual void operator()(RegisterTable &registers) override
    {
        DEBUG_BREAK();
        uint32_t cr2;
        asm volatile("movl %%cr2, %0" : "=r" (cr2));
        sys::debug_println(PAGE_FAULT_FORMAT);
        auto message = sys::format(PAGE_FAULT_FORMAT);
        kernel->panic(message.cstr());
    }
};
