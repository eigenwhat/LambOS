#include <device/input/KeyboardInputStream.hpp>
#include <sys/asm.h>

#define KBD_DEVICE    0x60
#define KEY_PENDING   0x64

class KeyboardISR : public InterruptServiceRoutine
{
public:
    KeyboardISR(KeyboardInputStream &keyboard) : _keyboard(keyboard) {}
    virtual void operator()(RegisterTable&) {
        while(inb(KEY_PENDING) & 2);
        uint8_t scancode = inb(KBD_DEVICE);
        _keyboard.pushScanCode(scancode);
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
    while(_buffer.isEmpty());

    return (int)_buffer.dequeue();
}
