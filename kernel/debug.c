//
// Portions of this code are from OpenBlt, by Brian Swetland
//

#ifndef KDEBUG

#include <io.h>
#include <conio.h>

#include <string.h>
#include <types.h>
#include "resource.h"
#include "task.h"
#include "port.h"
#include "i386.h"

uint32 EIP;
uint32 EBP;

#define ESC 27
#define BS 8
#define TAB 9
#define CR 13
char ScanTable [] =  {' ', ESC, '1', '2', '3', '4', '5', '6', '7', '8',
                      '9', '0', '-', '=', BS,  TAB, 'q', 'w', 'e', 'r',
                      't', 'y', 'u', 'i', 'o', 'p', '[', ']', CR,  ' ',
                      'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
                      '\'', '~', ' ', '\\', 'z', 'x', 'c', 'v', 'b', 'n',
                      'm', ',', '.', '/', ' ', ' ', ' ', ' ', ' '};
char ShiftTable [] = {' ', ESC, '!', '@', '#', '$', '%', '^', '&', '*',
                      '(', ')', '_', '+', ' ', ' ', 'Q', 'W', 'E', 'R',
                      'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', CR,  ' ',
                      'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
                      '\"', '~', ' ', '|', 'Z', 'X', 'C', 'V', 'B', 'N',
                      'M', '<', '>', '?', ' ', ' ', ' ', ' ', ' '};
#define LSHIFT 42
#define RSHIFT 54

extern uint32 *IDT;

void structs()
{
	uint32 mycr3;

	asm("mov %%cr3, %0":"=r" (mycr3));

	kprintf("IDT: 0x%x\n", IDT);
	kprintf("cr3: 0x%x\n", mycr3);

}

char *getline(void)
{
    static char line[80];
    int i,lp,key;
    int shift = 0;

  restart:
    for(i=1;i<79;i++) line[i] = ' ';
    line[0] = ':';
    line[79] = '\0';
    lp = 2;
    
    for(;;){
        con_goto(0,24);
        con_puts(line);
        while(!(inb(0x64) & 0x01));
        key = inb(0x60);
        switch(key){
        case LSHIFT:
        case RSHIFT:
            shift = 1;
            break;
        case LSHIFT | 0x80:
        case RSHIFT | 0x80:
            shift = 0;
            break;
        default:
            if(key & 0x80){
                    /* break */
            } else {
                if(key < 59){
                    key = shift ? ShiftTable[key] : ScanTable[key];

                    switch(key){
                    case CR:
                        line[lp] = 0;
                        con_puts("\n");
                        return line + 2;
                    case ESC:                        
                        goto restart;
                    case BS:
                        if(lp > 2){
                            lp--;
                            line[lp]=' ';
                        }
                        break;
                    default:
                        if(lp < 78);
                        line[lp] = key;
                        lp++;
                    }
                    
                }
            }
        }
    }
}

#define RMAX 1024

static char *tstate[] =
{ "KERNL", "RUNNG", "READY", "S/PRT", "S/IRQ", "DEAD ", "S/TMR"};

void dumptasks()
{
    int i,j,n;
    task *t;
    aspace *a;
    
    kprintf("Task Aspace   State Wait irq  brk      Name\n");
    kprintf("---- -------- ----- ---- ---- -------- --------------------------------\n");

    for(i=1;i<RMAX;i++){
        if((t = rsrc_find_task(i))){
            a = t->Parent->addr;
            for(j=0;a->ptab[j] && j < 1024;j++);
            
            kprintf("%U %x %s %U %U %x %s\n",
                    t->tid,t->Parent->addr,tstate[t->state],t->sleeping_on, t->irq, j*4096,t->Parent->name);
            
        }
    }
}

void dumpports()
{
    int i;
    msg_port *p;

    kprintf("Port Task Rstr Slav Size\n");
    kprintf("---- ---- ---- ---- ----\n");
    
    for(i=1;i<RMAX;i++){
        if((p = rsrc_find_port(i))){
            kprintf("%U %U %U %U %U\n",
                    i, 
		p->owner->tid, 
		p->restrict, 
		p->slaved, 
		p->msgcount);
        }
    }

}

static void trace(uint32 ebp, uint32 eip)
{
    int f = 1;

    kprintf("f# EBP      EIP\n");    
    kprintf("00 xxxxxxxx %x\n",eip);    
    do {        
/*        kprintf("%X %x %x\n",f,ebp, *((uint32*)ebp));*/
        kprintf("%X %x %x\n",f,ebp+4, *((uint32*)(ebp+4)));
        ebp = *((uint32*)ebp);
        f++;        
    } while(ebp < 0x00400000 && ebp > 0x00200000);    

}


static void dump(uint32 addr, int sections)
{
    int i;
    unsigned char *x;

kprintf("dump addr: 0x%x\n", addr);

    if(addr < 0) return;
    
    for(i=0;i<sections;i++){
        x = (unsigned char *) (addr + 16*i);
        kprintf("%x: %X %X %X %X  %X %X %X %X  %X %X %X %X  %X %X %X %X\n",
                addr+16*i,x[0],x[1],x[2],x[3],x[4],x[5],x[6],x[7],
                x[8],x[9],x[10],x[11],x[12],x[13],x[14],x[15]);
    }
}

static char *hex = "0123456789abcdef";

int readhex(char *s)
{
    int n=0;
    char *x;
    while(*s) {
        x = hex;
        while(*x) {
            if(*x == *s){
                n = n*16 + (x - hex);
                break;
            }
            x++;
        }
        s++;
    }
    return n;
    
}


void memory_status(void);

void 
reset( void )
{
	_start();
}

void 
reboot( void )
{
	kprintf("Rebooting...\n");
	i386lidt((void *)0xffffffff, 0);
	asm("int $0x00");
}

void
debughelp( void )
{
	kprintf("----  JeffOS Debugger Help  ----\n");
	kprintf("trace - dumps stack trace (sorta)\n");
	kprintf("dump (mem) - display a hex dump starting at memory location (mem)\n");
	kprintf("structs - prints out some kernel structure values\n");
	kprintf("reboot - reboots the computer\n");
	kprintf("tasks - display task list\n");
	kprintf("ports - display ports in use on the system\n");
	kprintf("help - help information\n");
	kprintf("exit - exit the debugger\n");
	kprintf("--------------------------------\n\n");
}

extern void dump_task();

void k_debugger(void)
{
    char *line;
    
    kprintf("\n\n");
    kprintf("JeffOS Kernel Debugger\n");

    for(;;){
        line = getline();

#ifdef ndef
        if(!strcmp(line,"memory")) { memory_status(); continue; };
#endif
        if(!strcmp(line,"tasks")) { dumptasks(); continue; };
        if(!strcmp(line,"ports")) { dumpports(); continue; };
        if(!strcmp(line,"trace")) { trace(EBP,EIP); continue; }
        if(!strncmp(line,"dump ", 5)) { dump(readhex(line+5),16); continue; }
	if(!strcmp(line, "structs")) { structs(); continue; }
	if(!strcmp(line, "reset")) { reset(); continue; }
	if(!strcmp(line, "reboot")) { reboot(); continue; }
	if(!strcmp(line, "dumptask")) { dump_task(); continue; }
	if(!strcmp(line, "help")) { debughelp() ; continue; }
            
        if(!strcmp(line,"exit")) break;
    }
}




#endif   
