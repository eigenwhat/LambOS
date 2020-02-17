//
// Created by Martin Miralles-Cordal on 10/14/2015.
//

#pragma once

#include <io/OutputStream.hpp>
#include <io/Print.hpp>

#include <system/asm.h>

namespace sys {

class BochsDebugOutputStream : public OutputStream
{
  public:
    virtual void write(std::byte byte) { outb(0xe9, (uint8_t)byte); }
};

template <typename... Ts>
void debug_print(char const *format, Ts&&...args) { BochsDebugOutputStream o{}; print(o, format, std::forward<Ts>(args)...); }
template <typename... Ts>
void debug_println(char const *format, Ts&&...args) { BochsDebugOutputStream o{}; println(o, format, std::forward<Ts>(args)...); }

} // namespace sys

#define DEBUG_BREAK() do { sys::debug_print("BREAK @ %@:%@\n", __FILE__, __LINE__); asm volatile ("xchg %bx, %bx"); } while (0)

#undef TRACE_FN
#define TRACE_FN() sys::debug_print("TRACE: %@ %@:%@\n", __FUNCTION__, __FILE__, __LINE__)
