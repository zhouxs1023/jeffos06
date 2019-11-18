/* $Id$ */

#ifndef _SEM_H
#define _SEM_H

#include "task.h"
#include "queue.h"

typedef struct _sem 
{
    task *owner;
    queue_t *queue;
    int count;
} sem;

int sem_create(int count);
int sem_destroy(int sem);
int sem_acquire(int id);
int sem_release(int id);

#endif
