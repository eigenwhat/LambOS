.section .text
.global _start
.type _start, @function
_start:
    call main
    ret

# Set the size of the _start symbol to the current location '.' minus its start.
# This is useful when debugging or when you implement call tracing.
.size _start, . - _start
