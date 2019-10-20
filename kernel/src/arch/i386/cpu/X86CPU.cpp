#include <arch/i386/cpu/X86CPU.hpp>
#include <arch/i386/sys/asm.h>
#include <string.h>
#include <stdlib.h>
#include <io/debug.h>

void init_pics(InterruptDescriptorTable &idt, uint8_t pic1, uint8_t pic2);

/* reinitialize the PIC controllers, giving them specified vector offsets
   rather than 8h and 70h, as configured by default */
#define PIC1        0x20        /* IO base address for master PIC */
#define PIC2        0xA0        /* IO base address for slave PIC */
#define PIC1_COMMAND    PIC1
#define PIC1_DATA   (PIC1+1)
#define PIC2_COMMAND    PIC2
#define PIC2_DATA   (PIC2+1)
#define ICW1_ICW4   0x01        /* ICW4 (not) needed */
#define ICW1_SINGLE 0x02        /* Single (cascade) mode */
#define ICW1_INTERVAL4  0x04        /* Call address interval 4 (8) */
#define ICW1_LEVEL  0x08        /* Level triggered (edge) mode */
#define ICW1_INIT   0x10        /* Initialization - required! */

#define ICW4_8086   0x01        /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO   0x02        /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE  0x08        /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C        /* Buffered mode/master */
#define ICW4_SFNM   0x10        /* Special fully nested (not) */

X86CPU::X86CPU()
{
    // Set up GDT
    _gdt.encodeEntry(0, GDTEntry());
    _gdt.encodeEntry(1, GDTEntry(0, 0xffffffff, GDTEntry::Type::KernelCode));
    _gdt.encodeEntry(2, GDTEntry(0, 0xffffffff, GDTEntry::Type::KernelData));
    _gdt.encodeEntry(3, GDTEntry(0, 0xffffffff, GDTEntry::Type::UserCode));
    _gdt.encodeEntry(4, GDTEntry(0, 0xffffffff, GDTEntry::Type::UserData));

    memset(&_tss, 0x0, sizeof(_tss));
    _tss.ss0 = 0x10;
    _tss.esp0 = 0x0;
    _tss.iomap_base = sizeof(_tss);

    _gdt.encodeEntry(5, GDTEntry((uint32_t) & _tss, sizeof(_tss), GDTEntry::Type::TSS));

}

void X86CPU::install()
{
    // Set up IDT
    _idt.encodeISRs();

    // install GDT
    _gdt.install();

    // install TSS
    _gdt.installTSS(5);

    // install IDT
    _idt.install();
}

void X86CPU::enableInterrupts()
{
    static bool pic_initialized = false;
    if (!pic_initialized) {
        init_pics(_idt, uint8_t(InterruptNumber::kIRQ0), uint8_t(InterruptNumber::kIRQ8));
        pic_initialized = true;
    }

    asm volatile ("sti");
}

void X86CPU::maskIRQ(unsigned int IRQ)
{
    uint16_t port;
    uint8_t value;

    if (IRQ < 8u) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQ -= 8u;
    }

    value = static_cast<uint8_t>(inb(port) | (1u << IRQ));
    outb(port, value);
}

void X86CPU::unmaskIRQ(unsigned int IRQ)
{
    uint16_t port;
    uint8_t value;

    if (IRQ < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQ = 8u;
    }
    value = static_cast<uint8_t>(inb(port) & ~(1u << IRQ));
    outb(port, value);
}

class IRQHandler : public InterruptServiceRoutine
{
public:
    IRQHandler(OutputStream &out) : _out(out)
    {}

    virtual void operator()(RegisterTable &registers)
    {
        char hexval[33];
        _out.write((uint8_t *) "(IRQ) int 0x", 12);
        uitoa(registers.int_no, hexval, 16);
        _out.write((uint8_t *) hexval, 2);
        _out.write((uint8_t *) ", err 0x", 8);
        uitoa(registers.err_code, hexval, 16);
        _out.write((uint8_t *) hexval, 2);
        _out.write('\n');
        outb(0x20, 0x20);
    }

private:
    OutputStream &_out;
};

/*
arguments:
offset1 - vector offset for master PIC
vectors on the master become offset1..offset1+7
offset2 - same for slave PIC: offset2..offset2+7
*/
void init_pics(InterruptDescriptorTable &idt, uint8_t pic1, uint8_t pic2)
{
    for (int i = pic1; i < pic1 + 8; ++i) {
        idt.setISR(static_cast<InterruptNumber>(i), new IRQHandler(*debugOut));
    }
    for (int i = pic2; i < pic2 + 8; ++i) {
        idt.setISR(static_cast<InterruptNumber>(i), new IRQHandler(*debugOut));
    }

    outb(PIC1_COMMAND, ICW1_INIT + ICW1_ICW4);    // starts the initialization sequence (in cascade mode)
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT + ICW1_ICW4);
    io_wait();
    outb(PIC1_DATA, pic1);                      // ICW2: Master PIC vector offset
    io_wait();
    outb(PIC2_DATA, pic2);                      // ICW2: Slave PIC vector offset
    io_wait();
    outb(PIC1_DATA, 4);                         // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    io_wait();
    outb(PIC2_DATA, 2);                         // ICW3: tell Slave PIC its cascade identity (0000 0010)
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    // Mask all IRQs. IRQ handlers should unmask them as part of their install sequence.
    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);
}

