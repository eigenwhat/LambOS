# Context switch
#
#   void context_switch(struct context **old, struct context *new);
#
# Save current register context in old
# and then load register context from new.

.globl context_switch
context_switch:
  # Save old callee-saved registers
  pushl %ebp
  pushl %eax
  pushl %ebx
  pushl %ecx
  pushl %edx
  pushl %esi
  pushl %edi

  # Switch stacks
  movl 32(%esp), %eax
  movl 36(%esp), %edx
  movl %esp, (%eax)
  movl %edx, %esp

  # Load new callee-saved registers
  popl %edi
  popl %esi
  popl %edx
  popl %ecx
  popl %ebx
  popl %eax
  popl %ebp
  ret
