# Declare constants used for creating a multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot
.set KERNEL_VIRTUAL_BASE, 0xC0000000
.set KERNEL_PDE_INDEX, 768

# Declare a header as in the Multiboot Standard. We put this into a special
# section so we can force the header to be in the start of the final program.
# You don't need to understand all these details as it is just magic values that
# is documented in the multiboot standard. The bootloader will search for this
# magic sequence and recognize us as a multiboot kernel.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Currently the stack pointer register (esp) points at anything and using it may
# cause massive harm. Instead, we'll provide our own stack. We will allocate
# room for a small temporary stack by creating a symbol at the bottom of it,
# then allocating 16384 bytes for it, and finally creating a symbol at the top.
.section .bootstrap_stack
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# Preallocate pages used for paging. Don't hard-code addresses and assume they
# are available, as the bootloader might have loaded its multiboot structures or
# modules there. This lets the bootloader know it must avoid the addresses.
.section .bss, "aw", @nobits
    .align 4096
boot_page_directory:
    .skip 4096

.align 4096
boot_page_table:
    .skip 4096



# The linker script specifies _start as the entry point to the kernel and the
# bootloader will jump to this position once the kernel has been loaded. It
# doesn't make sense to return from this function as the bootloader is gone.
.section .text
.global _start
.type _start, @function
_start:
    # Welcome to kernel mode! We now have sufficient code for the bootloader to
    # load and run our operating system. It doesn't do anything interesting yet.
    # Perhaps we would like to call printf("Hello, World\n"). You should now
    # realize one of the profound truths about kernel mode: There is nothing
    # there unless you provide it yourself. There is no printf function. There
    # is no <stdio.h> header. If you want a function, you will have to code it
    # yourself. And that is one of the best things about kernel development:
    # you get to make the entire system yourself. You have absolute and complete
    # power over the machine, there are no security restrictions, no safe
    # guards, no debugging mechanisms, there is nothing but what you build.

    # By now, you are perhaps tired of assembly language. You realize some
    # things simply cannot be done in C, such as making the multiboot header in
    # the right section and setting up the stack. However, you would like to
    # write the operating system in a higher level language, such as C or C++.
    # To that end, the next task is preparing the processor for execution of
    # such code. C doesn't expect much at this point and we only need to set up
    # a stack. Note that the processor is not fully initialized yet and stuff
    # such as floating point instructions are not available yet.

    # Use a temporary physical stack before paging is enabled.
    movl $(stack_top - KERNEL_VIRTUAL_BASE), %esp

    # Preserve multiboot registers while setting up paging state.
    push %eax
    push %ebx

    # Clear bootstrap page directory.
    movl $(boot_page_directory - KERNEL_VIRTUAL_BASE), %edi
    xorl %eax, %eax
    movl $1024, %ecx
    rep stosl

    # Fill a single page table to identity-map the first 4 MiB.
    movl $(boot_page_table - KERNEL_VIRTUAL_BASE), %edi
    xorl %ecx, %ecx

.Lfill_boot_pt:
    movl %ecx, %eax
    shll $12, %eax
    orl $0x3, %eax
    movl %eax, (%edi, %ecx, 4)
    incl %ecx
    cmpl $1024, %ecx
    jne .Lfill_boot_pt

    # Install the table at PDE 0 (identity) and PDE 768 (higher-half).
    movl $(boot_page_table - KERNEL_VIRTUAL_BASE), %eax
    orl $0x3, %eax
    movl $(boot_page_directory - KERNEL_VIRTUAL_BASE), %edi
    movl %eax, (%edi)
    movl %eax, (KERNEL_PDE_INDEX * 4)(%edi)

    # Enable paging and jump into the higher-half mapping.
    movl $(boot_page_directory - KERNEL_VIRTUAL_BASE), %eax
    movl %eax, %cr3
    movl %cr0, %eax
    orl $0x80000000, %eax
    movl %eax, %cr0
    leal .Lhigher_half_start, %ecx
    jmp *%ecx

    # We are now ready to actually execute C code. We cannot embed that in an
    # assembly file, so we'll create a kernel.c file in a moment. In that file,
    # we'll create a C entry point called kernel_main and call it here.
.Lhigher_half_start:
    # Translate physical ESP to higher-half virtual address.
    # Multiboot args (ebx, eax) are already on the stack in the correct
    # order for kernel_main(multiboot_info_t *info, uint32_t magic).
    addl $KERNEL_VIRTUAL_BASE, %esp

    # Execute C++ kernel entry point from higher-half virtual addresses.
    call kernel_main

    # In case the function returns, we'll want to put the computer into an
    # infinite loop. To do that, we use the clear interrupt ('cli') instruction
    # to disable interrupts, the halt instruction ('hlt') to stop the CPU until
    # the next interrupt arrives, and jumping to the halt instruction if it ever
    # continues execution, just to be safe. We will create a local label rather
    # than real symbol and jump to there endlessly.
    cli
.Lhang:
    hlt
    jmp .Lhang

# Set the size of the _start symbol to the current location '.' minus its start.
# This is useful when debugging or when you implement call tracing.
.size _start, . - _start
