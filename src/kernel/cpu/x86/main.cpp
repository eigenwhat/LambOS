#include <new>
#include <string.h>
#include <stdlib.h>
#include <cpu/multiboot.h>
#include <Kernel.hpp>
#include <cpu/X86CPU.hpp>
#include <mem/MMU.hpp>
#include <device/display/VGATextConsole.hpp>
#include <io/PrintStream.hpp>
#include <device/display/ConsoleOutputStream.hpp>
#include <device/input/KeyboardInputStream.hpp>
#include <mem/liballoc.h>
#include <io/BochsDebugOutputStream.hpp>

// ====================================================
// Globals
// ====================================================
extern uint32_t kernel_end;
VGA4BitColor defaultTextColor = COLOR_LIGHT_GREY;
Kernel *kernel = NULL;
uint8_t kern_mem[sizeof(Kernel)];
uint8_t term_mem[sizeof(VGATextConsole)];
uint8_t tout_mem[sizeof(ConsoleOutputStream)];
uint8_t stdout_mem[sizeof(PrintStream)];
uint8_t mmu_mem[sizeof(MMU)];
uint8_t x86cpu_mem[sizeof(X86CPU)];

extern "C" {

// ====================================================
// Function prototypes
// ====================================================
int install_cpu();
int install_mmu(uint32_t mmap_addr, uint32_t mmap_length);
int log_result(const char *printstr, int success, const char *ackstr, const char *nakstr);
int log_task(const char *printstr, int success);
int check_flag(multiboot_info_t *info, const char *printstr, uint32_t flag);
int log_test(const char *printstr, int success);

// ====================================================
// Functions
// ====================================================
void kernel_main(multiboot_info_t *info, uint32_t magic)
{
    kernel = new (kern_mem) Kernel;
    Console *term = new (term_mem) VGATextConsole;
    ConsoleOutputStream *tOut = new (tout_mem) ConsoleOutputStream(*term);
    kernel->setConsole(term);
    kernel->setOut(new (stdout_mem) PrintStream(*tOut));

    if(magic != 0x2BADB002) {
        kernel->panic("Operating system not loaded by multiboot compliant bootloader.");
    }

    log_task("Installing CPU descriptor tables...", install_cpu());

    int result;

    result = check_flag(info, "Checking for reported memory size...", MULTIBOOT_INFO_MEMORY);
    if(result) {
        char decval[33];
        itoa(info->mem_lower, decval, 10);
        kernel->out()->print(decval);
        kernel->out()->println("KiB lower memory.");

        itoa(info->mem_upper, decval, 10);
        kernel->out()->print(decval);
        kernel->out()->println("KiB upper memory.");
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
            kernel->out()->print("address: ");
            itoa(mmap->addr, hexval+2, 16);
            kernel->out()->print(hexval);

            kernel->out()->print(" length: ");
            itoa(mmap->len, hexval+2, 16);
            kernel->out()->print(hexval);

            kernel->out()->print(" type: ");
            itoa(mmap->type, hexval+2, 16);
            kernel->out()->println(hexval);
        }
    }

    log_task("Setting up memory management unit...", install_mmu(info->mmap_addr, info->mmap_length));

    const char *somestr = "This is a test string to see if kmalloc works now.";
    char *longstr = (char*)kmalloc(strlen(somestr) + 1);
    if(log_test("Testing if kmalloc gives us an appropriate pointer...", (uint32_t)longstr > (uint32_t)&kernel_end)) {
        strcpy(longstr, somestr);
        kernel->out()->println(longstr);
    } else {
        kernel->panic("kmalloc no work :(");
    }

    kernel->cpu()->enableInterrupts();

    KeyboardInputStream *in = new KeyboardInputStream((X86CPU&)*(kernel->cpu()));

    kernel->console()->setForegroundColor(COLOR_WHITE);
    kernel->out()->println("\n* * *");
    kernel->console()->setForegroundColor(COLOR_LIGHT_RED);
    kernel->out()->print("Kernel exited. Maybe you should write the rest of the operating system?");

    while(true) {
        kernel->out()->write(in->read());
    }
}

int install_cpu()
{
    kernel->setCPU(new (x86cpu_mem) X86CPU);
    kernel->cpu()->install();

    return true;
}

int install_mmu(uint32_t mmap_addr, uint32_t mmap_length)
{
    kernel->setMMU(new (mmu_mem) MMU(mmap_addr, mmap_length));
    kernel->mmu()->install();

    return true;
}

int log_result(const char *printstr, int success, const char *ackstr, const char *nakstr)
{
    kernel->console()->moveTo(kernel->console()->row(), 0);
    for(uint32_t i = 0; i < kernel->console()->width(); ++i) {
        kernel->console()->putChar(' ');
    }
    kernel->console()->moveTo(kernel->console()->row() - 1, 0);

    kernel->console()->setForegroundColor(COLOR_WHITE);
    kernel->console()->writeString(printstr);

    if(kernel->console()->column() > kernel->console()->width() - 6) {
        kernel->console()->writeString("\n");
    }
    
    if(success) {
        kernel->console()->moveTo(kernel->console()->row(), kernel->console()->width() - (strlen(ackstr) + 3));
        kernel->console()->writeString("[");
        kernel->console()->setForegroundColor(COLOR_GREEN);
        kernel->console()->writeString(ackstr);
    } else {
        kernel->console()->moveTo(kernel->console()->row(), kernel->console()->width() - (strlen(nakstr) + 3));
        kernel->console()->writeString("[");
        kernel->console()->setForegroundColor(COLOR_RED);
        kernel->console()->writeString(nakstr);
    }
    kernel->console()->setForegroundColor(COLOR_WHITE);
    kernel->console()->writeString("]\n");

    kernel->console()->setForegroundColor(defaultTextColor);

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

} // extern C
