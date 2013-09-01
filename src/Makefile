OBJDIR=../bin
INCLUDEDIR=./include
CC=i586-elf-gcc
CPP=i586-elf-g++
CFLAGS=-std=gnu99 $(COMMONFLAGS)
COMMONFLAGS=-I./kernel -I$(INCLUDEDIR) -I$(INCLUDEDIR)/libc -I$(INCLUDEDIR)/libcpp -ffreestanding -O2 -Wall -Wextra -g
CPPFLAGS=$(COMMONFLAGS) -fno-exceptions -fno-rtti
ASM=i586-elf-as
BOOT=$(OBJDIR)/boot.o

_LIBC=string.o itoa.o
_LIBCPP=
_LIBSYS=

_MMU_OBJS=PageTable.o PageFrameAllocator.o MMU.o
_CPU_OBJS=GlobalDescriptorTable.o InterruptDescriptorTable.o set_gdt.o set_idt.o interrupt.o
_KOBJS= $(_MMU_OBJS) $(_CPU_OBJS) main.o Kernel.o VGATextTerminal.o

_OBJS=$(_LIBC) $(_LIBCPP) $(_LIBSYS) $(_KOBJS)
OBJS=$(patsubst %,$(OBJDIR)/%,$(_OBJS))

_LIBC_VPATH=libc libc/stdlib
VPATH=$(_LIBC_VPATH) kernel kernel/device kernel/device/display kernel/cpu kernel/mem

.PHONY: all clean

all: $(OBJDIR)/kernel.bin $(OBJDIR)/lambos.iso

$(OBJDIR)/kernel.bin: $(BOOT) $(OBJS)
	$(CPP) -T linker.ld -o $(OBJDIR)/kernel.bin -ffreestanding -O2 -nostdlib $(BOOT) $(OBJS) -lgcc

$(OBJDIR)/%.o: %.cpp %.hpp | $(OBJDIR)
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.s | $(OBJDIR)
	$(ASM) $< -o $@

$(OBJDIR):
	mkdir -p $@

$(OBJDIR)/lambos.iso: $(OBJDIR)/kernel.bin 
	cp -r iso $(OBJDIR)/iso
	cp $(OBJDIR)/kernel.bin $(OBJDIR)/iso/boot/
	grub-mkrescue --output=$(OBJDIR)/lambos.iso $(OBJDIR)/iso


clean:
	rm -rf $(OBJDIR) 
