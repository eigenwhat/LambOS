//
// Created by Martin Miralles-Cordal on 10/22/19.
//

#include <arch/i386/sys/Syscall.hpp>
#include <system/Debug.hpp>

void SyscallHandler::reportUnknownSyscall(RegisterTable &registers)
{
    auto const callId = registers.eax;
    sys::debug_println("Received unidentified syscall: %@", callId);
    sys::debug_println("registers.ebx = %x", std::uint32_t{registers.ebx});
    sys::debug_println("registers.ecx = %x", std::uint32_t{registers.ecx});
    sys::debug_println("registers.edx = %x", std::uint32_t{registers.edx});
}
