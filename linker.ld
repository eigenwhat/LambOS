/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
    KERNEL_VIRTUAL_BASE = 0xC0000000;
    KERNEL_PHYSICAL_BASE = 1M;

    /* Link the kernel in the higher half, but keep the physical load at 1 MiB. */
    . = KERNEL_VIRTUAL_BASE + KERNEL_PHYSICAL_BASE;
    kernel_start = .;
    kernel_start_phys = kernel_start - KERNEL_VIRTUAL_BASE;

    /* First put the multiboot header, as it is required to be put very early
       early in the image or the bootloader won't recognize the file format.
       Next we'll put the .text section. */
    .text ALIGN(4K) : AT(ADDR(.text) - KERNEL_VIRTUAL_BASE)
    {
        *(.multiboot)
        *(.text)
        *(.text.*)
    }

    /* Read-only data. */
    .rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_VIRTUAL_BASE)
    {
        *(.rodata)
    }

    readonly_end = .;
    readonly_end_phys = readonly_end - KERNEL_VIRTUAL_BASE;

    /* Read-write data (initialized) */
    .data ALIGN(4K) : AT(ADDR(.data) - KERNEL_VIRTUAL_BASE)
    {
        *(.data)
    }

    /* Read-write data (uninitialized) */
    .bss ALIGN(4K) : AT(ADDR(.bss) - KERNEL_VIRTUAL_BASE)
    {
        *(COMMON)
        *(.bss)
    }

    .bootstrap_stack ALIGN(4K) : AT(ADDR(.bootstrap_stack) - KERNEL_VIRTUAL_BASE)
    {
        *(.bootstrap_stack)
    }

    /* The compiler may produce other sections, by default it will put them in
       a segment with the same name. Simply add stuff here as needed. */

    kernel_end = .;
    kernel_end_phys = kernel_end - KERNEL_VIRTUAL_BASE;
}
