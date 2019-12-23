.globl read_PIT_count
read_PIT_count:
    pushfd
    cli
    movb 0, %al          ; al = channel in bits 6 and 7, remaining bits clear
    outb %al, 0x43       ; Send the latch command

    inb 0x40, %al        ; al = low byte of count
    movb %al, %ah        ; ah = low byte of count
    inb 0x40, %al        ; al = high byte of count
    rol 8, %ax           ; al = low byte, ah = high byte (ax = current count)
    popfd
    ret

