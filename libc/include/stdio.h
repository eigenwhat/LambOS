#ifndef STDIO_H
#define STDIO_H

#include <decl.h>
#include <stdarg.h>

__BEGIN_DECLS

int putchar(int);
int puts(const char*);
int printf(const char * __restrict, ...);
int sprintf(char *str, const char * __restrict, ...);

int vprintf(const char * __restrict, va_list parameters);
int vsprintf(char *str, const char * __restrict, va_list parameters);
     
__END_DECLS

#endif
