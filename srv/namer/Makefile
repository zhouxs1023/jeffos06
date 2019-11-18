include ../../make.conf

CFLAGS = $(CF) -I../../include
LIBS = -L../../lib -lc

all: namer.bin

namer.bin: namer.o
	$(LD) -dN -Ttext 080 -o namer.bin ../../lib/crt0.o namer.o $(LIBS)
	$(NM) -nC namer.bin > namer.map
	$(ST) namer.bin

clean:
	rm -f *.o *~ os.* core *.core *.s *.map *.bin
