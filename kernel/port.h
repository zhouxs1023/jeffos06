//
// Portions of this code are from OpenBlt, by Brian Swetland
//

#ifndef _PORT_H_
#define _PORT_H_

#include "types.h"

typedef struct {
    int flags;
    int src;
    int dst;
    int size;
	int pid;
    void *data;    
} msg_hdr_t;

typedef struct _chain 
{
    struct _chain *next;
} chain;

typedef struct _msg {
    struct _msg *next;
    uint32 size;
    int from_port;
    int to_port;    
	int pid;
    void  *data;    
} message;

typedef struct _port 
{
    struct _task *owner;   /* task that owns this port */
    int restrict;          /* inbound messages restricted to this thread */
    int msgcount;          /* number of messages waiting in the queue */
    int slaved;            /* deliver my messages to a master port */     
    message *first;        /* head of the queue */
    message *last;         /* tail of the queue */
    int refcount;          /* counts owner and any slaved ports */
} msg_port;


int port_create(int restrict);
int port_destroy(int port);
uint32 port_option(uint32 port, uint32 opt, uint32 arg);
#ifdef OLD
int port_send(int from, int port, void *msg, int len);
int port_recv(int port, void *msg, int len, int *from);
#endif
int port_send(msg_hdr_t *mh);
int port_recv(msg_hdr_t *mh);

#endif
