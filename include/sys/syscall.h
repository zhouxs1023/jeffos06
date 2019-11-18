#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <types.h>

//
// Port options
//
#define PORT_OPT_NOWAIT		1
#define PORT_OPT_SETRESTRICT	2
#define PORT_OPT_SETDEFAULT	3
#define PORT_OPT_SLAVE		4

typedef struct {
	int flags;
	int src;
	int dst;
	int size;
	int pid;
	void *data;
} msg_hdr_t;

#define msg_SEND	1
#define msg_RECV	2
#define msg_NOWAIT	3

// Port System Calls
int port_create(int restrict);
int port_destroy(int port);
int port_option(int port, int opt, int arg);
int port_send(msg_hdr_t *mh);
int port_recv(msg_hdr_t *mh);

#define port_set_restrict(port, restrict) port_option(port, PORT_OPT_SETRESTRICT, restrict)
#define port_set_slave(master, slave) port_option(slave, PORT_OPT_SLAVE, master)

// Debugging system calls

void os_console(char *string);
void os_debug();
void wait();

// Process System calls
int os_thread(void *addr, unsigned int shmkey);
void terminate_process(int status);

// Shared Memory Calls
unsigned int os_CreateSegment(unsigned int LenInPages);
unsigned int os_AttachSegment(unsigned int key);
unsigned int os_CloseSegment(unsigned int key);



#endif /* _SYSCALL_H */



