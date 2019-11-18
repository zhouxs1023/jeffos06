#include <types.h>
#include "shm.h"
#include "mem.h"
#include "helper.h"
#include "status.h"

shm_t *ShmTable;
uint32 ShmEntries;
extern uint32 *KernelPageTable;
extern task *CurrentTask;

//
// Create a shared memory segment
// Returns key to segment and a pointer to the memory in the callers address 
// space
//
STATUS
SmCreateSeg(OUT unsigned long *key, IN uint32 SizeInPages)
{
	int i;
	uint32 tmpmem;
	STATUS Status;
	*key = 0;

	for(i = 0; i < ShmEntries; i++)
	{
		if(ShmTable[i].mem == 0)
			break;
	}
	if(i == ShmEntries) {
		kprintf("SmCreateSeg: NO MORE SHARED MEMORY SEGS only: %d\n", ShmEntries);
		KeHalt();
		return STATUS_OUT_OF_SHM;
	}

	*key = i;

	tmpmem = (uint32) MmAllocPages((aspace *)KernelPageTable, SizeInPages, 3, NULL);

	Status = kunmap_Page((aspace *)KernelPageTable, (PVIRTUAL)tmpmem, 
					(PPHYSICAL *)&(ShmTable[i].mem), SizeInPages);

	ShmTable[i].RefCount = 0;
	ShmTable[i].LenInPages = SizeInPages;

kprintf("SmCreateSeg: Created a segment with key: %d and mem: 0x%x\n", i, ShmTable[i].mem);

	return STATUS_SUCCESS;
}

STATUS
SmAttachSeg(IN uint32 key, OUT uint32 *mem)
{
	STATUS Status;
	unsigned long Handle;
	shm_t *obj;


	*mem = 0;

	// Sanity checks
	if(key >= ShmEntries)
	{
		kprintf("SmAttachSeg: key > ShmEntries\n");
		return STATUS_INVALID_SHMSEG; 
	}
	if(ShmTable[key].mem == 0)
	{
		return STATUS_INVALID_SHMSEG;
	}

	//
	// Create a handle for the process for this shared memory segment
	//
	Status = ObCreateObject(OBJ_TYPE_SHARED_MEMORY, &Handle, (void *)&obj);
	if(Status != STATUS_SUCCESS)
	{
		kprintf("SmAttachSeg: Unable to create handle for Shared Mem Segment\n");
		return STATUS_GENERAL_ERROR;
	}


	Status = kmap_LowPage(CurrentTask->Parent->addr, ShmTable[key].mem,
				(PPHYSICAL *)mem, ShmTable[key].LenInPages, 7);
	if(Status != STATUS_SUCCESS)
	{
		kprintf("SmAttachSeg: kmap_LowPage failed with status %x\n", Status);
		KeHalt();
		return Status;
	}

	//
	// Setup values for new object
	//
	obj->RefCount = key;
	obj->LenInPages = ShmTable[key].LenInPages;
	obj->mem = mem;

	ShmTable[key].RefCount++;

	return STATUS_SUCCESS;
}

STATUS
SmCloseSeg(IN uint32 key)
{
	int i;
	int startbit;
	STATUS Status;
	process *Process = CurrentTask->Parent;
	shm_t *obj;

	// Sanity checks

	// NOTE: NEED TO VERIFY THAT THIS TASKS HAS A REFERENCE TO THE SHM SEG
	for(i = 0; i < MAX_HANDLES; i++)
	{
		if( (Process->HandleList[i]->type == OBJ_TYPE_SHARED_MEMORY) &&
			((shm_t *)Process->HandleList[i]->ptr)->RefCount == key)
		{
			obj = (shm_t *)Process->HandleList[i]->ptr;
			break;
		}
	}

	if(i == MAX_HANDLES)
		return STATUS_INVALID_SHMSEG;

	if(key >= ShmEntries)
		return STATUS_INVALID_SHMSEG; 
	if(ShmTable[key].mem == 0)
		return STATUS_INVALID_SHMSEG;

	switch(ShmTable[key].RefCount)
	{
	case 0:
		Status = MmFreePages(CurrentTask->Parent->addr, obj->mem, 
					obj->LenInPages);
		return Status;
	default:
		Status = kunmap_Page(CurrentTask->Parent->addr, obj->mem,
					(PPHYSICAL *) &i, obj->LenInPages);
		if(Status != STATUS_SUCCESS)
			return Status;
		ShmTable[key].RefCount--;
		return STATUS_SUCCESS;
	}

	
}
