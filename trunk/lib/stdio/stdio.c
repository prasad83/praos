#include <x86.h>
#include <globals.h>
#include <types.h>
#include <cursor.h>
#include <scroll.h>
#include <echo.h>
#include <stdio.h>

/*
  Each character in the framebuffer takes 2 bytes:
  1) An ASCII value at address N and
  2) An Attribute byte at address N+1
  +-----------------------------------+
  |bit in attribute byte | usage      |
  +-----------------------------------+
  |b0:b3                 | foreground |
  |b4:b6                 | backgournd |
  |b7                    | blink      |
  +----------------------+------------+
*/
static void video_putch(void *video_mem, unsigned ch, unsigned attr) {
  char *vga_fb_addr = (char *) video_mem;
  *vga_fb_addr++ = ch;     /* Put Ascii value at the cursor */
  *vga_fb_addr++ = attr;   /* Change Attribute value behind the cursor */
}

/* Return video memory address at which the cursor is right now present */
static char *get_curvideo_mem() {
  return (char *)vga_framebuffer + (cursor_y*2*SCREEN_WIDTH) + cursor_x*2;
}

void putch(unsigned ch) {
	/* If echo is turned off then return. */
	if(getecho() == 0) return;

  char *vga_fb_addr = get_curvideo_mem();
  int tabs = 0; 
  if(ch == '\n') {
    if(new_curpos(NULL,NULL)){   /* Scroll needed? */
      scrollup();
    }
  }
	else if(ch == '\r') {
		setcurpos(0,-1); /* set the cursor to beginning of current line */
	}
  else if(ch == '\t') {
    /* For now lets replace '\t' with two spaces. */
    for(tabs = 0; tabs < TABEXPAND; ++tabs) {
      if(next_curpos(NULL,NULL)) {    /* Scroll needed? */
        scrollup();
        video_putch(get_curvideo_mem(),' ',color_attribute); /* put ' ' at new cursor location */
      }
      else {
        video_putch(vga_fb_addr, ' ',color_attribute);
      }
    }
  }
  else if(ch == '\b') {
    if(prev_curpos(NULL,NULL)) { /* Scroll needed? */
            ; /* Lets us avoid scrolling up right now */
    }
    else {                             /* Change Ascii Value behind the cursor */
      video_putch(vga_fb_addr-2,' ',color_attribute); /* Change Attribute value behind the cursor */
    }
  }    
  else {
    if(next_curpos(NULL,NULL)) { /* Scroll needed? */
      scrollup();
      video_putch(get_curvideo_mem(),ch,color_attribute);
    }
    else {
      video_putch(vga_fb_addr,ch,color_attribute);
    }
  }
  move();
}  

void strprint(const char *src) {
  while(*src != '\0')
    putch(*src++);
}


