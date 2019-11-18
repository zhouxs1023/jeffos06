
include ../../make.conf

CFLAGS = $(CF) -I../../include
LIBS = -L../../lib -lc

all: null.bin

null.bin: null.o
	$(LD) -dN -Ttext 080 -o null.bin ../../lib/crt0.o null.o $(LIBS)
	$(NM) -nC null.bin > null.map
	$(ST) null.bin

clean:
	rm -f *.o *~ os.* core *.core *.s *.map *.bin
