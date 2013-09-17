#include <device/input/PS2Keyboard.hpp>
#include <sys/asm.h>

#define KBD_DEVICE    0x60
#define KEY_PENDING   0x64

static KeyCode scan1_to_key[70] = {
    kKeyNull,       kKeyEscape,   kKey1,         kKey2,         kKey3,          kKey4,           kKey5,           kKey6,            kKey7,        kKey8,
    kKey9,          kKey0,        kKeyDash,      kKeyEquals,    kKeyBackspace,  kKeyTab,         kKeyQ,           kKeyW,            kKeyE,        kKeyR, 
    kKeyT,          kKeyY,        kKeyU,         kKeyI,         kKeyO,          kKeyP,           kKeyLeftBracket, kKeyRightBracket, kKeyEnter,    kKeyLeftControl, 
    kKeyA,          kKeyS,        kKeyD,         kKeyF,         kKeyG,          kKeyH,           kKeyJ,           kKeyK,            kKeyL,        kKeySemicolon,
    kKeyApostrophe, kKeyBacktick, kKeyLeftShift, kKeyBackslash, kKeyZ,          kKeyX,           kKeyC,           kKeyV,            kKeyB,        kKeyN,
    kKeyM,          kKeyComma,    kKeyPeriod,    kKeySlash,     kKeyRightShift, kKeyPadAsterisk, kKeyLeftAlt,     kKeySpace,        kKeyCapsLock, kKeyF1,
    kKeyF2,         kKeyF3,       kKeyF4,        kKeyF5,        kKeyF6,         kKeyF7,          kKeyF8,          kKeyF9,           kKeyF10,      kKeyNumLock
};

static KeyCode scan2_to_key[48] = {
    kKeyNull, kKeyF9,      kKeyNull,      kKeyF5,   kKeyF3,          kKeyF1, kKeyF2, kKeyF12,  kKeyNull, kKeyF10,  kKeyF8, kKeyF6, kKeyF4, kKeyTab, kKeyBacktick, kKeyNull,
    kKeyNull, kKeyLeftAlt, kKeyLeftShift, kKeyNull, kKeyLeftControl, kKeyQ,  kKey1,  kKeyNull, kKeyNull, kKeyNull, kKeyZ,  kKeyS,  kKeyA,  kKeyW,   kKey2,        kKeyNull,
    kKeyNull, kKeyC,       kKeyX,         kKeyD,    kKeyE,           kKey4,  kKey3,  kKeyNull, kKeyNull, kKeySpace, kKeyV, kKeyF,  kKeyT,  kKeyR,   kKey5,        kKeyNull
};

class KeyboardISR : public InterruptServiceRoutine
{
public:
    KeyboardISR(PS2Keyboard &keyboard) : _keyboard(keyboard) {}
    virtual void operator()(RegisterTable&) {
        while(inb(KEY_PENDING) & 2);
        uint8_t scancode = inb(KBD_DEVICE);

        _keyboard.pushScanCode(scancode);
        outb(0x20, 0x20);
    }
private:
    PS2Keyboard &_keyboard;
};

PS2Keyboard::PS2Keyboard(X86CPU &cpu) : _buffer(128), _cpu(cpu) 
{
    _cpu.idt()->setISR(kIntKeyboardIRQ, new KeyboardISR(*this));
}

void PS2Keyboard::pushScanCode(uint8_t scancode)
{
    _buffer.enqueue(scancode);
}

KeyCode PS2Keyboard::read()
{
    KeyCode retval = kKeyNull;
    while(true) {
        while(_buffer.isEmpty());
        int scancode = _buffer.dequeue();
        if((scancode & 128) == 128) {
            // Released
            continue;
        } else {
            // Pressed
            if(scancode < 70) {
                retval = scan1_to_key[scancode];
            } else {
                retval = kKeyNull;
            }
            break;
        }
    }

    return retval;
}
