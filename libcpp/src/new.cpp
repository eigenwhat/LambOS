//
// Created by Martin Miralles-Cordal on 10/20/19.
//

#include <new>

#include <cstdlib>

#ifdef KERNEL
__BEGIN_DECLS
void *kmalloc(size_t);                ///< The standard function.
void *krealloc(void *, size_t);       ///< The standard function.
void *kcalloc(size_t, size_t);        ///< The standard function.
void  kfree(void *);                  ///< The standard function.
__END_DECLS
#endif

#ifndef KERNEL
#define MALLOC std::malloc
#define FREE   std::free
#else
#define MALLOC kmalloc
#define FREE   kfree
#endif

void *operator new  (std::size_t size) { return MALLOC(size); }
void *operator new[](std::size_t size) { return MALLOC(size); }
void *operator new  (std::size_t size, [[maybe_unused]] std::align_val_t alignment) { return MALLOC(size); }
void *operator new[](std::size_t size, [[maybe_unused]] std::align_val_t alignment) { return MALLOC(size); }

void *operator new  (std::size_t size, const std::nothrow_t&) noexcept { return MALLOC(size); }
void *operator new[](std::size_t size, const std::nothrow_t&) noexcept { return MALLOC(size); }
void *operator new  (std::size_t size, [[maybe_unused]] std::align_val_t alignment, const std::nothrow_t&) noexcept { return MALLOC(size); }
void *operator new[](std::size_t size, [[maybe_unused]] std::align_val_t alignment, const std::nothrow_t&) noexcept { return MALLOC(size); }

void operator delete  (void* ptr) noexcept { FREE(ptr); }
void operator delete[](void* ptr) noexcept { FREE(ptr); }
void operator delete  (void* ptr, [[maybe_unused]] std::size_t size) noexcept { FREE(ptr); }
void operator delete[](void* ptr, [[maybe_unused]] std::size_t size) noexcept { FREE(ptr); }
void operator delete  (void* ptr, [[maybe_unused]] std::align_val_t alignment) noexcept { FREE(ptr); }
void operator delete[](void* ptr, [[maybe_unused]] std::align_val_t alignment) noexcept { FREE(ptr); }
void operator delete  (void* ptr, [[maybe_unused]] std::size_t size, [[maybe_unused]] std::align_val_t alignment) noexcept { FREE(ptr); }
void operator delete[](void* ptr, [[maybe_unused]] std::size_t size, [[maybe_unused]] std::align_val_t alignment) noexcept { FREE(ptr); }

void operator delete  (void* ptr, const std::nothrow_t&) noexcept { FREE(ptr); }
void operator delete[](void* ptr, const std::nothrow_t&) noexcept { FREE(ptr); }
void operator delete  (void* ptr, [[maybe_unused]] std::align_val_t alignment, const std::nothrow_t&) noexcept { FREE(ptr); }
void operator delete[](void* ptr, [[maybe_unused]] std::align_val_t alignment, const std::nothrow_t&) noexcept { FREE(ptr); }
