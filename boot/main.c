#include <globals.h>
#include <x86.h>
#include <conio.h>
#include <cursor.h>
#include <stdio.h>
#include <string.h>
#include <echo.h>
#include <palloc.h>

/* PROTOTYPES FOR FUNCITONS DEFINED AFTER MAIN */
void lib_test(void);
void delay(unsigned long count);
void printok(void);
void printerror(void);
void shutdown(void);

/* The code for setvect() and getvect() in kboot.asm depends on
 * the layout of this structure. */
typedef struct {
  unsigned access_byte, eip;
} vector_t;

/* IMPORTS from kboot.asm */
void setvect(vector_t *v, unsigned vect_num);
void getvect(vector_t *v, unsigned vect_num);

void delay(unsigned long count) {
  unsigned long temp = count;  
  while(count-- > 0) {
    temp = count+1;
    move2xy(0,15);
    strprint(unum2str(temp));
  }  
}

const char OKSTR[] =    { "[  OK  ]" };
const char ERRORSTR[] = { "[ERROR!]" };

void printok(void) {
  unsigned okstr_x = (SCREEN_WIDTH-2)- strlen(OKSTR);
  move2xy(okstr_x,-1);
  mprintf("[  "); cprintf(COLOR_GREEN,"OK"); mprintf("  ]\n");
}  

void printerror(void) {
  unsigned errorstr_x = (SCREEN_WIDTH-2) - strlen(ERRORSTR);
  move2xy(errorstr_x,-1);
  mprintf("["); cprintf(COLOR_RED,"ERROR!"); mprintf("]\n");
}

static void custom_handler(regs_t *regs) {
  fault_message(regs->which_int);
  mprintf("\n    ");
  cprintf(getcolor(COLOR_BRIGHTWHITE,COLOR_BROWN),"INSIDE Custom Handler :)");
}

void shutdown(void) {
  move2xy(0,++cursor_y);
  mprintf("Shutting down...\n");
  return;
}

int main(void) {  
  vector_t v;
  vector_t old_v, new_v;

  SCREEN_WIDTH = *(unsigned short *)0x44A;
  SCREEN_HEIGHT= *(unsigned char *)0x484+1;

  mprintf("Clearing the screen...");
  clrscr();

  /* Show the booting message */  
  mprintf("Prasad OS BOOTING...");
  printok();

  /* Check the interrupt handler setup, setvect() and getvect() */
  mprintf("Checking Assembly Interrupt Setup...");
  move2xy((SCREEN_WIDTH-2) - strlen("[ .... ]"),-1);
  mprintf("[ .... ]\n");

  /* Try interrupt only after setting up the handler, as the default
   * action is to halt/get into infinite loop see "lib/x86/interrupt.c" */
  /* mprintf("Trying INT 0x30...");
  __asm__ __volatile__("int %0" : : "i"(0x30));
  printok();*/

  mprintf("  *Saving INT %x vector...",0x30);
  getvect(&old_v,0x30);
  printok();

  mprintf("  *Installing new INT %x handler...",0x30);
  new_v.eip = (unsigned) custom_handler;
  new_v.access_byte = 0x8E;
  setvect(&new_v,0x30);
  printok();

  mprintf("  *Trying INT 0x30 to check custom handler...");
  /*__asm__ __volatile__("int $30");*/
  __asm__ __volatile__("int %0" : : "i"(0x30));
  printok();

  mprintf("  *Restoring INT %x's original handler...",0x30);
  setvect(&old_v,0x30);
  printok();

  /* Setup keyboard interrupt handler */
  mprintf("Initializing KEYBOARD (INT %x)...",0x21);  
  init_keyboard();  
  /* let us not save the old vector */
  v.eip = (unsigned) sys_keyboard_irq;
  v.access_byte = 0x8E; /* present, ring 0, '386 interrupt gate */
  setvect(&v, 0x21);
  printok();  

  mprintf("Enabling Hardware interrupts...");  
  enable(); /* This enables interrupts to be triggered, so user can now use keyboard */
  printok();

  mprintf("Can accept from Keyboard...");
  printok();
  
  cprintf(getcolor(COLOR_BRIGHTWHITE,COLOR_BLUE),"Use CTRL+ALT+DEL to reboot...\n"); 

	/* Let us now test echoing */
	setecho(0);
	mprintf("This message should not be displayed at all....\n");

	/* Turn on echo again */
	setecho(1);

	/* Start library implementation testing... */
	lib_test();
	

	
	/* Hang forever :) */ 
  while(1);

  shutdown();  
  return 0;
}

/* Tests for checking the library implementation. */
void lib_mem_test(void); /* Function to test memory */

void lib_test(void) {
	/*mprintf("\nTESTING MEMORY...\n");
	lib_mem_test();*/
}

void lib_mem_test(void) {
	/* MEMORY TESTING */
	int old_config = config_memcheck(2); /* STOP ALLOCATION/FREE if heap node is corrupted */

	char *ptr1, *ptr2, *ptr3, *ptr4, *ptr5;
	
	ptr1 = (char *)palloc(10);	
	if(ptr1 == NULL) {
		cprintf(getcolor(COLOR_BRIGHTWHITE,COLOR_RED),"Some palloc problem...\n");
		dump_heap();
	} else {
		mprintf("ALLOCATED: ptr1 now is %p\n", ptr1);

		ptr2 = (char *)palloc(1);
		mprintf("ALLOCATED: ptr2 now is %p\n", ptr2);
	
		strncpy(ptr1,"This is some test...\n",9); /* 10 or more will corrupt ptr2 */
		mprintf("STRNCPY of ptr1: %p, %s\n",ptr1,ptr1);

		mprintf("FREE: ptr1 NOW %p\n",ptr1);	

		pfree(ptr1);
		ptr2 = prealloc(ptr2,5);
		mprintf("REALLOCATED: ptr2 now is %p\n", ptr2);

		ptr3 = palloc(1);
		mprintf("ALLOCATED: ptr3 now is %p\n", ptr3);
		
		ptr4 = palloc(15);
		strncpy(ptr4,"FILL 10", 10); 
		mprintf("STRNCPY OF ptr4: %p, %s ...\n",ptr4,ptr4);

		ptr5 = palloc(5);
		mprintf("ALLOCATED: ptr5 now is %p\n", ptr5);

		mprintf("FREE: ptr3 NOW %p\n",ptr3); pfree(ptr3);
		mprintf("FREE: ptr4 NOW %p\n",ptr4); pfree(ptr4);
		mprintf("FREE: ptr5 NOW %p\n",ptr5); pfree(ptr5);
	}
	dump_heap();
	
	/* Reset memory config */
	config_memcheck(old_config);
}
