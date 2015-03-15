#include <globals.h>
#include <cursor.h>
#include <string.h> /* for memcpy(),memsetw() */
#include <scroll.h>

void scrollup(void) {
  unsigned short *vga_fb_addr = (unsigned short*) vga_framebuffer;
	int char_plus_attr = ' ' | (color_attribute << 8); /* character and attribute */
  
	/* Move SECOND_LINE to LAST_LINE data to FIRST_LINE to LAST_BUT_ONE_LINE
   * Note: You have to move both attributes and character, memcpy will treat pointers
   * in terms of (char *) so send suitable count at last. */
  memcpy(vga_fb_addr,vga_fb_addr+SCREEN_WIDTH, (SCREEN_HEIGHT-1)*SCREEN_WIDTH*2);
 
	/* Clear the last line.
   * Note: memsetw will treat pointers at word level (unsigned short *) 
   * so send both character and its attribute as value. */
	memsetw(vga_fb_addr+(SCREEN_HEIGHT-1)*SCREEN_WIDTH,char_plus_attr,SCREEN_WIDTH);

  cursor_x = 0;
  if(++cursor_y >= SCREEN_HEIGHT) cursor_y = SCREEN_HEIGHT-1;
  move();
}

  
