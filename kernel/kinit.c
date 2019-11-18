#include <types.h>
#include <boot.h>
#include "mem.h"
#include "int.h"
#include "task.h"
#include "status.h"
#include "i386.h"
#include "syscall.h"
#include "resource.h"
#include "memory.h"
#include "ktrace.h"
#include "port.h"
#include "shm.h"

extern uint32 _cr3;
extern uint32 *IDT;
extern taskq TaskList[MaxProc];
extern task *KernelTask;
extern task *CurrentTask;
extern unsigned long CurrentTaskNum;
extern char *gdt;
extern uint32 *KernelPageTable;
extern unsigned char PMAT[2048];
extern uint32 nextentry;
extern SYSCALL SysCallTable[256];
extern boot_dir *bdir;
extern taskq TaskList[MaxProc];
extern task *irq_task_map[16];
extern shm_t *ShmTable;
extern uint32 ShmEntries;
extern uint32 MaxUsedProc;
extern uint32 SystemTimer;

#define EX(n) extern void __ex##n(void);
EX(0); EX(1); EX(2); EX(3); EX(4); EX(5); EX(6); EX(7); EX(8); EX(9);
EX(10); EX(11); EX(12); EX(13); EX(14); EX(15); EX(16); EX(17); EX(18);

void kinit_ktask( void );
void kinit_gdt( void );

void
kinit_SharedMemory()
{
	int i;

	ShmTable = MmAllocPages(KernelPageTable, 1, 3, NULL);

	ShmEntries = 4096 / sizeof(shm_t);

	for(i = 0; i < ShmEntries; i++)
		ShmTable[i].mem = 0;
}

void
kinit_namerport()
{
	msg_port *namerport;
	uint32 namerportnum;
	uint32 i;

	namerport  = rsrc_find_port(namerportnum = port_create(0));

	kprintf("namer port allocated with rid of: %d\n", namerportnum);

	for(i = 0; i < 5; i++)
		if((TaskList[i].t != 0) && (TaskList[i].t->tid == 1))
		{
			namerport->owner = TaskList[i].t;
			kprintf("successfully assigned namer port\n");
		}
}

void
kinit_resource()
{
	rsrc_init(MmAllocPages((aspace *)KernelPageTable, 8, 3, NULL), 4096*3);
	memory_init();

}

void
kinit_syscall() {
	uint32 i;

	for(i = 0; i < 256; i++)
		SysCallTable[i] = NullSysCall;

	SysCallTable[1] = ScOsConsole;
	SysCallTable[3] = ScOsDebug;
	SysCallTable[10] = ScCreatePort;
	SysCallTable[11] = ScReadPort;
	SysCallTable[12] = ScSendPort;
	SysCallTable[13] = ScClosePort;
	SysCallTable[14] = ScSetPortOptions;
	SysCallTable[21] = ScHandleIRQ;
	SysCallTable[23] = ScSleepIRQ;
	SysCallTable[50] = ScCreateThread;
	SysCallTable[51] = ScTerminateProcess;
	SysCallTable[55] = ScSleep;
	SysCallTable[58] = ScWait;
	SysCallTable[90] = ScOsBrk;
	SysCallTable[91] = ScCreateSegment;
	SysCallTable[92] = ScAttachSegment;
	SysCallTable[93] = ScCloseSegment;
	SysCallTable[94] = ScCreateSemaphore;
	SysCallTable[95] = ScAcquireSemaphore;
	SysCallTable[96] = ScReleaseSemaphore;
	SysCallTable[97] = ScDestroySemaphore;
}

/*
 * Memory initialization routines
 */
void
kinit_page() {
	uint32 i;
	uint32 j;
//	uint32 *a = 0;

//	j = 262;
//	a = j * 4096;
//	kprintf("1: %d [0x%x]: 0x%x ", j, j * 4096, a[0]);

	/* Get a pointer to the Page tables */
	KernelPageTable = (uint32 *) 0x803FD000;

	/*
	 * Zero out the physical memory table - Each bit corresponds to 
	 * 1 page of physical memory.  So if PMAT[0] == 2 then page #0 and #1
	 * are allocated while pages #2 - #7 are unallocated
	 */
	for(i = 0; i < 2048; i++)
		PMAT[i] = 0;

	//j = 262;
	//a = j * 4096;
	//kprintf("2: %d [0x%x]: 0x%x ", j, j * 4096, a[0]);

	/* Need to mark all of the allocated OS pages */
	/* Set aside the first two pages*/
	kset_PageBit(0);
	kset_PageBit(1);

	//j = 262;
	//a = j * 4096;
	//kprintf("3: %d [0x%x]: 0x%x ", j, j * 4096, a[0]);

	/* 
	 * Mark the system memory as in use
	 * This memory contains the video RAM and other BUS device memory
	 */
	for(i = 160; i < 256; i++)
		kset_PageBit(i);

	//j = 262;
	//a = j * 4096;
	//kprintf("4: %d [0x%x]: 0x%x ", j, j * 4096, a[0]);

	/* 
	 * Mark the pages which are already loaded up
	 */
	for(i = 2; bdir->bd_entry[i].be_type; i++) {
		//kprintf("Setting pages for %s: ", bdir->bd_entry[i].be_name);
		for(j = bdir->bd_entry[i].be_offset + 0x100; 
		    j < bdir->bd_entry[i].be_offset + 0x100 + bdir->bd_entry[i].be_size;
		    j++) {
			//a = j * 4096;
			//kprintf("%d [0x%x]: 0x%x ", j, j * 4096, a[0]);
			kset_PageBit(j);
		}
		kprintf("\n");
	}

	/*
	 * Mark the page tables as used
	 * The physical address of the kernel page directory is equal to the
	 * physical address of the low page table - 4096 (the size of the page 
	 * table) We cannot use the pointer to the page directory as it is a 
	 * mapped address rather than a physical address.
	 * By dividing this number by 4096 we get the page number or Bit to set.
	 */
	kset_PageBit( ((unsigned long)KernelPageTable[0] - 4096) / 4096);
	kset_PageBit(( ((unsigned long)KernelPageTable[0] - 4096) / 4096) + 1);
	kset_PageBit(( ((unsigned long)KernelPageTable[0] - 4096) / 4096) + 2);
}


/*
 * IDT initialization routines
 */

void
kinit_idt() {
	unsigned long i;
	uint32 p;


	// Initializes the irq task map
	for(p = 0; p < 16; p++)
		irq_task_map[p] = NULL;

	/*
	 * point IDT to 0h linear
	 */
//	IDT = 0x00000000;
	IDT = MmAllocPages((aspace *)KernelPageTable, 1, 3, (PPHYSICAL *)&p);
//	kprintf("Allocted IDT at: 0x%x with physical 0x%x\n", IDT, p);


	/*
	 * Set all of the interrupts to point to a null handler
	 */
	for(i = 0; i < 256; i++)
		kadd_int(IDT, i, __null_irq);

	/*
	 * Set up system timer
	 */
	kadd_int(IDT, 0x30, __timer_irq);

	/*
	 * Set up system call handler interrupt
	 */
	kadd_intU(IDT, 0x22, __syscall22);
	
	/*
	 * Set up exception handlers
	 */
	kadd_int(IDT, 0x00, __ex0);
	kadd_int(IDT, 0x01, __ex1);
	kadd_int(IDT, 0x02, __ex2);
	kadd_int(IDT, 0x03, __ex3);
	kadd_int(IDT, 0x04, __ex4);
	kadd_int(IDT, 0x05, __ex5);
	kadd_int(IDT, 0x06, __ex6);
	kadd_int(IDT, 0x07, __ex7);
	kadd_int(IDT, 0x08, __ex8);
	kadd_int(IDT, 0x09, __ex9);
	kadd_int(IDT, 0x0a, __ex10);
	kadd_int(IDT, 0x0b, __ex11);
	kadd_int(IDT, 0x0c, __ex12);
	kadd_int(IDT, 0x0d, __ex13);
	kadd_int(IDT, 0x0e, __ex14);
	kadd_int(IDT, 0x0f, __ex15);
	kadd_int(IDT, 0x10, __ex16);
	kadd_int(IDT, 0x11, __ex17);
	kadd_int(IDT, 0x12, __ex18);

	/*
	 * Set up hardware interrupts
	 */
	kadd_int(IDT, 0x31, __irq1);
	kadd_int(IDT, 0x32, __irq2);
	kadd_int(IDT, 0x33, __irq3);
	kadd_int(IDT, 0x34, __irq4);
	kadd_int(IDT, 0x35, __irq5);
	kadd_int(IDT, 0x36, __irq6);
	kadd_int(IDT, 0x37, __irq7);
	kadd_int(IDT, 0x38, __irq8);
	kadd_int(IDT, 0x39, __irq9);
	kadd_int(IDT, 0x3a, __irq10);
	kadd_int(IDT, 0x3b, __irq11);
	kadd_int(IDT, 0x3c, __irq12);
	kadd_int(IDT, 0x3d, __irq13);
	kadd_int(IDT, 0x3e, __irq14);
	kadd_int(IDT, 0x3f, __irq15);

	i386lidt((uint32) IDT, 0x3FF);
	remap_irqs();
	unmask_irq(0);
	__enableirq();
}

/*
 * Kernel task initialization functions
 */
void
kinit_task( void ) {
	/*
	 * Get cr3 (page table address to store in tss
	 */
	asm("mov %%cr3, %0":"=r" (_cr3));
kprintf("Kernel cr3 is: 0x%x\n", _cr3);

	MaxUsedProc = 0;

	kinit_gdt();
	kinit_ktask();

	SystemTimer = 0;
	
}

void
kinit_ktask( void ) {
	TSS *ktss;
	unsigned long i;
	STATUS Status;
	struct _process *Process;

	Process = (process *) MmAllocPages((aspace *)KernelPageTable, 1, 3, NULL);
	Process->addr = (aspace *)KernelPageTable;
	Process->pid = 0;
	Process->name[0] = 'I';
	Process->name[1] = 'd';
	Process->name[2] = 'l';
	Process->name[3] = 'e';
	Process->name[4] = ' ';
	Process->name[5] = 'P';
	Process->name[6] = 'r';
	Process->name[7] = 'o';
	Process->name[8] = 'c';
	Process->name[9] = '\0';


	for(i = 0; i < MaxProc; i++)
		TaskList[i].t = 0;

	ktss = MmAllocPages((aspace *)KernelPageTable, 1, 3, NULL);
	KernelTask = (task *)ktss;
	KernelTask->state = tREADY;
	KernelTask->Parent = Process;
	CurrentTask = KernelTask;
	CurrentTaskNum = 0;

	i386SetSegment(gdt + SEL_KTSS,
		(uint32) ktss, 104,
		i386rPRESENT | i386rDPL0 | i386rTSS,
		0);


	/* Setup kernel TSS */
	KernelTask->tid = SEL_KTSS;
	ktss->ss0 = SEL_KDATA;
	ktss->esp1 = ktss->esp2 = ktss->ss1 = ktss->ss2 = 0;

	ktss->cs = SEL_KCODE;
	ktss->ss = ktss->ds = ktss->es = ktss->fs = ktss->gs = SEL_KDATA;
	ktss->ldt = ktss->debugtrap = ktss->iomapbase = 0;
	ktss->cr3 = _cr3;

	i386ltr(SEL_KTSS);

	Status = PsAddTaskToQueue(KernelTask, 0);
	if(Status != STATUS_SUCCESS) {
		kprintf("ERROR: unable to add kernel task to queue\n");
		return;
	}

//	kprintf("Allocated ktss at: %x\n", ktss);
}

void
kinit_gdt() {
	PPHYSICAL phys;

	gdt = MmAllocPages((aspace *)KernelPageTable, 1, 3, NULL);

	nextentry = 48;

//	kprintf("Allocated GDT at: 0x%x\n", gdt);
	MmMappedToPhysical((aspace *)KernelPageTable, (PVIRTUAL) gdt, (PPHYSICAL *)&phys);

	i386SetSegment(gdt + SEL_KCODE,
		0, 0xFFFFF,
		i386rPRESENT | i386rDPL0 | i386rCODE,
		i386g4K | i386g32BIT);

	i386SetSegment(gdt + SEL_KDATA,
		0, 0xFFFFF,
		i386rPRESENT | i386rDPL0 | i386rDATA,
		i386g4K | i386g32BIT);


	i386SetSegment(gdt + SEL_KDATA2,
		0, 0xFFFFF,
		i386rPRESENT | i386rDPL0 | i386rDATA,
		i386g4K | i386g32BIT);


	i386SetSegment(gdt + SEL_UCODE,
		0, 0xFFFFF,
		i386rPRESENT | i386rDPL3 | i386rCODE,
		i386g4K | i386g32BIT);


	i386SetSegment(gdt + SEL_UDATA,
		0, 0xFFFFF,
		i386rPRESENT | i386rDPL3 | i386rDATA,
		i386g4K | i386g32BIT);

	i386lgdt((uint32) gdt, 1024/8);
}

