//
// Portions of this code are from OpenBlt, by Brian Swetland
//

/* serial tracing */

#include <stdarg.h>

#include <io.h>
#include <conio.h>

//#define DONT_USE_SERIAL
#define USE_SERIAL

#define com1 0x3f8 
#define com2 0x2f8

#define combase com1

void va_snprintf(char *b, int l, char *fmt, va_list pvar);
static char kpbuf[256];

#ifdef USE_SERIAL
void init_serial(void)
{
    outb(inb(combase + 3) | 0x80, combase + 3);
    outb(12, combase);                           /* 9600 bps, 8-N-1 */
    outb(0, combase+1);
    outb(inb(combase + 3) & 0x7f, combase + 3);
}

int getDebugChar(void)
{
    while (!(inb(combase + 5) & 0x01));
    return inb(combase);
}

void putDebugChar(int ch)
{
    while (!(inb(combase + 5) & 0x20));
    outb((char) ch, combase);
}

void ser_puts(char *s)
{
    int t;
    while(*s){
        t=100000;        
        while (!(inb(combase + 5) & 0x20) && t--);
        outb((char) *s, combase);        
        s++;        
    }
}

#endif

void kprintf_init(void)
{
/*
#ifdef USE_SERIAL
        init_serial();
#else
        con_attr(CON_WHITE);
        con_clear();
#endif        
*/
        init_serial();
        con_attr(CON_WHITE);
        con_clear();
}

void kprintf(char *fmt, ...)
{
    va_list pvar;    
    va_start(pvar,fmt);
    va_snprintf(kpbuf,256,fmt,pvar);
    va_end(pvar);
/*
#ifdef USE_SERIAL
    ser_puts(kpbuf);    
#else    
    con_puts(kpbuf);
#endif    
*/
    ser_puts(kpbuf);    
    con_puts(kpbuf);
}

