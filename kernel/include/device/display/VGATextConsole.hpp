#pragma once

#include <stddef.h>
#include <stdint.h>
#include "Console.hpp"
#include "VGA4BitColor.h"

class VGATextConsole : public Console
{

public:
    VGATextConsole();

    virtual void setColor(uint8_t color);
    virtual void setForegroundColor(uint32_t fg);
    virtual void setBackgroundColor(uint32_t bg);
    virtual void putChar(char c);
    virtual void moveTo(size_t row, size_t col);
    virtual void writeString(const char *data);
    virtual void clear();
    virtual size_t width();
    virtual size_t height();
    virtual void setCursorVisible(bool isVisible);
    virtual size_t column() { return _consoleColumn; }
    virtual size_t row() { return _consoleRow; }

private:
    void scroll(size_t lines);
    virtual void putCharAt(char c, uint8_t color, size_t x, size_t y);

    bool _cursorIsVisible;
    size_t _consoleRow;
    size_t _consoleColumn;
    uint8_t _consoleColor;
    uint16_t *_consoleBuffer;
};
