#ifndef __PRA_KEYBOARD_H__
#define __PRA_KEYBOARD_H__

/* Keyboard scan codes */
#define KEY_NONE     0x0       /* Just a marker */

#define KEY_ESC      0x1
#define KEY_BKSP     0x0E
#define KEY_TAB      0x0F
#define KEY_ENTER    0x1C
#define KEY_CTRL     0x1D
#define KEY_LSHIFT   0x2A
#define KEY_RSHIFT   0x36
#define KEY_PRTSRN   0x37      /* E0 2A E0 37 */
#define KEY_ALT      0x38 
#define KEY_SPACE    0x39
#define KEY_CAPS     0x3A
#define KEY_F1       0x3B
#define KEY_F2       0x3C
#define KEY_F3       0x3D 
#define KEY_F4       0x3E
#define KEY_F5       0x3F
#define KEY_F6       0x40
#define KEY_F7       0x41
#define KEY_F8       0x42 
#define KEY_F9       0x43
#define KEY_F10      0x44
#define KEY_F11      0x57
#define KEY_F12      0x58
#define KEY_NUM      0x45
#define KEY_SCROLL   0x46
#define KEY_HOME     0x47     /* E0 47 */  
#define KEY_UP       0x48
#define KEY_PAGEUP   0x49     /* E0 49 */ 
#define KEY_LEFT     0x4B
#define KEY_CENTER   0x4C
#define KEY_RIGHT    0x4D
#define KEY_END      0x4F     /* E0 4F */
#define KEY_DOWN     0x50     /* E0 50 */ 
#define KEY_PAGEDOWN 0x51     /* E0 51 */
#define KEY_INS      0x52     /* E0 52 */
#define KEY_DEL      0x53     /* E0 53 */ 
#define KEY_PAUSE    0x45     /* E0 10 45 */ 

/* Some key codes to identify the status of keyboard key press */
#define KEY_PRESS_ALT   0x0200         /* Alt key is pressed     */
#define KEY_PRESS_CTRL  0x0400         /* Control key is pressed */
#define KEY_PRESS_SHIFT 0x0800         /* Shift key is pressed   */
#define KEY_PRESS_ANY (KEY_PRESS_ALT | KEY_PRESS_CTRL | KEY_PRESS_SHIFT)
#define KEY_PRESS_CAPS   0x1000        /* Caps lock is on        */
#define KEY_PRESS_NUM    0x2000        /* Num lock is on         */  
#define KEY_PRESS_SCROLL 0x4000        /* Scroll lock is on      */

#define KEY_CTRL_ALT_DEL (unsigned char)-1 /* Special code for CTRL+ALT+DEL */

/* ASCII codes for some keys */
#define KVAL_ESC 0x1
#define KVAL_F1  0x80
#define KVAL_F2  (KVAL_F1 + 1)
#define KVAL_F3  (KVAL_F2 + 1)
#define KVAL_F4  (KVAL_F3 + 1)
#define KVAL_F5  (KVAL_F4 + 1)
#define KVAL_F6  (KVAL_F5 + 1)
#define KVAL_F7  (KVAL_F6 + 1)
#define KVAL_F8  (KVAL_F7 + 1)
#define KVAL_F9  (KVAL_F8 + 1)
#define KVAL_F10  (KVAL_F9 + 1)
#define KVAL_F11  (KVAL_F10 + 1)
#define KVAL_F12  (KVAL_F11 + 1)
#define KVAL_INS   0x90
#define KVAL_DEL   (KVAL_INS + 1)
#define KVAL_HOME  (KVAL_DEL + 1)
#define KVAL_END   (KVAL_HOME + 1)
#define KVAL_PGUP  (KVAL_END  + 1)
#define KVAL_PGDN  (KVAL_PGUP + 1)
#define KVAL_LEFT  (KVAL_PGDN + 1)
#define KVAL_UP    (KVAL_LEFT + 1)
#define KVAL_DOWN  (KVAL_UP   + 1)
#define KVAL_RIGHT (KVAL_DOWN + 1)
#define KVAL_PRINT (KVAL_RIGHT +1)
#define KVAL_PAUSE (KVAL_PRINT +1)
#define KVAL_LWIN  (KVAL_PAUSE +1)
#define KVAL_RWIN  (KVAL_LWIN  +1)
#define KVAL_MENU  (KVAL_RWIN  +1)

void init_keyboard(void);
unsigned getkey(void);
void ackkey(void);

void reboot(void);
void write_kbd(unsigned address,unsigned data);

const char *keystr(unsigned scan_code);
unsigned char keyconvert(unsigned scan_code);
char keychar(unsigned scan_code);

unsigned char key_convert(unsigned scan_code);
void sys_keyboard_irq(void);

extern unsigned short _kbd_status;

#endif
