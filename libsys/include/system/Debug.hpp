//
// Created by Martin Miralles-Cordal on 10/14/2015.
//

#pragma once

#include <io/PrintStream.hpp>

extern sys::PrintStream *debugOut;

namespace sys {

template <typename... Ts>
void debug_print(char const *format, Ts&&...args) { print(*debugOut, format, std::forward<Ts>(args)...); }

} // namespace sys

#define DEBUG_BREAK() do { sys::debug_print("BREAK @ %@:%@\n", __FILE__, __LINE__); asm volatile ("xchg %bx, %bx"); } while (0)
