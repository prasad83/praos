#include <types.h>
#include <string.h>

/* Copy count number of contents from source memory to destination memory. 
 * RETURN: start location of destination memory.
 */
void *memcpy(void *dst, const void *src, size_t count) {
  void *start_dst_ptr = dst;

  const char *src_ptr = (const char *) src;
  char *dst_ptr = (char *)dst;
  
  while(count-- != 0) *dst_ptr++ = *src_ptr++;
  return start_dst_ptr;
}
  
/* Set count number of destination memory with the value 
 * RETURN: start location of destination memory.
 */
void *memsetw(void *dst, int val, size_t count) {
  unsigned short *dst_ptr = (unsigned short *)dst;

  while(count-- != 0) *dst_ptr++ = val;
  return dst;
}

