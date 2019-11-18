#include <types.h>
#include "i386.h"
#include "int.h"
#include <conio.h>
#include "task.h"
#include "syscall.h"
#include "ktrace.h"

uint32 kernel_timer = 0;
unsigned char x[] = { '-', '-', '\\', '\\', '|', '|', '/', '/' };
unsigned char *screen = (unsigned char *)0xB8000;

SYSCALL SysCallTable[256];

extern task* CurrentTask;
extern task *irq_task_map[16];

extern uint32 EIP;
extern uint32 EBP;

uint32 *IDT;

#define EX(n) extern void __ex##n(void);
EX(0); EX(1); EX(2); EX(3); EX(4); EX(5); EX(6); EX(7); EX(8); EX(9);
EX(10); EX(11); EX(12); EX(13); EX(14); EX(15); EX(16); EX(17); EX(18);

extern void k_debugger();

void
irq_dispatch(regs r, uint32 number)
{
	mask_irq(number);

#ifdef SysDebug
	kprintf("irq_dispatch: Called for irq #%d\n", number);
#endif

	if(irq_task_map[number])
	{
		if(irq_task_map[number]->state == tSLEEP_IRQ)
		{
#ifdef SysDebug
			kprintf("irq_dispatch: found a task handler\n");
#endif
			irq_task_map[number]->state = tREADY;
			PsPreemptTask(irq_task_map[number]);
		}
	}
}
			

void
kadd_int(uint32 *IDT, unsigned long num, void *func) {
	IDT[2*num+1] = (((uint32) func) & 0xFFFF0000) | 0x00008E00;
	IDT[2*num] = (((uint32) func) & 0x0000FFFF) | (SEL_KCODE << 16);
}

void
kadd_intU(uint32 *IDT, unsigned long num, void *func) {
	IDT[2*num+1] = (((uint32) func) & 0xFFFF0000) | 0x00008E00 | 0x00006000;
	IDT[2*num] = (((uint32) func) & 0x0000FFFF) | (SEL_KCODE << 16);
}

static void
dump_mem(uint32 cs, uint32 eip, uint32 len) {
	char *buf = 0;
	uint32 i;

	for(i = eip; i < (eip + len); i += 16) {
		kprintf("%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n",
			buf[i] & 0xff, buf[i + 1] & 0xff, buf[i + 2] & 0xff, buf[i + 3] & 0xff,
			buf[i + 4] & 0xff, buf[i + 5] & 0xff, buf[i + 6] & 0xff, buf[i + 7] & 0xff,
			buf[i + 8] & 0xff, buf[i + 9] & 0xff, buf[i + 10] & 0xff, buf[i + 11] & 0xff,
			buf[i + 12] & 0xff, buf[i + 13] & 0xff, buf[i + 14] & 0xff, buf[i + 15] & 0xff);
	}

}

static void
print_regs(regs *r, uint32 eip, uint32 cs, uint32 eflags) {
	uint16 ds, es, gs, fs;
	uint32 *a = (uint32 *) r->esp;

    kprintf("   EAX = %x   EBX = %x   ECX = %x   EDX = %x\n",
            r->eax, r->ebx, r->ecx, r->edx);
    kprintf("   EBP = %x   ESP = %x   ESI = %x   EDI = %x\n",
            r->ebp, r->esp, r->esi, r->edi);
    kprintf("EFLAGS = %x    CS = %x   EIP = %x\n",
            eflags, cs, eip);

	ds = es = fs = gs = 0;
	asm("mov %%ds, %%ax" : "=a" (ds));
	asm("mov %%es, %%ax" : "=a" (es));
	asm("mov %%fs, %%ax" : "=a" (fs));
	asm("mov %%gs, %%ax" : "=a" (gs));
	kprintf("DS = %x    ES = %x    GS = %x    FS = %x\n",
		ds, es, gs, fs);
	kprintf("KTSS DS = %x    ES = %x    GS = %x    FS = %x\n",
		CurrentTask->tss.ds, CurrentTask->tss.es,
		CurrentTask->tss.gs, CurrentTask->tss.fs);
	kprintf("Stack value: 0x%x\n", a[0]);
	a = (uint32 *) CurrentTask->tss.esp0;
	kprintf("Stack2 value: 0x%x\n", a[0]);

	asm("mov %%sp, %%ax" : "=a" (es));
	kprintf("Kernel Stack: 0x%x\n", es);
}         

void 
dummy(regs r, uint32 eip, uint32 cs, uint32 eflags)
{
	kprintf("got an interrupt");
}


void 
timer_irq(regs r, uint32 eip, uint32 cs, uint32 flags)
{
    kernel_timer++;
    screen[0] = x[kernel_timer%8];
    screen[1] = 7;
//kprintf("Timer IRQ called\n");
    PsSwitchTask();

//if(kernel_timer == 2)
//	haltit();
}                     

void
JeDebugPrint(char *text) {
	//con_attr(CON_WHITE);
	con_fgbg(CON_WHITE, CON_BLUE);
	con_puts(text);
	con_fgbg(CON_WHITE, CON_BLUE);
	con_puts("\n");
}


void
syscall22(regs r, uint32 eip, uint32 cs, uint32 eflags) {

//	kprintf("Function 0x%x for CurrentTask(%d): 0x%x\n", 
//		r.eax & 0xff,
//		CurrentTask->tid, 
//		CurrentTask
//		);
	r.eax = SysCallTable[r.eax & 0xff](r, eip, cs, eflags);

}

void
dump_task() {
	
	kprintf("Task(%d): ip: 0x%x ds: 0x%x ss: 0x%x sp: 0x%x\n",
		CurrentTask->tid, CurrentTask->tss.eip,
		CurrentTask->tss.ds, CurrentTask->tss.ss,
		CurrentTask->tss.esp);
	kprintf("ax: 0x%x bx: 0x%x cx: 0x%x dx: 0x%x\n",
		CurrentTask->tss.eax, CurrentTask->tss.ebx,
		CurrentTask->tss.ecx, CurrentTask->tss.edx);

	kprintf("pdir->pdir[0] = 0x%x  pdir[512] = 0x%x\n",
			CurrentTask->Parent->addr->pdir[0], CurrentTask->Parent->addr->pdir[512]);

#ifdef ndef
	kprintf("IP: pdir->ptab[%x] = 0x%x\n",
		CurrentTask->tss.eip / 4096, CurrentTask->Parent->addr->ptab[CurrentTask->tss.eip / 4096]);

	kprintf("SP: pdir->ptab[%x] = 0x%x\n",
		CurrentTask->tss.esp / 4096, CurrentTask->Parent->addr->ptab[CurrentTask->tss.esp / 4096]);
#endif

}

void
faultE(uint32 number,
	regs r, uint32 error,
	uint32 eip, uint32 cs, uint32 eflags) {
	uint32 _cr2;

	kprintf("Task #%d <%s> terminated with exception 0x%X* (%s)\n",
		CurrentTask->tid, CurrentTask->Parent->name, number, etable[number]);
	kprintf("\n*** Exception 0x%X* (%s)\n", number, etable[number]);
	print_regs(&r, eip, cs, eflags);
//	dump_mem(cs, eip, 64);
//	dump_task();

	asm("mov %%cr2, %0":"=r" (_cr2));
	kprintf("   cr2 = 0x%x", _cr2);
	asm("mov %%cr3, %0":"=r" (_cr2));
	kprintf("   cr3 = 0x%x error = %x", _cr2,error);

	EIP = eip;
	EBP = r.ebp;
	k_debugger();
done:


	PsTerminateTask(CurrentTask);
}

void fault(uint32 number,
           regs r,
           uint32 eip, uint32 cs, uint32 eflags)
{
kprintf("\nREALLY BAD\n");
    kprintf("\n*** Exception 0x%X (%s)\n",number,etable[number]);
    print_regs(&r, eip, cs, eflags);
    kprintf("Task %X has crashed.\n", CurrentTask->tid);

    PsTerminateTask(CurrentTask);

} 
