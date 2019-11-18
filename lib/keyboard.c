#include <sys/syscall.h>
#include <namer.h>
#include <conio.h>



int
getch()
{
	int nh;
	static int keyh;
	static int stdin = 0;
	static int init = 0;
	msg_hdr_t msg;
	int data[8];
	int val;



	data[0] = 0xf;
	msg.data = data;
	msg.size = 4;

	if(!init) 
	{
		// Find the keyboard service
		nh = namer_newhandle();
		stdin = namer_find(nh, "keyboard\0");
		namer_delhandle(nh);
		keyh = port_create(0);
		msg.src = keyh;
		msg.dst = stdin;
		port_send(&msg);
		init = 1;
	}

	msg.src = stdin;
	msg.dst = keyh;
	port_recv(&msg);

	val = ((int *)msg.data)[0];
	return val;
}

void
printf(char *fmt,...)
{
	va_list pvar;
	char *x;
	static msg_hdr_t msg;
	static char kpbuf[512];
	static char *kp = kpbuf;
	int nh;
	static int consh;
	static int stdout = 0;
	static int init = 0;

#ifdef ndef
	if(!init) 
	{
		// Find the keyboard service
		nh = namer_newhandle();
		stdout = namer_find(nh, "console\0");
		namer_delhandle(nh);
		consh = port_create(0);
		msg.src = consh;
		msg.dst = stdout;
		port_send(&msg);
		init = 1;
	}
#endif

	va_start(pvar, fmt);
	va_snprintf(kp, 512 - (kp - kpbuf), fmt, pvar);
	va_end(pvar);

	for(x = kp; *x; x++)
	{
		if(*x == '\n')
		{
			*x = 0;
#ifdef ndef
			msg.size =strlen(kpbuf);
			msg.data = kpbuf;
			msg.src = consh;
			msg.dst = stdout;
			port_send(&msg);
#endif
			os_console(msg.data);
			kp = kpbuf;
			return;
		}
	}
	kp = x;
}
	
void
oprintf(char *fmt,...)
{
	va_list pvar;
	char *x;
	static char kpbuf[256];
	static char *kp = kpbuf;

	va_start(pvar, fmt);
	va_snprintf(kp, 256 - (kp - kpbuf), fmt, pvar);
	va_end(pvar);

	for(x = kp; *x; x++)
	{
		if(*x == '\n')
		{
			*x = 0;
			os_console(kpbuf);
			kp = kpbuf;
			return;
		}
	}
	kp = x;
}
	

