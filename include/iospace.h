#ifndef __IOSPACE_H
#define __IOSPACE_H

#define IONAMESIZE	256

//
// IO Message Types
//
#define IO_REGISTER			0x00000001	// Register a IO driver name
#define IO_FIND				0x00000002	// Find a IO driver by name
#define IO_LIST				0x00000003  // List IO drivers under a name
#define IO_ENDOFLIST		0x00000004  // Signal end of IO driver list replies

//
// IO Return Values
//
#define IOR_SUCCESS			0x00000000	// Successful handling
#define IOR_UNKNOWN			0x00000001	// Unknown Message Type
#define IOR_EXISTS			0x00000002	// Namespace already exists
#define IOR_DOESNOTEXIST	0x00000003	// Namespace does not exist

typedef struct _io_message_t
{
	unsigned long Type;				// Type of message
	unsigned long Port;
	unsigned char Name[IONAMESIZE];
} io_message_t;

#endif /* __IOSPACE_H */
