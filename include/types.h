#ifndef __TYPES_H
#define __TYPES_H

#include <error.h>

#define CHAR_BIT 8
#define NULL ((void *) 0)


#define ntohs(n) ( (((n) & 0xFF00) >> 8) | (((n) & 0x00FF) << 8) )
#define htons(n) ( (((n) & 0xFF00) >> 8) | (((n) & 0x00FF) << 8) )

typedef unsigned int size_t;

typedef unsigned long uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

typedef unsigned long STATUS;

typedef void *PVIRTUAL;
typedef void *PPHYSICAL;

#define IN
#define OUT

#define E_NONE 		0
#define E_PERMISSION	-1
#define E_RESOURCE	-2
#define E_MEMORY	-3

#endif
