#include <mem/liballoc.h>
#include <Kernel.hpp>

int liballoc_lock()
{
	asm volatile ("cli");

	return 0;
}

int liballoc_unlock()
{
	//asm volatile ("sti");

	return 0;
}

extern void* liballoc_alloc(size_t numPages)
{
	return kernel->mmu()->palloc(numPages);
}

extern int liballoc_free(void* ptr, size_t numPages)
{
	return kernel->mmu()->pfree(ptr, numPages);
}
