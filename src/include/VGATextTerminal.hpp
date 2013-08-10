#pragma once

#include <stddef.h>
#include <stdint.h>
#include "ITerminal.hpp"
#include "VGA4BitColor.h"

class BasicTerminal : ITerminal {

public:
	BasicTerminal();

	virtual void setColor(uint8_t color);
	virtual void setForegroundColor(VGA4BitColor fg);
	virtual void setBackgroundColor(VGA4BitColor bg);
	virtual void putChar(char c);
	virtual void putCharAt(char c, uint8_t color, size_t x, size_t y);
	virtual void writeString(const char* data);
	virtual void clear();

private:
	size_t terminal_row;
	size_t terminal_column;
	uint8_t terminal_color;
	uint16_t* terminal_buffer;
};
