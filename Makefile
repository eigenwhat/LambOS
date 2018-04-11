.PHONY: all clean go-bochs go-qemu go

all:
	@cd src; $(MAKE)

clean:
	@cd src; $(MAKE) clean

go-bochs: 
	@bochs -f bochsrc.bxrc -q; rm none

go-qemu:
	@qemu-system-i386 -cdrom bin/lambos.iso -m 32

go: go-qemu

