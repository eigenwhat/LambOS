#ifndef ASM_H
#define ASM_H

#include <decl.h>
#include <stddef.h>
#include <stdint.h>

__BEGIN_DECLS

static inline void outb(uint16_t port, uint8_t val)
{
    asm volatile( "outb %0, %1"
                  : : "a"(val), "Nd"(port) );
}

static inline void outw(uint16_t port, uint16_t val)
{
    asm volatile( "outw %0, %1"
                  : : "a"(val), "Nd"(port) );
}

static inline void outl(uint16_t port, uint32_t val)
{
    asm volatile( "outl %0, %1"
                  : : "a"(val), "Nd"(port) );
}

static inline void cpuid(int32_t code, uint32_t *a, uint32_t *d)
{
    asm volatile( "cpuid"
                  : "=a"(*a), "=d"(*d) : "0"(code) : "ebx", "ecx");
}

static inline uint8_t inb(uint16_t port)
{
    unsigned char ret;
    asm volatile( "inb %1, %0"
                  : "=a"(ret) : "Nd"(port) );
    return ret;
}

static inline uint16_t inw(uint16_t port)
{
    unsigned char ret;
    asm volatile( "inw %1, %0"
                  : "=a"(ret) : "Nd"(port) );
    return ret;
}

static inline uint32_t inl(uint16_t port)
{
    unsigned char ret;
    asm volatile( "inl %1, %0"
                  : "=a"(ret) : "Nd"(port) );
    return ret;
}

static inline void io_wait( void )
{
    asm volatile( "jmp 1f\n\t"
                  "1:jmp 2f\n\t"
                  "2:" );
}

static inline void invlpg(uint32_t m)
{
    /* Clobber memory to avoid optimizer re-ordering access before invlpg, which may cause nasty bugs. */
    asm volatile ( "invlpg (%0)" : : "b"(m) : "memory" );
}

__END_DECLS

#endif
