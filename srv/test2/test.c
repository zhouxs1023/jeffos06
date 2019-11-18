#include <sys/syscall.h>
#include <namer.h>
#include <conio.h>

void main(void)
{
	unsigned long i, j;
	int nh;
	int p;
	char a[2];
	char *mem, *mem2;

oprintf("Test 2: Starting up\n");

	a[1] = 0;

	for(i = 0; i < 2000000; i++)
		j = 0;
	while((mem = (char *)os_AttachSegment(0)) == 0);
	while((mem2= (char *)os_AttachSegment(1)) == 0);

	getch();
	
	oprintf("Test\n");

	while(1){
		for(i = 0; i < 5000000; i++)
			j = 0;
		oprintf("%s\n", mem);
		oprintf("%s\n", mem2);
	}

    
}
