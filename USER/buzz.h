#ifndef	BUZZ_H
#define	BUZZ_H
#include "stm32f10x.h"



#define	BUZZ_ON()		GPIO_ResetBits(GPIOA,GPIO_Pin_8)	 
#define	BUZZ_OFF()		GPIO_SetBits(GPIOA,GPIO_Pin_8)


void BUZZ_Initial(void);
void BUZZ_Op(u8 ct,u8 type);

#endif
