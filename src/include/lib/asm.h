#include <decl.h>

__BEGIN_DECLS

#define outb(port, value) \
do { \
	unsigned short pt = (port); \
	unsigned char val = (value); \
    asm volatile( "outb %0, %1" : : "a"(val), "Nd"(pt) ); \
} while(0)

__END_DECLS