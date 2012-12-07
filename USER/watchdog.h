	  #ifndef __WDT_H
#define __WDT_H	    
 
#include "stm32f10x.h"



void WWDG_Init(u8 tr,u8 wr,u32 fprer);
void WWDG_NVIC_Init(void);
void IWDG_Feed(void);			   


#endif
