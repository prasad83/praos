#include <string.h> /* memcpy(), memset() */
#include <stdio.h>
#include <conio.h>
#include <types.h>
#include <palloc.h>

#define HEAP_SIZE 500000uL
/*#define MIN_HEAP 12 / * HEADER SIZE * / */

#define PALLOC_MAGIC 0x7A54 /* must be < 0x8000 (32767) (only 15 bits is used) */

/* prefix: p = prasad */

/* Header node for dynamically memory allocated. */
typedef struct palloc_node {
  unsigned magic:15;        /* 4 bytes - 1 bit */
  unsigned used:1;          /* 1 bit   */

  size_t size;              /* 4 bytes */
  struct palloc_node *next; /* 4 bytes */
} palloc_t;                 /* Total 12 bytes */

static char *p_heap_bottom = NULL, *p_kbrk = NULL, *p_heap_top = NULL;

/* Actions to be taken by palloc or pfree is depends on check_memory.
 * check_memory = 0: no action is taken when heap memory allocated already is corrupt.
 * check_memory = 1: a warning message is printed when heap memory allocated already is corrupt.
 * check_memory = 2: return NULL when heap memory allocated already is corrupt.
 */
static short int check_memory = 0; 

short int config_memcheck(short int type) {
	int old_check_memory = check_memory;
	if(type <= 0) check_memory = 0;
	else if(type == 1) check_memory = 1;
	else if(type >= 2) check_memory = 2;
	return old_check_memory;
}

void dump_heap(void) {
  unsigned used_blks = 0, free_blks = 0;
  size_t used_bytes = 0, free_bytes = 0;
  palloc_t *m = NULL;
  int fulltotal = 0;

  mprintf("----------------------->>> HEAP DUMP IS SHOWN BELOW <<<------------------------\n");
  for(m = (palloc_t *)p_heap_bottom; m != NULL; m = m -> next) {
    mprintf("blk: header 0x%p : data 0x%p, size %u bytes <%s>, magic(%u/%u)\n",
      m, ((char *)m) + sizeof(palloc_t), m -> size, 
			(m -> used)? "used":"free", m->magic, PALLOC_MAGIC);

    if(m->used) {
      ++used_blks;
      used_bytes += m -> size;
    }
    else {
      ++free_blks;
      free_bytes += m -> size;
    }
  }
  mprintf("blks: %u used, %u free, %u total\n", used_blks, free_blks, (used_blks+free_blks));
  mprintf("bytes:%u used, %u free, %u total\n", used_bytes,free_bytes,(used_bytes+free_bytes));
  mprintf("p_heap_bottom=0x%p, p_kbrk=0x%p, p_heap_top=0x%p\n", p_heap_bottom, p_kbrk, p_heap_top);
  fulltotal = (used_bytes + free_bytes) + (used_blks + free_blks) * sizeof(palloc_t);
  
  if(fulltotal != p_kbrk - p_heap_bottom)  
    mprintf("*** Some heap memory is not accounted...\n");
  mprintf("-------------------------------------------------------------------------------\n");
}

static void *kbrk(int *delta) {
  static char heap[HEAP_SIZE];
  char *new_brk, *old_brk;

  /* heap doesn't exists yet */
  if(p_heap_bottom == NULL) {
    p_heap_bottom = p_kbrk = heap;
    p_heap_top = p_heap_bottom + HEAP_SIZE;
  }
  new_brk = p_kbrk + (*delta);
  
  /* too low: return NULL */
  if(new_brk < p_heap_bottom) return NULL;

  /* too high: return NULL */
  if(new_brk >= p_heap_top) return NULL;

  /* success: adjust brk value... */
  old_brk = p_kbrk;
  p_kbrk = new_brk;

  /* return actual delta (for this they are same) */
  (*delta) = (*delta);

  /* return old brk value */
  return old_brk;
}

/* Check whether the node is corrupted only when check_alloc > 0
 * return TRUE if node is not corrupted (or check_memory == 0) else return FALSE.
 */
static int check_magic(palloc_t *node) {
	if(check_memory > 0 && node != NULL && node -> magic != PALLOC_MAGIC) {
    return FALSE;
  }
  return TRUE;
}

/* print warning message when check_memory > 0, 
 * return TRUE if check_memory > 1 (so that core_palloc can return NULL)
 * otherwise return FALSE.
 */
static int palloc_error_message(palloc_t *node) {	
	if(check_memory > 0)
	  mprintf("pmalloc: Heap node 0x%p is corrupt (magic: %u/expected: %u) %s\n", 
			node , node->magic, PALLOC_MAGIC, (check_memory > 1)? "ABORTING":"WARNING");
	return (check_memory > 1); 
}

static void *core_palloc(size_t size) {
  unsigned total_size;
  palloc_t *m, *n;
  int delta;

  if(size == 0)  return NULL;
  total_size = size + sizeof(palloc_t);

  /* search heap for free block (FIRST FIT) */
  m = (palloc_t *)p_heap_bottom;
  /* p_heap_bottom == 0 == NULL if heap does not yet exists */
  if(m != NULL) {
    for( ;m -> next != NULL; m = m -> next) {
      /* Check the heap nodes already allocated */
      if(check_magic(m) == FALSE) { 
				if(palloc_error_message(m) == TRUE)
					return NULL; 
			}

      if(m -> used) { 
        continue;  
      }
      /* size == m -> size is a perfect fit */
      if(size == m -> size) { m -> used = 1; }
      else {
        /* otherwise, we need an extra sizeof(palloc_t) bytes for the header
         * of a second, free block. */
        if(total_size > m -> size)  continue;

        /* create a new, smaller free block after this one. */
        n = (palloc_t *)((char *)m + total_size);
        n -> size = m -> size - total_size;
        n -> next = m -> next;
        n -> magic = PALLOC_MAGIC;
        /* An extra check to make sure all nodes allocated are proper */
        if(check_magic(m) == FALSE) { palloc_error_message(m); return NULL;  }
        n -> used = 0;
      
        /* reduce the size of this block and mark it used. */
        m -> size = size;
        m -> next = n;
        m -> used = 1;
      }
      /* return location of memory that is after palloc_t node (header) */
      return (char *)m + sizeof(palloc_t);
    }
  }
  /* Check the heap nodes already allocated */
  if(check_magic(m) == FALSE) {
    if(palloc_error_message(m) == TRUE)
	    return NULL;
  }
  /* use kbrk() to enlarge (or create!) heap */
  delta = total_size;
  n = kbrk(&delta);
    
  /* oops! allocation failed */
  if(n == NULL) return NULL;
  
  if(m != NULL) { m -> next = n; }

  n -> size = size;
  n -> magic = PALLOC_MAGIC;
  n -> used = 1;
  
  /* did kbrk() return exact amount of memory we wanted? */
  if((int) total_size == delta) n -> next = NULL;
  else {
    /* it returned more than we wanted (it will never return less)
     * create a new, free block. */
    m = (palloc_t *)((char *)n + total_size);
    m -> size = delta - total_size - sizeof(palloc_t);
    m -> next = NULL;
    m -> magic = PALLOC_MAGIC;
    m -> used = 0;

    n -> next = m;
  }
  /* return location of memory that is after palloc_t node (header) */
  return (char *) n + sizeof(palloc_t);
}

void *palloc(size_t size) { return core_palloc(size); }

/* print warning message when check_memory > 0, 
 * return TRUE if check_memory > 1 (so that core_pfree can stop freeing node)
 * otherwise return FALSE.
 */
static int pfree_error_message(palloc_t *node) {
	if(check_memory > 0)
	  mprintf("pfree: Heap node 0x%p is corrupt (magic: %u/expected: %u) %s\n",
  	  node, node->magic, PALLOC_MAGIC, (check_memory > 1)? "ABORTING": "WARNING");
	return (check_memory > 1);	
}

static void core_pfree(void *blk) {
  palloc_t *m, *n;
  
  /* get address of header */
  m = (palloc_t *) ((char *)blk - sizeof(palloc_t));
  if(check_magic(m) == FALSE) {
    if(pfree_error_message(m) == TRUE)
	    return;
  }
  /* find this block in heap */
  n = (palloc_t *) p_heap_bottom;
  for( ; n != NULL; n = n -> next) {
    if(check_magic(n) == FALSE) {
      if(pfree_error_message(n) == TRUE)
	      return;
    }
    if(n == m) break;
  }
  
  /* Node not found. Bad pointer or no heap or something else! */
  if(n == NULL) {
    mprintf("pfree: Attempt to free block at 0x%p that is not in the heap.\n", blk);
    return;
  }
  
  /* free the block */
  m -> used = 0;
  
  /* do some compaction */
  for(m = (palloc_t *)p_heap_bottom; m != NULL; m = m -> next) {
    while(!(m -> used) && m -> next != NULL && !((m -> next) -> used)) {
      /* resize this block */
      m -> size += sizeof(palloc_t) + (m -> next) -> size;
      /* merge with next block */
      m -> next = (m -> next) -> next;
    }
  }
}

void pfree(void *blk) { core_pfree(blk); }

/* print warning message when check_memory > 0, 
 * return TRUE if check_memory > 1 (so that core_prealloc can return NULL)
 * otherwise return FALSE.
 */
static int prealloc_error_message(palloc_t *node) {
	if(check_memory > 0)
	  mprintf("prealloc: Heap node 0x%p is corrupt (magic: %u/expected: %u) %s\n",
  	  node, node->magic, PALLOC_MAGIC, (check_memory > 1)? "ABORTING": "WARNING");
	return (check_memory > 1);	
}


static void *core_prealloc(void *blk, size_t size) {
	void *new_blk;
	palloc_t *m;

	/* size == 0: free block */
	if(size == 0) {
		if(blk != NULL) pfree(blk);
		new_blk = NULL;		
	}
	else {
		/* allocate a new_blk */
		new_blk = palloc(size);
		/* if allocation was OK, and if old block exists, copy old block to new */
		if(new_blk != NULL && blk != NULL) {
			m = (palloc_t *)((char *)blk - sizeof(palloc_t));
			if(check_magic(m) == FALSE) {
				if(prealloc_error_message(m) == TRUE)
					return NULL;
			}
			/* Copy minimum of old and new block sizes */
			if(size > m -> size) size = m -> size;
			memcpy(new_blk,blk,size);
			/* free the old block */
			pfree(blk);
		}
	}
	return new_blk;
}

void *prealloc(void *blk, size_t size) { return core_prealloc(blk,size); }

