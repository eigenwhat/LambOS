OBJDIR=../bin
INCLUDEDIR=./include
CC=i586-elf-gcc
CPP=i586-elf-g++
CFLAGS=-std=gnu99 $(COMMONFLAGS)
COMMONFLAGS=-I./kernel -I$(INCLUDEDIR) -I$(INCLUDEDIR)/libc -I$(INCLUDEDIR)/libcpp -ffreestanding -O2 -Wall -Wextra -g
CPPFLAGS=$(COMMONFLAGS) -fno-exceptions -fno-rtti
ASM=i586-elf-as
BOOT=$(OBJDIR)/boot.o

_LIBC=string.o stdlib.o
_LIBCPP=
_KOBJS=main.o Kernel.o GlobalDescriptorTable.o InterruptDescriptorTable.o VGATextTerminal.o set_gdt.o set_idt.o interrupt.o PageTable.o PageFrameAllocator.o MMU.o

_OBJS=$(_LIBC) $(_LIBCPP) $(_KOBJS)
OBJS=$(patsubst %,$(OBJDIR)/%,$(_OBJS))

VPATH=kernel kernel/device kernel/device/display kernel/cpu kernel/mem

.PHONY: all clean

all: $(OBJDIR)/kernel.bin $(OBJDIR)/mongoose.iso

$(OBJDIR)/kernel.bin: $(BOOT) $(OBJS)
	$(CPP) -T linker.ld -o $(OBJDIR)/kernel.bin -ffreestanding -O2 -nostdlib $(BOOT) $(OBJS) -lgcc

$(OBJDIR)/%.o: libc/%.c $(INCLUDEDIR)/libc/%.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.c %.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

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

$(OBJDIR)/mongoose.iso: $(OBJDIR)/kernel.bin 
	cp -r iso $(OBJDIR)/iso
	cp $(OBJDIR)/kernel.bin $(OBJDIR)/iso/boot/
	grub-mkrescue --output=$(OBJDIR)/mongoose.iso $(OBJDIR)/iso


clean:
	rm -rf $(OBJDIR) 
