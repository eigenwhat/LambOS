//
// Created by Martin Miralles-Cordal on 12/23/2019.
//

#pragma once

#include <arch/i386/cpu/RegisterTable.h>
#include <proc/Process.hpp>

namespace X86Process {

struct State : Process::ICpuState
{
    struct Context
    {
        std::uint32_t edi;
        std::uint32_t esi;
        std::uint32_t ebx;
        std::uint32_t ebp;
        std::uint32_t eip;
    };

    RegisterTable trapFrame;               // Trap frame for current syscall
    Context *context;                      // context_switch() here to run process
    void *chan;                            // If non-zero, sleeping on chan
};

} // namespace X86Process

extern "C" {
extern void context_switch(X86Process::State::Context **old, X86Process::State::Context *next);
}

namespace X86Process {

void swapActive(Process::ICpuState *curr, Process::ICpuState *next)
{
    auto *currCtx = static_cast<X86Process::State*>(curr);
    auto *nextCtx = static_cast<X86Process::State*>(next);
    context_switch(&currCtx->context, nextCtx->context);
}

} // namespace X86Process
