# NOTE: use gmake for this Makefile

include ../make.conf

CFLAGS = $(CF) -I../include 
LIBS = -L../lib -lkern -lconsole

all: kernel.bin


OBJS = main.o ktrace.o mem.o jump.o int.o task.o i386.o kinit.o helper.o syscall.o debug.o port.o resource.o shm.o alloc.o memory.o object.o queue.o sem.o

kernel.bin: $(OBJS)
	$(LD) -dN -Ttext 80000080 -o kernel.bin $(OBJS) $(LIBS)
	$(NM) -nC kernel.bin > kernel.map
	$(ST) kernel.bin
	cat tmp >> kernel.bin

jump.o: jump.S
	$(CC) -c jump.S

clean:
	rm -f *.o *~ os.* core *.core *.s *.map *.bin *.BIN
