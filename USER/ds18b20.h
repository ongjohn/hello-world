#ifndef __DS18B20_H
#define __DS18B20_H 
#include "stm32f10x.h"   
 

#define	DS18B20_DQ_OUT_LOW		GPIO_ResetBits(GPIOA,GPIO_Pin_11|GPIO_Pin_12)	 
#define	DS18B20_DQ_OUT_HIGH		GPIO_SetBits(GPIOA,GPIO_Pin_11|GPIO_Pin_12)
#define	DS18B20_DQ_IN			GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)
#define	DS18B20_DQ_IN_CON16		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12)
  
#define	DS18B20_T_0		0
#define	DS18B20_T_1		1			


void DS18B20_Initial(void);
short DS18B20_Get_Temp(u8 num);


#endif















