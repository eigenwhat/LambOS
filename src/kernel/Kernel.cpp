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
#define GDT_SIZE 5
uint64_t gdt_ptr[GDT_SIZE];
uint64_t idt_ptr[256];

extern "C" void __cxa_pure_virtual()
{
    // Do nothing or print an error message.
}

extern "C" void interrupt_handler() {
    __asm__("pushal");
    /* do something */
    __asm__("popal; leave; iret"); /* BLACK MAGIC! */
}

extern "C" void kernel_main()
{
    VGATextTerminal terminal;
    Kernel kernel;
    kernel.setStdout(terminal);
    
    GlobalDescriptorTable gdt(gdt_ptr, GDT_SIZE);
    gdt.encodeEntry(0, GDTEntry());
    gdt.encodeEntry(1, GDTEntry(0, 0xffffffff, 0x9A));
    gdt.encodeEntry(2, GDTEntry(0, 0xffffffff, 0x92));
    gdt.encodeEntry(3, GDTEntry(0, 0xffffffff, 0xFA));
    gdt.encodeEntry(4, GDTEntry(0, 0xffffffff, 0xF2));
    gdt.writeToMemory();

    InterruptDescriptorTable idt(idt_ptr, 256);
    memset(idt_ptr, 0, 256*sizeof(uint64_t));
    for(int i = 0; i < 256; ++i) {
        idt.encodeEntry(i, IDTEntry(0x08, (uint32_t)interrupt_handler, 0x8E));
    }
    idt.writeToMemory();

    kernel.panic("Kernel exited. Maybe you should write the rest of the operating system?");
    asm("int $0x21"); // triple fault!
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
