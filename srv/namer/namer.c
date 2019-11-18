#include <sys/syscall.h>
#include <namer.h>
#include <error.h>
#include <string.h>

static char names[32][32];
static int ports[32];
static int count = 0;

static int port_register(int port, char *name)
{
    int i;
    for(i=0;i<count;i++){
        if(ports[i] == port) {
		os_console("port_register: ERR_PERMISSION");
		return ERR_PERMISSION;        
	}
    }
    if(count == 32) return ERR_MEMORY;

    ports[count] = port;
    strcpy(names[count],name);
    count++;

	oprintf("namer: Registered port for: %s\n", name);

    return ERR_NONE;    
}

static int port_find(char *name)
{
    int i;
    
    for(i=0;i<count;i++){
        if(!strcmp(name,names[i])) return ports[i];
    }
    return 0;    
}

int main(void)
{
    msg_hdr_t mh;
    namer_message_t msg;    
    int l;

    port_register(1,"namer");
    
//	oprintf("namer: entering service loop\n");

    for(;;){        
        mh.dst = 1;
        mh.data = &msg;
        mh.size = sizeof(namer_message_t);        
        l = port_recv(&mh);
//oprintf("namer: received request from port: 0x%x to port: 0x%x\n", mh.src, mh.dst);
        
        if(l < 0) {
            os_console("namer: can't get port");
            terminate_process(-1);            
        }

        msg.text[NAMER_TEXT_MAX-1] = 0;        
        if(msg.number){
		//oprintf("namer: registering port: 0x%x\n", msg.number);
            msg.number = port_register(msg.number,msg.text);            
        } else {
		//oprintf("namer: finding port: %s\n", msg.text);
            msg.number = port_find(msg.text);            

		//oprintf("namer: port was found at: 0x%x\n", msg.number);
        }
        mh.data = &msg;
        mh.size = 4;
if(mh.src == 1) {
	oprintf("namer: error src is 1\n");
blah:
	goto blah;
}
        mh.dst = mh.src;
        mh.src = 1;
	//oprintf("sending reply to: 0x%x\n", mh.dst);
        l = port_send(&mh);

/*        if(l == ERR_RESOURCE) os_console("namer: ERR_RESOURCE");
        if(l == ERR_PERMISSION) os_console("namer: ERR_PERMISSION");
        if(l == ERR_MEMORY) os_console("namer: ERR_MEMORY");*/
    }
    return 0;
}
