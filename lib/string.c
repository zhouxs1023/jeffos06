/* $Id: string.c,v 1.3 1998/03/09 13:56:05 swetland Exp $ */

#include <string.h>

char *strcpy(char *dst, const char *src)
{
    char *d = dst;

    while(*src) *dst++ = *src++;            
    *dst = 0;

    return d;        
}

char *strncpy(char *dst, const char *src, size_t size)
{
    char *d = dst;

    while(size && *src) size--, *dst++ = *src++;            
    if(size) *dst = 0;

    return d;        
}

/* WRONG! */
int strcmp(const char *s1, const char *s2)
{
    while(*s1 && *s2){
        if(*s1 != *s2) return -1;
        s1++;
        s2++;        
    }
    if(*s1 || *s2) return -1;

    return 0;    
}

int strncmp(const char *s1, const char *s2, size_t n)
{    
    while(n && *s1 && *s2){
        if(*s1 != *s2) return -1;
        s1++;
        s2++;
        n--;
        
    }
    if(n) return -1;

    return 0;    
}

size_t strlen(const char *str)
{
    int l = 0;
    while(*str) l++,str++;
    return l;    
}

void *memset(void *s, int c, size_t n)
{
    unsigned char *u = (unsigned char *) s;
    while(n) {
        *u = c;
        u++;
        n--;
    }
    return s;
}

void *memmove(void *dest, const void *src, size_t n)
{
    unsigned char *d;
    unsigned char *s;

    if(dest < src){
        d = (unsigned char *)dest;
        s = (unsigned char *)src;
        while(n){
            *d = *s;
            d++;
            s++;
            n--;
        }
    } else {
        d = ((unsigned char *) dest)+n;
        s = ((unsigned char *) src)+n;
        while(n){
            d--;
            s--;
            *d = *s;
            n--;
        }        
    }
    return dest;
}



int memcmp(const void *dst, const void *src, size_t size)
{
    while(size) {
	if(*((char *)dst) != *((char *)src)) return 1;
	((char *) dst)++;
	((char *) src)++;
        size--;        
    }
    return 0;    
}

void *memcpy(void *dst, const void *src, size_t size)
{
    void *r = dst;
    
    while(size) {
        *(((unsigned char *) dst)++) = *(((unsigned char *) src)++);
        size--;        
    }
    return r;
}

