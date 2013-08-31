#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdint.h>
#include <decl.h>

__BEGIN_DECLS

size_t strlen(const char* str);
char *strcpy(char *destination, const char *source);
char *strncpy(char *dst, const char *src, size_t num);

void *memset(void *ptr, int value, size_t num);
void *memcpy(void *dst, const void *src, size_t num);

__END_DECLS

#endif
