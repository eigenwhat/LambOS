#include <device/input/KeyboardInputStream.hpp>
#include <sys/asm.h>
#include <sys/keys.h>

#define KBD_DEVICE    0x60
#define KEY_PENDING   0x64

char key_to_char[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\\',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '\n',
    0, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0,
    0, 0, 0, ' ', 0, 0, 0,
    '*', 0
};

class KeyboardISR : public InterruptServiceRoutine
{
public:
    KeyboardISR(KeyboardInputStream &keyboard) : _keyboard(keyboard) {}
    virtual void operator()(RegisterTable&) {
        while(inb(KEY_PENDING) & 2);
        uint8_t scancode = inb(KBD_DEVICE);

        _keyboard.pushScanCode(scancode);
        outb(0x20, 0x20);
    }
private:
    KeyboardInputStream &_keyboard;
};

KeyboardInputStream::KeyboardInputStream(X86CPU &cpu) : _buffer(128), _cpu(cpu) 
{
    _cpu.idt()->setISR(kIntKeyboardIRQ, new KeyboardISR(*this));
}

void KeyboardInputStream::pushScanCode(uint8_t scancode)
{
    _buffer.enqueue(scancode);
}

int KeyboardInputStream::read()
{
    int retval = 0;
    do {
        uint8_t scancode = 0;
        while(!(scancode = _buffer.dequeue()));
        if((scancode & 128) == 128) {
            // Released
            continue;
        } else {
            // Pressed
            if(scancode < 70) {
                retval = int(key_to_char[scan1_to_key[scancode]]);
            } else {
                retval = int('?');
            }
        }

    } while(!retval);

    return retval;
}
