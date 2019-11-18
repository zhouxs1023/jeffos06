#ifndef _HELPER_H
#define _HELPER_H

#include "types.h"
#include "task.h"

void KeHalt( void );
STATUS KeGetNextGDT( OUT uint32 *next );
STATUS KeDumpTask( IN task *t );
void KePanic( char *reason );


#endif
