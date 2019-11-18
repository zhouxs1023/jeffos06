//
// Portions of this code are from OpenBlt, by Brian Swetland
//

#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef struct _qnode_t 
{
    struct _qnode_t *next;
    struct _qnode_t *prev;
    void *data;
    int dsize;
} qnode_t;

typedef struct _queue_t 
{
    int count;
    int limit;    
    struct _qnode_t *head;
    struct _qnode_t *tail;
} queue_t;

queue_t *queue_new(int limit);
void queue_del(queue_t *q);

void queue_addHead(queue_t *q, void *data, int dsize);
void queue_addTail(queue_t *q, void *data, int dsize);

void *queue_removeHead(queue_t *q, int *dsize);
void *queue_removeTail(queue_t *q, int *dsize);

#define queue_removeHeadT(q,s,t)  ((t) queue_removeHead(q,s))
#define queue_removeTailT(q,s,t)  ((t) queue_removeTail(q,s))

#define queue_peekHead(q) ( q->head ? q->head->data : NULL )
#define queue_peekTail(q) ( q->head ? q->head->data : NULL )

#define queue_peekHeadT(q,t) ((t) ( q->head ? q->head->data : NULL ))
#define queue_peekTailT(q,t) ((t) ( q->head ? q->head->data : NULL ))

#define queue_count(q) ((q)->count)
#define queue_limit(q) ((q)->count)

#endif
