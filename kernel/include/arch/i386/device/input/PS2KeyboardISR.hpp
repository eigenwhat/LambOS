#pragma once

#include <device/input/PS2Keyboard.hpp>
#include <arch/i386/cpu/X86CPU.hpp>
#include <sys/asm.h>

#define KBD_DEVICE    0x60
#define KEY_PENDING   0x64

class PS2KeyboardISR : public InterruptServiceRoutine
{
  public:
    /**
     * Installs an ISR for the PS2 keyboard into the CPU.
     * @param cpu The CPU to install into.
     * @param keyboard The PS2Keyboard driver object to send events to.
     */
    static void install(X86CPU &cpu, PS2Keyboard *keyboard) {
        cpu.idt()->setISR(InterruptNumber::kKeyboardIRQ, new PS2KeyboardISR(keyboard));
        cpu.unmaskIRQ(1);
    }

    PS2KeyboardISR(PS2Keyboard *keyboard) : _keyboard(keyboard) {}

    virtual void operator()(RegisterTable &)
    {
        while (inb(KEY_PENDING) & 2);
        uint8_t scancode = inb(KBD_DEVICE);

        _keyboard->pushScanCode(scancode);
        outb(0x20, 0x20);
    }

  private:
    ArcPtr<PS2Keyboard> _keyboard;
};
