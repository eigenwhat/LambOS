#ifndef KERNEL

#include <sys/syscall.h>


// ====================================================
// libc syscall definitions
// ====================================================

extern "C" {

DEFN_SYSCALL3(write, 0, uint32_t, uint8_t const *, size_t);

} // extern "C"

#endif
