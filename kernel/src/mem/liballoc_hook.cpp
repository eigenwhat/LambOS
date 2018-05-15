#include <mem/liballoc.h>
#include <Kernel.hpp>

extern "C" {

int liballoc_lock()
{
    return 0;
}

int liballoc_unlock()
{
    return 0;
}

void *liballoc_alloc(size_t numPages)
{
    return kernel->mmu()->palloc(numPages);
}

int liballoc_free(void *ptr, size_t numPages)
{
    return kernel->mmu()->pfree(ptr, numPages);
}

} // extern "C"

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

void operator delete(void *p, long unsigned int)
{
    kfree(p);
}

void operator delete[](void *p)
{
    kfree(p);
}

void operator delete[](void *p, long unsigned int)
{
    kfree(p);
}