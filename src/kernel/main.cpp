#include <new>
#include <string.h>
#include "Kernel.hpp"
#include "cpu/GlobalDescriptorTable.hpp"
#include "cpu/InterruptDescriptorTable.hpp"
#include "device/display/VGATextTerminal.hpp"

// ====================================================
// Globals
// ====================================================

Kernel *kernel = NULL;
uint8_t kern_mem[sizeof(Kernel)];

VGATextTerminal *terminal = NULL;
uint8_t term_mem[sizeof(VGATextTerminal)];

GlobalDescriptorTable *gdt = NULL;
uint8_t gdt_mem[sizeof(GlobalDescriptorTable)];

InterruptDescriptorTable *idt = NULL;
uint8_t idt_mem[sizeof(InterruptDescriptorTable)];


#define GDT_SIZE 5
uint64_t gdt_ptr[GDT_SIZE];
uint64_t idt_ptr[256];

extern "C" void __cxa_pure_virtual()
{
    // Do nothing or print an error message.
}

extern "C" char hex2char(uint8_t nib)
{
    nib = nib & 0xF;
    if(nib < 10) {
        return '0' + nib;
    } else {
        nib -= 10;
        return 'A' + nib; 
    }
}

char retval[7];

extern "C" char *hex2str(uint16_t num)
{
    retval[0] = '0';
    retval[1] = 'x';
    retval[2] = hex2char((num & 0xF000) >> 12);
    retval[3] = hex2char((num & 0xF00) >> 8);
    retval[4] = hex2char((num & 0xF0) >> 4);
    retval[5] = hex2char(num & 0xF);
    retval[6] = 0;

    return retval;
}

extern "C" void interrupt_handler(RegisterTable registers) {
    kernel->stdout()->writeString("int ");
    hex2str(registers.int_no);
    kernel->stdout()->writeString(retval);
    kernel->stdout()->writeString(", err ");
    hex2str(registers.err_code);
    kernel->stdout()->writeString(retval);
    kernel->stdout()->putChar('\n');
    idt->callInterruptServiceRoutine(registers.int_no, registers);
}

extern "C" void kernel_main()
{
    kernel = new (kern_mem) Kernel;
    terminal = new (term_mem) VGATextTerminal;
    kernel->setStdout(terminal);
    
    gdt = new (gdt_mem) GlobalDescriptorTable(gdt_ptr, GDT_SIZE);
    gdt->encodeEntry(0, GDTEntry());
    gdt->encodeEntry(1, GDTEntry(0, 0xffffffff, 0x9A));
    gdt->encodeEntry(2, GDTEntry(0, 0xffffffff, 0x92));
    gdt->encodeEntry(3, GDTEntry(0, 0xffffffff, 0xFA));
    gdt->encodeEntry(4, GDTEntry(0, 0xffffffff, 0xF2));
    gdt->install();

    idt = new (idt_mem) InterruptDescriptorTable(idt_ptr, 256);
    idt->encodeHWExceptionISRs();
    idt->install();

    kernel->panic("Kernel exited. Maybe you should write the rest of the operating system?");
    asm volatile ("int $0x15");
}
