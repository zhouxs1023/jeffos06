
include ../../make.conf

CFLAGS = $(CF) -I../../include
LIBS = -L../../lib -lc

all: test.bin

test.bin: test.o
	$(LD) -dN -Ttext 080 -o test.bin ../../lib/crt0.o test.o $(LIBS)
	$(NM) -nC test.bin > test.map
	$(ST) test.bin

clean:
	rm -f *.o *~ os.* core *.core *.s *.map *.bin
