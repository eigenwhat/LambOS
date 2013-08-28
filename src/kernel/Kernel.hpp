#pragma once
#include <stddef.h>
#include <stdint.h>
#include "device/display/ITerminal.hpp"
#include <mem/MMU.hpp>

class Kernel
{
public:
	Kernel() : _out(NULL) {}

    // accessors/mutators
	void setOut(ITerminal *terminal) { this->_out = terminal; }
	ITerminal* out() { return this->_out; }
    void setMMU(MMU *mmu) { _mmu = mmu; }
    MMU *mmu() { return this->_mmu; }

    // methods
	void panic(const char *errorMessage);
private:
	ITerminal *_out;
    MMU *_mmu;
};

extern Kernel *kernel;
