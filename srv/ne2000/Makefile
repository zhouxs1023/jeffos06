# $Id: Makefile,v 1.2 1998/03/09 10:24:16 swetland Exp $
include ../../make.conf

CFLAGS = $(CF) -I../../include
LIBS = -L../../lib -lconsole -lc

all: ne2000.bin

OBJS = ne2k.o ne2000.o


ne2000.bin: $(OBJS)
	$(LD) -dN -Ttext 080 -o ne2000.bin ../../lib/crt0.o $(OBJS) $(LIBS)
	$(NM) -nC ne2000.bin > ne2000.map
	$(ST) ne2000.bin

clean:
	rm -f *.o *~ os.* core *.core *.s *.map *.bin
