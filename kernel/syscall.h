#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"
#include "conio.h"

typedef STATUS (*SYSCALL)(regs r, uint32 eip, uint32 cs, uint32 eflags);

STATUS NullSysCall(regs r, uint32 eip, uint32 cs, uint32 eflags);
STATUS ScOsConsole(regs r, uint32 eip, uint32 cs, uint32 eflags);
STATUS ScCreateThread(regs r, uint32 eip, uint32 cs, uint32 eflags);
STATUS ScCreatePort(regs r, uint32 eip, uint32 cs, uint32 eflags);
STATUS ScReadPort(regs r, uint32 eip, uint32 cs, uint32 eflags);
STATUS ScSendPort(regs r, uint32 eip, uint32 cs, uint32 eflags);
STATUS ScClosePort(regs r, uint32 eip, uint32 cs, uint32 eflags);
STATUS ScSetPortOptions(regs r, uint32 eip, uint32 cs, uint32 eflags);
STATUS ScTerminateProcess(regs r, uint32 eip, uint32 cs, uint32 eflags);
STATUS ScOsDebug(regs r, uint32 eip, uint32 cs, uint32 eflags);
STATUS ScOsBrk(regs r, uint32 eip, uint32 cs, uint32 eflags);
STATUS ScSleepIRQ(regs r, uint32 eip, uint32 cs, uint32 eflags);
STATUS ScHandleIRQ(regs r, uint32 eip, uint32 cs, uint32 eflags);
STATUS ScWait(regs r, uint32 eip, uint32 cs, uint32 eflags);
STATUS ScCreateSegment(regs r, uint32 eip, uint32 cs, uint32 eflags);
STATUS ScAttachSegment(regs r, uint32 eip, uint32 cs, uint32 eflags);
STATUS ScCloseSegment(regs r, uint32 eip, uint32 cs, uint32 eflags);
STATUS ScCreateSemaphore(regs r, uint32 eip, uint32 cs, uint32 eflags);
STATUS ScAcquireSemaphore(regs r, uint32 eip, uint32 cs, uint32 eflags);
STATUS ScReleaseSemaphore(regs r, uint32 eip, uint32 cs, uint32 eflags);
STATUS ScDestroySemaphore(regs r, uint32 eip, uint32 cs, uint32 eflags);
STATUS ScSleep(regs r, uint32 eip, uint32 cs, uint32 eflags);

/*
 * System Call		Function
 *
 *			Debugging System Calls
 *
 * 0			Null System Call
 *
 * 1			OS console printing (debugging)
 * 2                    OS Beep (debugging) (NYI)
 * 3			Turn on the OS Debugger
 * 4 - 9		Reserved
 *
 *			Messaging System Calls
 *
 * 10			Create message port
 * 11			Read from message port
 * 12			Send to message port
 * 13			Close message port
 * 14			Set port options
 * 15 - 19		Reserved
 *
 *			Device System Calls
 *
 * 21	        Register IRQ handle
 * 22           Unregister IRQ handle
 * 23			Sleep on IRQ
 * 23 - 49		Reserved
 *
 *			Process System Calls
 *
 * 50			Create new thread
 * 51			Terminate thread
 * 52			Set process priority (NYI)
 * 53			Get process priority (NYI)
 * 54			thread
 * 55			Sleep
 * 56			alarm (NYI)
 * 57			kill alarm (NYI)
 * 58 			wait
 * 59 - 69		Reserved
 *
 *			Security related system calls
 *
 * 70 - 89		Reserved
 *
 *			Memory system calls
 *
 * 90			brk
 * 91			Create Shared Memory Segment
 * 92			Attach to Shared Memory Segment
 * 93			Close Shared Memory Segment
 * 94			Create Semaphore
 * 95			Acquire Semaphore 
 * 96			Release Semaphore
 * 97			Destroy Semaphore
 * 96 - 99		Reserved
 *
 * 109 - 199		Reserved
 * 200 - 255		Unused
 */

 /*
  * NYI = Not Yet Implemented
  */
#endif
