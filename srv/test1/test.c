#include <sys/syscall.h>
#include <namer.h>
#include <conio.h>

void main(void)
{
	unsigned long i, j;
	int nh;
	int p;
	char a[2];
	unsigned int key, key2;
	unsigned char *mem, *mem2;

oprintf("Test1: Starting up\n");

	a[1] = 0;

	key = os_CreateSegment(2);
	mem = (unsigned char *)os_AttachSegment(key);
	key2 = os_CreateSegment(2);
	mem2 = (unsigned char *)os_AttachSegment(key2);
	
	mem[0] = 'H';
	mem[1] = 'I';
	mem[2] = '\n';
	mem[3] = 0;

	mem2[0] = 'T';
	mem2[1] = 'H';
	mem2[2] = 'E';
	mem2[3] = 'r';
	mem2[4] = 'e';
	mem2[5] = '\n';
	mem2[6] = 0;

    
blah:
	for(i = 0; i < 20000000; i++)
		j = 0;
	goto blah;
}

