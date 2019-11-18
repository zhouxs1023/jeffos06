include ../../make.conf

CFLAGS = $(CF) -I../../include
LIBS = -L../../lib -lc

all: keyboard.bin

keyboard.bin: keyboard.o 
	$(LD) -dN -Ttext 080 -o keyboard.bin ../../lib/crt0.o keyboard.o $(LIBS)
	$(NM) -nC keyboard.bin > keyboard.map
	$(ST) keyboard.bin

clean:
	rm -f *.o *~ os.* core *.core *.s *.map *.bin
