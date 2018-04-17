#include <device/input/PS2Keyboard.hpp>

#include <sys/asm.h>
#include <string.h>

PS2Keyboard::PS2Keyboard() : _buffer(128)
{
    memset(_keysPressed, 0, sizeof(bool) * 256);
}



static constexpr KeyCode scan1_to_key[70] = {
    kKeyNull,       kKeyEscape,   kKey1,         kKey2,         kKey3,          kKey4,           kKey5,           kKey6,            kKey7,        kKey8,
    kKey9,          kKey0,        kKeyDash,      kKeyEquals,    kKeyBackspace,  kKeyTab,         kKeyQ,           kKeyW,            kKeyE,        kKeyR,
    kKeyT,          kKeyY,        kKeyU,         kKeyI,         kKeyO,          kKeyP,           kKeyLeftBracket, kKeyRightBracket, kKeyEnter,    kKeyLeftControl,
    kKeyA,          kKeyS,        kKeyD,         kKeyF,         kKeyG,          kKeyH,           kKeyJ,           kKeyK,            kKeyL,        kKeySemicolon,
    kKeyApostrophe, kKeyBacktick, kKeyLeftShift, kKeyBackslash, kKeyZ,          kKeyX,           kKeyC,           kKeyV,            kKeyB,        kKeyN,
    kKeyM,          kKeyComma,    kKeyPeriod,    kKeySlash,     kKeyRightShift, kKeyPadAsterisk, kKeyLeftAlt,     kKeySpace,        kKeyCapsLock, kKeyF1,
    kKeyF2,         kKeyF3,       kKeyF4,        kKeyF5,        kKeyF6,         kKeyF7,          kKeyF8,          kKeyF9,           kKeyF10,      kKeyNumLock
};

void PS2Keyboard::pushScanCode(uint8_t scancode)
{
    _buffer.enqueue(scancode);
}

KeyEvent PS2Keyboard::read()
{
    KeyEvent retval;
    while (_buffer.isEmpty()) { halt(); }
    int scancode = _buffer.dequeue();
    if ((scancode & 128) == 128) {
        retval.type = kKeyEventReleased;
    } else {
        retval.type = kKeyEventPressed;
    }

    scancode = scancode & 0x7F;

    if (scancode < 70) {
        retval.keyCode = scan1_to_key[scancode];
    } else {
        retval.keyCode = kKeyNull;
    }

    _keysPressed[retval.keyCode] = retval.type == kKeyEventPressed ? true : false;

    return retval;
}
