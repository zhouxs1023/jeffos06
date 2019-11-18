include ../../make.conf

CFLAGS = $(CF) -I../../include
LIBS = -L../../lib -lc

all: io.bin

io.bin: io.o
	$(LD) -dN -Ttext 080 -o io.bin ../../lib/crt0.o io.o $(LIBS)
	$(NM) -nC io.bin > io.map
	$(ST) io.bin

clean:
	rm -f *.o *~ os.* core *.core *.s *.map *.bin
