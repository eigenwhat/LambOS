#pragma once

#include <stddef.h>
#include <stdint.h>
#include "Terminal.hpp"
#include "VGA4BitColor.h"

class VGATextTerminal : public Terminal {

public:
	VGATextTerminal();

	virtual void setColor(uint8_t color);
	virtual void setForegroundColor(uint32_t fg);
	virtual void setBackgroundColor(uint32_t bg);
	virtual void putChar(char c);
	virtual void moveTo(size_t row, size_t col);
	virtual void writeString(const char* data);
	virtual void clear();
	virtual size_t width();
	virtual size_t height();
	virtual void setCursorVisible(bool isVisible);
	virtual size_t column() { return terminal_column; }
	virtual size_t row() { return terminal_row; }

private:
	virtual void putCharAt(char c, uint8_t color, size_t x, size_t y);
	bool cursorIsVisible;
	size_t terminal_row;
	size_t terminal_column;
	uint8_t terminal_color;
	uint16_t* terminal_buffer;
};
