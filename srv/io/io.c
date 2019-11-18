#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <iospace.h>

#include "ionode.h"

io_node_t *NodeList;

static int sendport;


unsigned int
io_register(io_message_t *msg)
{
	io_node_t *Node = NodeList;
	io_node_t *PrevNode = NULL;


	while(Node)
	{
		if(strlen(msg->Name) == strlen(Node->Name))
			if(!strcmp(msg->Name, Node->Name))
				return IOR_EXISTS;

		PrevNode = Node;
		Node = Node->Next;
	}

	PrevNode->Next = (io_node_t *) malloc(sizeof(io_node_t));
	strncpy(PrevNode->Next->Name, msg->Name, IONAMESIZE);
	PrevNode->Next->Port = msg->Port;
	PrevNode->Next->Next = NULL;

oprintf("IO: Registered '%s' from port: %d\n", msg->Name, msg->Port);
	
	return IOR_SUCCESS;
}

void
io_find(io_message_t *msg)
{
	io_node_t *Node = NodeList;
	io_node_t *PrevNode = NULL;

oprintf("IO: Trying to find '%s'\n", msg->Name);

	while(Node)
	{
		if(strlen(msg->Name) == strlen(Node->Name))
			if(!strcmp(msg->Name, Node->Name))
			{
				msg->Type = IOR_SUCCESS;
				msg->Port = Node->Port;

oprintf("IO: Found on port %d\n", msg->Port);

				return;
			}

		PrevNode = Node;
		Node = Node->Next;
	}

oprintf("IO: Not Found");

	msg->Type = IOR_DOESNOTEXIST;
	msg->Port = 0;
}

void
io_send(msg_hdr_t *msg, io_message_t *iomsg)
{
	msg_hdr_t msg2;

	msg2.src = sendport;
	msg2.dst = msg->src;
	msg2.size = sizeof(io_node_t);
	msg2.data = iomsg;

	if(port_send(&msg2) < sizeof(io_node_t))
		oprintf("IOSPACE: Error sending packet\n");

	return;
}

void main(void)
{
	int nh;
	int ioport;
	msg_hdr_t msg;
	io_message_t iomsg;

oprintf("IO: Starting up\n");

	// Create a port which accepts from anyone
	ioport = port_create(0);

	// Create a sending port for the io manager
	sendport = port_create(0);

	// Register the 'io' port
	nh = namer_newhandle();
	namer_register(nh, ioport, "io");
	namer_delhandle(nh);

	msg.data = &iomsg;
	msg.dst = ioport;
	msg.size = sizeof(io_node_t);

	NodeList = NULL;

	//
	// Message loop for io driver
	//
	while(port_recv(&msg) < 1)
	{
oprintf("IO: got a message\n");
		switch(iomsg.Type)
		{
		case IO_REGISTER:
			io_register(&iomsg);
			break;
		case IO_FIND:
			io_find(&iomsg);
			
			io_send(&msg, &iomsg);
			break;
		case IO_LIST:
//			io_list(&msg, &iomsg);
			break;
		default:
oprintf("IO: GOT AN UNKNOWN MESSAGE: %d\n", iomsg.Type);
			break;
		}
	}
	
}


