#ifndef STDIO_H
#define STDIO_H

#include <decl.h>
#include <stdarg.h>

#define EOF -1

__BEGIN_DECLS

int getchar();
char *gets(char *str);

int putchar(int);
int puts(char const*);
int printf(char const * __restrict, ...);
int sprintf(char *str, char const * __restrict, ...);

int vprintf(char const * __restrict, va_list parameters);
int vsprintf(char *str, char const * __restrict, va_list parameters);
     
__END_DECLS

#endif
