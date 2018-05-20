#include <string.h>
#include <arch/i386/sys/asm.h>
#include <device/display/VGATextConsole.hpp>

static size_t const VGA_WIDTH = 80;
static size_t const VGA_HEIGHT = 25;

//======================================================
// Helper functions
//======================================================
uint16_t make_vgaentry(char c, uint8_t color)
{
    uint16_t c16 = c;
    uint16_t color16 = color;
    return c16 | color16 << 8;
}

void move_cursor_to(size_t row, size_t col)
{
    unsigned short position = (row * 80) + col;
    // cursor LOW port to vga INDEX register
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(position & 0xFF));
    // cursor HIGH port to vga INDEX register
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((position >> 8) & 0xFF));
}

//======================================================
// VGATextConsole
//======================================================
VGATextConsole::VGATextConsole()
{
    clear();
    setCursorVisible(_cursorIsVisible);
}

void VGATextConsole::clear()
{
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            size_t const index = y * VGA_WIDTH + x;
            _consoleBuffer[index] = make_vgaentry(' ', _consoleColor);
        }
    }

    moveTo(0, 0);
}

void VGATextConsole::setForegroundColor(uint32_t fg)
{
    this->_consoleColor = (this->_consoleColor & 0xF0) | fg;
}

void VGATextConsole::setBackgroundColor(uint32_t bg)
{
    this->_consoleColor = (this->_consoleColor & 0x0F) | (bg << 4);
}

void VGATextConsole::putCharAt(char c, size_t row, size_t col)
{
    size_t const index = row * VGA_WIDTH + col;
    _consoleBuffer[index] = make_vgaentry(c, _consoleColor);
}

void VGATextConsole::putChar(char c)
{
    if (c == '\n') {
        _consoleColumn = 0;
        if (++_consoleRow == VGA_HEIGHT) {
            scroll(1);
            moveTo(VGA_HEIGHT - 1, 0);
        }
    } else {
        putCharAt(c, _consoleRow, _consoleColumn);
        if (++_consoleColumn == VGA_WIDTH) {
            _consoleColumn = 0;
            if (++_consoleRow == VGA_HEIGHT) {
                scroll(1);
                moveTo(VGA_HEIGHT - 1, 0);
            }
        }
    }

    moveTo(_consoleRow, _consoleColumn);
}

void VGATextConsole::scroll(size_t lines)
{
    if (lines) {
        size_t old_row = _consoleRow;
        memcpy(_consoleBuffer, _consoleBuffer + (VGA_WIDTH * lines), 2 * (VGA_WIDTH * (VGA_HEIGHT - lines)));
        _consoleRow = VGA_HEIGHT - lines;
        for (uint8_t j = _consoleRow; j < VGA_HEIGHT; ++j) {
            for (uint8_t i = 0; i < VGA_WIDTH; ++i) {
                putCharAt(' ', j, i);
            }
        }
        _consoleRow = old_row;
    }
}

void VGATextConsole::moveTo(size_t row, size_t col)
{
    if (_cursorIsVisible) {
        move_cursor_to(row, col);
    }
    _consoleRow = row;
    _consoleColumn = col;
}

void VGATextConsole::writeString(char const *data)
{
    bool moveCursor = _cursorIsVisible;
    if (moveCursor) {
        setCursorVisible(false);
    }
    size_t datalen = strlen(data);
    for (size_t i = 0; i < datalen; i++) {
        putChar(data[i]);
    }
    if (moveCursor) {
        setCursorVisible(true);
    }
}

size_t VGATextConsole::width()
{
    return VGA_WIDTH;
}

size_t VGATextConsole::height()
{
    return VGA_HEIGHT;
}

void VGATextConsole::setCursorVisible(bool isVisible)
{
    _cursorIsVisible = isVisible;
    if (!_cursorIsVisible) {
        move_cursor_to(0xFF, 0xFF);
    } else {
        move_cursor_to(_consoleRow, _consoleColumn);
    }
}
