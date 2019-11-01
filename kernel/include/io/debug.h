#pragma once

#include <io/PrintStream.hpp>

extern sys::PrintStream *debugOut;

inline void magicBreakpoint() { asm volatile ("xchg %bx, %bx"); }
