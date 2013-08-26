#include <new>
#include <string.h>
#include <stdlib.h>
#include "multiboot.h"
#include "../Kernel.hpp"
#include "GlobalDescriptorTable.hpp"
#include "InterruptDescriptorTable.hpp"
#include "Paging.hpp"
#include <mem/PageFrameAllocator.hpp>
#include "../device/display/VGATextTerminal.hpp"

// ====================================================
// Globals
// ====================================================
extern uint32_t kernel_end;
VGA4BitColor defaultTextColor = COLOR_LIGHT_GREY;
Kernel *kernel = NULL;
GlobalDescriptorTable *gdt = NULL;
InterruptDescriptorTable *idt = NULL;
PageTable *pageDirectory = NULL;
uint8_t kern_mem[sizeof(Kernel)];
uint8_t term_mem[sizeof(VGATextTerminal)];
uint8_t gdt_mem[sizeof(GlobalDescriptorTable)];
uint8_t idt_mem[sizeof(InterruptDescriptorTable)];
uint8_t pd_mem[sizeof(PageTable)];

PageFrameAllocator *frameAllocator = NULL;
uint8_t pfa_mem[sizeof(PageFrameAllocator)];

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
int install_paging();
int install_gdt();
int install_idt();
int init_pfalloc(uint32_t mmap_addr, uint32_t mmap_length);

int testPFA();

// ====================================================
// Functions
// ====================================================
void __cxa_pure_virtual()
{
    // Do nothing or print an error message.
}

void interrupt_handler(RegisterTable registers) {
    char hexval[33];
    kernel->out()->writeString("int ");
    itoa(registers.int_no, hexval, 16);
    kernel->out()->writeString(hexval);
    kernel->out()->writeString(", err ");
    itoa(registers.err_code, hexval, 16);
    kernel->out()->writeString(hexval);
    kernel->out()->putChar('\n');
    idt->callInterruptServiceRoutine(registers.int_no, registers);
}

int log_result(const char *printstr, int success, const char *ackstr, const char *nakstr)
{
    kernel->out()->setForegroundColor(COLOR_WHITE);
    kernel->out()->writeString(printstr);

    if(kernel->out()->column() > kernel->out()->width() - 6) {
        kernel->out()->writeString("\n");
    }
    
    if(success) {
        kernel->out()->moveTo(kernel->out()->row(), kernel->out()->width() - (strlen(ackstr) + 3));
        kernel->out()->writeString("[");
        kernel->out()->setForegroundColor(COLOR_GREEN);
        kernel->out()->writeString(ackstr);
    } else {
        kernel->out()->moveTo(kernel->out()->row(), kernel->out()->width() - (strlen(nakstr) + 3));
        kernel->out()->writeString("[");
        kernel->out()->setForegroundColor(COLOR_RED);
        kernel->out()->writeString(nakstr);
    }
    kernel->out()->setForegroundColor(COLOR_WHITE);
    kernel->out()->writeString("]\n");

    kernel->out()->setForegroundColor(defaultTextColor);

    return success;
}

int log_task(const char *printstr, int success)
{
    return log_result(printstr, success, "DONE", "FAIL");
}

int check_flag(multiboot_info_t *info, const char *printstr, uint32_t flag)
{
    return log_result(printstr, info->flags & flag, "FOUND", "FAIL");
}

int log_test(const char *printstr, int success)
{
    return log_result(printstr, success, "PASS", "FAIL");
}

void kernel_main(multiboot_info_t *info, uint32_t magic)
{
    kernel = new (kern_mem) Kernel;
    kernel->setOut(new (term_mem) VGATextTerminal);

    int result;

    log_task("Installing global descriptor table...", install_gdt());
    log_task("Installing interrupt descriptor table...", install_idt());

    result = check_flag(info, "Checking for reported memory size...", MULTIBOOT_INFO_MEMORY);
    if(result) {
        char decval[33];
        itoa(info->mem_lower, decval, 10);
        kernel->out()->writeString(decval);
        kernel->out()->writeString("KiB lower memory.\n");

        itoa(info->mem_upper, decval, 10);
        kernel->out()->writeString(decval);
        kernel->out()->writeString("KiB upper memory.\n");
    }

    result = check_flag(info, "Checking for reported memory map...", MULTIBOOT_INFO_MEM_MAP);

    if(!result) {
        kernel->out()->setForegroundColor(COLOR_LIGHT_RED);
        kernel->out()->writeString("ERROR: Cannot set up paging without an accurate memory map!");

        return;
    } else {
        multiboot_memory_map_t *mmap;
        for (mmap = (multiboot_memory_map_t *) info->mmap_addr; 
            (uint32_t) mmap < info->mmap_addr + info->mmap_length;
            mmap = (multiboot_memory_map_t *) ((uint32_t) mmap + mmap->size + sizeof (mmap->size)))
        {
            char hexval[35];
            hexval[0] = '0';
            hexval[1] = 'x';
            kernel->out()->writeString("address: ");
            itoa(mmap->addr, hexval+2, 16);
            kernel->out()->writeString(hexval);

            kernel->out()->writeString(" length: ");
            itoa(mmap->len, hexval+2, 16);
            kernel->out()->writeString(hexval);

            kernel->out()->writeString(" type: ");
            itoa(mmap->type, hexval+2, 16);
            kernel->out()->writeString(hexval);

            kernel->out()->writeString("\n");
        }
    }

    if(log_task("Setting up page frame allocator...", init_pfalloc(info->mmap_addr, info->mmap_length))) {
        testPFA();
    }
    log_task("Enabling paging...", install_paging());

    kernel->out()->setForegroundColor(COLOR_WHITE);
    kernel->out()->writeString("\n* * *\n");
    kernel->out()->setForegroundColor(COLOR_LIGHT_RED);
    kernel->out()->writeString("Kernel exited. Maybe you should write the rest of the operating system?");
}

int install_gdt()
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

    return true;
}

int install_idt()
{
    idt = new (idt_mem) InterruptDescriptorTable(idt_ptr, 256);
    idt->encodeHWExceptionISRs();
    idt->install();

    return true;
}

int install_paging()
{
    uint32_t kernel_end_addr = (uint32_t) &kernel_end;
    uint32_t pageDirectoryAddress = (kernel_end_addr & k4KPageAddressMask) + 0x1000;
    pageDirectory = new (pd_mem) PageTable((uint32_t*)pageDirectoryAddress);

    PageTable firstTable((uint32_t*)(pageDirectoryAddress + 0x1000));
    uint32_t address = 0;

    // identity map current used address space
    int lastUsedPage = (pageDirectoryAddress + 0x1000) / 0x1000;
    for(int i = 0; i < lastUsedPage; ++i) {
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

    return true;
}

int init_pfalloc(uint32_t mmap_addr, uint32_t mmap_length)
{
    frameAllocator = new (pfa_mem) PageFrameAllocator(mmap_addr, mmap_length);
    return true;
}

int testPFA()
{
    bool foundFreeFrame = false;
    for(int i = 0; i < 256; ++i) {
        foundFreeFrame = frameAllocator->frameIsAvailable(i);
        if(foundFreeFrame) {
            char num[33];
            itoa(i, num, 10);
            kernel->out()->writeString(num);
            kernel->out()->writeString("\n");
            break;
        }
    }

    log_test(" > Testing if first 1MB is allocated...", !foundFreeFrame);
}

} // extern C
