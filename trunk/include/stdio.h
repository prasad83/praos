#ifndef __PRA_STDIO_H__
#define __PRA_STDIO_H__

#include <types.h>

void strprint(const char *str);
void putch(unsigned ch);

void mprintf(const char *fmt, ...);
void parse_printf(const char *fmt,va_list args);

#endif
