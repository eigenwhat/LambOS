#pragma once

#include <stddef.h>
#include <stdint.h>
#include "VGA4BitColor.h"

class Console
{
public:
    virtual void setColor(uint8_t color) = 0;
    virtual void setForegroundColor(uint32_t fg) = 0;
    virtual void setBackgroundColor(uint32_t bg) = 0;
    virtual void putChar(char c) = 0;
    virtual void moveTo(size_t row, size_t col) = 0;
    virtual void writeString(const char *data) = 0;
    virtual void clear() = 0;
    virtual size_t width() = 0;
    virtual size_t height() = 0;
    virtual void setCursorVisible(bool isVisible) = 0;
    virtual size_t column() = 0;
    virtual size_t row() = 0;
};
