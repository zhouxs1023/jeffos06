//
// Portions of this code are from OpenBlt, by Brian Swetland
//

#include <types.h>

#include "resource.h"

typedef struct _resource {
    int type;
    union {
        void *data;
        int next;
    } r;
} resource;

static resource *rmap;
static uint32 rmax = 0;
static uint32 rfree = 0;

void rsrc_init(void *map, int size)
{
    int i;
    
    rfree = 1;    
    rmax = size / sizeof(resource);
    rmap = (resource *) map;
    for(i = 0; i < rmax; i++) {
        rmap[i].type = RSRC_NONE;
        rmap[i].r.next = i+1;        
    }
    rmap[rmax-1].r.next = 0;            

kprintf("rsrc_init: completed\n");
}

void *rsrc_find(int type, int id)
{
    
    if((id < rmax) && (rmap[id].type == type)) return rmap[id].r.data;
    return NULL;    
}

uint32 rsrc_new(int type, void *data)
{
    uint32 id;
    
    if(rfree){
        id = rfree;
        rfree = rmap[rfree].r.next;
    } else {
        return 0;
    }

    rmap[id].type = type;
    rmap[id].r.data = data;

    return id;    
}

void rsrc_del(uint32 id)
{
    if((id < rmax) && (rmap[id].type != RSRC_NONE)){        
        rmap[id].type = RSRC_NONE;
        rmap[id].r.next = rfree;
        rfree = id;
    }
}


