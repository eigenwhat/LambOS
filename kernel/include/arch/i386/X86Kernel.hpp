#pragma once

#include <new>

#include <Kernel.hpp>
#include <arch/i386/cpu/X86CPU.hpp>
#include <device/display/VGATextConsole.hpp>
#include <device/display/ConsoleOutputStream.hpp>

class X86Kernel : public Kernel
{
public:
    X86Kernel();

    X86CPU *x86CPU() { return static_cast<X86CPU *>(cpu()); }

    /**
     * Prepares the memory management unit for us.
     * @param mmap_addr
     * @param mmap_length
     */
    void installMMU(uint32_t mmap_addr, uint32_t mmap_length);

    void installSyscalls();

private:
    X86CPU _x86cpu;
    VGATextConsole _vgaConsole;
    ConsoleOutputStream _consoleOutputStream;
    PrintStream _stdOut;
    uint8_t _mmuMem[sizeof(MMU)];
};
