#ifndef __PRA_X86_H__
#define __PRA_X86_H__

#include <keyboard.h>

unsigned inportb(unsigned short port);
void outportb(unsigned port, unsigned val);
void enable(void);
unsigned disable(void);

/* The layout of this structure must match the order of registers
 * pushed and popped by the exception handlers in kboot.asm
 */
typedef struct {
  /* pushed by pusha */
  unsigned edi, esi, ebp, esp, ebx, edx, ecx, eax;
  /* pushed separately */
  unsigned ds, es, fs, gs;
  unsigned which_int, error_code;
  /* pushed by exception: Exception may also push err_code.
     user_esp and user_ss are pushed only if a privilege change occurs. */
  unsigned eip, cs, eflags, user_esp, user_ss;
} regs_t;

extern const char *const intr_message[];

void fault_message(unsigned intr_num);
void fault(regs_t *regs);
void dump_regs(regs_t *regs);

#endif
