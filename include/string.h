/* $Id: string.h,v 1.3 1998/03/09 13:56:03 swetland Exp $ */

#ifndef _STRING_H_
#define _STRING_H_

#include <types.h>

char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t size);
int strncmp(const char *s1, const char *s2, size_t n);
int strcmp(const char *s1, const char *s2);
size_t strlen(const char *str);

void *memset(void *s, int c, size_t n);
int memcmp(const void *dst, const void *src, size_t size);
void *memcpy(void *dst, const void *src, size_t size);
void *memmove(void *dest, const void *src, size_t n);

#endif
