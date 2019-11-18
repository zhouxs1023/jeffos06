#include <sys/syscall.h>

void
os_Sleep(int sec)
{
	asm("mov $55, %%ax ; int $0x22":"=eax" ((int) sec)
					:"ebx" ((int) sec));
	return;
}
	

int
os_CreateSem(int count)
{
	asm("mov $94, %%ax ; int $0x22":"=eax" ((int) count)
					:"ebx" ((int) count));
	return count;
}

int
os_AcquireSem(int id)
{
	asm("mov $95, %%ax ; int $0x22":"=eax" ((int) id)
					:"ebx" ((int) id));
	return id;
}

int
os_ReleaseSem(int id)
{
	asm("mov $96, %%ax ; int $0x22":"=eax" ((int) id)
					:"ebx" ((int) id));
	return id;
}

int
os_DestroySem(int id)
{
	asm("mov $94, %%ax ; int $0x22":"=eax" ((int) id)
					:"ebx" ((int) id));
	return id;
}

unsigned int
os_CreateSegment(unsigned int LenInPages)
{
	unsigned int key;

	asm("mov $91, %%ax ; int $0x22":"=eax" ((unsigned int) key)
					:"ebx" ((unsigned int) LenInPages));

	return key;
}

unsigned int 
os_AttachSegment(unsigned int key)
{
	unsigned int mem;

	asm("mov $92, %%ax ; int $0x22":"=eax" ((unsigned int) mem)
					:"ebx" ((unsigned int) key));

	return mem;
}

unsigned int
os_CloseSegment(unsigned int key)
{
	asm("mov $93, %%ax ; int $0x22":"=eax" ((unsigned int) key)
					:"ebx" ((unsigned int) key));

	return key;
}

int
os_brk(int addr)
{
	asm("mov $90, %%ax ; int $0x22":"=ebx" ((unsigned int) addr)
				      :"ebx" ((unsigned int) addr));
	return (int) addr;
}

void os_console(char *string)
{
	asm("mov $1, %%ax ; int $0x22"::"ebx" ((unsigned int) string)); 
}

void
os_sleep_irq(void)
{
	int r = 0;

	asm("mov $23, %%ax ; int $0x22"::"ebx"((unsigned int) r));
}

void
os_handle_irq(int irq)
{

	asm("mov $21, %%ax ; int $0x22"::"ebx"((unsigned int) irq));
}


void
terminate_process(int status)
{
	asm("mov $51, %%ax ; int $0x22"::"ebx" ((unsigned int) status));
}


int
port_create(int restrict)
{
	asm("mov $10, %%ax ; int $0x22":"=eax" ((unsigned int) restrict)
				      :"ecx"((unsigned int) restrict));
	return restrict;
}

int
port_destroy(int port)
{
	asm("mov $13, %%ax ; int $0x22":"=eax" ((unsigned int) port)
				      :"ecx"((unsigned int) port));
	return port;
}

int
port_send(msg_hdr_t *mh)
{
	int r;

	asm("mov $12, %%ax ; int $0x22":"=eax" ((unsigned int) r)
				      :"ebx"((unsigned int) mh));
	return r;
}

int
port_recv(msg_hdr_t *mh)
{
	int r;

	asm("mov $11, %%ax ; int $0x22":"=eax" ((unsigned int) r)
		       :"ebx"((unsigned int) mh));
	return r;
}

int
port_option(int port, int opt, int arg)
{
	asm("mov $14, %%ax ; int $0x22":"=eax" ((unsigned int) opt)
				      :"ecx"((unsigned int) port),
				       "ebx"((unsigned int) arg),
				       "edx"((unsigned int) opt));
	return opt;
}

int
os_thread(void *addr, unsigned int arg)
{
	asm("mov $50, %%ax ; int $0x22":"=eax" ((unsigned int) addr)
				      :"ebx" ((unsigned int) addr),
						"ecx"((unsigned int) arg));
	return (unsigned int) addr;
}

void os_debug() 
{
	int r = 0;

	asm("mov $3, %%ax ; int $0x22"::"ebx" ((unsigned int) r));
}

void wait()
{
	int r = 0;
	asm("mov $58, %%ax ; int $0x22"::"ebx" ((unsigned int) r));
}
