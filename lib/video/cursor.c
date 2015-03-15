#include <x86.h>
#include <globals.h>
#include <types.h>
#include <cursor.h>

/* shows the cursor to the position specified by parameter: pcursor_x and pcursor_y */
void showcursor(int pcursor_x, int pcursor_y) {
  unsigned offset = pcursor_y * SCREEN_WIDTH + pcursor_x; /* SCREEN_WIDTH characters per line */
  outportb(crtc_io_addr+0, 14);         /* MSB of offset to CRTC reg 14 */
  outportb(crtc_io_addr+1, offset >> 8);
  outportb(crtc_io_addr+0, 15);         /* LSB of offset to CRTC reg 15 */
  outportb(crtc_io_addr+1, offset);
}

/* moves the cursor using global cursor_x and cursor_y */
void move(void) { showcursor(cursor_x,cursor_y); }

/* moves the cursor to the desired location. */
void move2xy(int x, int y) { 
  if(x != -1) cursor_x = x; 
  if(y != -1) cursor_y = y; 
  move(); 
}

/* set the global cursor_x and cursor_y */
void setcurpos(int positionx,int positiony) { 
  if(positionx != -1) cursor_x = positionx; 
  if(positiony != -1) cursor_y = positiony; 
}

/* Position the cursor in new line.
 *
 * If the parameter passed represents memory (not NULL) 
 * then the global cursor_x and cursor_y is not set but the 
 * cursor position values are set in it.
 * RETURNS: TRUE if scroll is needed else FALSE.
 */
int new_curpos(int *curpos_x,int *curpos_y) {
  int scrollNeeded = FALSE;
  int new_cursor_x = cursor_x;
  int new_cursor_y = cursor_y;

  new_cursor_x = 0;
  ++new_cursor_y;            /* Signal if down scroll is needed. */
  if(new_cursor_y == SCREEN_HEIGHT) {
      scrollNeeded = TRUE;
  }

  if(curpos_x != NULL || curpos_x != NULL) {
    if(curpos_x != NULL) *curpos_x = new_cursor_x;
    if(curpos_y != NULL) *curpos_y = new_cursor_y;
  }
  else {
    cursor_x = new_cursor_x;
    cursor_y = new_cursor_y;
  }
  return scrollNeeded;
}

/* Position the cursor in next position.
 *
 * If the parameter passed represents memory (not NULL) 
 * then the global cursor_x and cursor_y is not set but the 
 * cursor position values are set in it.
 * RETURNS: TRUE if scroll is needed else FALSE.
 */
int next_curpos(int *curpos_x,int *curpos_y) {
  int scrollNeeded = FALSE;
  int new_cursor_x = cursor_x;
  int new_cursor_y = cursor_y;

  ++new_cursor_x;
  if(new_cursor_x == SCREEN_WIDTH) {
    new_cursor_x = 0;
    ++new_cursor_y;          /* Signal if down scroll is needed. */
    if(new_cursor_y == SCREEN_HEIGHT)
      scrollNeeded = TRUE;
  }

  if(curpos_x != NULL || curpos_x != NULL) {
    if(curpos_x != NULL) *curpos_x = new_cursor_x;
    if(curpos_y != NULL) *curpos_y = new_cursor_y;
  }
  else {
    cursor_x = new_cursor_x;
    cursor_y = new_cursor_y;
  }
  return scrollNeeded;
}

/* Position the cursor in previous position.
 *
 * If the parameter passed represents memory (not NULL) 
 * then the global cursor_x and cursor_y is not set but the 
 * cursor position values are set in it.
 * RETURNS: TRUE if scroll is needed else FALSE.
 */
int prev_curpos(int *curpos_x,int *curpos_y) {
  int scrollNeeded = FALSE;
  int new_cursor_x = cursor_x;
  int new_cursor_y = cursor_y;

  --new_cursor_x;
  if(new_cursor_x < 0) {
    new_cursor_x = SCREEN_WIDTH-1;
    --new_cursor_y;          /* Signal if up scroll is needed. */
    if(new_cursor_y < 0)
      scrollNeeded = TRUE;
  }

  if(curpos_x != NULL || curpos_x != NULL) {
    if(curpos_x != NULL) *curpos_x = new_cursor_x;
    if(curpos_y != NULL) *curpos_y = new_cursor_y;
  }
  else {
    cursor_x = new_cursor_x;
    cursor_y = new_cursor_y;
  }
  return scrollNeeded;
}

