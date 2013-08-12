#include <lib/string.h>
#include <kernel/Kernel.hpp>
#include <kernel/GlobalDescriptorTable.hpp>
#include <device/display/VGATextTerminal.hpp>
 
/* Check if the compiler thinks if we are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

// ====================================================
// Globals
// ====================================================
uint64_t gdt_ptr[GDT_SIZE];

extern "C" void __cxa_pure_virtual()
{
    // Do nothing or print an error message.
}

extern "C" void kernel_main()
{
    VGATextTerminal terminal;
    Kernel kernel;
    kernel.setStdout(terminal);
	/* Since there is no support for newlines in terminal_putchar yet, \n will
	   produce some VGA specific character instead. This is normal. */
	kernel.stdout()->writeString("Hello, kernel World!\n");
	kernel.stdout()->setForegroundColor(COLOR_LIGHT_BLUE);
	kernel.stdout()->writeString("Setting up GDT...\n");
    
    GlobalDescriptorTable gdt(gdt_ptr, 3);
    gdt.encodeEntry(0, GDTEntry());
    gdt.encodeEntry(1, GDTEntry(0, 0xffffffff, 0x9A));
    gdt.encodeEntry(2, GDTEntry(0, 0xffffffff, 0x92));
    gdt.writeToMemory();

    kernel.panic("Kernel exited. Maybe you should write the rest of the operating system?");
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

    stdout()->moveTo(stdout()->height()-2, (stdout()->width() - 8) / 2);
    stdout()->writeString("0xL4MBOS");
}

