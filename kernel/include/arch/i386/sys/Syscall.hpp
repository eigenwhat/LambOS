#pragma once

#include <arch/i386/X86Kernel.hpp>

class Syscall
{
  public:
    static constexpr uint32_t kStdOut = 0;
    static constexpr uint32_t kStdIn = 1;

    static uint32_t write(X86Kernel &k, RegisterTable const &registers)
    {
        auto fd = registers.ebx;
        if (fd == kStdOut) {
            uint8_t const *buf = reinterpret_cast<uint8_t const *>(registers.ecx);
            auto len = registers.edx;
            k.out()->write(buf, len);

            return len;
        }

        return static_cast<uint32_t>(-1);
    }
};