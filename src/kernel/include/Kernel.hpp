#pragma once
#include <stddef.h>
#include <stdint.h>
#include <device/display/Terminal.hpp>
#include <io/PrintStream.hpp>
#include <mem/MMU.hpp>

class Kernel
{
public:
	Kernel() : _terminal(NULL), _mmu(NULL), _out(NULL) {}

    // accessors/mutators
	void setTerminal(Terminal *terminal) { this->_terminal = terminal; }
	Terminal* terminal() { return this->_terminal; }
    void setMMU(MMU *mmu) { _mmu = mmu; }
    MMU *mmu() { return this->_mmu; }
    void setOut(PrintStream *out) { this->_out = out; }
    PrintStream *out() { return this->_out; }

    // methods
	void panic(const char *errorMessage);
private:
	Terminal *_terminal;
    MMU *_mmu;
    PrintStream *_out;
};

extern Kernel *kernel;
