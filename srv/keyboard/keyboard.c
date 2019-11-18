/* $Id: console.c,v 1.7 1998/03/10 06:27:54 swetland Exp $ */

#include <sys/syscall.h>
#include <error.h>
#include <namer.h>
#include <conio.h>
#include <io.h>
#include <string.h>

#define ESC 27
#define BS 8
#define TAB 9
#define CR 13

static int listenPort;

static char ScanTable [] =  {' ', ESC, '1', '2', '3', '4', '5', '6', '7', '8',
                      '9', '0', '-', '=', BS,  TAB, 'q', 'w', 'e', 'r',
                      't', 'y', 'u', 'i', 'o', 'p', '[', ']', CR,  ' ',
                      'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
                      '\'', '~', ' ', '\\', 'z', 'x', 'c', 'v', 'b', 'n',
                      'm', ',', '.', '/', ' ', ' ', ' ', ' ', ' '};
static char ShiftTable [] = {' ', ESC, '!', '@', '#', '$', '%', '^', '&', '*',
                      '(', ')', '_', '+', ' ', ' ', 'Q', 'W', 'E', 'R',
                      'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', CR,  ' ',
                      'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
                      '\"', '~', ' ', '|', 'Z', 'X', 'C', 'V', 'B', 'N',
                      'M', '<', '>', '?', ' ', ' ', ' ', ' ', ' '};
#define LSHIFT 42
#define RSHIFT 54

void keypress(int key);

int keyboard_nh;

void keyboard_thread(void)
{
	char x[128];
	int i;
	int send_port;
	msg_hdr_t msg;
	int data[8];
    
	int shift = 0;    
	int key;

	os_handle_irq(1);

	x[2] = 0;
	i = 0;

	msg.dst = listenPort;
	msg.src = 0;
	data[0] = 0;
	msg.data = data;
	msg.size = 4;
	port_recv(&msg);
	send_port = msg.src;

	for(;;) {

		os_sleep_irq();
#ifdef MULTI
		while(inb(0x64) & 0x01) {
#endif
			key = inb(0x60);
			if(key == 1) {
				os_debug();
				continue;
			}


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
					msg.dst = send_port;
					msg.src = listenPort;
					data[0] = key;
					msg.data = (char *)data;
					port_send(&msg);
				}
			}
		}
#ifdef MULTI
	}
#endif
}

}

#define NULL ((void *) 0)




char *divider =
"### JeffOS System Console #######################################################";



int main(void)
{
	int nh;
	msg_hdr_t msg;
	char data[8];
    
    
	// Create a port to listen on
	listenPort = port_create(0);
	
	// Get handle to the namer
	nh = namer_newhandle();    

	// Register the listener port
	namer_register(nh, listenPort,"keyboard");

	// Delete the namer port handle
	namer_delhandle(nh);    

	keyboard_thread();

	return 0;
}


