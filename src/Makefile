CC=i586-elf-g++
CPPFLAGS=-ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
ASM=i586-elf-as
OBJDIR=../bin
BOOT=$(OBJDIR)/boot.o

_OBJS=kernel.o VGATextTerminal.o
OBJS=$(patsubst %,$(OBJDIR)/%,$(_OBJS))

.PHONY: all clean

all: $(OBJDIR)/kernel.bin $(OBJDIR)/mongoose.iso

$(OBJDIR)/kernel.bin: $(BOOT) $(OBJS)
	$(CC) -T linker.ld -o $(OBJDIR)/kernel.bin -ffreestanding -O2 -nostdlib $(BOOT) $(OBJS) -lgcc

$(BOOT): boot.s | $(OBJDIR)
	$(ASM) boot.s -o $@

$(OBJDIR)/%.o: %.cpp %.hpp | $(OBJDIR)
	$(CC) $(CPPFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $@

$(OBJDIR)/mongoose.iso: $(OBJDIR)/kernel.bin 
	cp -r iso $(OBJDIR)/iso
	cp $(OBJDIR)/kernel.bin $(OBJDIR)/iso/boot/
	grub-mkrescue --output=$(OBJDIR)/mongoose.iso $(OBJDIR)/iso


clean:
	rm -rf $(OBJDIR) 
