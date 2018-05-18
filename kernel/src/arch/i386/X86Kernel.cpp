//
// Created by Martin Miralles-Cordal on 4/15/18.
//

#include <arch/i386/X86Kernel.hpp>
#include <device/display/VGATextConsole.hpp>
#include <device/display/ConsoleOutputStream.hpp>
#include <io/debug.h>
#include <arch/i386/sys/Syscall.hpp>

#include <new>

namespace {

struct SyscallHandler : public InterruptServiceRoutine
{
    SyscallHandler(X86Kernel &k) : _kernel(k) {}

    void operator()(RegisterTable &registers) override
    {
        auto callId = registers.eax;
        if (callId == 0) {
            Syscall::write(_kernel, registers);
        } else {
            debugOut->print("Received unidentified syscall: ");
            debugOut->println<int>(callId);
            debugOut->print("registers.ebx = ");
            debugOut->println<int>(registers.ebx);
            debugOut->print("registers.ecx = ");
            debugOut->println<int>(registers.ecx);
            debugOut->print("registers.edx = ");
            debugOut->println<int>(registers.edx);
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
