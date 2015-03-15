#ifndef __PRA_CONIO_H__
#define __PRA_CONIO_H__

/* For both background and foreground */
#define COLOR_BLACK 0x0
#define COLOR_BLUE  0x1
#define COLOR_GREEN 0x2
#define COLOR_CYAN  0x3
#define COLOR_RED   0x4
#define COLOR_MAGENTA 0x5
#define COLOR_BROWN 0x6
#define COLOR_WHITE 0x7
 
/* For foreground only */
#define COLOR_DARKGRAY      0x8
#define COLOR_BRIGHTBLUE    0x9
#define COLOR_BRIGHTGREEN   0xA
#define COLOR_BRIGHTCYAN    0xB
#define COLOR_PINK          0xC
#define COLOR_BRIGHTMAGENTA 0xD
#define COLOR_YELLOW        0xE
#define COLOR_BRIGHTWHITE   0xF

void clrscr(void);
unsigned getcolor(short foreground,short background);
void cprintf(unsigned color, const char *fmt,...);
void blink_cursor(void);

#endif
