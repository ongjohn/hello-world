#include "led.h"

void LED_Initial(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);  
	GPIO_ResetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
}


//void LED_Operate(u8 num, u8 on)
//{
//	u16 GPIO_Pin_num;
//
//	switch(num)
//	{
//		case 1:
//			GPIO_Pin_num=GPIO_Pin_13;
//			break;
//		case 2:
//			GPIO_Pin_num=GPIO_Pin_14;
//			break;
//		case 3:
//			GPIO_Pin_num=GPIO_Pin_15;
//			break;
//	}
//
//	if(on&1)
//	{
//		GPIO_SetBits(GPIOB,GPIO_Pin_num);
//	}
//	else
//	{
//		GPIO_ResetBits(GPIOB,GPIO_Pin_num);
//	}
//}
