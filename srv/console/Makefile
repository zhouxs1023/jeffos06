# $Id: Makefile,v 1.2 1998/03/09 10:24:16 swetland Exp $
include ../../make.conf

CFLAGS = $(CF) -I../../include
LIBS = -L../../lib -lconsole -lc

all: console.bin

console.bin: console.o
	$(LD) -dN -Ttext 080 -o console.bin ../../lib/crt0.o console.o $(LIBS)
	$(NM) -nC console.bin > console.map
	$(ST) console.bin

clean:
	rm -f *.o *~ os.* core *.core *.s *.map *.bin
