//
// Portions of this code are from OpenBlt, by Brian Swetland
//

#include <types.h>

#include "queue.h"
#include "memory.h"

queue_t *queue_new(int limit)
{
    queue_t *q = kmalloc(KM16);
    q->limit = limit;
    q->count = 0;
    q->head = q->tail = NULL;
    return q;
}

void queue_del(queue_t *q)
{
    kfree(KM16,q);    
}

void queue_addHead(queue_t *q, void *data, int dsize)
{
    qnode_t *qn = kmalloc(KM16);
    qn->data = data;
    qn->dsize = dsize;

    if(q->head) {
        qn->next = q->head;
        qn->prev = NULL;
        q->head = q->head->prev = qn;        
    } else {
        q->head = q->tail = qn;
        qn->next = qn->prev = NULL;        
    }

    q->count++;    
}

void queue_addTail(queue_t *q, void *data, int dsize)
{
    qnode_t *qn = kmalloc(KM16);
    qn->data = data;
    qn->dsize = dsize;

    if(q->head) {
        qn->prev = q->head;
        qn->next = NULL;
        q->tail = q->tail->next = qn;        
    } else {
        q->head = q->tail = qn;
        qn->next = qn->prev = NULL;        
    }

    q->count++;    
}

void *queue_removeHead(queue_t *q, int *dsize)
{
    qnode_t *qn;    
    void *block;
    
    if(q->head){
        qn = q->head;
        q->count--;
        
        if(q->tail == q->head){
            q->head = q->tail = NULL;            
        } else {
            qn->next->prev = NULL;
            q->head = qn->next;            
        }
        
        if(dsize) *dsize = qn->dsize;
        block = qn->data;
        kfree(KM16,qn);
        return block;        
    } else {
        return NULL;        
    }
}

void *queue_removeTail(queue_t *q, int *dsize)
{
    qnode_t *qn;    
    void *block;
    
    if(q->head){
        qn = q->head;
        q->count--;
        
        if(q->tail == q->head){
            q->head = q->tail = NULL;            
        } else {
            qn->prev->next = NULL;
            q->tail = qn->prev;            
        }
        
        if(dsize) *dsize = qn->dsize;
        block = qn->data;
        kfree(KM16,qn);
        return block;        
    } else {
        return NULL;        
    }
}


