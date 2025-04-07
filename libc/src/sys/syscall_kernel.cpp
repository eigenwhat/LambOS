#ifdef KERNEL

#include <sys/syscall.h>
#include <arch/i386/sys/Syscall.hpp>

// ====================================================
// libk "syscalls" which skip to the good part
// ====================================================

namespace {

RegisterTable fake_syscall(uint32_t p1 = 0, uint32_t p2 = 0, uint32_t p3 = 0,
                           uint32_t p4 = 0, uint32_t p5 = 0)
{
    RegisterTable r;
    r.ebx = p1; r.ecx = p2; r.edx = p3; r.esi = p4; r.edi = p5;
    return r;
}

}

extern "C" {

int sys_write(uint32_t fd, uint8_t const *buf, size_t bytes)
{
    auto registers = fake_syscall(fd, (uint32_t)buf, bytes);
    return (int)Syscall::write((X86Kernel&)*kernel, registers);
}

int sys_read(uint32_t fd, uint8_t *buf, size_t bytes)
{
    auto registers = fake_syscall(fd, (uint32_t)buf, bytes);
    return (int)Syscall::read((X86Kernel&)*kernel, registers);
}

int sys_exit(int status)
{
    auto registers = fake_syscall((uint32_t)status);
    return (int)Syscall::exit((X86Kernel&)*kernel, registers);
}

int sys_sleep(int status)
{
    auto registers = fake_syscall((uint32_t)status);
    return (int)Syscall::sleep((X86Kernel&)*kernel, registers);
}

int sys_yield() { return 0; }

int sys_die()
{
    auto registers = fake_syscall();
    return (int)Syscall::die((X86Kernel&)*kernel, registers);
}

} // extern "C"

#endif
