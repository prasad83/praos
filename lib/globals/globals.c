#include <globals.h>

int SCREEN_WIDTH = 80;
int SCREEN_HEIGHT= 25;
const unsigned vga_framebuffer = 0xB8000;
const unsigned crtc_io_addr = 0x3D4;
                                                                                                               
int cursor_x = 0, cursor_y = 0;
short color_attribute = DEFAULT_ATTRIBUTE;
