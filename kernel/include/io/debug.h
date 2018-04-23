#pragma once

#include <io/PrintStream.hpp>

extern PrintStream *debugOut;

inline void magicBreakpoint() { asm volatile ("xchg %bx, %bx"); }
