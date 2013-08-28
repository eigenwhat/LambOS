#include <new>
#include <string.h>
#include <stdlib.h>
#include "multiboot.h"
#include "../Kernel.hpp"
#include "GlobalDescriptorTable.hpp"
#include "InterruptDescriptorTable.hpp"
#include <mem/MMU.hpp>
#include "../device/display/VGATextTerminal.hpp"

// ====================================================
// Globals
// ====================================================
extern uint32_t kernel_end;
VGA4BitColor defaultTextColor = COLOR_LIGHT_GREY;
Kernel *kernel = NULL;
GlobalDescriptorTable *gdt = NULL;
InterruptDescriptorTable *idt = NULL;
uint8_t kern_mem[sizeof(Kernel)];
uint8_t term_mem[sizeof(VGATextTerminal)];
uint8_t gdt_mem[sizeof(GlobalDescriptorTable)];
uint8_t idt_mem[sizeof(InterruptDescriptorTable)];
uint8_t mmu_mem[sizeof(MMU)];

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
int init_mmu(uint32_t mmap_addr, uint32_t mmap_length);

// ====================================================
// Functions
// ====================================================
void __cxa_pure_virtual()
{
    // Do nothing or print an error message.
}

void interrupt_handler(RegisterTable registers)
{
    idt->callInterruptServiceRoutine(registers.int_no, registers);
}

int log_result(const char *printstr, int success, const char *ackstr, const char *nakstr)
{
    kernel->out()->moveTo(kernel->out()->row(), 0);
    for(int i = 0; i < kernel->out()->width(); ++i) {
        kernel->out()->putChar(' ');
    }
    kernel->out()->moveTo(kernel->out()->row() - 1, 0);

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
        kernel->panic("Fatal ERROR: Cannot install paging due to missing memory map.");
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

    log_task("Setting up memory management unit...", init_mmu(info->mmap_addr, info->mmap_length));

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

int init_mmu(uint32_t mmap_addr, uint32_t mmap_length)
{
    kernel->setMMU(new (mmu_mem) MMU(mmap_addr, mmap_length));

    return true;
}

} // extern C
