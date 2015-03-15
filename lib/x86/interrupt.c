#include <globals.h>
#include <x86.h>
#include <stdio.h>
#include <conio.h>
#include <cursor.h>

const char *const intr_message[] = {
  "Divide Error",                  /* 0 */  
  "Debug Exception",               /* 1 */
  "NMI",                           /* 2 */
  "INT3",                          /* 3 */
  "INTO",                          /* 4 */
  "BOUND exception",               /* 5 */  
  "Invalid opcode",                /* 6 */ 
  "No Coprocessor",                /* 7 */ 
  "Double Fault",                  /* 8 */ 
  "Coprocessor Segment Overrun",   /* 9 */ 
  "Bad TSS",                       /* 10 */
  "Segment Not Present",           /* 11 */ 
  "Stack Fault",                   /* 12 */  
  "GPF",                           /* 13 */
  "Page Fault",                    /* 14 */
  "???",                           /* 15 */
  "Coprocessor errror",            /* 16 */
  "Alignment Check",               /* 17 */  
  "???", "???", "???", "???", "???", "???", "???",           /* 18-24 */ 
  "???", "???", "???", "???", "???", "???", "???",           /* 25-31 */
  "IRQ0", "IRQ1", "IRQ2", "IRQ3", "IRQ4", "IRQ5", "IRQ6",    /* 32-38 */ 
  "IRQ7", "IRQ8", "IRQ9", "IRQ10", "IRQ11", "IRQ12", "IRQ13",/* 39-45 */
  "IRQ14", "IRQ15",                                          /* 46-47 */
  "Syscall"                                                  /* 48    */ 
};

void fault_message(unsigned intr_num) {
  mprintf("\n    ");
  
  if(intr_num > sizeof(intr_message) / sizeof(intr_message[0])) 
    cprintf(getcolor(COLOR_BRIGHTWHITE,COLOR_RED),
      "Fault %x -> '%s' has occured!", intr_num, "UNKNOWN!");
  else
    cprintf(getcolor(COLOR_BRIGHTWHITE, COLOR_RED),
      "Fault: %x -> '%s' has occured!",intr_num,intr_message[intr_num]);
}

void dump_regs(regs_t *regs) {
  mprintf("\nDump of register values at this point is here...\n");
  mprintf("EDI=%x    ESI=%x    EBP=%x    ESP=%x\n", regs->edi, regs->esi, regs->ebp, regs->esp);
  mprintf("EBX=%x    ECX=%x    EDX=%x    EAX=%x\n", regs->ebx, regs->ecx, regs->edx, regs->eax);
  mprintf(" DS=%x     ES=%x     FS=%x     GS=%x\n", regs->ds,  regs->es,  regs->fs,  regs->gs );
  mprintf("int=%x    err=%x    EIP=%x     CS=%x\n", regs->which_int,regs->error_code,regs->eip,regs->cs);
  mprintf("userSP=%x           userSS=%x\n", regs->user_esp, regs->user_ss);
}

void fault(regs_t *regs) { 
  switch(regs->which_int) {
    case 0x20: /* timer IRQ0 */
      /* fault_message(regs->which_int); */
      blink_cursor();
      /* reset hardware interrupt at 8259 chip */
      outportb(0x20,0x20);
      break;
    default:
      fault_message(regs->which_int);
  
      cprintf(getcolor(COLOR_BRIGHTWHITE,COLOR_RED),"\nNo Exception Handlers set for handling the fault!\n");
      dump_regs(regs);
      mprintf("Use Reset Button to Restart...");

      while(1) 
        ; /* Hang */
      break;
  }
}

