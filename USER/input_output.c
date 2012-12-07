/*
 * PC7 - 74HC165_CP
 * PC8 - 74HC165_PL
 * PC9 - 74HC165_Q7
 */
#include "input_output.h"
#include "delay.h"

#define	MAX_INPUT_SIGNAL_NUMS	24


void Input_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
 
 	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_9;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;       
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
}

void Output_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
 
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_15;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;      
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	CONTROL_12V_PA0_OUT_DIS;
	CONTROL_12V_PA1_OUT_DIS;
	CONTROL_12V_PA15_OUT_DIS;	
}


void IO_Initial(void)
{	
	Input_Config();
	Output_Config();	
}


u32 Get_Input(void)
{
	u8 i;
	u32 dat=0;
	
	HC165_PL_LOW();
	delay_us(2);
	HC165_PL_HIGH();
	
	for(i=0;i<MAX_INPUT_SIGNAL_NUMS;i++)           
	{							  
	    HC165_CLK_LOW();
	    dat >>= 1;
	    
	    if(HC165_DAT_IN()==Bit_SET)
		      dat |= 0x800000;
	    else
		      dat &= 0x7fffff;
	    
	   HC165_CLK_HIGH();
		
	}
	
	return dat;	
}
