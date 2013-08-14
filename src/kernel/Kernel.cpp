#include <new>
#include <lib/string.h>
#include <kernel/Kernel.hpp>
#include <kernel/GlobalDescriptorTable.hpp>
#include <kernel/InterruptDescriptorTable.hpp>
#include <device/display/VGATextTerminal.hpp>
 
/* Check if the compiler thinks if we are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

// ====================================================
// Globals
// ====================================================
uint8_t kern_mem[sizeof(Kernel)];
uint8_t term_mem[sizeof(VGATextTerminal)];

Kernel *kernel;
VGATextTerminal *terminal;

#define GDT_SIZE 5
uint64_t gdt_ptr[GDT_SIZE];
uint64_t idt_ptr[256];

extern "C" void isr21(void);

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

char retval[5];

extern "C" char *hex2str(uint16_t num)
{
    retval[0] = '0';
    retval[1] = 'x';
    retval[2] = hex2char((num & 0xF0) >> 4);
    retval[3] = hex2char(num & 0xF);
    retval[4] = 0;

    return retval;
}

extern "C" void interrupt_handler(registers_t registers) {
    const char *data = "\nInterrupt # ";
    kernel->stdout()->writeString(data);
    hex2str(registers.int_no);
    kernel->stdout()->writeString(retval);
}

extern "C" void kernel_main()
{
    kernel = new (kern_mem) Kernel;
    terminal = new (term_mem) VGATextTerminal;
    kernel->setStdout(terminal);
    
    GlobalDescriptorTable gdt(gdt_ptr, GDT_SIZE);
    gdt.encodeEntry(0, GDTEntry());
    gdt.encodeEntry(1, GDTEntry(0, 0xffffffff, 0x9A));
    gdt.encodeEntry(2, GDTEntry(0, 0xffffffff, 0x92));
    gdt.encodeEntry(3, GDTEntry(0, 0xffffffff, 0xFA));
    gdt.encodeEntry(4, GDTEntry(0, 0xffffffff, 0xF2));
    gdt.install();

    InterruptDescriptorTable idt(idt_ptr, 256);
    idt.encodeHWExceptionISRs();
    idt.install();

    kernel->panic("Kernel exited. Maybe you should write the rest of the operating system?");
    asm volatile ("int $0x15");
}

//======================================================
// Kernel
//======================================================
void Kernel::panic(const char *string)
{
    size_t length = strlen(string);
    stdout()->setBackgroundColor(COLOR_RED);
    stdout()->setForegroundColor(COLOR_WHITE);
    stdout()->clear();
    const char *banner[3]= {"* * * * * * * * * * * * * * * * * * * *",
                            "* LAMBOS FOUND OUT ABOUT WHAT YOU DID *",
                            "* * * * * * * * * * * * * * * * * * * *"};
    for(int i = 0; i < 3; ++i) {
        size_t bannerlength = strlen(banner[i]);
        size_t columnToCenter = stdout()->width() > bannerlength ? (stdout()->width() - bannerlength) / 2 : 0;
        stdout()->moveTo(i+1,columnToCenter);
        stdout()->writeString(banner[i]);
    }
    
    size_t rowToCenter = stdout()->height()/2;
    size_t columnToCenter = (stdout()->width() - length) / 2;
    stdout()->moveTo(rowToCenter, columnToCenter);
    stdout()->writeString(string);

    const char *msg = "0xL4MBOS";
    stdout()->moveTo(stdout()->height()-2, (stdout()->width() - strlen(msg)) / 2);
    stdout()->writeString(msg);
}
