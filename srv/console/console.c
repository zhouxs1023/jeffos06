/* $Id: console.c,v 1.7 1998/03/10 06:27:54 swetland Exp $ */

#include <sys/syscall.h>
#include <error.h>
#include <namer.h>
#include <conio.h>
//#include <io.h>
#include <string.h>

int console_port;
int send_port;

//#define MONO

#define BS 8

#ifdef MONO
#define SCREEN 0xB0000
#else
#define SCREEN 0xB8000
#endif


#define NULL ((void *) 0)

typedef struct _tline 
{
    char raw[160];    
    struct _tline *prev, *next;    
} tline;

typedef struct _tpane
{    
    tline *top, *bottom;
    int visible;
    int height;
    int y0,y1;    
} tpane;


tpane commandline;
tpane console;
tline lines[128];
tline *nextline = NULL;

tline *newline(tline *next, tline *prev)
{
    tline *l = nextline;
    if(!l) terminate_process(0);    
    nextline = nextline->next;
    l->next = next;
    l->prev = prev;    
    return l;    
}

void delline(tline *line)
{
    line->next = nextline;
    nextline = line;    
}

void wipeline(tline *line, int attr)
{
    int i = 0;    
    unsigned char *x = line->raw;
    for(i=0;i<160;i++){
        *x++ = ' ';
        *x++ = attr;
    }
}


void init(tpane *pane, int attr)
{
    int i;
    pane->bottom = pane->top = newline(NULL,NULL);
    wipeline(pane->bottom, attr);
    
    for(i=1;i<pane->height;i++){
        pane->bottom->next = newline(NULL,pane->bottom);
        wipeline(pane->bottom->next, attr);
        pane->bottom = pane->bottom->next;
    }
/*    wipeline(pane->bottom, attr);
    wipeline(pane->bottom->prev, attr);*/
    
}

void clr(tpane *pane, int attr)
{
    int i;
    
    tline *l = pane->top;

    for(i=0;i<pane->height;i++){
        wipeline(l,attr);
        l = l->next;
    }
}

void addline(tpane *pane, char *text)
{
    int i;    

    tline *l = pane->top;
    pane->top = pane->top->next;
    pane->top->prev = NULL;
    pane->bottom->next = l;
    l->prev = pane->bottom;    
    l->next = NULL;
    pane->bottom = l;
    for(i=0;i<80;i++){
        if(*text >= ' ') l->raw[i*2] = *text++;
        else l->raw[i*2] = ' ';
    }
}

void paint(tpane *pane)
{
    int y;
    tline *l;    
    int *a,*b;
    int i;

    for(l=pane->top,y=pane->y0;y<=pane->y1;y++){
        a = (int *) (l->raw);
        b = (int *) (SCREEN + 160*y);
        for(i=0;i<40;i++) *b++ = *a++;
        l = l->next;        
    }
}

int y = 2;

char *divider =
"### JeffOS System Console #######################################################";


void console_thread(void)
{
    int i, j, k;
    int l;    
    char data[81];
    char buf[81];
    msg_hdr_t msg;    
    msg.data = data;
    msg.dst = console_port;
    msg.size = 80;
    
    for(i=0;i<128;i++) delline(&lines[i]);
    
    console.y0 = 3;
    console.y1 = 22;
    console.height = 20;
    
    commandline.y0 = 23;
    commandline.y1 = 24;
    commandline.height = 2;    

    init(&console, CON_WHITE);
    init(&commandline, CON_BLACK << 4 | CON_WHITE);

    for(i=0;i<80;i++) commandline.top->raw[i*2] = divider[i];
    commandline.bottom->raw[0] = 0;    
        

/*    clr(&console, CON_WHITE);*/
    
    paint(&console);
    paint(&commandline);

    i = 0;
    while((l = port_recv(&msg)) > 0){
        data[l]=0;        
	for(j = 0; j < strlen(data); j++)
	{
		if(data[j] == '\r')
		{
			buf[i] = 0;
			addline(&console, buf);
			i = 0;
			for(k = 0; k < 81; k++)
			{
				buf[k] = 0;
				commandline.bottom->raw[k*2] = 0;
			}
			
		}
		else if(data[j] == BS)
		{
			i--;
			buf[i] = 0;
			commandline.bottom->raw[i*2] = 0;
		}
		else
		{
			commandline.bottom->raw[i*2] = data[j];
			buf[i] = data[j];
			i++;
		}

	}
        paint(&console);        
	paint(&commandline);
    }
    addline(&console,"console: ready.");
    terminate_process(0);
    
}


int main(void)
{
    int nh;
    
    os_brk(2*32768);

    
    console_port = port_create(0);
    nh = namer_newhandle();    
    namer_register(nh,console_port,"console");
    namer_delhandle(nh);    

    console_thread();

    return 0;
}


