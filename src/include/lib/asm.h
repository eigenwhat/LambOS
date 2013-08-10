#ifndef ASM_H
#define ASM_H

#include <decl.h>
#include <stddef.h>
#include <stdint.h>

__BEGIN_DECLS

inline uint8_t inb(uint16_t port);
inline uint8_t inw(uint16_t port);
inline uint8_t inl(uint16_t port);

inline void outb(unsigned short port, uint8_t val);
inline void outw(unsigned short port, uint16_t val);
inline void outl(unsigned short port, uint32_t val);

inline void cpuid(int32_t code, uint32_t *a, uint32_t *d);

__END_DECLS

#endif