#include <device/input/PS2Keyboard.hpp>
#include <sys/asm.h>

#define KBD_DEVICE    0x60
#define KEY_PENDING   0x64

class KeyboardISR : public InterruptServiceRoutine
{
public:
    KeyboardISR(PS2Keyboard &keyboard) : _keyboard(keyboard) {}
    virtual void operator()(RegisterTable &registers) {
        uint8_t scancode;
        inb(KBD_DEVICE);

        _keyboard.pushScanCode(scancode);
    }
private:
    PS2Keyboard &_keyboard;
};

PS2Keyboard::PS2Keyboard(X86CPU &cpu) : _buffer(128)
{

}

void PS2Keyboard::pushScanCode(uint8_t scancode)
{
    _buffer.enqueue(scancode);
}
