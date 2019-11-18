/* $Id: boot.c,v 1.3 1998/03/09 13:01:45 swetland Exp $ */

/* Stage Two Bootloader -- init paged memory, map the kernel, and
 * relocate to k_start()
 */
#define nDIAG


#include "types.h"
#include "boot.h"

#ifdef DIAG
#include "../include/conio.h"
#endif

void (*k_start)(void) = (void (*)(void)) 0x80000080;

struct _kinfo
{
    uint32 memsize;
    uint32 entry_ebp;
    boot_dir *bd;
} *kinfo = (struct _kinfo *) 0x00102000;


void bootstrap(boot_dir *bd, uint32 memsize, uint32 entry_ebp)
{
    uint32 *flat;
    uint32 _cr3;
    int i;
    uint32 *a;


    kinfo->memsize = memsize;
    kinfo->entry_ebp = entry_ebp;
    kinfo->bd = bd;

#ifdef DIAG    
    con_init();
    cprintf("memsize = %x, bdir @ %x, ebp = %x\n",memsize,(int)bd,entry_ebp);
    cprintf("name[0] = %s\n",bd->bd_entry[0].be_name);
    cprintf("name[1] = %s\n",bd->bd_entry[1].be_name);
    cprintf("name[2] = %s\n",bd->bd_entry[2].be_name);
    cprintf("name[3] = %s\n",bd->bd_entry[3].be_name);
#endif    

    flat = (void*) ((4096*(memsize/4096)) - 4096*3);

    
    for(i=0;i<1024;i++){
        flat[i] = 0;
        flat[1024+i] = 4096*i | 3;
        flat[2048+i] = i > (bd->bd_entry[2].be_size) ? 0 : /* XXX! EEK! */
            ( (bd->bd_entry[2].be_offset*4096+0x100000) + 4096*i) | 3;

#ifdef DIAG
        if(flat[2048+i]){
            cprintf("%x -> %x\n",
                    (bd->bd_entry[2].be_offset*4096+0x100000) + 4096*i,
                    0x80000000+4096*i);
		a = (bd->bd_entry[2].be_offset*4096+0x100000) + 4096*i + 0x80;
		cprintf("phys: 0x%x\n", a[0]);
        }
#endif                    
    }

        /* point the pdir at ptab1 and ptab2 */
    flat[0] = (uint32) &flat[1024] | 3;
    flat[512] = (uint32) &flat[2048] | 3;

        /* map the pdir, ptab1, ptab2 starting at 0x803FD000 */
    flat[2048+1023] = ((uint32) flat + 2*4096) | 3;
    flat[2048+1022] = ((uint32) flat + 1*4096) | 3;
    flat[2048+1021] = ((uint32) flat) | 3;

    _cr3 = (uint32) flat;
    __asm__ ("mov %0, %%cr3"::"r" (_cr3));

    flat = 0x803fd000;

#ifdef DIAG
    for(i=0;i<3000000;i++);
    for(i=0;i<3000000;i++);
    if(flat[0] & 0xffffff00 != 0x007fe000) cprintf("danger will robinson! (a)\n");
    if(flat[512] & 0xffffff00 != 0x007ff000) cprintf("danger will robinson! (b)\n");
   	cprintf("a = %x b = %x\n",flat[0],flat[512]); 
    for(i=0;i<3000000;i++);
    cprintf("go! go! go!\n\n");
    for(i=0;i<3000000;i++);

    
/*    for(i=0;i<80*4;i++){
        cprintf(".");
        *((char *) 0x100000+i) = 'X';
    }
    asm("hlt"); */
#endif

    k_start();
}

