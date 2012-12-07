#ifndef __RS485_RS3_H
#define __RS485_RS3_H 			   
#include <stm32f10x.h>
 


#define RS485_RS3_EN()	 	GPIO_SetBits(GPIOC,GPIO_Pin_13)
#define	RS485_RS3_DIS()		GPIO_ResetBits(GPIOC,GPIO_Pin_13)
 
 

#define FRAME_NULL		0
#define FRAME_START		3
#define FRAME_TYPE		4
#define FRAME_ADDR		5
#define FRAME_CTRL		12
#define FRAME_DLEN		13	
#define FRAME_DATA		14
#define	FRAME_CRC		rs485_rs3_txbuf[FRAME_DLEN]+11+3	
#define	FRAME_END		rs485_rs3_txbuf[FRAME_DLEN]+12+3

#define	FRAME_RX_CRC		rs485_rs3_rxbuf[FRAME_DLEN]+11+3	
#define	FRAME_RX_END		rs485_rs3_rxbuf[FRAME_DLEN]+12+3



//�������
#define TYPE_0			0x10//��ˮ��
#define TYPE_1			0x30//ȼ���� 
#define TYPE_2			0x40//���



//����������
#define CTR_0			0x01//���������
#define CTR_1			0x16//���ñ�Ƶ���
#define CTR_2			0x03//����Ƶ�ַ  
#define CTR_3			0x15//���ñ�Ƶ�ַ	
#define CTR_4			0x2A//д���ſ���

extern u8 ct_readDate;
extern u8 rs485_rs3_rxIndex;
extern u8 rs485_rs3_rxbuf[100];
extern u8 rs485_rs3_txbuf[100];

extern u8 bcd_to_hex(u8 val);
extern void rs485_rs3_GetWaterData(u32 inputAddr);
extern void rs485_rs3_init(void);
extern void rs485_rs3_send (u8 controlCode,u8 typeCode,u8* dataArea,u8 len,u32 addr); 
extern void RS485rs3Proc(void);
 		
#endif
