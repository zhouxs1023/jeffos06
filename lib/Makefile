# $Id: Makefile,v 1.4 1998/03/29 16:45:56 jwthomp Exp $

#include ../make.conf

CFLAGS = $(CF) -I../include

all: crt0.o libc.a libconsole.a libkern.a

KERN = snprintf.o string.o
USER = syscall.o conio.o namer.o keyboard.o
MEM = free.o malloc.o calloc.o realloc.o memory.o

libc.a: $(USER) $(KERN) $(MEM)
	rm -f libc.a
	$(AR) r libc.a $(USER) $(KERN) $(MEM)

libconsole.a: conio.o
	rm -f libconsole.a
	$(AR) r libconsole.a conio.o

libkern.a: $(KERN)
	rm -f libkern.a
	$(AR) r libkern.a $(KERN)

clean:
	rm -f *.o *~ os.* core *.core *.s *.map *.bin *.a
