/* $Id: bootstub.c,v 1.3 1998/03/09 13:01:45 swetland Exp $ */

/* interface with paul's spiffy bootloader... */
#include "../include/types.h"
#include "boot.h"


void bootstrap(boot_dir *bd, uint32 mem, uint32 entry_ebp);

void _start(uint32 mem, char *params, boot_dir *bd)
{
    uint32 entry_ebp;
    asm ("mov %%ebp, %0":"=m" (entry_ebp));
    bootstrap(bd, mem, entry_ebp);
}
