#pragma once
#include <stddef.h>
#include <stdint.h>
#include "device/display/ITerminal.hpp"

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

extern Kernel *kernel;
