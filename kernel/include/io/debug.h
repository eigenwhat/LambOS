#pragma once

#include <io/PrintStream.hpp>

extern sys::PrintStream *debugOut;

template <typename... Ts>
void debug_print(char const *format, Ts&&...args) { sys::print(*debugOut, format, std::forward<Ts>(args)...); }

#define DEBUG_BREAK() do { debug_print("BREAK @ %@:%@\n", __FILE__, __LINE__); asm volatile ("xchg %bx, %bx"); } while (0)