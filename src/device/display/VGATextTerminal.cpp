#include <device/display/VGATextTerminal.hpp>
#include <string.h>
#include <lib/asm.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

uint16_t make_vgaentry(char c, uint8_t color)
{
	uint16_t c16 = c;
	uint16_t color16 = color;
	return c16 | color16 << 8;
}

void move_cursor_to(size_t row, size_t col)
{
	unsigned short position=(row*80) + col;
	// cursor LOW port to vga INDEX register
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(position&0xFF));
    // cursor HIGH port to vga INDEX register
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((position>>8)&0xFF));
}

VGATextTerminal::VGATextTerminal() : terminal_row(0), terminal_column(0)
{
	terminal_color = COLOR_LIGHT_GREY | COLOR_BLACK << 4;
	terminal_buffer = (uint16_t*) 0xB8000;
	setCursorVisible(false);
	clear();
}

void VGATextTerminal::clear()
{
	terminal_row = 0;
	terminal_column = 0;
	for ( size_t y = 0; y < VGA_HEIGHT; y++ ) {
		for ( size_t x = 0; x < VGA_WIDTH; x++ ) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = make_vgaentry(' ', terminal_color);
		}
	}

	moveTo(0,0);
}
 
void VGATextTerminal::setColor(uint8_t color)
{
	this->terminal_color = color;
}

void VGATextTerminal::setForegroundColor(uint32_t fg)
{
	this->terminal_color = (this->terminal_color & 0xF0) | fg;
}

void VGATextTerminal::setBackgroundColor(uint32_t bg)
{
	this->terminal_color = (this->terminal_color & 0x0F) | (bg << 4);
}
 
void VGATextTerminal::putCharAt(char c, uint8_t color, size_t x, size_t y)
{

	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = make_vgaentry(c, color);
	moveTo(terminal_row, terminal_column);
}
 
void VGATextTerminal::putChar(char c)
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

void VGATextTerminal::moveTo(size_t row, size_t col)
{
 	if(cursorIsVisible) {
 		move_cursor_to(row, col);
	}
    terminal_row = row;
    terminal_column = col;
}
 
void VGATextTerminal::writeString(const char* data)
{
	bool moveCursor = cursorIsVisible;
	if(moveCursor) {
		setCursorVisible(false);
	}
	size_t datalen = strlen(data);
	for ( size_t i = 0; i < datalen; i++ ) {
		putChar(data[i]);
	}
	if(moveCursor) {
		setCursorVisible(true);
	}
}

size_t VGATextTerminal::width()
{
	return VGA_WIDTH;
}

size_t VGATextTerminal::height()
{
	return VGA_HEIGHT;
}

void VGATextTerminal::setCursorVisible(bool isVisible) {
	cursorIsVisible = isVisible;
	if(!cursorIsVisible) {
		move_cursor_to(0xFF, 0xFF);
	} else {
		move_cursor_to(terminal_row, terminal_column);
	}
}
