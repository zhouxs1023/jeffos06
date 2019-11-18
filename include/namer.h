#ifndef _NAMER_H_
#define _NAMER_H_

#define NAMER_PORT 1
#define NAMER_TEXT_MAX 32

typedef struct _namer_message_t
{
    int number;
    char text[NAMER_TEXT_MAX];    
} namer_message_t;

int namer_newhandle(void);
int namer_delhandle(int nh);
int namer_register(int nh, int port, char *name);
int namer_find(int nh, char *name);


#endif
