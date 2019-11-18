#ifndef __SHM_H
#define __SHM_H

typedef struct SHMSEG {
	unsigned long RefCount;
	unsigned long LenInPages;
	unsigned long *mem;
} shm_t;

#endif
