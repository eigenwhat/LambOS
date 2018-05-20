#pragma once

#include <stddef.h>
#include <stdint.h>
#include "Console.hpp"
#include "VGA4BitColor.h"

class VGATextConsole : public Console
{
  public:
    static constexpr uint8_t kDefaultTextColor = COLOR_LIGHT_GREY;
    static constexpr uint8_t kDefaultBackgroundColor = COLOR_BLACK;

    VGATextConsole();

    /**
     * Sets the foreground color for future writes.
     * @param fg The VGA color code to use.
     */
    void setForegroundColor(uint32_t fg) override;

    /**
     * Sets the background color for future writes.
     * @param bg The VGA color code to use.
     */
    void setBackgroundColor(uint32_t bg);

    /**
     * Writes a single character to the console at the cursor's position,
     * advancing the cursor.
     * @param c The character to write.
     */
    void putChar(char c);

    /**
     * Moves the cursor to the given position.
     * @param row The row to move to.
     * @param col The column to move to.
     */
    void moveTo(size_t row, size_t col);

    /**
     * Writes a string at the cursor's position, advancing the cursor that many
     * spaces.
     * @param data The string of characters to write.
     */
    void writeString(char const *data);

    /** Clears the console. */
    void clear();

    /** The width of the console. */
    size_t width();

    /** The height of the console. */
    size_t height();

    /**
     * Sets the visibility of the cursor.
     * @param isVisible Whether or not it should be visible.
     */
    void setCursorVisible(bool isVisible);

    /** The column at which the cursor is. */
    size_t column() override { return _consoleColumn; }

    /** The row at which the cursor is. */
    size_t row() override { return _consoleRow; }

    /**
     * Puts a character with the given color at the given spot. The cursor is
     * NOT changed.
     * @param c The character to place.
     * @param row The row.
     * @param col The column.
     */
    void putCharAt(char c, size_t row, size_t col) override;

  private:
    void scroll(size_t lines);

    bool _cursorIsVisible = false;
    size_t _consoleRow = 0;
    size_t _consoleColumn = 0;
    uint8_t _consoleColor = kDefaultTextColor | kDefaultBackgroundColor << 4;
    uint16_t *_consoleBuffer = (uint16_t *)0xB8000;
};
