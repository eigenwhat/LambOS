.section .data
idtr:
    .int 0 # For limit storage
    .long 0 # For base storage

.section .text
.globl set_idt
# set_idt(IDT, sizeof(IDT))
set_idt:
    movl 4(%esp), %eax
    movl %eax, (idtr+2)
    movw 8(%esp), %ax
    movw %ax, (idtr)
    lidt (idtr)
    ret
