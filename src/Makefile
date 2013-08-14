CC=i586-elf-gcc
CPP=i586-elf-g++
CFLAGS=-std=gnu99 $(COMMONFLAGS)
COMMONFLAGS=-I./include -I./include/libc -I./include/libcpp -ffreestanding -Wall -Wextra
CPPFLAGS=$(COMMONFLAGS) -fno-exceptions -fno-rtti
ASM=i586-elf-as
OBJDIR=../bin
BOOT=$(OBJDIR)/boot.o

_OBJS=Kernel.o GlobalDescriptorTable.o InterruptDescriptorTable.o VGATextTerminal.o string.o set_gdt.o set_idt.o interrupt.o
OBJS=$(patsubst %,$(OBJDIR)/%,$(_OBJS))

.PHONY: all clean

all: $(OBJDIR)/kernel.bin $(OBJDIR)/mongoose.iso

$(OBJDIR)/kernel.bin: $(BOOT) $(OBJS)
	$(CPP) -T linker.ld -o $(OBJDIR)/kernel.bin -ffreestanding -O2 -nostdlib $(BOOT) $(OBJS) -lgcc

$(OBJDIR)/%.o: kernel/%.s | $(OBJDIR)
	$(ASM) $< -o $@

$(OBJDIR)/%.o: libc/%.c include/libc/%.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: kernel/%.c include/kernel/%.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: kernel/%.cpp include/kernel/%.hpp | $(OBJDIR)
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(OBJDIR)/%.o: device/display/%.cpp include/device/display/%.hpp | $(OBJDIR)
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.cpp include/%.hpp | $(OBJDIR)
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $@

$(OBJDIR)/mongoose.iso: $(OBJDIR)/kernel.bin 
	cp -r iso $(OBJDIR)/iso
	cp $(OBJDIR)/kernel.bin $(OBJDIR)/iso/boot/
	grub-mkrescue --output=$(OBJDIR)/mongoose.iso $(OBJDIR)/iso


clean:
	rm -rf $(OBJDIR) 
