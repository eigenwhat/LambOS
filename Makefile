.PHONY: all clean go

all:
	cd src; $(MAKE)

clean:
	cd src; $(MAKE) clean

go: 
	bochs -f bochsrc.bxrc -q; rm none

godebug: 
	bochsdbg -f bochsrc-dbg.bxrc -q; rm none

