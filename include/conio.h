/* $Id: conio.h,v 1.2 1998/02/26 06:11:16 swetland Exp $ */

#ifndef _CONIO_H
#define _CONIO_H

#include <stdarg.h>

#ifdef MONO
#define CON_SCREEN	0x000B0000
#else
#define CON_SCREEN	0x000B8000
#endif

#define CON_BLACK       0
#define CON_BLUE        1
#define CON_GREEN       2
#define CON_CYAN        3
#define CON_RED         4
#define CON_MAGENTA     5
#define CON_YELLOW      6
#define CON_WHITE       7

void con_attr(int a);
void con_clear(void);
void con_putc(char ch);
void con_puts(char *s);
void con_putp(unsigned int p);
void con_putx(unsigned char x);

#define con_fgbg(fg,bg) con_attr((bg) << 4 | (fg));

int getch();
void printf(char *fmt, ...);
void oprintf(char *fmt, ...);
void snprintf(char *str, int len, char *fmt, ...);
void va_snprintf(char *b, int l, char *fmt, va_list pvar);

#define con_init() { con_attr(CON_WHITE); con_clear(); }

void con_start(int video);

void cprintf(char *fmt, ...);

#endif /* _CONIO_H */
