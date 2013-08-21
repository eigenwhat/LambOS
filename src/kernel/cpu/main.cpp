#include <new>
#include <string.h>
#include <stdlib.h>
#include "../Kernel.hpp"
#include "GlobalDescriptorTable.hpp"
#include "InterruptDescriptorTable.hpp"
#include "Paging.hpp"
#include "../device/display/VGATextTerminal.hpp"

// ====================================================
// Globals
// ====================================================
extern uint32_t kernel_end;

Kernel *kernel = NULL;
uint8_t kern_mem[sizeof(Kernel)];

VGATextTerminal *terminal = NULL;
uint8_t term_mem[sizeof(VGATextTerminal)];

GlobalDescriptorTable *gdt = NULL;
uint8_t gdt_mem[sizeof(GlobalDescriptorTable)];

InterruptDescriptorTable *idt = NULL;
uint8_t idt_mem[sizeof(InterruptDescriptorTable)];

PageTable *pageDirectory = NULL;
uint8_t pd_mem[sizeof(PageTable)];

TaskStateSegment tss;


#define GDT_SIZE 6
uint64_t gdt_ptr[GDT_SIZE];
uint64_t idt_ptr[256];

extern "C" void __cxa_pure_virtual()
{
    // Do nothing or print an error message.
}

extern "C" void interrupt_handler(RegisterTable registers) {
    char hexval[33];
    kernel->stdout()->writeString("int ");
    itoa(registers.int_no, hexval, 16);
    kernel->stdout()->writeString(hexval);
    kernel->stdout()->writeString(", err ");
    itoa(registers.err_code, hexval, 16);
    kernel->stdout()->writeString(hexval);
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

    memset(&tss, 0x0, sizeof(tss));
    tss.ss0 = 0x10;
    tss.esp0 = 0x0;
    tss.iomap_base = sizeof(tss);

    gdt->encodeEntry(5, GDTEntry((uint32_t)&tss, sizeof(tss), 0x89));
    gdt->install();
    gdt->installTSS(5);

    idt = new (idt_mem) InterruptDescriptorTable(idt_ptr, 256);
    idt->encodeHWExceptionISRs();
    idt->install();

    uint32_t pageDirectoryAddress = (kernel_end & k4KPageAddressMask) + 0x1000;
    pageDirectory = new (pd_mem) PageTable((uint32_t*)pageDirectoryAddress);

    PageTable firstTable((uint32_t*)(pageDirectoryAddress + 0x1000));
    uint32_t address = 0;

    // identity map first 4MB
    for(int i = 0; i < 1024; ++i) {
        PageEntry entry(address);
        entry.setFlag(kPresentBit);
        entry.setFlag(kReadWriteBit);
        firstTable.setEntry(i, entry);
        address += 0x1000;
    }

    PageEntry entry((uint32_t)firstTable.address());
    entry.setFlag(kPresentBit);
    entry.setFlag(kReadWriteBit);
    pageDirectory->setEntry(0, entry);
    pageDirectory->install();

    //kernel->panic("Kernel exited. Maybe you should write the rest of the operating system?");
    asm volatile ("int $0x15");
}
