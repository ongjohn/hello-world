/*
 *RS3�ӿ�
 *ʹ�ô���4
 * 
 */
#include <string.h>
#include "rs485_rs3.h"	
#include "usart1.h"	  	
#include "storage.h" 	
#include "input_output.h"  
#include "delay.h"



u8 ct_readDate;
u8 rs485_rs3_rxIndex;
u8 rs485_rs3_rxbuf[100];
u8 rs485_rs3_txbuf[100];

u8 bcd_to_hex(u8 val)
{
  return ((val) & 0x0f) + ((val) >> 4) * 10;
}
//֡��ʼ��У����������ݵ��ۼ�
static u8 crc(u8* dat,u8 len)
{
	u8 i;
	u8 sum=0;
	
	for(i=0;i<len;i++)//��ʼ֡��ʼ�ۼ�
	{
		sum+=dat[i];
	}	

	return (sum&0xff);
}


void rs485_rs3_init(void)
{
	memset(rs485_rs3_rxbuf,0,sizeof(rs485_rs3_rxbuf));
 	RS485_RS3_DIS();
	rs485_rs3_GetWaterData(dargs.rs485InWaterAddr);
	rs485_rs3_GetWaterData(dargs.rs485OutWaterAddr);
	rs485_rs3_GetWaterData(dargs.rs485LoopWaterAddr);
    rs485_rs3_GetWaterData(dargs.rs485ElecAddr);

}





void rs485_rs3_send (u8 controlCode,u8 typeCode,u8* dataArea,u8 len,u32 addr)
{
	CONTROL_12V_PA15_OUT_EN;//������Դ
	Delay(1000); Delay(1000);
 	rs485_rs3_txbuf[FRAME_NULL+0] =0xFE;//�հ� 
	rs485_rs3_txbuf[FRAME_NULL+1] =0xFE;//�հ�  
	rs485_rs3_txbuf[FRAME_NULL+2] =0xFE;//�հ�  

	rs485_rs3_txbuf[FRAME_START] =0x68;	//��ʼ֡					 
	rs485_rs3_txbuf[FRAME_TYPE]  =typeCode;							 
	rs485_rs3_txbuf[FRAME_ADDR+0] =addr&0xFF;//��λ��ǰ,7���ֽ�				 
	rs485_rs3_txbuf[FRAME_ADDR+1]  =(addr>>8)&0xFF; 			 
	rs485_rs3_txbuf[FRAME_ADDR+2]  =(addr>>16)&0xFF; 			 
	rs485_rs3_txbuf[FRAME_ADDR+3]  =(addr>>24)&0xFF; 			 
	rs485_rs3_txbuf[FRAME_ADDR+4]  =0; 			 
	rs485_rs3_txbuf[FRAME_ADDR+5]  =0; 		
	rs485_rs3_txbuf[FRAME_ADDR+6]  =0; 		
	rs485_rs3_txbuf[FRAME_CTRL]    =controlCode;//������  
 	rs485_rs3_txbuf[FRAME_DLEN]    =len;//�����򳤶�
    
    memcpy(&rs485_rs3_txbuf[FRAME_DATA],dataArea,len);	//�������ݵ�������
	rs485_rs3_txbuf[FRAME_CRC]	=crc(&rs485_rs3_txbuf[FRAME_START],len+11);//crc
	rs485_rs3_txbuf[FRAME_END]  =0x16;//��������

	RS485_RS3_EN();//��������
	USART_SendNByte(UART4,rs485_rs3_txbuf,FRAME_END+1);
	RS485_RS3_DIS();//ֹͣ
 	
}	

void rs485_rs3_GetWaterData(u32 inputAddr)
{
	u8 buffer[3];

	buffer[0]=0x90;
	buffer[1]=0x1F;
	buffer[2]=0x00;
	rs485_rs3_send(CTR_0,TYPE_0,buffer,3,inputAddr);
	delay_ms(500);			 
}

void RS485rs3Proc(void)
{
	u8 rxcheckSum;
	u8 rxLen;
	u8 rxCode;
	u8 rxEndFrame;
	u8 res;
	u8 data[4];
	u32 rxAddr;

//��Ϣͷ�д���
	if((rs485_rs3_rxbuf[FRAME_NULL+0]!=0xFE) ) 
	{
		rs485_rs3_rxIndex=0;
		return;
	}
//��ʼ������	
	if(rs485_rs3_txbuf[FRAME_START]!=0x68)
	{
		rs485_rs3_rxIndex=0;
		return;
	}  

	rxLen=rs485_rs3_rxbuf[FRAME_DLEN]; //ת����յ��ĳ���

 //���ȹ���
 	if(rxLen>100)
	{
		rs485_rs3_rxIndex=0;
		return;		
	}

	rxcheckSum=crc(&rs485_rs3_rxbuf[FRAME_START],rxLen+11);//������յ�У���
//У��ʹ��� 
	if(rxcheckSum!=rs485_rs3_rxbuf[FRAME_RX_CRC])
	{
		rs485_rs3_rxIndex=0;
		return;	
	}
//����������
	rxEndFrame=	rs485_rs3_rxbuf[FRAME_RX_END];
	if(rxEndFrame!=0x16) 
	{
		rs485_rs3_rxIndex=0;
		return;	
	}


//�ж����ĸ���ĵ�ַ
	rxAddr= (rs485_rs3_rxbuf[FRAME_ADDR+0]) | (rs485_rs3_rxbuf[FRAME_ADDR+1]<<8) |	(rs485_rs3_rxbuf[FRAME_ADDR+2]<<16) | (rs485_rs3_rxbuf[FRAME_ADDR+3]<<24);

	if(rxAddr==dargs.rs485InWaterAddr)
	{
		res=1;
	}
	else if(rxAddr==dargs.rs485OutWaterAddr)
	{
		res=2;
	}
	else if(rxAddr==dargs.rs485LoopWaterAddr)
	{
		res=3;
	}
	else if(rxAddr==dargs.rs485ElecAddr)
	{
		res=4;
	}
	else
	{
		rs485_rs3_rxIndex=0;
		return;			
	}
//��ȷ
	rxCode=rs485_rs3_rxbuf[FRAME_CTRL];
	switch(rxCode)
	{
		case 0x81://��������
			data[0]=bcd_to_hex(rs485_rs3_rxbuf[FRAME_DLEN+3]);
			data[1]=bcd_to_hex(rs485_rs3_rxbuf[FRAME_DLEN+4]);
			data[2]=bcd_to_hex(rs485_rs3_rxbuf[FRAME_DLEN+5]);
			data[3]=bcd_to_hex(rs485_rs3_rxbuf[FRAME_DLEN+6]);
			if(res==1)
			{
 				in_water=data[0]+data[1]*100+data[2]*10000+data[3]*1000000; 
			}
			else if(res==2)
			{
			
				out_water=data[0]+data[1]*100+data[2]*10000+data[3]*1000000; 
			}
			else if(res==3)
			{
			
				loop_water=data[0]+data[1]*100+data[2]*10000+data[3]*1000000; 
			}
			else if(res==4)
			{
			
				elec=data[0]+data[1]*100+data[2]*10000+data[3]*1000000; 
			}
			break;
	} 

//�������
 	CONTROL_12V_PA15_OUT_DIS;
	rs485_rs3_rxIndex=0;
	memset(rs485_rs3_rxbuf,0,sizeof(rs485_rs3_rxbuf)); 		
} 

