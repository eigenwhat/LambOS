#pragma once

#include <arch/i386/X86Kernel.hpp>

#include <sys/_syscall_numbers.h>

#include <io/Print.hpp>

#include <cstring>

namespace Syscall {
    inline std::uint32_t write(X86Kernel &k, RegisterTable const &registers);
    inline std::uint32_t read(X86Kernel &k, RegisterTable const &registers);
    inline std::uint32_t exit(X86Kernel &, RegisterTable const &registers);
    inline std::uint32_t sleep(X86Kernel &, RegisterTable const &registers);
    inline std::uint32_t yield(X86Kernel &, RegisterTable const &registers);
    inline std::uint32_t die(X86Kernel &, RegisterTable const &registers);
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
                sti();
                registers.eax = Syscall::read(_kernel, registers);
                cli();
                break;
            }
            case SyscallId::kWrite:
                registers.eax = Syscall::write(_kernel, registers); break;
            case SyscallId::kOpen:
            case SyscallId::kClose:
            case SyscallId::kExit:
                registers.eax = Syscall::exit(_kernel, registers); break;
            case SyscallId::kSleep:
                registers.eax = Syscall::sleep(_kernel, registers); break;
            case SyscallId::kYield:
                registers.eax = Syscall::yield(_kernel, registers); break;
            case SyscallId::kDie:
                registers.eax = Syscall::die(_kernel, registers); break;
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
    sys::println(*k.out(), "\nProcess exit with code %x", uint32_t{registers.ebx});
    return registers.ebx;
}

inline std::uint32_t sleep(X86Kernel &, RegisterTable const &) { return 0; }
inline std::uint32_t yield(X86Kernel &, RegisterTable const &) { return 0; }
inline std::uint32_t die(X86Kernel &, RegisterTable const &)
{
    kernel->panic("Committed honorable sudoku");
    return 0;
}

} // namespace Syscall
