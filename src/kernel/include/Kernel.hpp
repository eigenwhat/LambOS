#pragma once
#include <stddef.h>
#include <stdint.h>
#include <device/display/Console.hpp>
#include <io/PrintStream.hpp>
#include <mem/MMU.hpp>
#include <cpu/CPU.hpp>

class Kernel
{
public:
	Kernel() : _cpu(NULL), _console(NULL), _mmu(NULL), _out(NULL) {}

    // accessors/mutators
	void setConsole(Console *console) { this->_console = console; }
	Console* console() { return this->_console; }
    void setMMU(MMU *mmu) { _mmu = mmu; }
    MMU *mmu() { return this->_mmu; }
    void setCPU(CPU *cpu) { _cpu = cpu; }
    CPU *cpu() { return this->_cpu; }
    void setOut(PrintStream *out) { this->_out = out; }
    PrintStream *out() { return this->_out; }

    // methods
	void panic(const char *errorMessage);
private:
    CPU *_cpu;
	Console *_console;
    MMU *_mmu;
    PrintStream *_out;
};

extern Kernel *kernel;
