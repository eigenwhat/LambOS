
.PHONY: all clean go

all: bin/mongoose.iso

bin/mongoose.iso:
	cd src; make

clean:
	cd src; make clean

go: all
	bochs -f bochsrc.bxrc -q; rm none

