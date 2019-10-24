//
// Created by Martin Miralles-Cordal on 4/15/18.
//

#include <arch/i386/X86Kernel.hpp>
#include <arch/i386/sys/Syscall.hpp>
#include <device/display/VGATextConsole.hpp>
#include <device/display/ConsoleOutputStream.hpp>

#include <new>

X86Kernel::X86Kernel() : _x86cpu{}, _vgaConsole(), _consoleOutputStream(_vgaConsole), _stdOut(_consoleOutputStream)
{
    setConsole(&_vgaConsole);
    setOut(&_stdOut);
}

void X86Kernel::installMMU(uint32_t mmap_addr, uint32_t mmap_length)
{
    setMMU(new(_mmuMem) MMU(mmap_addr, mmap_length));
    mmu()->install();
}

void X86Kernel::installSyscalls()
{
    cpu().idt()->setISR(InterruptNumber::kSystemCall, new SyscallHandler{*this});
}
