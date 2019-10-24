//
// Created by Martin Miralles-Cordal on 10/22/19.
//

#include <arch/i386/sys/Syscall.hpp>
#include <io/debug.h>

void SyscallHandler::reportUnknownSyscall(RegisterTable &registers)
{
    auto const callId = registers.eax;
    debugOut->print("Received unidentified syscall: ");
    debugOut->println(callId);
    debugOut->print("registers.ebx = ");
    debugOut->println(registers.ebx);
    debugOut->print("registers.ecx = ");
    debugOut->println(registers.ecx);
    debugOut->print("registers.edx = ");
    debugOut->println(registers.edx);
}