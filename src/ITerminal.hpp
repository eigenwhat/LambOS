#pragma once

#include <stddef.h>
#include <stdint.h>
#include "VGA4BitColor.h"

class ITerminal {
	public:
		virtual void setColor(uint8_t color) = 0;
		virtual void setForegroundColor(VGA4BitColor fg) = 0;
		virtual void setBackgroundColor(VGA4BitColor bg) = 0;
		virtual void putChar(char c) = 0;
		virtual void putCharAt(char c, uint8_t color, size_t x, size_t y) = 0;
		virtual void writeString(const char* data) = 0;
};
