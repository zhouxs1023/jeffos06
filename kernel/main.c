#include <types.h>
#include <io.h>
#include <boot.h>
#include "i386.h"
#include "conio.h"
#include "ktrace.h"
#include "mem.h"
#include "int.h"
#include "task.h"
#include "kinit.h"
#include "status.h"
#include "helper.h"
#include "resource.h"
#include "alloc.h"
#include "port.h"

extern unsigned long nMemMax;
extern uint32 *KernelPageTable;
extern void __testint(void);
extern uint32 _cr3;
extern task *KernelTask;
extern taskq TaskList[];

boot_dir *bdir;
uint32 entry_ebp;

struct _kinfo
{
	uint32 memsize;
	uint32 entry_ebp;
	boot_dir *bd;
} *kinfo = (struct _kinfo *) 0x80000000;

void _start() {
	PVIRTUAL i;
	uint32 j, k;
	STATUS Status;
	unsigned char *a;
	msg_port *namerport;
	uint32 namerportnum;
	process *Process;

	nMemMax = (kinfo->memsize) / 4096;
	entry_ebp = kinfo->entry_ebp;
	bdir = kinfo->bd;

	// Set up the console
	kprintf_init();
	con_fgbg(CON_WHITE, CON_BLUE);
	con_clear();
	kprintf("Main kernel loaded.\n");

	kprintf("Memory Size: %d\n", nMemMax);


	// Initialize the Memory Management Subsystem
	kinit_page();

	// Initialize the Resource Management Subsystem
	kinit_resource();

	// Initialize the Shared Memory Resources
	kinit_SharedMemory();

	// Initialize the memory allocation subsystem
	init_alloc();

	// Initialize the kernel task
	kinit_task();

	/* WARNING: Do not allocate any memory before here.  kinit_task
	 *     relocates the gdt into mapped memory
	 */

	i = MmAllocPages( (aspace *)KernelPageTable, 3, 3, NULL);
	if(i == NULL) {
		kprintf("Error allocating pages for esp for kernel\n");
		KeHalt();
	}
	asm("mov %0, %%esp"::"r" ( ((uint32) i + 4096 + 4096 + 4092) ));

	// Allocate a new stack for the kernel
//	kprintf("allocating stack: 0x%x\n", i + 4092);

	// Initialize the namer port
	namerport = rsrc_find_port(namerportnum = port_create(0));

	// Put kernel task into task list
//	KernelTask->rid = rsrc_new_task(KernelTask);

	for(j = 3; bdir->bd_entry[j].be_type; j++) {
		// Find tasks and get them set up

		Process = PsCreateProcess( 4096 *(bdir->bd_entry[j].be_offset + 0x100),
					 bdir->bd_entry[j].be_size, bdir->bd_entry[j].be_name);
		PsCreateNewTask(0x80, Process);
	}

	for(j = 0; j < 5; j++)
	{
		if((TaskList[j].t != 0) && (TaskList[j].t->tid == 1))
		{
			namerport->owner = TaskList[j].t;
			kprintf("successfully assigned namer port\n");
		}
	}



	kinit_syscall();

	// Initialize the Interrupt Descriptor Table and start up the
	// interrupt timer
	kinit_idt();

	// Switch to a new task
	PsSwitchTask();

	// The kernel task just goes through these simple operations to
	// prove that it is still running
	kprintf("System Idle Thread running...\n");

blah:
	// Kernel Idle thread
	// Loop a whole bunch to add some delay
	for(k = 0; k < 20000000; k++)
		j = 0;
	// Do it again
	goto blah;
	
}
