#ifndef __USART1_H
#define	__USART1_H

#include "stm32f10x.h"
#include <stdio.h>

#define	DBG_RS232		1

#define		DBG			1						   
 
#define	RS485_TX3_ENABLE()	GPIO_SetBits(GPIOB,GPIO_Pin_12)	 
#define	RS485_TX3_DISABLE()	GPIO_ResetBits(GPIOB,GPIO_Pin_12)


#define	RS485_TX4_ENABLE()	GPIO_SetBits(GPIOC,GPIO_Pin_13)	 
#define	RS485_TX4_DISABLE()	GPIO_ResetBits(GPIOC,GPIO_Pin_13)		

 

void USART_Initial(void);
void USART_SendByte(USART_TypeDef* USARTx,u16 dat);
void USART_SendNByte(USART_TypeDef* USARTx,u8 *pbuf, u16 len);

int fputc(int ch, FILE *f);
void USART_printf(USART_TypeDef* USARTx, uint8_t *Data,...); 


//#define		DEBUG(x,y,...)	if(DEBUG) USART_Pirntf(x,y,...)  


extern u8 tx_int_flag;
extern u8 rx_flag;

#endif /* __USART1_H */
