#include <device/input/KeyboardInputStream.hpp>
#include <sys/asm.h>
#include <sys/keys.h>
#include <ctype.h>

char key_to_char[128] = {
    0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\\',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '\n',
    0, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    0, 0, 0, ' ', 0, 0, 0,
    '*', 0
};

char key_to_char_shift[128] = {
    0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '|',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '\n',
    0, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '\?', 0,
    0, 0, 0, ' ', 0, 0, 0,
    '*', 0
};

char shift_digit[10] = { ')','!','@','#','$','%','^','&','*','(' };

int KeyboardInputStream::read()
{
    KeyEvent event;
    do {
        event = _keyboard.read();
    } while(event.type != kKeyEventPressed || key_to_char[event.keyCode] == 0);

    char ret = key_to_char[event.keyCode];

    if(_keyboard.keyIsPressed(kKeyLeftShift) || _keyboard.keyIsPressed(kKeyRightShift)) {
        ret = key_to_char_shift[event.keyCode];
    }
    
    return ret;
}
