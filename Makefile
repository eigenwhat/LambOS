.PHONY: all clean go-bochs go-qemu go

all:
	@$(MAKE) --file=Makefile2

clean:
	@$(MAKE) --file=Makefile2 clean

go-bochs: 
	@bochs -f bochsrc.bxrc -q; rm none

go-qemu:
	@qemu-system-i386 -cdrom bin/lambos.iso -m 32

go: go-qemu

