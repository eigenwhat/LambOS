//
// interrupt.s -- Contains interrupt service routine wrappers.
//                Based on Bran's kernel development tutorials.
//                Rewritten for JamesM's kernel development tutorials.

// This macro creates a stub for an ISR which does NOT pass it's own
// error code (adds a dummy errcode byte).
.macro ISR_NOERRCODE intnum
  .globl isr\intnum
  isr\intnum:
    cli
    xchg %bx, %bx
    pushl $0                     // Push a dummy error code.
    pushl $\intnum               // Push the interrupt number.
    jmp isr_common_stub
.endm

// This macro creates a stub for an ISR which passes it's own
// error code.
.macro ISR_ERRCODE intnum
  .globl isr\intnum
  isr\intnum:
    cli
    pushl $\intnum                // Push the interrupt number
    jmp isr_common_stub
.endm

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

// This is our common ISR stub. It saves the processor state, sets
// up for kernel mode segments, calls the C-level fault handler,
// and finally restores the stack frame.
isr_common_stub:
    pushal

    xorl %eax, %eax
    movw %ds, %ax       // Lower 16-bits of eax = ds.
    push %eax         // save the data segment descriptor

    movw $0x10, %ax    // load the kernel data segment descriptor
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    call interrupt_handler

    pop %ebx          // reload the original data segment descriptor
    movw %bx, %ds
    movw %bx, %es
    movw %bx, %fs
    movw %bx, %gs

    popal              // Pops edi,esi,ebp...
    add $8, %esp       // Cleans up the pushed error code and pushed ISR number
    //sti
    iret               // pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP
