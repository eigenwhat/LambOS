/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)

SECTIONS
{
    . = 0x1000;

    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.text)
        *(.text.*)
    }

    /* Read-only data. */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }

    readonly_end = .;

    /* Read-write data (initialized) */
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    /* Read-write data (uninitialized) */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
}
