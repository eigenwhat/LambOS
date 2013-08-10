.section .data
gdtr:
	.int 0 # For limit storage
    .long 0 # For base storage

.section .text
.globl set_gdt
# set_gdt(GDT, sizeof(GDT))
set_gdt:
   movl 4(%esp), %eax
   movl %eax, (gdtr+2)
   movw 8(%esp), %ax
   movw %ax, (gdtr)
   lgdt (gdtr)
   ret
