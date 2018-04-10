#include <string.h>
#include <sys/asm.h>
#include <device/display/VGATextConsole.hpp>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

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
VGATextConsole::VGATextConsole() : console_row(0), console_column(0)
{
    console_color = COLOR_LIGHT_GREY | COLOR_BLACK << 4;
    console_buffer = (uint16_t *) 0xB8000;
    setCursorVisible(false);
    clear();
}

void VGATextConsole::clear()
{
    console_row = 0;
    console_column = 0;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            console_buffer[index] = make_vgaentry(' ', console_color);
        }
    }

    moveTo(0, 0);
}

void VGATextConsole::setColor(uint8_t color)
{
    this->console_color = color;
}

void VGATextConsole::setForegroundColor(uint32_t fg)
{
    this->console_color = (this->console_color & 0xF0) | fg;
}

void VGATextConsole::setBackgroundColor(uint32_t bg)
{
    this->console_color = (this->console_color & 0x0F) | (bg << 4);
}

void VGATextConsole::putCharAt(char c, uint8_t color, size_t x, size_t y)
{

    const size_t index = y * VGA_WIDTH + x;
    console_buffer[index] = make_vgaentry(c, color);
}

void VGATextConsole::putChar(char c)
{
    if (c == '\n') {
        console_column = 0;
        if (++console_row == VGA_HEIGHT) {
            scroll(1);
            moveTo(VGA_HEIGHT - 1, 0);
        }
    } else {
        putCharAt(c, console_color, console_column, console_row);
        if (++console_column == VGA_WIDTH) {
            console_column = 0;
            if (++console_row == VGA_HEIGHT) {
                scroll(1);
                moveTo(VGA_HEIGHT - 1, 0);
            }
        }
    }

    moveTo(console_row, console_column);
}

void VGATextConsole::scroll(size_t lines)
{
    if (lines) {
        size_t old_row = console_row;
        memcpy(console_buffer, console_buffer + (VGA_WIDTH * lines), 2 * (VGA_WIDTH * (VGA_HEIGHT - lines)));
        console_row = VGA_HEIGHT - lines;
        for (uint8_t j = console_row; j < VGA_HEIGHT; ++j) {
            for (uint8_t i = 0; i < VGA_WIDTH; ++i) {
                putCharAt(' ', console_color, i, j);
            }
        }
        console_row = old_row;
    }
}

void VGATextConsole::moveTo(size_t row, size_t col)
{
    if (cursorIsVisible) {
        move_cursor_to(row, col);
    }
    console_row = row;
    console_column = col;
}

void VGATextConsole::writeString(const char *data)
{
    bool moveCursor = cursorIsVisible;
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
    cursorIsVisible = isVisible;
    if (!cursorIsVisible) {
        move_cursor_to(0xFF, 0xFF);
    } else {
        move_cursor_to(console_row, console_column);
    }
}
