#include <lib/string.h>
#include <Kernel.hpp>
#include <VGATextTerminal.hpp>
 
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

    kernel.kerror("Whomp whomp.");
}

//======================================================
// Kernel
//======================================================
void Kernel::kerror(const char *string)
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
        size_t columnToCenter = (stdout()->width() - bannerlength) / 2;
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

//======================================================
// GlobalDescriptorTable
//======================================================
void GlobalDescriptorTable::encodeEntry(uint8_t entryNumber, GDTEntry source)
{
    uint8_t *target = (uint8_t*)&this->gdt[entryNumber];

    if (source.limit > 65536) {
        // Adjust granularity if required
        source.limit = source.limit >> 12;
        target[6] = 0xC0;
    } else {
        target[6] = 0x40;
    }
 
    // Encode the limit
    target[0] = source.limit & 0xFF;
    target[1] = (source.limit >> 8) & 0xFF;
    target[6] |= (source.limit >> 16) & 0xF;
 
    // Encode the base 
    target[2] = source.base & 0xFF;
    target[3] = (source.base >> 8) & 0xFF;
    target[4] = (source.base >> 16) & 0xFF;
    target[7] = (source.base >> 24) & 0xFF;
 
    // And... Type
    target[5] = source.type;
}