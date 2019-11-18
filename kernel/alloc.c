#include <types.h>
#include "task.h"
#include "conio.h"
#include "ktrace.h"
#include "mem.h"
#include "alloc.h"

#define INITIALCHUNKSIZE	100

int memlock;
#define LOCKMEM while(memlock == 1) { PsSwitchTask(); } memlock = 1
#define UNLOCKMEM memlock = 0

memchunk_t *FreeList;
memchunk_t *UsedList;
memchunk_t *FreeTail;
memchunk_t *UsedTail;

extern uint32 *KernelPageTable;
extern void k_debugger();


void
displist(memchunk_t *List)
{
	memchunk_t *tmp = List;
return;

	kprintf("list: ");

	while(tmp)
	{
		kprintf("0x%x ", tmp);
		tmp = tmp->Next;
	}

	kprintf("\n");

}

void
printchunk(char *trail, memchunk_t *chunk)
{
return;
kprintf("%s: 0x%x  S: 0x%x  L: 0x%x  P: 0x%x  N: 0x%x\n",
	trail, chunk, chunk->Start, chunk->Length, chunk->Prev, chunk->Next);

}
void
init_alloc()
{
	// Initially there are no memory chunks in use
	UsedList = NULL;
	UsedTail = NULL;

	// We can only allocate at a one page granularity right now so
	// allocate 1 page for the freelist, then fix it up later
	FreeList = AllocateChunk();

	// Allocate an initial 10 pages for the memory unit
	FreeList->Start = (uint32) MmAllocPages((aspace *)KernelPageTable, INITIALCHUNKSIZE, 3, NULL);

	FreeList->Length = INITIALCHUNKSIZE * 4096;

	FreeList->Prev = NULL;
	FreeList->Next = NULL;
	FreeTail = FreeList;

	memlock = 0;

	printchunk("I chunk", FreeList);
}

memchunk_t *
AllocateChunk()
{
	return (memchunk_t *)MmAllocPages((aspace *)KernelPageTable, 1, 3, NULL);
}

void
FreeChunk(memchunk_t *chunk)
{
	MmFreePages((aspace *)KernelPageTable, chunk, 1);
	return;
}

	

memchunk_t *
GetChunk(unsigned long size)
{
	memchunk_t *entry = FreeList;

	while(entry) {
		if(entry->Length > size)
			return entry;
		entry = entry->Next;
	}

	return NULL;
}


void
PutChunkInUsedList(memchunk_t *chunk)
{
	// Remove chunk from freelist
	if(chunk->Prev)					// Are we at the Head of FreeList?
	{										// No
		if(chunk == FreeTail)					// Are we the Tail?
			FreeTail = chunk->Prev;					// Yes

		chunk->Prev->Next = chunk->Next;
	}
	else
	{										// Yes
		if(chunk == FreeTail)					// Are we also the Tail?
			FreeTail = NULL;						// Yes

		FreeList = chunk->Next;
		if(chunk->Next)
			chunk->Next->Prev = NULL;
	}

	// Place chunk at top of used list
	chunk->Next = UsedList;
	chunk->Prev = NULL;
	if(UsedList)
		UsedList->Prev = chunk;
	UsedList = chunk;
		
	return;
}


memchunk_t *
BreakChunk(memchunk_t *chunk, unsigned long size)
{
	memchunk_t *newchunk = AllocateChunk();

	//printchunk("before chunk", chunk);

	newchunk->Start = chunk->Start;
	newchunk->Length = size;
	newchunk->Prev = chunk->Prev;
	newchunk->Next = chunk;
	chunk->Prev = newchunk;

	chunk->Start = chunk->Start + size;
	chunk->Length = chunk->Length - size;

	if(chunk == FreeList)
		FreeList = newchunk;

	//printchunk("new chunk", newchunk);
	//printchunk("after chunk", chunk);


//k_debugger();
	
	return newchunk;
}

memchunk_t *
UseChunk(memchunk_t *chunk, unsigned long size)
{
	memchunk_t *newchunk;

	if(chunk->Length == size)
	{
		// The chunk is the exact size so move it over to the Used list
		PutChunkInUsedList(chunk);

	}
	else
	{
		// Shave off the required amount and put it onto the used list
//kprintf("Freeprior B: ");
//displist(FreeList);
//kprintf("Usingprior B: ");
//displist(UsedList);
		newchunk = BreakChunk(chunk, size);
//kprintf("Freepost B: ");
//displist(FreeList);
//kprintf("Usingpost B: ");
//displist(UsedList);
		PutChunkInUsedList(newchunk);
//kprintf("Freepost P: ");
//displist(FreeList);
//kprintf("Usingpost P: ");
//displist(UsedList);
	}

	return newchunk;
}

memchunk_t *
FindChunk(void *ptr, memchunk_t *MemList)
{
	memchunk_t *tmp = MemList;

	while(tmp)
	{
		if(tmp->Start == (uint32) ptr)
			return tmp; 

		tmp = tmp->Next;
	}

	kprintf("<FC>: Could not find chunk for 0x%x\n", ptr);
	return NULL;
}
void
MergeChunkIntoFreeList(memchunk_t *chunk)
{
	memchunk_t *tmp = FreeList;

	//
	// First step is to clean up the UsedList
	//
	if(chunk->Prev == NULL)			// Are we the Head?
	{
		if(chunk->Next == NULL)		// Are we also the tail?
		{
			UsedList = NULL;
			UsedTail = NULL;
		}
		else
		{
			UsedList = chunk->Next;
			UsedList->Prev = NULL;
		}

	}
	else
	{
		chunk->Prev->Next = chunk->Next;
		chunk->Next->Prev = chunk->Prev;
	}
#ifdef ndef
kprintf("Chunk Removed:\n");
displist(UsedList);
#endif

	//
	// Now that the chunk is removed from the Used List, it can be reattached
	// to the FreeList in the correct order
	//
	// Since the Free List is in ascending order of start addresses of chunks
	// we simply need to cycle through the list until we find a chunk which
	// starts after us.  The passed in chunk is then inserted before this one.
	//
	while(tmp)
	{
		if(tmp->Start > chunk->Start)
		{
			chunk->Prev = tmp->Prev;
			chunk->Next = tmp;
			tmp->Prev->Next = chunk;
			tmp->Prev = chunk;
			return;
		}
	}

	//
	// We have scanned the whole list with no luck
	// Therefore this chunk needs to be inserted at the end of the FreeList
	//
	chunk->Prev = FreeTail;
	chunk->Next = NULL;
	FreeTail->Next = chunk;
	
}

void *
alloc(unsigned long size)
{
	memchunk_t *ptr;
	memchunk_t *newchunk;

	while(memlock)
		PsSwitchTask();
	LOCKMEM;

	if(size == 0)
	{
		UNLOCKMEM;
		return 0;
	}

	// Is there a free chunk of this size?
	ptr = GetChunk(size);

//printchunk("chunk", ptr);

	// If yes, then put it onto the used list
	if(ptr)
	{
		ptr = UseChunk(ptr, size);
	}
	else // If no, then try to allocate more memory
	{
		//
		// Add on some more pages to the tail of the free list
		//
		FreeTail->Next = MmAllocPages((aspace *)KernelPageTable, INITIALCHUNKSIZE, 3, NULL);
		if(FreeTail->Next == NULL) // Check for alloc error
		{
			kprintf("<ALLOC>: Could not allocate memory\n");
			UNLOCKMEM;
			return NULL;
		}
		FreeTail->Next->Next = NULL;
		FreeTail->Next->Prev = FreeTail;
		FreeTail = FreeTail->Next;

		ptr = GetChunk(size);
		if(!ptr)
		{
			UNLOCKMEM;
			return NULL;
		}
		ptr = UseChunk(ptr, size);
	}

if(ptr == NULL)
	kprintf("RETURNING NULL POINTER IN ALLOC\n");

printchunk("A Chunk", ptr);
displist(UsedList);

	// Return the memory pointer
	UNLOCKMEM;


	return (void *) ptr->Start;
}

void
free(void *ptr)
{
	memchunk_t *chunk;

#ifdef ndef
	kprintf("FREE: Mem ptr: 0x%x\n", ptr);
#endif

	while(memlock)
		PsSwitchTask();
	LOCKMEM;


	// Is pointer on used list?
	chunk = FindChunk(ptr, UsedList);

printchunk("F Chunk", chunk);
displist(UsedList);
	//
	// If not return cause they just made something up (or my code is broken)
	//
	if(chunk == NULL)
	{
		UNLOCKMEM;
		return;
	}


	// Otherwise merge it back into the free list
	MergeChunkIntoFreeList(chunk);

printchunk("F2 Chnk", chunk);
displist(UsedList);

	UNLOCKMEM;
	return;
}
