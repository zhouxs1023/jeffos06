#ifndef __DEVICE_H
#define __DEVICE_H

// Device Message Types
#define 0	DVC_NULL
#define 1	DVC_READ
#define 2	DVC_WRITE
#define 3	DVC_CONTROL

typedef struct device_message_t
{
	unsigned long type;

}

#endif
