#ifndef _UI_H 
#define _UI_H

extern  u8 realError;
extern  u8 level_percent;
extern   char display_tmp[10];
 
void OnKey(unsigned char key);

void ui_flush_idle_display(void);


void DisMenuTopInit(void);
 
#endif
