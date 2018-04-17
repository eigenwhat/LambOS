#include <new>
#include <string.h>
#include <stdio.h>

#include <arch/i386/cpu/multiboot.h>
#include <arch/i386/cpu/X86CPU.hpp>
#include <arch/i386/cpu/X86RealTimeClock.hpp>
#include <device/input/PS2Keyboard.hpp>
#include <device/display/ConsoleOutputStream.hpp>
#include <device/display/VGATextConsole.hpp>
#include <device/input/KeyboardInputStream.hpp>
#include <io/BochsDebugOutputStream.hpp>
#include <io/PrintStream.hpp>
#include <io/debug.h>
#include <mem/MMU.hpp>
#include <Kernel.hpp>
#include <arch/i386/device/input/PS2KeyboardISR.hpp>


// ====================================================
// Globals
// ====================================================
extern uint32_t kernel_end;
VGA4BitColor defaultTextColor = COLOR_LIGHT_GREY;

Kernel *kernel = NULL;
uint8_t kern_mem[sizeof(Kernel)];
uint8_t mmu_mem[sizeof(MMU)];
uint8_t x86cpu_mem[sizeof(X86CPU)];

uint8_t term_mem[sizeof(VGATextConsole)];
uint8_t tout_mem[sizeof(ConsoleOutputStream)];
uint8_t stdout_mem[sizeof(PrintStream)];

uint8_t dbgout_mem[sizeof(PrintStream)];
uint8_t bochsout_mem[sizeof(BochsDebugOutputStream)];
PrintStream *debugOut;

extern "C" {

// ====================================================
// Function prototypes
// ====================================================
int install_cpu();
int install_mmu(uint32_t mmap_addr, uint32_t mmap_length);
void init_context();
void init_system();
void read_multiboot(multiboot_info_t *info);

int log_result(const char *printstr, int success, const char *ackstr, const char *nakstr);
int log_task(const char *printstr, int success);
int check_flag(multiboot_info_t *info, const char *printstr, uint32_t flag);
int log_test(const char *printstr, int success);

// ====================================================
// Functions
// ====================================================
void kernel_main(multiboot_info_t *info, uint32_t magic)
{
    OutputStream *stream = new(bochsout_mem) BochsDebugOutputStream();
    debugOut = new(dbgout_mem) PrintStream(*stream);
    init_context();

    if (magic != 0x2BADB002) {
        kernel->panic("Operating system not loaded by multiboot compliant bootloader.");
    }

    read_multiboot(info);
    log_task("Installing CPU descriptor tables...", install_cpu());
    log_task("Setting up memory management unit...", install_mmu(info->mmap_addr, info->mmap_length));
    kernel->cpu()->enableInterrupts();
    init_system();
}

void init_system()
{
    kernel->console()->setForegroundColor(COLOR_WHITE);
    puts("\n* * *");
    X86RealTimeClock clock;
    DateTime now = clock.currentTime();
    printf("The current date and time is %d/%d/%d %d:%d:%d", now.month, now.monthday, now.year + (now.century * 100),
           now.hours, now.minutes, now.seconds);
    puts("\n* * *");
    kernel->console()->setForegroundColor(COLOR_LIGHT_RED);
    puts("Kernel exited. Maybe you should write the rest of the operating system?");
    kernel->console()->setCursorVisible(true);

    auto *kb = new PS2Keyboard();
    PS2KeyboardISR::install(*(X86CPU*)kernel->cpu(), kb);
    auto *in = new KeyboardInputStream(kb);
    kb->release();

    while (true) {
        kernel->out()->write(in->read());
    }
}

void init_context()
{
    kernel = new(kern_mem) Kernel;
    Console *term = new(term_mem) VGATextConsole;
    ConsoleOutputStream *tOut = new(tout_mem) ConsoleOutputStream(*term);
    kernel->setConsole(term);
    kernel->setOut(new(stdout_mem) PrintStream(*tOut));
}

int install_cpu()
{
    kernel->setCPU(new(x86cpu_mem) X86CPU);
    kernel->cpu()->install();

    return true;
}

int install_mmu(uint32_t mmap_addr, uint32_t mmap_length)
{
    kernel->setMMU(new(mmu_mem) MMU(mmap_addr, mmap_length));
    kernel->mmu()->install();

    return true;
}

void read_multiboot(multiboot_info_t *info)
{
    int result;

    result = check_flag(info, "Checking for reported memory size...", MULTIBOOT_INFO_MEMORY);
    if (result) {
        printf("%dKiB lower memory.\n", info->mem_lower);
        printf("%dKiB upper memory.\n", info->mem_upper);
    }

    result = check_flag(info, "Checking for reported memory map...", MULTIBOOT_INFO_MEM_MAP);

    if (!result) {
        kernel->panic("Fatal ERROR: Cannot install paging due to missing memory map.");
    } else {
        multiboot_memory_map_t *mmap;
        for (mmap = (multiboot_memory_map_t *) info->mmap_addr;
             (uint32_t) mmap < info->mmap_addr + info->mmap_length;
             mmap = (multiboot_memory_map_t * )((uint32_t) mmap + mmap->size + sizeof(mmap->size))) {
            printf("address: %x length: %x type: %x\n", (unsigned int) mmap->addr, (unsigned int) mmap->len,
                   (unsigned int) mmap->type);
        }
    }
}

int log_result(const char *printstr, int success, const char *ackstr, const char *nakstr)
{
    kernel->console()->moveTo(kernel->console()->row(), 0);
    for (uint32_t i = 0; i < kernel->console()->width(); ++i) {
        kernel->console()->putChar(' ');
    }
    kernel->console()->moveTo(kernel->console()->row() - 1, 0);

    kernel->console()->setForegroundColor(COLOR_WHITE);
    kernel->console()->writeString(printstr);

    if (kernel->console()->column() > kernel->console()->width() - 6) {
        kernel->console()->writeString("\n");
    }

    if (success) {
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
