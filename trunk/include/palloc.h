#ifndef __PRAOS_PALLOC_H_
#define __PRAOS_PALLOC_H_

#include <types.h>

short int config_memcheck(short int type);

void *palloc(size_t size);
void pfree(void *blk);
void *prealloc(void *blk,size_t size);
void dump_heap(void);

#endif
