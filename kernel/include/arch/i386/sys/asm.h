#ifndef ASM_H
#define ASM_H

#include <decl.h>
#include <stddef.h>
#include <stdint.h>

__BEGIN_DECLS

static inline void halt()
{
    asm volatile("hlt");
}

static inline void outb(int intPort, uint8_t val)
{
    uint16_t port = (uint16_t)intPort;
    asm volatile( "outb %0, %1"
                  : : "a"(val), "Nd"(port) );
}

static inline void outw(int intPort, uint16_t val)
{
    uint16_t port = (uint16_t)intPort;
    asm volatile( "outw %0, %1"
                  : : "a"(val), "Nd"(port) );
}

static inline void outl(int intPort, uint32_t val)
{
    uint16_t port = (uint16_t)intPort;
    asm volatile( "outl %0, %1"
                  : : "a"(val), "Nd"(port) );
}

static inline void cpuid(int32_t code, uint32_t *a, uint32_t *d)
{
    asm volatile( "cpuid"
                  : "=a"(*a), "=d"(*d) : "0"(code) : "ebx", "ecx");
}

static inline uint8_t inb(int intPort)
{
    uint16_t port = (uint16_t)intPort;
    uint8_t ret;
    asm volatile( "inb %1, %0"
                  : "=a"(ret) : "Nd"(port) );
    return ret;
}

static inline uint16_t inw(int intPort)
{
    uint16_t port = (uint16_t)intPort;
    uint16_t ret;
    asm volatile( "inw %1, %0"
                  : "=a"(ret) : "Nd"(port) );
    return ret;
}

static inline uint32_t inl(int intPort)
{
    uint16_t port = (uint16_t)intPort;
    uint32_t ret;
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
