#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdint.h>
#include <decl.h>

__BEGIN_DECLS

size_t strlen(char const* str);
char *strcpy(char *destination, char const *source);
char *strncpy(char *dst, char const *src, size_t num);

void *memset(void *ptr, int value, size_t num);
void *memcpy(void *dst, void const *src, size_t num);
void *memmove(void *dst, void const *src, size_t num);

__END_DECLS

#endif
