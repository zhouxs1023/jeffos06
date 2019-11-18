#ifndef __OBJECT_H
#define __OBJECT_H

#include <types.h>

//
// NOTE: All objects are mapped into virtual kernel memory and are referenced 
// from there.
//


//-------------------------
// Defines
//-------------------------
#define MAX_HANDLES					64

#define OBJ_TYPE_NULL				0		// Not an object
#define OBJ_TYPE_ASPACE				1		// Uses aspace typedef
#define OBJ_TYPE_PROCESS			2		// Uses task typedef
#define OBJ_TYPE_THREAD				3		// Uses task typedef
#define OBJ_TYPE_PORT				4		// Uses msg_port typedef
#define OBJ_TYPE_SHARED_MEMORY		5		// Uses shm_t typedef
#define OBJ_TYPE_SEMAPHORE			6		// (NYI)

//---------------------------------------------------------------------
// Structures
//---------------------------------------------------------------------

//
// Object structure which maintains the type, the number of references
//   and a pointer to a memory structure which describes that particular
//   object.
//
typedef struct OBJECT
{
	unsigned long type;
	unsigned long refcount;
	void *ptr;
} OBJECT_t;

//---------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------
STATUS
ObCreateObject(
	IN unsigned long type,
	OUT unsigned long *Handle,
	OUT void **obj
	);

STATUS
ObReferenceObjectByHandle(
	IN unsigned long Handle,
	OUT struct OBJECT **Obj
	);

STATUS
ObOpenObject(
	IN struct OBJECT *obj
	);

STATUS
ObDereferenceObject(
	IN struct OBJECT *ptr
	);

STATUS
ObCloseObject(
	IN unsigned long Handle
	);


#endif /* __OBJECT_H */
