#pragma once

#include <stddef.h>
#include <stdint.h>
#include "VGA4BitColor.h"

class Console
{
public:
    /**
     * Sets the foreground color for future writes.
     * @param fg The color to use.
     */
    virtual void setForegroundColor(uint32_t fg) = 0;

    /**
     * Sets the background color for future writes.
     * @param bg The color to use.
     */
    virtual void setBackgroundColor(uint32_t bg) = 0;

    /**
     * Writes a single character to the console at the cursor's position,
     * advancing the cursor.
     * @param c The character to write.
     */
    virtual void putChar(char c) = 0;

    /**
     * Moves the cursor to the given position.
     * @param row The row to move to.
     * @param col The column to move to.
     */
    virtual void moveTo(size_t row, size_t col) = 0;

    /**
     * Writes a string at the cursor's position, advancing the cursor that many
     * spaces.
     * @param data The string of characters to write.
     */
    virtual void writeString(char const *data) = 0;

    /** Clears the console. */
    virtual void clear() = 0;

    /** The width of the console. */
    virtual size_t width() = 0;

    /** The height of the console. */
    virtual size_t height() = 0;

    /**
     * Sets the visibility of the cursor.
     * @param isVisible Whether or not it should be visible.
     */
    virtual void setCursorVisible(bool isVisible) = 0;

    /** The column at which the cursor is. */
    virtual size_t column() = 0;

    /** The row at which the cursor is. */
    virtual size_t row() = 0;

    /**
     * Puts a character with the given color at the given spot. The cursor is
     * NOT changed.
     * @param c The character to place.
     * @param row The row.
     * @param col The column.
     */
    virtual void putCharAt(char c, size_t row, size_t col) = 0;
};
