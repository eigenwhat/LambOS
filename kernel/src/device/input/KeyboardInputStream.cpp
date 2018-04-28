#include <device/input/KeyboardInputStream.hpp>

namespace {
#define NUL 0
constexpr char kKeyToChar[128] = {
        NUL,
        NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,
        '`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', NUL,
        '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\\',
        NUL, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '\n',
        NUL, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', NUL,
        NUL, NUL, NUL, ' ', NUL, NUL, NUL, NUL,
        NUL, NUL, NUL,
        NUL, NUL, NUL,    NUL, '/', '*', '-',
        NUL, NUL, NUL,    '7', '8', '9', '+',
                          '4', '5', '6',
             NUL,         '1', '2', '3', NUL,
        NUL, NUL, NUL,    '0', '.'
};

constexpr char kKeyToCharShift[128] = {
        NUL,
        NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL, NUL,
        '~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', NUL,
        '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '|',
        NUL, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '\n',
        NUL, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '\?', NUL,
        NUL, NUL, NUL, ' ', NUL, NUL, NUL, NUL,
        NUL, NUL, NUL,
        NUL, NUL, NUL,    NUL, '/', '*', '-',
        NUL, NUL, NUL,    '7', '8', '9', '+',
        '4', '5', '6',
        NUL,         '1', '2', '3', NUL,
        NUL, NUL, NUL,    '0', '.'
};

char charWithModKeys(const Keyboard &keyboard, KeyCode keyCode)
{
    bool isShiftPressed = keyboard.keyIsPressed(kKeyLeftShift)
                          || keyboard.keyIsPressed(kKeyRightShift);

    return isShiftPressed ? kKeyToCharShift[keyCode] : kKeyToChar[keyCode];
}

}

int KeyboardInputStream::read()
{
    KeyEvent event;
    do {
        event = _keyboard->read();
    } while (event.type != kKeyEventPressed || kKeyToChar[event.keyCode] == 0);
    return charWithModKeys(*_keyboard, event.keyCode);
}
