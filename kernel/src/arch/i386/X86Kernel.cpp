//
// Created by Martin Miralles-Cordal on 4/15/18.
//

#include <arch/i386/X86Kernel.hpp>
#include <arch/i386/sys/Syscall.hpp>
#include <device/display/VGATextConsole.hpp>
#include <device/display/ConsoleOutputStream.hpp>
#include <io/debug.h>

#include <new>

#include <sys/_syscall_numbers.h>

namespace {

struct SyscallHandler : public InterruptServiceRoutine
{
    SyscallHandler(X86Kernel &k) : _kernel(k) {}

    void operator()(RegisterTable &registers) override
    {
        auto callId = registers.eax;
        switch (callId)
        {
            case SyscallId::kRead: {
                // ISR clears IF, but keyboard input is interrupt driven so we
                // need to set IF again.
                asm volatile ("sti");
                registers.eax = Syscall::read(_kernel, registers);
                asm volatile ("cli");
                break;
            }
            case SyscallId::kWrite:
                registers.eax = Syscall::write(_kernel, registers); break;
            case SyscallId::kOpen:
            case SyscallId::kClose:
            case SyscallId::kExit:
            default:
                debugOut->print("Received unidentified syscall: ");
                debugOut->println(callId);
                debugOut->print("registers.ebx = ");
                debugOut->println(registers.ebx);
                debugOut->print("registers.ecx = ");
                debugOut->println(registers.ecx);
                debugOut->print("registers.edx = ");
                debugOut->println(registers.edx);
        }
    }

  private:
    X86Kernel &_kernel;
};

}

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

void X86Kernel::installSyscalls()
{
    x86CPU()->idt()->setISR(InterruptNumber::kSystemCall, new SyscallHandler{*this});
}
