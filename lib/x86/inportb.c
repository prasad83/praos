#include <x86.h>
unsigned inportb(unsigned short port) {
  unsigned char ret_val;
  
  __asm__ __volatile__(
    "inb %1, %0"
    : "=a"(ret_val) /* output */
    : "d"(port)     /* port */
  );
  return ret_val;
}
