#include <device/input/KeyboardInputStream.hpp>
#include <sys/asm.h>
#include <sys/keys.h>

char key_to_char[128] = {
    0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\\',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '\n',
    0, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0,
    0, 0, 0, ' ', 0, 0, 0,
    '*', 0
};

int KeyboardInputStream::read()
{
    KeyEvent event;
    do {
        event = _keyboard.read();
    } while(event.type != kKeyEventPressed);
    return key_to_char[event.keyCode];
}
