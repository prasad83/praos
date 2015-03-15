#ifndef __PRA_STDARG_H_
#define __PRA_STDARG_H_

#include <types.h>

/* I am assuming the size of stack item = size of INT */
#define STACKITEMWIDTH sizeof(int)

/* Round up the width of objects pushed on to stack, & as division  */
#define STACKARGSIZE(TYPE)\
 ((sizeof(TYPE)+STACKITEMWIDTH-1) & ~(STACKITEMWIDTH-1))

/* Point the va_list to point to first argument of the call.
 * First point va_list ot LASTARG then advance it to the first variable arg */
#define va_start(ARGPTR,LASTARG) \
 (ARGPTR=(va_list)((char *)&(LASTARG) + STACKARGSIZE(LASTARG)))

#define va_end(ARGPTR) /* do nothing */

/* Increment the va_list pointer, then return the previous value of the pointer 
 * Comma operator is being used. So expression is evaluated from left-to-right and
 * value of last expression is actually used for assignment (here +=) */
#define va_arg(ARGPTR,TYPE) \
 ((char *)ARGPTR += STACKARGSIZE(TYPE), *((TYPE *)((char *)(ARGPTR)-STACKARGSIZE(TYPE))))

#endif



