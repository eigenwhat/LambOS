#pragma once

#include <arch/i386/X86Kernel.hpp>
#include <sys/_syscall_numbers.h>
#include <cstring>

namespace Syscall {
    inline std::uint32_t write(X86Kernel &k, RegisterTable const &registers);
    inline std::uint32_t read(X86Kernel &k, RegisterTable const &registers);
    inline std::uint32_t exit(X86Kernel &, RegisterTable const &registers);
} // namespace Syscall

struct SyscallHandler : public InterruptServiceRoutine
{
    SyscallHandler(X86Kernel &k) : _kernel(k) {}

    void operator()(RegisterTable &registers) override
    {
        auto const callId = registers.eax;
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
                registers.eax = Syscall::exit(_kernel, registers); break;
            default:
                reportUnknownSyscall(registers);
        }
    }

  private:
    void reportUnknownSyscall(RegisterTable &registers);

    X86Kernel &_kernel;
};

namespace Syscall {

constexpr inline std::uint32_t kStdOut = 0;
constexpr inline std::uint32_t kStdIn = 1;

inline std::uint32_t write(X86Kernel &k, RegisterTable const &registers)
{
    auto fd = registers.ebx;
    if (fd == kStdOut) {
        auto const *buf = reinterpret_cast<std::byte const *>(registers.ecx);
        auto len = registers.edx;
        k.out()->write(buf, len);
        return len;
    }

    return static_cast<uint32_t>(-1);
}

inline std::uint32_t read(X86Kernel &k, RegisterTable const &registers)
{
    auto fd = registers.ebx;
    if (fd == kStdIn) {
        auto *buf = reinterpret_cast<std::byte *>(registers.ecx);
        auto len = registers.edx;
        return k.in()->read(buf, len);
    }

    return static_cast<std::uint32_t>(-1);
}

inline std::uint32_t exit(X86Kernel &k, RegisterTable const &registers)
{
    constexpr char const *msg = "Process exit with code 0x";
    constexpr size_t len = std::strlen(msg);
    k.out()->write(msg, len);
    char buf[16];
    auto returnValue = std::uitoa(registers.ebx, buf, 16);
    k.out()->write(returnValue, std::strlen(returnValue));
    k.out()->write('\n');
    return registers.ebx;
}

} // namespace Syscall