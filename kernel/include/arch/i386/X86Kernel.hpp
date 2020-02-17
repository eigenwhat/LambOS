#pragma once

#include <Kernel.hpp>
#include <arch/i386/cpu/X86.hpp>
#include <device/display/VGATextConsole.hpp>
#include <device/display/ConsoleOutputStream.hpp>

class X86Kernel : public Kernel
{
public:
    X86Kernel();

    X86 &cpu() { return _x86cpu; }

    /**
     * Prepares the memory management unit for us.
     * @param mmap_addr
     * @param mmap_length
     */
    void installMMU(uint32_t mmap_addr, uint32_t mmap_length);

    void installSyscalls();

private:
    X86 _x86cpu;
    VGATextConsole _vgaConsole;
    ConsoleOutputStream _consoleOutputStream;
    sys::Maybe<MMU> _maybemmu{};
};

extern X86Kernel *x86Kernel;
