#include <globals.h>
#include <cursor.h>
#include <stdio.h>
#include <stdarg.h>
#include <conio.h>

void clrscr(void) {
  unsigned pixels = SCREEN_WIDTH * SCREEN_HEIGHT;
  
  char *vga_fb_addr = (char *) vga_framebuffer + 0;

  while(pixels > 0) {
    *vga_fb_addr = ' ';
    vga_fb_addr++;
    vga_fb_addr++;
    pixels -= 1;
  }
  cursor_x = cursor_y = 0;
  move();
}  

unsigned getcolor(short foreground,short background) {
  unsigned color = 0x0;
  /* 0x[1->blink_bit 3->background_bits 4->foreground_bits] */
  
  /* 0x70 - (0x7-backround)*0x10: will shift the 3 bits from position 012 to 456
   * | foreground will set the 4 bits from 0123
   * so finally color will be: (456)->background and (3210)->foreground.
   */

  color = (0x70 - (0x7 - background) * (0x10)) | foreground;
  return color;
}  

void cprintf(unsigned color, const char *fmt, ...) {
  color_attribute = color;
	
	va_list args;
	va_start(args,fmt);
  parse_printf(fmt,args);
	va_end(args)

  color_attribute = DEFAULT_ATTRIBUTE;
}  

void blink_cursor(void) {
	char *vga_fb_addr = (char *) vga_framebuffer +  (cursor_y*2*SCREEN_WIDTH) + cursor_x*2;
	*(vga_fb_addr++);
}
