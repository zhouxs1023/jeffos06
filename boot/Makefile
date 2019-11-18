# $Id: Makefile,v 1.3 1998/03/16 22:15:53 jwthomp Exp $
include ../make.conf

CFLAGS = $(CF) -I../kernel -I../include

all: boot.bin

boot.bin: bootstub.o boot.o 
	$(LD) -dN -Ttext 101080 -o BOOT.BIN bootstub.o boot.o ../lib/libconsole.a ../lib/libkern.a
	$(NM) -nC BOOT.BIN > boot.map
	$(ST) BOOT.BIN

clean:
	rm -f *.o *~ os.* core *.core *.s *.map *.bin

