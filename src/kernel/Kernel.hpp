#pragma once
#include <stddef.h>
#include <stdint.h>
#include "device/display/Terminal.hpp"
#include <mem/MMU.hpp>

class Kernel
{
public:
	Kernel() : _terminal(NULL) {}

    // accessors/mutators
	void setTerminal(Terminal *terminal) { this->_terminal = terminal; }
	Terminal* terminal() { return this->_terminal; }
    void setMMU(MMU *mmu) { _mmu = mmu; }
    MMU *mmu() { return this->_mmu; }

    // methods
	void panic(const char *errorMessage);
private:
	Terminal *_terminal;
    MMU *_mmu;
};

extern Kernel *kernel;
