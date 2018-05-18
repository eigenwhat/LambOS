#ifndef LAMBOS_SYSCALL_H
#define LAMBOS_SYSCALL_H

#include <decl.h>
#include <stddef.h>
#include <stdint.h>

#include "_syscall_macros.h"

__BEGIN_DECLS

DECL_SYSCALL3(write, uint32_t, uint8_t const *, size_t);

__END_DECLS

#endif //LAMBOS_SYSCALL_H
