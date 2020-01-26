//
// Created by Martin Miralles-Cordal on 4/15/18.
//

#include <arch/i386/X86Kernel.hpp>
#include <arch/i386/proc/X86Process.hpp>
#include <arch/i386/sys/Syscall.hpp>
#include <device/display/VGATextConsole.hpp>
#include <device/display/ConsoleOutputStream.hpp>

#include <new>

X86Kernel::X86Kernel() : _x86cpu{}, _vgaConsole(), _consoleOutputStream(_vgaConsole)
{
    setConsole(&_vgaConsole);
    setOut(&_consoleOutputStream);
}

void X86Kernel::installMMU(uint32_t mmap_addr, uint32_t mmap_length)
{
//    _mmu = new(_mmuMem) MMU(mmap_addr, mmap_length);
    _maybemmu.emplace(mmap_addr, mmap_length);
    _mmu = _maybemmu.operator->();
    setAddressSpace(_mmu->create());
    _mmu->install(addressSpace());
}

void X86Kernel::installSyscalls()
{
    cpu().idt()->setISR(InterruptNumber::kSystemCall, new SyscallHandler{*this});
}

void X86Kernel::schedule()
{
//    auto curr = scheduler().currentProcess()->procState;
//    auto next = scheduler().nextProcess()->procState;
//    if (curr != next) {
//        X86Process::swapActive(curr, next);
//    }
}