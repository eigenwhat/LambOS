#include <mem/liballoc.h>
#include <Kernel.hpp>

int liballoc_lock()
{
    return 0;
}

int liballoc_unlock()
{
    return 0;
}

extern void *liballoc_alloc(size_t numPages)
{
    return kernel->mmu()->palloc(numPages);
}

extern int liballoc_free(void *ptr, size_t numPages)
{
    return kernel->mmu()->pfree(ptr, numPages);
}

void *operator new(size_t size)
{
    return kmalloc(size);
}

void *operator new[](size_t size)
{
    return kmalloc(size);
}

void operator delete(void *p)
{
    kfree(p);
}

void operator delete[](void *p)
{
    kfree(p);
}
