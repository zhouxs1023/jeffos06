#ifndef _TASK_H_
#define _TASK_H_

#include <types.h>
#include <sys/sec.h>

#include "object.h"

#define tKERNEL       0		// Kernel task
#define tRUNNING      1		// Task is running
#define tREADY        2		// Task is ready
#define tSLEEP_PORT   3		// Task is blocked on port 
#define tSLEEP_IRQ    4		// Task is blocked on irq
#define tDEAD         5		// Task is dead and needs cleanup
#define tSLEEP_SEM	  6		// Task is sleeping on a semaphore
#define tSLEEP_TIMER  7		// Task is sleeping on a timer

#define MaxProc 1024

#define SEL_KCODE (1*8)
#define SEL_KDATA (2*8)
#define SEL_KDATA2 (3*8)
#define SEL_UCODE (4*8)
#define SEL_UDATA (5*8)
#define SEL_KTSS (6*8)
#define SEL_UTSS (7*8)

typedef struct _aspace {
	uint32 pdir[1023]; /* page directory -- 4k */
	uint32 refcount;
	uint32 ptab[1024]; /* page table -- 4k */
	uint32 high[1024]; /* high page table -- 4k (0x80000000 -> )*/
} aspace;

typedef struct TSS32 {
   unsigned short link, __unused0;
   unsigned long esp0;
   unsigned short ss0, __unused1;
   unsigned long esp1;
   unsigned short ss1, __unused2;
   unsigned long esp2;
   unsigned short ss2, __unused3;
   unsigned long cr3, eip, eflags;
   unsigned long eax,ecx,edx,ebx,esp,ebp,esi,edi;
   unsigned short es, __unused4;
   unsigned short cs, __unused5;
   unsigned short ss, __unused6;
   unsigned short ds, __unused7;
   unsigned short fs, __unused8;
   unsigned short gs, __unused9;
   unsigned short ldt, __unused10;
   unsigned short debugtrap, iomapbase;
}TSS;

typedef struct _task {
    TSS tss;
    char iomap[512];	// IO Privilege Map
    uint32 tid;		// Thread ID (To be used in the future)
	uint32 rid;		// Resource ID of task
    uint32 state;	// Current task state
    uint32 sleeping_on;
	uint32 timer;	// Time remaining on timer
    uint32 irq;		// irq handler associated with this task
    struct _process *Parent;
}task;

typedef struct _process {
	uint32 pid;					// Process ID
	unsigned char name[32];		// Process Name
	aspace *addr;				// Address space pointer
	struct OBJECT *HandleList[MAX_HANDLES];
} process;
	

typedef struct _taskq {
	task *t;
	unsigned long prior;
} taskq;


STATUS 
PsCreateNewTask(
	IN uint32 ip, 
	IN struct _process *Process
	);

STATUS 
PsTerminateTask(
	IN task *t
	);
void task_call(task *t);
STATUS PsAddTaskToQueue(IN task *t, IN uint32 prior);
STATUS PsRemoveTaskFromQueue(IN task *t);
void PsSwitchTask( void );
process * PsCreateProcess(IN uint32 phys, IN uint32 LenInPages, IN unsigned char *name);
STATUS PsCleanUpTask(IN task *Task);

#endif
