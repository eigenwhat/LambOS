#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdint.h>
#include <decl.h>

__BEGIN_DECLS

__LIBC_CONSTEXPR size_t strlen(char const *str);
__LIBC_CONSTEXPR char *strcpy(char *destination, char const *source);
__LIBC_CONSTEXPR char *strncpy(char *dst, char const *src, size_t num);
__LIBC_CONSTEXPR int strcmp(char const *str1, char const *str2);
__LIBC_CONSTEXPR int strncmp(char const *str1, char const *str2, size_t num);
char *strtok(char * __restrict str, char const * __restrict delim);

void *memset(void *ptr, int value, size_t num);
__LIBC_CONSTEXPR void *memcpy(void *dst, void const *src, size_t num);
__LIBC_CONSTEXPR void *memmove(void *dst, void const *src, size_t num);

#ifdef __cplusplus
#include "bits/string-inl.h"
#endif

__END_DECLS

#endif
