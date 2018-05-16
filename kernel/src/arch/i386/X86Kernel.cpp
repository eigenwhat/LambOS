//
// Created by Martin Miralles-Cordal on 4/15/18.
//

#include <arch/i386/X86Kernel.hpp>
#include <device/display/VGATextConsole.hpp>
#include <device/display/ConsoleOutputStream.hpp>

#include <new>

X86Kernel::X86Kernel() : Kernel(&_x86cpu, nullptr)
        , _vgaConsole(), _consoleOutputStream(_vgaConsole), _stdOut(_consoleOutputStream)
{
    setConsole(&_vgaConsole);
    setOut(&_stdOut);
}

void X86Kernel::installMMU(uint32_t mmap_addr, uint32_t mmap_length)
{
    setMMU(new(_mmuMem) MMU(mmap_addr, mmap_length));
    mmu()->install();
}
