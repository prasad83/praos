#ifndef __PRA_STRING_H__
#define __PRA_STRING_H__

#include <types.h>

unsigned strlen(const char *src);
char *strrev(char *src);
char *strncpy(char *dst,const char *src, size_t length);

void *memcpy(void *dst, const void *src, size_t count);
void *memsetw(void *dst, int val, size_t count);

const char *unum2str(unsigned number);

#endif
