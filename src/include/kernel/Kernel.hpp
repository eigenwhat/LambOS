#pragma once
#include <stddef.h>
#include <stdint.h>
#include <device/display/ITerminal.hpp>

typedef struct registers
{
   uint32_t ds;                  // Data segment selector
   uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
   uint32_t int_no, err_code;    // Interrupt number and error code (if applicable)
   uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
} __attribute__((packed)) registers_t;

class Kernel
{
public:
	Kernel() : _stdout(NULL) {}
	void setStdout(ITerminal *terminal) { this->_stdout = terminal; }
	ITerminal* stdout() { return this->_stdout; }
	void panic(const char *errorMessage);
private:
	ITerminal *_stdout;
};

