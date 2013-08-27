#pragma once
#include <stddef.h>
#include <stdint.h>
#include "device/display/ITerminal.hpp"
#include <mem/PageFrameAllocator.hpp>

class Kernel
{
public:
	Kernel() : _stdout(NULL) {}

    // accessors/mutators
	void setOut(ITerminal *terminal) { this->_stdout = terminal; }
	ITerminal* out() { return this->_stdout; }
    void setFrameAllocator(PageFrameAllocator *allocator) { _frameAllocator = allocator; }
    PageFrameAllocator *frameAllocator() { return this->_frameAllocator; }

    // methods
	void panic(const char *errorMessage);
private:
	ITerminal *_stdout;
    PageFrameAllocator *_frameAllocator;
};

extern Kernel *kernel;
