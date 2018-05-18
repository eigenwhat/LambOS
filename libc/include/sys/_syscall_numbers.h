#ifndef LAMBOS_SYSCALL_NUMBERS_H
#define LAMBOS_SYSCALL_NUMBERS_H

#include <decl.h>

__BEGIN_DECLS

typedef struct {
    enum {
        kOpen,
        kRead,
        kWrite,
        kClose,
        kExit,
    };
} SyscallId;

__END_DECLS

#endif //LAMBOS_SYSCALL_NUMBERS_H
