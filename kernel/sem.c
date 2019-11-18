#include <types.h>

#include "resource.h"
#include "queue.h"
#include "memory.h"
#include "sem.h"

extern task *CurrentTask;

int sem_create(int count) 
{
    sem *s = (sem *) kmalloc16();
    s->queue = queue_new(0);
    s->count = count;
    return rsrc_new_sem(s);
}

int sem_destroy(int sem)
{

}

int sem_acquire(int id) 
{
    sem *s;
    
    if(!(s = rsrc_find_sem(id))) {
        return ERR_RESOURCE;
    }
    
    if(s->count > 0 ){
        s->count--;
    } else {
        s->count--;
        queue_addTail(s->queue,CurrentTask,0);
        CurrentTask->sleeping_on = id;
        CurrentTask->state = tSLEEP_SEM;
        PsSwitchTask();
    }
    return ERR_NONE;
}

int sem_release(int id) 
{
    int x;
    sem *s;
    task *t;
    
    if(!(s = rsrc_find_sem(id))) {
        return ERR_RESOURCE;
    }

    s->count++;

    if(t = queue_removeHeadT(s->queue,&x,task*)){
        t->state = tREADY;
		PsPreemptTask(t);
    }

    return ERR_NONE;
}
