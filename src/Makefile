CC=i586-elf-g++
CPPFLAGS=-ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
ASM=i586-elf-as

.PHONY: bindir all clean

all: kernel.bin boot_iso

kernel.bin: bindir boot.o kernel.o
	$(CC) -T linker.ld -o ../bin/kernel.bin -ffreestanding -O2 -nostdlib ../bin/boot.o ../bin/kernel.o -lgcc

bindir:
	mkdir -p ../bin

boot.o: bindir boot.s
	$(ASM) boot.s -o ../bin/boot.o

kernel.o: bindir kernel.cpp
	$(CC) $(CPPFLAGS) -c kernel.cpp -o ../bin/kernel.o

boot_iso: kernel.bin 
	cp -r iso ../bin/iso
	cp ../bin/kernel.bin ../bin/iso/boot/
	grub-mkrescue --output=../bin/mongoose.iso ../bin/iso

clean:
	rm -rf ../bin
