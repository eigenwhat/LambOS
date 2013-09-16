#pragma once

class CPU
{
public:
	virtual void install() = 0;
	virtual void enableInterrupts() = 0;
	virtual void disableInterrupts() = 0;
};
