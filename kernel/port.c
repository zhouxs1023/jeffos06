//
// Portions of this code are from OpenBlt, by Brian Swetland
//

#include <types.h>    
#include "memory.h"
#include "port.h"
#include "resource.h"
#include "task.h"
#include "mem.h"
#include "alloc.h"

//#include "assert.h"

#define Assert(a)

extern task *CurrentTask;
extern uint32 *KernelPageTable;

int port_create(int restrict)
{
    msg_port *p;

        /* create new port */
    p = kmalloc32();
    
    p->owner = CurrentTask;
    p->restrict = restrict;
    p->msgcount = 0;
    p->first = p->last = NULL;
    p->slaved = 0;
    p->refcount = 1;    
    return rsrc_new_port(p);
}

int port_destroy(int port)
{
    msg_port *p;
    if(!(p = rsrc_find_port(port))) return ERR_RESOURCE;
    if(p->owner != CurrentTask) return ERR_PERMISSION;


    if(p->refcount == 1) {                
            /* destroy port */
        kfree32(p);
        rsrc_del(port);
        return ERR_NONE;    
    }

        /* port is the master of one or more slaves */
    return ERR_RESOURCE;       
}

#define PORT_OPT_NOWAIT        1
#define PORT_OPT_SETRESTRICT   2
#define PORT_OPT_SETDEFAULT    3
#define PORT_OPT_SLAVE         4

uint32 port_option(uint32 port, uint32 opt, uint32 arg)
{
    msg_port *p;
    
    if(!(p = rsrc_find_port(port))) return ERR_RESOURCE;
    if(p->owner != CurrentTask) return ERR_PERMISSION;

    if(opt == PORT_OPT_SETRESTRICT){
        p->restrict = arg;        
        return ERR_NONE;        
    }

    if(opt == PORT_OPT_SLAVE){
        msg_port *master;

        if(arg){
                /* arg == 0 will simply release the old master */
            
            if(!(master = rsrc_find_port(arg))) return ERR_RESOURCE;
            if(master->owner != CurrentTask) return ERR_PERMISSION;
            
                /* indicate that our master has one more slave */
            master->refcount++;
        }
        
        if(p->slaved){
                /* change in slave status, deref our old master */
            if(!(master = rsrc_find_port(p->slaved)))
                KePanic("port_option(): master port not found?");
            
            master->refcount--;            
        }
        p->slaved = arg;
        return ERR_NONE;        
    }
    return ERR_PERMISSION;
}

static chain *msg_pool = NULL;


int port_send(msg_hdr_t *mh)
{
    int i,size;
    message *m;
    void *msg;
    
    msg_port *f,*p;

    if(((uint32) mh) > 0x400000) return ERR_MEMORY;
    size = mh->size;    
    msg = mh->data;
    

    if(!(f = rsrc_find_port(mh->src))) 
	{
		kprintf("port_send: could not find source port\n");
		return ERR_SENDPORT;
	}
    if( (f->owner != CurrentTask) && (f->owner->Parent != CurrentTask->Parent))
	{
			kprintf("port_send: source port permission error\n");
			return ERR_PERMISSION;
    }

        /* insure the port exists and we may send to it */
    if(!(p = rsrc_find_port(mh->dst))) 
	{
		kprintf("port_send: could not find dest port\n");
		return ERR_RECVPORT;
	}
    if((p->restrict) &&
       (p->restrict != mh->src)) 
	{
		kprintf("port_send: permission error with dest port restrict(%d) src(%d)\n", p->restrict, mh->src);
		return ERR_PERMISSION;
	}

        /* are we slaved to a different port? */
    if(p->slaved){
        if(!(p = rsrc_find_port(p->slaved))) return ERR_RESOURCE;
        if(p->slaved) return ERR_RESOURCE;
        if((p->restrict) &&
           (p->restrict != mh->src)) return ERR_PERMISSION;        
    }
        /* ignore invalid sizes/locations */
    if( (size < 1) ||
        (((uint32) msg) > 0x400000) ||
        (size > 4096)) return ERR_MEMORY;    

    m = kmalloc32();
        /* allocate a 4k page to carry the message. klunky... */
    if(size < 1025){
        m->data = kmallocB(size);        
    } else {
        if(msg_pool){
            m->data = (void *) msg_pool;
            msg_pool = (chain *) msg_pool->next;        
        } else {
            m->data = MmAllocPages((aspace *)KernelPageTable, 1, 3, NULL);
        }
    }

/*    kprintf("task %X: copyin %x -> %x (%d)\n",CurrentTask->tid,
            (int) msg, (int) m->data,  size);*/
    
    for(i=0;i<size;i++)
        ((unsigned char *) m->data)[i] = *((unsigned char *) msg++);

    m->from_port = mh->src;
    m->to_port = mh->dst;    
	m->pid = CurrentTask->tid;
    m->size = size;
    m->next = NULL;
    if(p->last){
        p->last->next = m;
    } else {
        p->first = m;
    }
    p->last = m;
    p->msgcount++;


        /* wake our owner if he's sleeping on us */
    if(p->owner->state == tSLEEP_PORT &&
       p->owner->sleeping_on == mh->dst) {
#ifdef SysDebug
        kprintf("task %X: waking task %X on port %d\n",CurrentTask->tid,
                p->owner->tid,mh->dst);
#endif
        p->owner->state = tREADY;
        //queue_addTail(run_queue, p->owner, 0);        
    }
    
    return size;
}

/*int port_recv(int port, void *msg, int size, int *from)*/
int port_recv(msg_hdr_t *mh)
{
    int i,size;
    message *m;
    void *msg;    
    msg_port *p;

    if(((uint32) mh) > 0x400000) return ERR_MEMORY;
    size = mh->size;    
    msg = mh->data;

        /* insure the port exists and we may receive on it */
    if(!(p = rsrc_find_port(mh->dst))) return ERR_RECVPORT;
    if(p->owner != CurrentTask) return ERR_PERMISSION;

        /* bounds check the message... should be more careful */
    if(((uint32) msg) > 0x400000) return ERR_MEMORY;

        /* no messages -- sleep */
    while(!p->msgcount) {
        CurrentTask->sleeping_on = mh->dst;
        CurrentTask->state = tSLEEP_PORT;
#ifdef SysDebug
        kprintf("task %X: sleeping on port %d\n",CurrentTask->tid,mh->dst);     
#endif
        PsSwitchTask();        
/*        if(i != CurrentTask->tid) KePanic("who am i?"); 
        kprintf("task %X: waking up\n",CurrentTask->tid);        */
    }

    
    m = p->first;
/*    kprintf("task %X: copyout %x -> %x (%d/%d)\n",CurrentTask->tid,
            (int) m->data, (int) msg, m->size, size);
            */  
    Assert(m->size < 4096);
    for(i=0;i<m->size && (i <size);i++){
        *((unsigned char *) msg++) = ((unsigned char *) m->data)[i];
    }
    mh->src = m->from_port;    
    mh->dst = m->to_port;
	mh->pid = m->pid;
	

        /* unchain from the head of the queue */
    if(!(p->first = p->first->next)) p->last = NULL;    
    
    p->msgcount--;

        /* add to the freepool */
    if(m->size < 1025){
        kfreeB(m->size,m->data);
//	MmFreePages((aspace *)KernelPageTable, m->data, 1);
//		free(m->data);
    } else {
        ((chain *) m->data)->next = msg_pool;
        msg_pool = ((chain *) m->data);
    }
    kfree32(m);
//	free(m);
/*  kprintf("       : DONE\n");
 */
    return size < m->size ? size : m->size;

}

