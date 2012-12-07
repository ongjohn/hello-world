

#ifndef	INPUT_OUTPUT_H
#define	INPUT_OUTPUT_H
#include "stm32f10x.h"

#define	CONTROL_12V_PA0_OUT_EN			GPIO_ResetBits(GPIOA,GPIO_Pin_0)
#define	CONTROL_12V_PA1_OUT_EN			GPIO_ResetBits(GPIOA,GPIO_Pin_1)
#define	CONTROL_12V_PA15_OUT_EN			GPIO_ResetBits(GPIOA,GPIO_Pin_15)


#define	CONTROL_12V_PA0_OUT_DIS			GPIO_SetBits(GPIOA,GPIO_Pin_0)
#define	CONTROL_12V_PA1_OUT_DIS			GPIO_SetBits(GPIOA,GPIO_Pin_1)
#define	CONTROL_12V_PA15_OUT_DIS		GPIO_SetBits(GPIOA,GPIO_Pin_15)	



#define	HC165_PL_HIGH()			GPIO_SetBits(GPIOC,GPIO_Pin_8)
#define	HC165_PL_LOW()			GPIO_ResetBits(GPIOC,GPIO_Pin_8)
#define	HC165_CLK_HIGH()		GPIO_SetBits(GPIOC,GPIO_Pin_7)
#define	HC165_CLK_LOW()			GPIO_ResetBits(GPIOC,GPIO_Pin_7)
#define	HC165_DAT_IN()			GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)   	

void IO_Initial(void);
u32 Get_Input(void);


#endif
