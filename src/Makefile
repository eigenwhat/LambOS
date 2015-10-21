ARCH=i386
OBJDIR=../bin
INCLUDEDIR=./include
CC=i586-elf-gcc
CPP=i586-elf-g++
CFLAGS=-std=gnu99 $(COMMONFLAGS)
COMMONFLAGS=-I./kernel/include -I./kernel/include/arch/$(ARCH) -I$(INCLUDEDIR) -I$(INCLUDEDIR)/libc -I$(INCLUDEDIR)/libcpp -ffreestanding -O2 -Wall -Wextra -fstack-protector-all
CPPFLAGS=$(COMMONFLAGS) -fno-exceptions -fno-rtti
ASM=i586-elf-as
BOOT=$(OBJDIR)/boot.o

_LIBC_OBJS=string.o itoa.o ctype.o putchar.o printf.o
_LIBCPP_OBJS=_lang.o
_LIBSYS_OBJS=PrintStream.o

_MMU_OBJS=PageTable.o PageFrameAllocator.o MMU.o liballoc.o liballoc_hook.o
_CPU_OBJS=X86CPU.o GlobalDescriptorTable.o InterruptDescriptorTable.o X86RealTimeClock.o set_gdt.o set_idt.o interrupt.o
_KOBJS= $(_MMU_OBJS) $(_CPU_OBJS) main.o Kernel.o VGATextConsole.o KeyboardInputStream.o PS2Keyboard.o

_OBJS=$(_LIBC_OBJS) $(_LIBCPP_OBJS) $(_LIBSYS_OBJS) $(_KOBJS)
OBJS=$(patsubst %,$(OBJDIR)/%,$(_OBJS))

ARCH_PATH=kernel/arch/$(ARCH)

_LIBC_VPATH=libc libc/stdlib libc/stdio
_LIBCPP_VPATH=libcpp
_LIBSYS_VPATH=libsys
_ARCH_VPATH=$(ARCH_PATH) $(ARCH_PATH)/cpu $(ARCH_PATH)/mem $(ARCH_PATH)/device $(ARCH_PATH)/device/display $(ARCH_PATH)/device/input
_KERNEL_VPATH=kernel kernel/device/display kernel/device/input kernel/cpu kernel/cpu/x86 kernel/mem
VPATH=$(_ARCH_VPATH) $(_LIBC_VPATH) $(_LIBCPP_VPATH) $(_LIBSYS_VPATH) $(_KERNEL_VPATH)

.PHONY: all clean

all: $(OBJDIR)/kernel.bin $(OBJDIR)/lambos.iso

$(OBJDIR)/kernel.bin: $(BOOT) $(OBJS)
	$(CPP) -T linker.ld -o $(OBJDIR)/kernel.bin -ffreestanding -O2 -nostdlib $(BOOT) $(OBJS) -lgcc

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.s | $(OBJDIR)
	$(ASM) $< -o $@

$(OBJDIR):
	@mkdir -p $@

$(OBJDIR)/lambos.iso: $(OBJDIR)/kernel.bin 
	@cp -r iso $(OBJDIR)/iso
	@cp $(OBJDIR)/kernel.bin $(OBJDIR)/iso/boot/
	grub-mkrescue --output=$(OBJDIR)/lambos.iso $(OBJDIR)/iso


clean:
	@rm -rf $(OBJDIR) 
