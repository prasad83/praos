#ifndef __PRA_GLOBALS_H_
#define __PRA_GLOBALS_H_

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern const unsigned vga_framebuffer;
extern const unsigned crtc_io_addr;

extern int cursor_x, cursor_y;
extern short color_attribute;

#define TABEXPAND 4

/* attribute byte = 8 bit: 7 -> blink, 654 -> background, 3210 -> foreground */
#define DEFAULT_ATTRIBUTE 0x7 /* gray on black */

#endif
