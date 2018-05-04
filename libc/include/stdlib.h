#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <decl.h>

__BEGIN_DECLS

#define MIN(a, b) (a) > (b) ? (b) : (a)
#define MAX(a, b) (a) > (b) ? (a) : (b)
#define CLAMP(num, min, max) MAX(MIN((num), (max)), (min))

/**
 * Converts int to string. Nonstandard.
 * @param value The int to convert.
 * @param str Pointer to the C string to populate.
 * @param base The base to convert to.
 * @return Pointer to the produced C string. (This is equal to `str`.)
 */
char *itoa(int value, char *str, int base);

/**
 * Allocates a block of memory of `size` bytes.
 * @param size The size of the memory block to allocate.
 * @return A pointer to the allocated block, or NULL if it failed.
 */
void *malloc(size_t size);

/**
 * Changes the size of the memory block pointed to by ptr. It may be relocated;
 * the pointer to the final address is returned.
 * @param ptr The pointer to the memory block to resize. If the pointer is null,
 *            the result is equivalent to malloc(size).
 * @param size The new size of the memory block, in bytes.
 * @return A pointer to the reallocated memory block, which may or may not have
 *         changed, or NULL if it failed (the original block is left intact).
 */
void *realloc(void *ptr, size_t size);

/**
 * Allocates a block of memory for an array of `num` elements `size` bytes long
 * and initializes all bits to 0.
 * @param num The number of elements to allocate.
 * @param size The size of each element.
 * @return A pointer to the allocated block, or NULL if it failed.
 */
void *calloc(size_t num, size_t size);

/**
 * Frees a previously allocated block of memory.
 * @param ptr The pointer to the memory block to free.
 * @note If you call free on something that wasn't previously allocated or
 *       already freed, you're going to have a bad time.
 */
void free(void *ptr);

__END_DECLS

#endif