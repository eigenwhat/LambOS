#ifndef KERNEL

#include <sys/syscall.h>
#include <sys/_syscall_numbers.h>


// ====================================================
// libc syscall definitions
// ====================================================

extern "C" {

DEFN_SYSCALL1(exit, SyscallId::kExit, int);
DEFN_SYSCALL3(write, SyscallId::kWrite, uint32_t, uint8_t const *, size_t);
DEFN_SYSCALL3(read, SyscallId::kRead, uint32_t, uint8_t *, size_t);
DEFN_SYSCALL1(sleep, SyscallId::kSleep, int);
DEFN_SYSCALL0(yield, SyscallId::kYield);

} // extern "C"

#endif
