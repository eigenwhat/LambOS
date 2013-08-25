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
GlobalDescriptorTable *gdt = NULL;
InterruptDescriptorTable *idt = NULL;
PageTable *pageDirectory = NULL;
uint8_t kern_mem[sizeof(Kernel)];
uint8_t term_mem[sizeof(VGATextTerminal)];
uint8_t gdt_mem[sizeof(GlobalDescriptorTable)];
uint8_t idt_mem[sizeof(InterruptDescriptorTable)];
uint8_t pd_mem[sizeof(PageTable)];

TaskStateSegment tss;

// ====================================================
// CPU tables
// ====================================================
#define GDT_SIZE 6
uint64_t gdt_ptr[GDT_SIZE];
uint64_t idt_ptr[256];

extern "C" {

// ====================================================
// Function prototypes
// ====================================================
void install_paging();
void install_gdt();
void install_idt();

// ====================================================
// Functions
// ====================================================
void __cxa_pure_virtual()
{
    // Do nothing or print an error message.
}

void interrupt_handler(RegisterTable registers) {
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

void kernel_main()
{
    kernel = new (kern_mem) Kernel;
    kernel->setStdout(new (term_mem) VGATextTerminal);

    install_gdt();

    install_idt();

    install_paging();

    asm volatile ("sti");
    kernel->panic("Kernel exited. Maybe you should write the rest of the operating system?");
    asm volatile ("int $0x15");
}

void install_gdt()
{
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
}

void install_idt()
{
    idt = new (idt_mem) InterruptDescriptorTable(idt_ptr, 256);
    idt->encodeHWExceptionISRs();
    idt->install();
}

void install_paging()
{
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

    // Install table for first 4MB
    PageEntry entry((uint32_t)firstTable.address());
    entry.setFlag(kPresentBit);
    entry.setFlag(kReadWriteBit);
    pageDirectory->setEntry(0, entry);

    // Set last PDE to PD itself
    PageEntry directory((uint32_t)pageDirectory->address());
    directory.setFlag(kPresentBit);
    directory.setFlag(kReadWriteBit);
    pageDirectory->setEntry(1023, directory);

    pageDirectory->install();
}

} // extern C
