#ifndef __SEC_H
#define __SEC_H

#include <types.h>

typedef struct _DECAF {
	uint32 bits[4];
} decaf_t;

typedef struct _SL {
	unsigned char sq;
	uint32 sc[4];
} sl_t;

typedef struct _PRIV {
	uint32 bits[4];
} priv_t;

typedef struct _SEC {
	uint32	uid;
	uint32	gid;
	decaf_t	decaf;
	sl_t	clearance;
	sl_t	effective;
	priv_t	privs;
} sec_t;

#endif __SEC_H
