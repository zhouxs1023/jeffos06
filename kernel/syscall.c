#include <types.h>
#include "i386.h"
#include "int.h"
#include "conio.h"
#include "task.h"
#include "status.h"
#include "port.h"

//#define SysDebug
//#define PortDebug
//#define SemDebug

extern task *CurrentTask;
extern task *irq_task_map[16];
extern uint32 *KernelPageTable;

STATUS
ScSleep(regs r, uint32 eip, uint32 cs, uint32 eflags) {

#ifdef SysDebug
	kprintf("ScSleep: Called with a time of: %d\n", r.ebx);
#endif

	CurrentTask->state = tSLEEP_TIMER;
	CurrentTask->timer = r.ebx;

	PsSwitchTask();

	return r.ebx;
}

STATUS
ScCreateSemaphore(regs r, uint32 eip, uint32 cs, uint32 eflags) {

#ifdef SemDebug
	kprintf("<ScCreateSemaphore>: Entered with: 0x%x\n", r.ebx);
#endif
	return sem_create(r.ebx);
}

STATUS
ScAcquireSemaphore(regs r, uint32 eip, uint32 cs, uint32 eflags) {
#ifdef SemDebug
	kprintf("<ScAcquireSemaphore>: Entered with: 0x%x\n", r.ebx);
#endif
	return sem_acquire(r.ebx);
}

STATUS
ScReleaseSemaphore(regs r, uint32 eip, uint32 cs, uint32 eflags) {
#ifdef SemDebug
	kprintf("<ScReleaseSemaphore>: Entered with: 0x%x\n", r.ebx);
#endif
	return sem_release(r.ebx);
}

STATUS
ScDestroySemaphore(regs r, uint32 eip, uint32 cs, uint32 eflags) {
#ifdef SemDebug
	kprintf("<ScDestroySemaphore>: Entered with: 0x%x\n", r.ebx);
#endif

	return sem_destroy(r.ebx);
}




STATUS
ScCreateSegment(regs r, uint32 eip, uint32 cs, uint32 eflags) {
	unsigned long key;

#ifdef SysDebug
	kprintf("ScCreateSegment: Task #d called\n", CurrentTask->tid);
#endif

	SmCreateSeg(&key, r.ebx);

	return key;
}

STATUS
ScAttachSegment(regs r, uint32 eip, uint32 cs, uint32 eflags) {
	uint32 mem;

#ifdef SysDebug
	kprintf("ScAttachSegment: Task #d called\n", CurrentTask->tid);
#endif

	SmAttachSeg(r.ebx, &mem);

	return mem;
}

STATUS
ScCloseSegment(regs r, uint32 eip, uint32 cs, uint32 eflags) {

#ifdef SysDebug
	kprintf("ScCloseSegment: Task #d called\n", CurrentTask->tid);
#endif

	return SmCloseSeg(r.ebx);
}

STATUS
ScWait(regs r, uint32 eip, uint32 cs, uint32 eflags) {
#ifdef SysDebug
	kprintf("ScWait: Task #d called\n", CurrentTask->tid);
#endif
	PsSwitchTask();
}

STATUS
ScOsBrk(regs r, uint32 eip, uint32 cs, uint32 eflags) {

#ifdef SysDebug
	kprintf("ScOsBrk: Task %d called with address: 0x%x\n", CurrentTask->tid, r.ebx);
#endif

	return brk(r.ebx);
}


STATUS
ScUnHandleIRQ(regs r, uint32 eip, uint32 cs, uint32 eflags) {

#ifdef SysDebug
	kprintf("ScHandleIRQ: Task #%d called for irq: %d\n", CurrentTask->tid, r.ebx);
#endif

	if(r.ebx > 0 && r.ebx < 16)
	{
		// If this task actually is handling this interrupt then
		// unregister the handler
		if(CurrentTask->irq != r.ebx)
			return E_PERMISSION;
		irq_task_map[r.ebx] = 0;
	}
	// Strip off the eflags settings for IOPL as this task no longer needs
	// them
	CurrentTask->tss.eflags ^= 2<<12 | 2<<13;
	eflags ^= 2<<12 | 2<<13;

	return E_NONE;
}

STATUS
ScHandleIRQ(regs r, uint32 eip, uint32 cs, uint32 eflags) {

#ifdef SysDebug
	kprintf("ScHandleIRQ: Task #%d called for irq: %d\n", CurrentTask->tid, r.ebx);
#endif

	if(r.ebx > 0 && r.ebx < 16)
	{
		CurrentTask->irq = r.ebx;
		irq_task_map[r.ebx] = CurrentTask;
	}
	CurrentTask->tss.eflags |= 2<<12 | 2<<13;

	eflags |= 2<<12 | 2<<13;

	PsSwitchTask();
}

STATUS
ScSleepIRQ(regs r, uint32 eip, uint32 cs, uint32 eflags) {

#ifdef SysDebug
	kprintf("ScSleepIRQ: Task %d called\n", CurrentTask->tid);
#endif

	if(CurrentTask->irq) {

#ifdef SysDebug
		kprintf("ScSleepIRQ: The task does own the irq\n");
#endif

		CurrentTask->state = tSLEEP_IRQ;
		unmask_irq(CurrentTask->irq);
	}

	PsSwitchTask();
}

STATUS
ScOsDebug(regs r, uint32 eip, uint32 cs, uint32 eflags) {

#ifdef SysDebug
	kprintf("Kernel debugger called by task: %d\n", CurrentTask->tid);
#endif

	k_debugger();
	return STATUS_SUCCESS;
}

STATUS NullSysCall(regs r, uint32 eip, uint32 cs, uint32 eflags) {
#ifdef SysDebug
	kprintf("NullSysCall: Task #d called\n", CurrentTask->tid);
#endif

	return STATUS_GENERAL_ERROR;
}

STATUS ScOsConsole(regs r, uint32 eip, uint32 cs, uint32 eflags) {


	JeDebugPrint((char *)r.ebx);
	return STATUS_SUCCESS;
}



//
// ScCreateThread - Creates a new thread which has memory address r.eax
//                   and length r.ebx
//
STATUS 
ScCreateThread(regs r, uint32 eip, uint32 cs, uint32 eflags) {
	STATUS Status;
	task *Task;
	aspace *a;
	uint32 mem;

	// Later we will not allow unprivileged process to do this

#ifdef SysDebug
	kprintf("ScCreateThread: Task %d called with new ip of 0x%x\n", CurrentTask->tid, r.ebx);
#endif
	
	if(r.ecx == 0)
	{
		// Just create a normal new thread
		PsCreateNewTask(r.ebx, CurrentTask->Parent);
	}
	else if(r.ecx == 1)
	{

		//
		// This implements a fork
		//

		// Create a brand new process


//
// NOTE: THIS IS NOT FINISHED YET
//
kprintf("IN VERY BAD PLACE\n");
		// This will create a new process address space
		a = MmCreatePageTable();

		// Map in video memory
		aspace_map(a, 0xB0, 0xB0, 0x10 + 2, 7);

		// Then create a new thread which uses it
//		Status = PsCreateNewTask(a, 0x80, &Task);
		if(Status != STATUS_SUCCESS)
			kprintf("<ScCreateThread>: ERROR, cannot create new task\n");

		// Map in a shared memory segment from creating process
		Status = SmAttachSeg(Task, r.ecx, &mem);
		if(Status != STATUS_SUCCESS)
			kprintf("<ScCreateThread>: ERROR, cannot attach Shared Mem Segment.\n");
	}

#ifdef SysDebug
	kprintf("ScCreateThread: new task is tid: %x\n", Task->tid);
#endif
	return 0;

}

STATUS
ScCreatePort(regs r, uint32 eip, uint32 cs, uint32 eflags) {
	int a;

	a = port_create(r.ecx);

#ifdef PortDebug
	kprintf("task %X: port_create(%x) returned %d\n", CurrentTask->tid, r.ecx, a);
#endif

	return a;
}

STATUS
ScReadPort(regs r, uint32 eip, uint32 cs, uint32 eflags) {
	uint32 tcr3;

#ifdef PortDebug
	kprintf("task %X: port_recieve(%x)\n", CurrentTask->tid, r.ebx);
	kprintf("       : src %x  dst: %x  size %x  data %x\n",
		((msg_hdr_t *)r.ebx)->src,
		((msg_hdr_t *)r.ebx)->dst,
		((msg_hdr_t *)r.ebx)->size,
		((int) ((msg_hdr_t *)r.ebx)->data));
#endif

	return port_recv((void *) r.ebx);
}

STATUS
ScSendPort(regs r, uint32 eip, uint32 cs, uint32 eflags) {
#ifdef PortDebug
	kprintf("task %X: port_send(%x)\n", CurrentTask->tid, r.ebx);
	kprintf("       : src %x  dst %x  data %x  size %d\n",
		((msg_hdr_t *)r.ebx)->src,
		((msg_hdr_t *)r.ebx)->dst,
		((int) ((msg_hdr_t *)r.ebx)->data),
		((msg_hdr_t *)r.ebx)->size);
#endif

	return port_send((void *) r.ebx);
}

STATUS
ScClosePort(regs r, uint32 eip, uint32 cs, uint32 eflags) {

#ifdef SysDebug
	kprintf("ScClosePort: Task #d called\n", CurrentTask->tid);
#endif

	return port_destroy(r.ecx);
}

STATUS
ScSetPortOptions(regs r, uint32 eip, uint32 cs, uint32 eflags) {

#ifdef SysDebug
	kprintf("ScSetPortOptions: Task #d called\n", CurrentTask->tid);
#endif

	return port_option(r.ecx, r.ebx, r.edx);
}

STATUS
ScTerminateProcess(regs r, uint32 eip, uint32 cs, uint32 eflags) {

#ifdef SysDebug
	kprintf("ScTerminateProcess: Task %d called\n", CurrentTask->tid);
#endif

	if(CurrentTask->tid == 0)
	{
		kprintf("Trying to sleep tid #0\n");
		return 0;
	}
	// NOTE: This should be set to tDEAD - Test later
	CurrentTask->state = tDEAD;
	PsSwitchTask();

	return 0;
}
