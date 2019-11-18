//
// object - is the object manager.  All actions done on an object must go
//			through here.
//

//-----------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------
#include <types.h>
#include "status.h"

#include "object.h"

#include "task.h"
#include "port.h"
#include "shm.h"

#include "mem.h"

//-----------------------------------------------------------------------
// Externs
//-----------------------------------------------------------------------
extern uint32 *KernelPageTable;
extern task *CurrentTask;


//-----------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------

//
// ObCreateObject - Creates an object of 'type' and returns a handle to it
//
STATUS
ObCreateObject(IN unsigned long type, OUT unsigned long *Handle, OUT void **Obj)
{
	struct OBJECT *obj;
	unsigned long i;

	// Allocate space for a new object;
//	obj = (struct OBJECT *) alloc(sizeof(struct OBJECT));
	obj = (struct OBJECT *) MmAllocPages((aspace *)KernelPageTable, 1, 7, NULL);

	obj->type = type;
	obj->refcount = 1;

	// Allocate space for the kernel object
	switch(type)
	{
	case OBJ_TYPE_ASPACE: 	// DONE
		// If type aspace then handle holds pointer to process
//		obj->ptr = (void *)alloc(sizeof(struct _aspace));
		obj->ptr = (void *) MmCreatePageTable();
		*Obj = obj->ptr;
		return STATUS_SUCCESS;

	case OBJ_TYPE_PROCESS:	// DONE
//		obj->ptr = (void *) alloc(sizeof(struct _process));
		obj->ptr = (void *) MmAllocPages((aspace *)KernelPageTable, 1, 7, NULL);
		//
		// We don't need to do anything with a process entry
		// as it is a root entity.
		//
		*Handle = 0;
		*Obj = obj->ptr;
		return STATUS_SUCCESS;

	case OBJ_TYPE_THREAD:	// DONE
		// If type thread then handle holds pointer to process
//		obj->ptr = (void *) alloc(sizeof(struct _task));
		obj->ptr = (void *) MmAllocPages((aspace *)KernelPageTable, 3, 3, NULL);

		// Set entry in process handle list
		for(i = 0; i < MAX_HANDLES; i++)
		{
			if( ((process *)Handle)->HandleList[i] == NULL )
			{
				((process *)Handle)->HandleList[i] = obj;
				*Obj = obj->ptr;
				return STATUS_SUCCESS;
			}
		}

kprintf("Object: CALLING FREE\n"); KeHalt();
		free(obj);
		return STATUS_OUT_OF_HANDLES;

	case OBJ_TYPE_PORT:				// TODO
		obj->ptr = (void *) alloc(sizeof(struct _port));
		break;

	case OBJ_TYPE_SHARED_MEMORY:	// TODO
		// Take the passed in handle to use as the object data
		obj->ptr = (void *) MmAllocPages((aspace *)KernelPageTable, 1, 7, NULL);
		break;

	case OBJ_TYPE_SEMAPHORE:		// TODO
	default:
kprintf("<ObCreateObject>: In case DEFAULT.\n");
		free(obj);
		return STATUS_INVALID_OBJECT_TYPE;
	}

	// Set entry in process handle list
	for(i == 0; i < MAX_HANDLES; i++)
	{
		if(CurrentTask->Parent->HandleList[i] == NULL)
		{
			CurrentTask->Parent->HandleList[i] = obj;
			*Obj = obj->ptr;
			return STATUS_SUCCESS;
		}
	}

kprintf("Object: CALLING FREE\n"); KeHalt();
	free(obj);
	return STATUS_OUT_OF_HANDLES;
}

//
// ObReferenceObjectByHandle - Takes a valid Handle for the task
//   and returns a pointer to the object in kernel space.  
//
//
//   NOTE: This also increments the reference count.
//                 WHY? WHY? WHY?
//
STATUS
ObReferenceObjectByHandle(IN unsigned long Handle, OUT struct OBJECT **ptr)
{
	if( Handle >= MAX_HANDLES)
	{
		*ptr = NULL;
		return STATUS_INVALID_HANDLE;
	}

	*ptr =  CurrentTask->Parent->HandleList[Handle];

	return STATUS_SUCCESS;
}

//
// ObOpenObject - Takes a valid pointer to an object and creates
//                handle for it in the current process
//				  This increments the refcount
//
// NOTE: This really needs some way to verify obj is a valid 
//		 object pointer.
//
STATUS
ObOpenObject(IN struct OBJECT *obj)
{
	unsigned long i;
	
	// Set entry in process handle list
	for(i == 0; i < MAX_HANDLES; i++)
	{
		if(CurrentTask->Parent->HandleList[i] == NULL)
		{
			CurrentTask->Parent->HandleList[i] = obj;
			return STATUS_SUCCESS;
		}
	}

	return STATUS_OUT_OF_HANDLES;
}

//
// ObDereferenceObject - decrements reference count for object 'ptr'
//   NOTE: This should only be called after a valid ObReference has been made.
//
STATUS
ObDereferenceObject(IN struct OBJECT *ptr)
{
	return STATUS_SUCCESS;
}

//
// ObCloseObject - takes a handle and removes it from the current process
//   and decrements the objects reference count.  This will remove the handle
//   from all process threads.
//
STATUS
ObCloseObject(IN unsigned long Handle)
{
	unsigned long i;
	struct OBJECT *obj;

	obj = CurrentTask->Parent->HandleList[Handle];

	if( (Handle >= MAX_HANDLES) || (obj == NULL) )
		return STATUS_INVALID_HANDLE;

	if( obj->refcount == 1 )
	{
		// The object is now unused so kill it off
		//if(obj->ptr)
			//free(obj->ptr);

		//free(obj);
		return STATUS_SUCCESS;
	}

	//
	// Just dereference the count by one and remove it from this
	// process's handle list
	obj->refcount--;
	CurrentTask->Parent->HandleList[Handle] = NULL;

	return STATUS_SUCCESS;
}
