#ifndef __PRA_CURSOR_H__
#define __PRA_CURSOR_H__

void showcursor(int pcursor_x, int pcursor_y);
void move(void);
void move2xy(int x,int y);
void setcurpos(int positionx,int positiony);
int new_curpos(int *curpos_x,int *curpos_y);
int next_curpos(int *curpos_x,int *curpos_y);
int prev_curpos(int *curpos_x,int *curpos_y);

#endif
