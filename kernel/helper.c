#include <types.h>
#include "status.h"
#include "ktrace.h"

#include "helper.h"

extern uint32 nextentry;

void
KeHalt( void ) {
	kprintf("Halting...\n");
loop:
	goto loop;
}

STATUS
KeDumpTask( IN task *t) {
	kprintf("task: 0x%x\n", t);
	kprintf("esp0: 0x%x    ss0: 0x%x\n",
		t->tss.esp0, t->tss.ss0);
    kprintf("eax: 0x%x    ebx: 0x%x    ecx: 0x%x    ed: 0x%x\n",
        t->tss.eax, t->tss.ebx, t->tss.ecx, t->tss.edx);
    kprintf("esp1: 0x%x    esp2: 0x%x    ss1: 0x%x    ss2: 0x%x\n",
        t->tss.esp1, t->tss.esp2, t->tss.ss1, t->tss.ss2);
    kprintf("cr3: 0x%x    eip: 0x%x    eflags: 0x%x    esp: 0x%x\n",
        t->tss.cr3, t->tss.eip, t->tss.eflags, t->tss.esp);
    kprintf("cs: 0x%x    ds: 0x%x    es: 0x%x    fs: 0x%x    gs: 0x%x\n",
        t->tss.cs, t->tss.ds, t->tss.es, t->tss.fs, t->tss.gs);
    kprintf("ldt: 0x%x    debugtrap: 0x%x    iomap: 0x%x    pdir: 0x%x\n",
        t->tss.ldt, t->tss.debugtrap, t->tss.iomapbase, t->Parent->addr); 

	return STATUS_SUCCESS;
}

void
KePanic(char *reason)
{
	kprintf("\nPANIC: %s\n", reason);
	KeHalt();
}
