#include <types.h>
#include <sys/syscall.h>
#include "malloc.h"
#include <conio.h>

void __malloc_initialize();

static unsigned int __sbrk_max;
static unsigned int __sbrk_cur;


void *sbrk(int size)
{
    if(size < 0 ){
        __sbrk_cur -= size;
        return (void *) __sbrk_cur;
    } else {
        unsigned int tmp = __sbrk_cur;
        __sbrk_cur += size;
        if(__sbrk_cur > __sbrk_max){
            __sbrk_max = __sbrk_cur;
            os_brk(__sbrk_max);
        }
        return (void *) tmp;
    }

    return (void *) __sbrk_cur;
}

#ifdef OLD
static sem_t sem_malloc = {0};
#endif
int sem_malloc = 0;

void *malloc(size_t size)
{
    void *r;
    os_AcquireSem(sem_malloc);
    r = __malloc(size);
    os_ReleaseSem(sem_malloc);
    return r;
}

void free(void *ptr)
{
    os_AcquireSem(sem_malloc);
    __free(ptr);
    os_ReleaseSem(sem_malloc);
}

void *realloc(void *ptr, size_t size)
{
    void *r;
    os_AcquireSem(sem_malloc);
    r = __realloc(ptr,size);
    os_ReleaseSem(sem_malloc);
    return r;
}


void * _default_morecore(long size)
{
    void *result;

    result = sbrk(size);
    if (result == (void *) -1)
        return NULL;
    return result;
}

void __libc_init_memory(unsigned int top_of_binary,
                        unsigned int start_bss)
{
    char m[80];
    unsigned char *x = (unsigned char *) start_bss;
    unsigned int tob = (top_of_binary/4096+2)*4096;
    os_brk(tob);

    sem_malloc = os_CreateSem(1);
    
    while(x <= ((unsigned char *) top_of_binary)) {
        *x = 0;
        x++;
    }
    __sbrk_max = __sbrk_cur  = tob;
    __malloc_initialize();
}
