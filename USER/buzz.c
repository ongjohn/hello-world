#include "buzz.h"
#include "delay.h"
#include "time_malloc.h"


 


void BUZZ_Initial(void)
{	
 	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
 
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;      
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	BUZZ_OFF();
}

void BUZZ_Op(u8 ct,u8 type)
{
	u16	i;
	u16 cts=ct*2;
	
	for(i=0;i<cts;i++)
	{
		if((i%2)==0)
		{
			BUZZ_ON();
		}
		else
		{
			BUZZ_OFF();	
		}
		
		switch(type)
		{
			case 0:
				delay_ms(100);
				break;
			case 1:
				delay_ms(300);
				break;
		}		
	}

 	
	BUZZ_OFF();		
}




