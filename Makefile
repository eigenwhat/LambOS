
.PHONY: all clean go

all:
	cd src; make

clean:
	cd src; make clean

go: 
	bochs -f bochsrc.bxrc -q; rm none

godebug: 
	bochsdbg -f bochsrc.bxrc -q; rm none

