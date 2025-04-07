#ifndef LAMBOS_SYSCALL_H
#define LAMBOS_SYSCALL_H

#include <decl.h>
#include <stddef.h>
#include <stdint.h>

#include "_syscall_macros.h"

__BEGIN_DECLS

DECL_SYSCALL1(exit, int);
DECL_SYSCALL3(write, uint32_t, uint8_t const *, size_t);
DECL_SYSCALL3(read, uint32_t, uint8_t *, size_t);
DECL_SYSCALL1(sleep, int);
DECL_SYSCALL0(yield);
DECL_SYSCALL0(die);

__END_DECLS

#endif //LAMBOS_SYSCALL_H
