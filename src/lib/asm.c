#include <lib/asm.h>

inline void outb(uint16_t port, uint8_t val)
{
    asm volatile( "outb %0, %1"
                  : : "a"(val), "Nd"(port) );
}

inline void outw(uint16_t port, uint16_t val)
{
    asm volatile( "outw %0, %1"
                  : : "a"(val), "Nd"(port) );
}

inline void outl(uint16_t port, uint32_t val)
{
    asm volatile( "outl %0, %1"
                  : : "a"(val), "Nd"(port) );
}

inline void cpuid(int32_t code, uint32_t *a, uint32_t *d)
{
    asm volatile( "cpuid"
                  : "=a"(*a), "=d"(*d) : "0"(code) : "ebx", "ecx");
}

inline uint8_t inb(uint16_t port)
{
    unsigned char ret;
    asm volatile( "inb %1, %0"
                  : "=a"(ret) : "Nd"(port) );
    return ret;
}

inline uint16_t inw(uint16_t port)
{
    unsigned char ret;
    asm volatile( "inw %1, %0"
                  : "=a"(ret) : "Nd"(port) );
    return ret;
}

inline uint32_t inl(uint16_t port)
{
    unsigned char ret;
    asm volatile( "inl %1, %0"
                  : "=a"(ret) : "Nd"(port) );
    return ret;
}
