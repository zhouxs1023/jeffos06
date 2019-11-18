#include <sys/syscall.h>
#include <iospace.h>

static int ioport;

void main(void)
{
	int nh;
	int nullport;
	msg_hdr_t msg;
	char data[81];
	io_message_t iomsg;

	// Create a port which accepts from anyone
	nullport = port_create(0);

	while(!(nh = namer_newhandle()));

oprintf("NULL: Found the namer handle\n");

	while(!(ioport = namer_find(nh, "io")));

oprintf("NULL: Found the IO handle: %d\n", ioport);

	namer_delhandle(nh);


	// Set up the iomsg
	iomsg.Type = IO_REGISTER;
	iomsg.Port = nullport;
	strcpy(iomsg.Name, "/dev/null\0");

	// Set up port message
	msg.src = nullport;
	msg.data = &iomsg;
	msg.dst = ioport;
	msg.size = sizeof(iomsg);

	// Send the io register
	if(port_send(&msg) < sizeof(iomsg))
	{
		oprintf("NULL: Error registering\n");
		terminate_process(0);
	}

oprintf("NULL: Sent the IO register\n");

	//
	// Try and find myself
	//

	// Set up a FIND io message
	iomsg.Port = 0;
	iomsg.Type = IO_FIND;

	// Send the io find
	if(port_send(&msg) < sizeof(iomsg))
	{
		oprintf("NULL: Error finding myself\n");
		terminate_process(0);
	}

oprintf("NULL: Sent the IO find\n");

	// Wait for the reply
	nh = port_create(ioport);
	msg.src = nh;
	if(port_recv(&msg) < sizeof(iomsg))
	{
		oprintf("NULL: Error getting find reply\n");
		terminate_process(0);
	}

	port_destroy(nh);

	oprintf("Found my io space and port as: %d  I am: %d\n", iomsg.Port, nullport);
	

	//
	// Loop and do nothing with data
	//
	msg.src = nullport;
	while(1)
	{
		port_recv(&msg);
	}
	
}


