/* $Id: namer.c,v 1.1.1.1 1998/02/19 04:23:54 swetland Exp $ */

#include <namer.h>
#include <error.h>
#include <sys/syscall.h>

#include <string.h>

int namer_newhandle(void)
{
    return port_create(NAMER_PORT);    
}

int namer_delhandle(int nh)
{
    return port_destroy(nh);    
}

int namer_register(int nh, int port, char *name)
{
    namer_message_t msg;
    msg_hdr_t mh;

    msg.number = port;

    strncpy(msg.text,name,32);
    msg.text[NAMER_TEXT_MAX-1] = 0;

    mh.src = nh;
    mh.dst = NAMER_PORT;
    mh.size = sizeof(namer_message_t);
    mh.data = &msg;    
    if(port_send(&mh) != sizeof(namer_message_t)) {
	os_console("namer_register: Error on send");
	return ERR_RESOURCE;
    }

    mh.dst = nh;    
    if(port_recv(&mh) != sizeof(int)) {
	os_console("namer_register: error on recv");
	return ERR_RESOURCE;
    }

    return msg.number;    
}

int namer_find(int nh, char *name)
{
    return namer_register(nh, 0, name);    
}
