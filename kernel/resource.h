//
// Portions of this code are from OpenBlt, by Brian Swetland
//

#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#define RSRC_NONE   0
#define RSRC_TASK   1
#define RSRC_ASPACE 2
#define RSRC_PORT   3
#define RSRC_SEM	4

void   rsrc_init(void *map, int size);
void  *rsrc_find(int type, int id);
uint32 rsrc_new(int type, void *data);
void   rsrc_del(uint32 id);

#define rsrc_find_task(id)   ((task *) rsrc_find(RSRC_TASK,   id))
#define rsrc_find_port(id)   ((msg_port *) rsrc_find(RSRC_PORT,   id))
#define rsrc_find_aspace(id) ((aspace *) rsrc_find(RSRC_ASPACE, id))
#define rsrc_find_sem(id)    ((sem *) rsrc_find(RSRC_SEM, id))

#define rsrc_new_task(x)   rsrc_new(RSRC_TASK,   x)
#define rsrc_new_port(x)   rsrc_new(RSRC_PORT,   x)
#define rsrc_new_aspace(x) rsrc_new(RSRC_ASPACE, x)
#define rsrc_new_sem(x)    rsrc_new(RSRC_SEM,    x)

#endif
