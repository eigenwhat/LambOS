#include <system/asm.h>
#include <device/display/VGATextConsole.hpp>

#include <cstdint>
#include <cstring>

static std::uint16_t const VGA_WIDTH = 80;
static std::uint16_t const VGA_HEIGHT = 25;

//======================================================
// Helper functions
//======================================================
std::uint16_t make_vgaentry(char c, uint8_t color)
{
    auto c16 = static_cast<std::uint16_t>(c);
    std::uint16_t color16 = color;
    return c16 | static_cast<std::uint16_t>(color16 << 8u);
}

void move_cursor_to(std::uint16_t row, std::uint16_t col)
{
    auto position = static_cast<std::uint16_t>(row * 80 + col);
    // cursor LOW port to vga INDEX register
    outb(0x3D4, 0x0F);
    outb(0x3D5, static_cast<uint8_t>(position & 0xFFu));
    // cursor HIGH port to vga INDEX register
    outb(0x3D4, 0x0E);
    outb(0x3D5, static_cast<uint8_t>((position >> 8u)) & 0xFFu);
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
    this->_consoleColor = static_cast<uint8_t>((this->_consoleColor & 0xF0u) | fg);
}

void VGATextConsole::setBackgroundColor(uint32_t bg)
{
    this->_consoleColor = static_cast<uint8_t>((this->_consoleColor & 0x0Fu) | (bg << 4u));
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
        for (size_t j = _consoleRow; j < VGA_HEIGHT; ++j) {
            for (size_t i = 0; i < VGA_WIDTH; ++i) {
                putCharAt(' ', j, i);
            }
        }
        _consoleRow = old_row;
    }
}

void VGATextConsole::moveTo(std::size_t row, std::size_t col)
{
    if (_cursorIsVisible) {
        move_cursor_to(static_cast<std::uint16_t>(row), static_cast<std::uint16_t>(col));
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
        move_cursor_to(static_cast<std::uint16_t>(_consoleRow), static_cast<std::uint16_t>(_consoleColumn));
    }
}
