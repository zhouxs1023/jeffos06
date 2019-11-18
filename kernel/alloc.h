#ifndef __ALLOC_H
#define __ALLOC_H

#include <types.h>



//-------------------------------------------------
// Structures
//-------------------------------------------------
typedef struct MEMCHUNK {
	uint32 Start;			// Starting address of memory chunk
	uint32 Length;			// Length in bytes
	struct MEMCHUNK *Prev;		// Pointer to previous chunk
	struct MEMCHUNK *Next;		// Pointer to next chunk
} memchunk_t;

//-------------------------------------------------
// Prototypes
//-------------------------------------------------
void init_alloc();
memchunk_t *AllocateChunk();
void FreeChunk(memchunk_t *chunk);
memchunk_t *GetChunk(unsigned long size);
void PutChunkInUsedList(memchunk_t *chunk);
memchunk_t *BreakChunk(memchunk_t *chunk, unsigned long size);
memchunk_t *UseChunk(memchunk_t *chunk, unsigned long size);
void *alloc(unsigned long size);
void free(void *ptr);

#endif /* __ALLOC_H */
