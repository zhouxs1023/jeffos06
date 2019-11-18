#ifndef __IONODE_H
#define __IONODE_H

typedef struct _io_node_t
{
	unsigned char Name[IONAMESIZE];
	unsigned long Port;
	struct _io_node_t *Next;
} io_node_t;


#endif /* __IONODE_H */
