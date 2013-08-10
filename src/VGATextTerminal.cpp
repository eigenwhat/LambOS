#include <VGATextTerminal.hpp>
#include <lib/string.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 24;

uint16_t make_vgaentry(char c, uint8_t color)
{
	uint16_t c16 = c;
	uint16_t color16 = color;
	return c16 | color16 << 8;
}

BasicTerminal::BasicTerminal()
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = COLOR_LIGHT_GREY | COLOR_BLACK << 4;
	terminal_buffer = (uint16_t*) 0xB8000;
	clear();
}

void BasicTerminal::clear()
{
	terminal_row = 0;
	terminal_column = 0;
	for ( size_t y = 0; y < VGA_HEIGHT; y++ ) {
		for ( size_t x = 0; x < VGA_WIDTH; x++ ) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = make_vgaentry(' ', terminal_color);
		}
	}
}
 
void BasicTerminal::setColor(uint8_t color)
{
	this->terminal_color = color;
}

void BasicTerminal::setForegroundColor(VGA4BitColor fg)
{
	this->terminal_color = (this->terminal_color & 0xF0) | fg;
}

void BasicTerminal::setBackgroundColor(VGA4BitColor bg)
{
	this->terminal_color = (this->terminal_color & 0x0F) | (bg << 4);
}
 
void BasicTerminal::putCharAt(char c, uint8_t color, size_t x, size_t y)
{

	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = make_vgaentry(c, color);
}
 
void BasicTerminal::putChar(char c)
{
	if(c == '\n') {
		terminal_column = 0;
		if ( ++terminal_row == VGA_HEIGHT ) {
			terminal_row = 0;
		}
	} else {
		putCharAt(c, terminal_color, terminal_column, terminal_row);
		if ( ++terminal_column == VGA_WIDTH ) {
			terminal_column = 0;
			if ( ++terminal_row == VGA_HEIGHT ) {
				terminal_row = 0;
			}
		}
	}
}
 
void BasicTerminal::writeString(const char* data)
{
	size_t datalen = strlen(data);
	for ( size_t i = 0; i < datalen; i++ ) {
		putChar(data[i]);
	}
}
