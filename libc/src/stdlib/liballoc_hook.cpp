//
// Created by Martin Miralles-Cordal on 10/20/19.
//

#include <decl.h>
#include "liballoc.h"

#ifdef KERNEL
#include <Kernel.hpp>
#endif

__BEGIN_DECLS

int liballoc_lock() { return 0; }

int liballoc_unlock() { return 0; }

#ifdef KERNEL
void *liballoc_alloc(size_t numPages) { return kernel->mmu()->palloc(numPages); }
int liballoc_free(void *ptr, size_t numPages) { return kernel->mmu()->pfree(ptr, numPages); }
#else
void *liballoc_alloc([[maybe_unused]] size_t numPages) { return nullptr; }
int liballoc_free([[maybe_unused]] void *ptr, [[maybe_unused]] size_t numPages) { return 0; }
#endif

__END_DECLS
