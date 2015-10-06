#ifndef STDIO_H
#define STDIO_H

#include <decl.h>
 
__BEGIN_DECLS
     
int printf(const char* __restrict, ...);
int putchar(int);
int puts(const char*);
     
__END_DECLS

#endif
