CC=i586-elf-g++
CPPFLAGS=-ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
ASM=i586-elf-as
BOOT=../bin/boot.o
OBJS=../bin/kernel.o ../bin/BasicTerminal.o

.PHONY: all clean

all: ../bin/kernel.bin ../bin/mongoose.iso

../bin/kernel.bin: ../bin/boot.o ../bin/kernel.o ../bin/BasicTerminal.o
	$(CC) -T linker.ld -o ../bin/kernel.bin -ffreestanding -O2 -nostdlib $(BOOT) $(OBJS) -lgcc

../bin/boot.o: boot.s
	mkdir -p ../bin
	$(ASM) boot.s -o ../bin/boot.o

../bin/kernel.o: kernel.cpp ../bin/BasicTerminal.o
	mkdir -p ../bin
	$(CC) $(CPPFLAGS) -c kernel.cpp -o ../bin/kernel.o

../bin/BasicTerminal.o: BasicTerminal.h BasicTerminal.cpp ITerminal.h
	mkdir -p ../bin
	$(CC) $(CPPFLAGS) -c BasicTerminal.cpp -o ../bin/BasicTerminal.o

../bin/mongoose.iso: ../bin/kernel.bin 
	cp -r iso ../bin/iso
	cp ../bin/kernel.bin ../bin/iso/boot/
	grub-mkrescue --output=../bin/mongoose.iso ../bin/iso

clean:
	rm -rf ../bin
