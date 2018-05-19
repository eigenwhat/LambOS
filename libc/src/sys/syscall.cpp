#ifndef KERNEL

#include <sys/syscall.h>
#include <sys/_syscall_numbers.h>


// ====================================================
// libc syscall definitions
// ====================================================

extern "C" {

DEFN_SYSCALL3(write, SyscallId::kWrite, uint32_t, uint8_t const *, size_t);

} // extern "C"

#endif