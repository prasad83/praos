#include <x86.h>
#include <stdio.h>

void init_keyboard(void) {
  static const unsigned irq0_int = 0x20, irq8_int = 0x28;

  /* Initialization Control Word #1 (ICW1) */
  outportb(0x20, 0x11);
  outportb(0xA0, 0x11);
  /* ICW2: route IRQs 0-7 to INTs 20h-27h */
  outportb(0x21, irq0_int);
  /* route IRQs 8-15 to INTs 28h - 2Fh */
  outportb(0xA1, irq8_int);
  /* ICW3 */
  outportb(0x21, 0x04);
  outportb(0xA1, 0x02);  
  /* ICW4 */
  outportb(0x21, 0x01);
  outportb(0xA1, 0x01);
  /* enable IRQ0 (timer) and IRQ1 (keyboard) */
  outportb(0x21, ~0x03);
  outportb(0xA1, ~0x00);
}  

unsigned getkey(void) { return inportb(0x60); }
void ackkey(void) { outportb(0x20,0x20); }

void reboot(void) {
  unsigned status;
         
  disable();
  /* flush the keyboard controller */
  do {
    status = inportb(0x64);
    if((status & 0x01) != 0) {
      inportb(0x64);
      continue;
    }
  } while((status & 0x02) != 0);
  /* pulse the CPU reset line */
  outportb(0x64, 0xFE);
  /* Oops! CPU was not able to reboot, Lets halt */
  while(1)
    ;/* do nothing */
}  

void write_kbd(unsigned address,unsigned data) {
  unsigned long timeout;
  unsigned status;
  
  for(timeout = 500000L; timeout != 0; --timeout) {
    status = inportb(0x64);
    /* Loop until 8042 input buffer empty */
    if((status & 0x02) == 0)  break;
  }
  if(timeout != 0) outportb(address,data);
}

/* Returns string representation of the scan_code */
const char *keystr(unsigned scan_code) {
  static const char *keystrcodes[] = {
    "NONE",
    "ESC", "1","2","3","4","5","6","7","8","9","0","-","=","BKSP","TAB",
    "q","w","e","r","t","y","u","i","o","p","[","]","ENTER","L/RCTRL",
    "a","s","d","f","g","h","j","k","l",";","'",
    "`","LSHIFT","\\",
    "z","x","c","v","b","n","m",",",".","/","RSHIFT",
    "PRNTSRN","L/RALT", "SPACE","CAPS", 
    "F1","F2","F3","F4","F5","F6","F7","F8","F9","F10",
    "NUM","SCROLL","HOME","UP","PGUP","NUM-","LEFT","NUM5","RIGHT","NUM+","END",
    "DOWN","PGDN","INS","DEL","NONE","NONE","NONE","F11","F12",0,0,"LWIN","RWIN","MENU"
  };
  /* Ignore break code */
  if(scan_code >= 0x80) return 0; 
  return keystrcodes[scan_code];
}

/* Returns customized char representation of the scan_code */
char keychar(unsigned scan_code) {
  static const char keych[] = {
    0, /* NONE */
    0x1,'1','2','3','4','5','6','7','8','9','0','-','=',0x11,0x10,
    'q','w','e','r','t','y','u','i','o','p','[',']',0x14,0x12,
    'a','s','d','f','g','h','j','k','l',';','\'',
    '`',0xA9,'\\',
    'z','x','c','v','b','n','m',',','.','/',0xAA,
    0x8,0xD,' ',0x80,
    0XE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,
    0xA5,0x16,0X17,0X18,0X1E,0XC4,0X1B,0X1D,0X1A,0XC2,0X16,
    0X19,0X1F,0X7,0X9,0,0,0,0XEA,0XEB,0,0,0xAE,0xAF,0x9F
  };
  /* Ignore break code */
  if(scan_code >= 0x80) return 0;
  /* Ignore invalid scan code */
  if(scan_code >= sizeof(keych)/sizeof(keych[0])) return 0;
  return keych[scan_code];
}
  
unsigned short _kbd_status;

unsigned char keyconvert(unsigned scan_code) {
  static const unsigned char key_map[] = {
    0,                                                                                             /* NONE */
    KVAL_ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-' ,'=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p','[',']', '\n',  0,               /* 0 = L/RCTRL */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', 
    '`',   0, '\\' ,                                                                         /* 0 = LSHIFT */
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,                                /* 0 = RSHIFT */
    0, 0, ' ', 0,                                           /* (0,0,' ',0) = PRNTSRN, L/R ALT, SPACE, CAPS */
    KVAL_F1,KVAL_F2,KVAL_F3,KVAL_F4,KVAL_F5,KVAL_F6,KVAL_F7,KVAL_F8,KVAL_F9,KVAL_F10,
    0,0,KVAL_HOME,KVAL_UP,KVAL_PGUP,'-',KVAL_LEFT,'5',KVAL_RIGHT,'+',KVAL_END,      /* (0,0) =(NUM,SCROLL) */
    KVAL_DOWN,KVAL_PGDN,KVAL_INS,KVAL_DEL,0,0,0,KVAL_F11,KVAL_F12,             /* (0,0,0)=(NONE,NONE,NONE) */
    0,0,KVAL_LWIN,KVAL_RWIN,KVAL_MENU                                                 /* (0,0)=(NONE,NONE) */
  };
  static const unsigned char shift_key_map[] = {
    0,
    KVAL_ESC, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"',
    '~', 0, '|',
    'Z', 'X', 'C', 'V','B','N','M', '<', '>', '?', 0,
    0, 0, ' ', 0,
    KVAL_F1,KVAL_F2,KVAL_F3,KVAL_F4,KVAL_F5,KVAL_F6,KVAL_F7,KVAL_F8,KVAL_F9,KVAL_F10,
    0,0,KVAL_HOME,KVAL_UP,KVAL_PGUP,'-',KVAL_LEFT,'5',KVAL_RIGHT,'+',KVAL_END,      
    KVAL_DOWN,KVAL_PGDN,KVAL_INS,KVAL_DEL,0,0,0,KVAL_F11,KVAL_F12,             
    0,0,KVAL_LWIN,KVAL_RWIN,KVAL_MENU                                                 
  };  
  static unsigned short got_break_code;
  
  unsigned short leds;

  /* Check for break key code */
  if(scan_code >= 0x80) {
    got_break_code = 1;
    scan_code &= 0x7F;
  }
  /* We shall take action for only (ALT,CTRL,SHIFT) break key codes */
  if(got_break_code) {
    if(scan_code == KEY_ALT) _kbd_status &= ~KEY_PRESS_ALT;
    else if(scan_code == KEY_CTRL) _kbd_status &= ~KEY_PRESS_CTRL;
    else if(scan_code == KEY_LSHIFT || scan_code == KEY_RSHIFT) _kbd_status &= ~KEY_PRESS_SHIFT;
    got_break_code = 0;
    return 0;
  }

  /* It was not a break key code, but a make key code */
  if(scan_code == KEY_ALT) { _kbd_status |= KEY_PRESS_ALT; return 0; }
  if(scan_code == KEY_CTRL){ _kbd_status |= KEY_PRESS_CTRL; return 0;}
  if(scan_code == KEY_LSHIFT || scan_code == KEY_RSHIFT) {
    _kbd_status |= KEY_PRESS_SHIFT;
    return 0;
  }
  
  /* For Scroll/Num/Caps lock key press set the LEDs. These keys
   * have on-off (toggle or XOR) action. */
  if(scan_code == KEY_SCROLL) {
    _kbd_status ^= KEY_PRESS_SCROLL;
    goto LEDS;
  }
  if(scan_code == KEY_NUM) {
    _kbd_status ^= KEY_PRESS_NUM;
    goto LEDS;
  }
  if(scan_code == KEY_CAPS) {
    _kbd_status ^= KEY_PRESS_CAPS;
  LEDS:
    write_kbd(0x60, 0xED); /* "set LEDs" command */
    leds = 0;
    if(_kbd_status & KEY_PRESS_SCROLL) leds |= 1;
    if(_kbd_status & KEY_PRESS_NUM)    leds |= 2;
    if(_kbd_status & KEY_PRESS_CAPS)   leds |= 4;
    write_kbd(0x60,leds);
    return 0;
  }

  /* Ignore invalid scan codes */
  if(scan_code >= sizeof(key_map)/sizeof(key_map[0])) return 0;  

  /* Return Special code for CTRL+ALT+DEL */
  if((_kbd_status & KEY_PRESS_CTRL) && (_kbd_status & KEY_PRESS_ALT) && (scan_code == KEY_DEL))
    return KEY_CTRL_ALT_DEL;
   
  /* Covert raw scancode in code to ASCII, check the status of CAPS lock and SHIFT key press */
	if(_kbd_status & KEY_PRESS_CAPS) {
		/* Caps lock is on, so for alphabets return as if shift key is pressed, 
     * if shift key is also pressed then return as if no-shift key is used. */
		if(key_map[scan_code] >= 'a' && key_map[scan_code] <= 'z') 
			return (_kbd_status & KEY_PRESS_SHIFT)? key_map[scan_code]:shift_key_map[scan_code];
	}
	return (_kbd_status & KEY_PRESS_SHIFT)? shift_key_map[scan_code]:key_map[scan_code];
}

void sys_keyboard_irq(void) {
  unsigned scan_code, ascii;
  
  scan_code = getkey();
  ascii = keyconvert(scan_code);

  if(ascii != 0 && ascii != KEY_CTRL_ALT_DEL) {
      putch(ascii);
  }
  if(ascii == KEY_CTRL_ALT_DEL) {
    mprintf("Rebooting the System...\nPlease wait -- Prasad.A");
    reboot();
  }
  
  /* Reset hardware interrupt at 8259 chip */
  ackkey();
}

