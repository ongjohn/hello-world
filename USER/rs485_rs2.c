/*
 *RS2�ӿ�
 *ʹ�ô���3	,band:4800 8 1 N
 * 
 */
#include <string.h>
#include <stdio.h>
#include "rs485_rs2.h"	
#include "storage.h"
#include "usart1.h"	  
#include "gprs.h"	  
#include "rtc.h"    
#include "time_outProcess.h"
#include "time_malloc.h"


u8 rs485_rs2_rxbuf[32];
u8 rs485_rs2_txbuf[32];
u8 rs485_ack_time;//Ӧ��ʱ�䣬��100msΪһ�μ��
u8 rs485_ack_flag;//Ӧ���־
u8 oc_status;
u8 rd_idx;
u8 rs485_ack_flag;
u16 rs485_send_time;

u8 getMediaOutWaterTemper;//�����Ļ���������ȡ���ĳ�ˮˮ���¶�,��������ת�����¶�
u8 getMediaEnvTemper;//�����Ļ���������ȡ���Ļ����¶�,��������ת�����¶�
u8 getMediaWaterHigh;//�����Ļ��������õ���ˮλ�߶�



MEIDA_MSG_T getMediaMSG[15];//���Ļ���+�߿���һ�����Դ�16̨�豸
u8 recordERROR[15];//��¼�����ϱ���־λ

static u8 crc_chksum(u8* dat)
{
	u8 i;
	u8 sum=0;
	
	for(i=1;i<29;i++)//�����뿪ʼ�ۼ�
	{
		sum+=dat[i];
	}	
	sum^=0xFF;
	sum++;

	return (sum&0xff);
}

void rs485_rs2_init(void)
{
    u8 i;

	oc_status=0x02;
	rd_idx=0;
	RS485_RS2_DIS();

	for(i=0;i<15;i++)
	{
		memset(&getMediaMSG[i],0,sizeof(MEIDA_MSG_T));
	}
}

void rs485_rs2_send (void)	
{
	rs485_rs2_txbuf[0]	=0xAA;						//��ʼ��
	rs485_rs2_txbuf[1]  =oc_status;					//������,�Ƿ����Ǹ��ȱû��鿪������ػ�
	rs485_rs2_txbuf[2]	=dargs.rs485_adr1;			//Դ��ַ
	rs485_rs2_txbuf[3]	=0x00;						//Ŀ�ĵ�ַ
	rs485_rs2_txbuf[4]	=0x00;						//��һĿ�ĵ�ַ
	rs485_rs2_txbuf[5]	=dargs.setMideaDegree0;		//��ˮ�¶����� 0-250��	  				*
	rs485_rs2_txbuf[6]	=dargs.setMideaLoopDegree;	//ѭ����ˮ�¶����� 1-250��				*
	rs485_rs2_txbuf[7]	=dargs.setMideaWaterLevel;	//ˮλ����,0-S1(4��) 1-S2(3��) 2-S3(1��)*
	rs485_rs2_txbuf[8]	=0;							//��ʱ��Ϣ 0-������ʱ��Ϣ 1-����ʱ��Ϣ
	rs485_rs2_txbuf[9]	=dargs.setMideaIfLoop;		//����ѭ�� 1-�� 0-��					*
	rs485_rs2_txbuf[10]	=dargs.setMideaWaterOffset;	//ˮλƫ�� 0-250						*
	rs485_rs2_txbuf[11]	=dargs.setMideaIfHot;		//���õ���ȸ�������	 				*	
	rs485_rs2_txbuf[12]	=dargs.setMideaHandOpenNumbers;			//�����ֶ���������̨��	 				*
	rs485_rs2_txbuf[13]	=0;							//�߿���״̬�ı�     
	rs485_rs2_txbuf[14]	=0;							//�߿���Ϊ1��ʱ��ֵ	
	rs485_rs2_txbuf[19]	=0;							//< ��ů�����¶�0-250�� >
	rs485_rs2_txbuf[20]	=0;							//< ��ů�ź� 1-�� 0-��  >
	rs485_rs2_txbuf[30] =crc_chksum(rs485_rs2_txbuf);
	rs485_rs2_txbuf[31]	=0x55;
	
	RS485_RS2_EN();
	USART_SendNByte(USART3,rs485_rs2_txbuf,32);
	RS485_RS2_DIS();
}


/*
 * ��¼���ϵ�������	, �й��Ͼ��ϱ�
 */ 
void copy_data(u8 index)
{
 
	if(index>15) return;

	getMediaMSG[index].board_outwater_t=rs485_rs2_rxbuf[RS485_DAT+5-1]/2-15;	//���ˮ�¶�
	getMediaMSG[index].err_code[0]=rs485_rs2_rxbuf[RS485_DAT+7-1]; 		//���ϴ���
	getMediaMSG[index].err_code[1]=rs485_rs2_rxbuf[RS485_DAT+8-1];		//���ϴ���
	getMediaMSG[index].protect_code[0]=rs485_rs2_rxbuf[RS485_DAT+9-1];	//�������� 
	getMediaMSG[index].protect_code[1]=rs485_rs2_rxbuf[RS485_DAT+10-1];	//�������� 
	getMediaMSG[index].cool_t[0]=rs485_rs2_rxbuf[RS485_DAT+11-1]/2-15;		 //�������¶�
	getMediaMSG[index].cool_t[1]=rs485_rs2_rxbuf[RS485_DAT+12-1]/2-15;		 //�������¶�
	getMediaMSG[index].compress_c[0]=rs485_rs2_rxbuf[RS485_DAT+13-1];	    //ѹ��������
	getMediaMSG[index].compress_c[1]=rs485_rs2_rxbuf[RS485_DAT+14-1];	    //ѹ��������
	getMediaMSG[index].output_state=rs485_rs2_rxbuf[RS485_DAT+17-1];	    //���״̬
	getMediaMSG[index].water_t=rs485_rs2_rxbuf[RS485_DAT+19-1];				//ˮ���¶�
	getMediaMSG[index].outdoor_t=rs485_rs2_rxbuf[RS485_DAT+20-1];		    //�����¶�
	getMediaMSG[index].online_m=rs485_rs2_rxbuf[RS485_DAT+21-1];		    //���߻�����
	getMediaMSG[index].run_m=rs485_rs2_rxbuf[RS485_DAT+22-1];  			//���л�����
	getMediaMSG[index].water_level=	rs485_rs2_rxbuf[RS485_DAT+24-1];	    //ˮλ�߶�
	getMediaMSG[index].outwater_t=rs485_rs2_rxbuf[RS485_DAT+25-1]; 		//��ˮ�¶�����
	getMediaMSG[index].water_offset=rs485_rs2_rxbuf[RS485_DAT+26-1];	    //ˮλƫ��
												 
	if(g_sAppContext.eGPRSState!=eGPRS_ONLINE) return;//δ����,���ϱ�


//===================== E_xx���ϴ����ϱ� ==========================//
	if( (recordERROR[index]&ERR0_MASK)==0x00)//����Ƿ��й���
	{
		if((getMediaMSG[index].err_code[0]&ERR0_DET)==ERR0_DET)//E0����
		{
			GPRS_SendErrorCode(index,ERR_0);
 			recordERROR[index]|=ERR0_MASK;
		}	
	}
 	else if( (recordERROR[index]&ERR0_MASK)==ERR0_MASK)//�������Ƿ�ָ�
 	{
		if((getMediaMSG[index].err_code[0]&ERR0_DET)==0)//
		{
			GPRS_SendErrorCode(index,0);
 			recordERROR[index]&=~ERR0_MASK;
		}
	}
																	   
	if( (recordERROR[index]&ERR1_MASK)==0x00)
	{
		if((getMediaMSG[index].err_code[0]&ERR1_DET)==ERR1_DET)//E1����
		{
			GPRS_SendErrorCode(index,ERR_1);
			recordERROR[index]|=ERR1_MASK;
		}	
	}																		    
	else if( (recordERROR[index]&ERR1_MASK)==ERR1_MASK)
	{
		if((getMediaMSG[index].err_code[0]&ERR1_DET)==0x00)//
		{

			GPRS_SendErrorCode(index,0);
			recordERROR[index]&=~ERR1_MASK;
		}
	}
																	 
	if( (recordERROR[index]&ERR2_MASK)==0x00)
	{
		if((getMediaMSG[index].err_code[0]&ERR2_DET)==ERR2_DET)//E2����
		{

			GPRS_SendErrorCode(index,ERR_2);
			recordERROR[index]|=ERR2_MASK;
		}	
	}																			 
	else if( (recordERROR[index]&ERR2_MASK)==ERR2_MASK)
	{
		if((getMediaMSG[index].err_code[0]&ERR2_DET)==0x00)//
		{
		
			GPRS_SendErrorCode(index,0);
			recordERROR[index]&=~ERR2_MASK;
		}
	}
																	 
	if( (recordERROR[index]&ERR3_MASK)==0x00)
	{
		if((getMediaMSG[index].err_code[0]&ERR3_DET)==ERR3_DET)//E3����
		{

			GPRS_SendErrorCode(index,ERR_3);

			recordERROR[index]|=ERR3_MASK;
		}	
	}																		    
	else if( (recordERROR[index]&ERR3_MASK)==ERR3_MASK)
	{
		if((getMediaMSG[index].err_code[0]&ERR3_DET)==0x00)//
		{
		
			GPRS_SendErrorCode(index,0);
			recordERROR[index]&=~ERR3_MASK;
		}
	}
																	 
	if( (recordERROR[index]&ERR4_MASK)==0x00)
	{
		if((getMediaMSG[index].err_code[0]&ERR4_DET)==ERR4_DET)//E4����
		{
			
			GPRS_SendErrorCode(index,ERR_4);
			recordERROR[index]|=ERR4_MASK;
		}	
	}																			 
	else if( (recordERROR[index]&ERR4_MASK)==ERR4_MASK)
	{
		if((getMediaMSG[index].err_code[0]&ERR4_DET)==0x00)//
		{
			
			GPRS_SendErrorCode(index,0);
			recordERROR[index]&=~ERR4_MASK;
		}
	}
																	 
	if( (recordERROR[index]&ERR5_MASK)==0x00)
	{
		if((getMediaMSG[index].err_code[0]&ERR5_DET)==ERR5_DET)//e5����
		{
			
			GPRS_SendErrorCode(index,ERR_5);
			recordERROR[index]|=ERR5_MASK;
		}	
	}																	   
	else if( (recordERROR[index]&ERR5_MASK)==ERR5_MASK)
	{
		if((getMediaMSG[index].err_code[0]&ERR5_DET)==0x00)//
		{
			
			GPRS_SendErrorCode(index,0);
			recordERROR[index]&=~ERR5_MASK;
		}
	}

																   
	if( (recordERROR[index]&ERR6_MASK)==0x00)
	{
		if((getMediaMSG[index].err_code[0]&ERR6_DET)==ERR6_DET)//E0����
		{
			
			GPRS_SendErrorCode(index,ERR_6);
			recordERROR[index]|=ERR6_MASK;
		}					    
	}																		    
	else if( (recordERROR[index]&ERR6_MASK)==ERR6_MASK )
	{
		if((getMediaMSG[index].err_code[0]&ERR6_DET)==0x00)//
		{
		
			GPRS_SendErrorCode(index,0);
			recordERROR[index]&=~ERR6_MASK;
 		}
	}
 
 	if( (recordERROR[index]&ERR7_MASK)==0x00)
	{
		if((getMediaMSG[index].err_code[0]&ERR7_DET)==ERR7_DET)//E7����
		{
			
			GPRS_SendErrorCode(index,ERR_7);
			recordERROR[index]|=ERR7_MASK;
		}	
	}																			 
	else if( (recordERROR[index]&ERR7_MASK)==ERR7_MASK)
	{
		if((getMediaMSG[index].err_code[0]&ERR7_DET)==0x00)//
		{
		
			GPRS_SendErrorCode(index,0);
			recordERROR[index]&=~ERR7_MASK;
		}
	}

																	  
	if( (recordERROR[index]&ERR8_MASK)==0x00)
	{
		if((getMediaMSG[index].err_code[1]&ERR8_DET)==ERR8_DET)//E8����
		{
			
			GPRS_SendErrorCode(index,ERR_8);
			recordERROR[index]|=ERR8_MASK;
		}	
	}																			   
	else if( (recordERROR[index]&ERR8_MASK)==ERR8_MASK)
	{
		if((getMediaMSG[index].err_code[1]&ERR8_DET)==0x00)//
		{
			
			GPRS_SendErrorCode(index,0);
			recordERROR[index]&=~ERR8_MASK;
		}
	}

//====================== P_xx���������ϱ� ==========================//
	if( (recordERROR[index]&PRO0_MASK)==0x00)
	{
		 if((getMediaMSG[index].protect_code[0]&PRO0_DET)==PRO0_DET)//˵���й���,�����ϱ�
		 {
			GPRS_SendErrorCode(index,PRO_0);
			recordERROR[index]|=PRO0_MASK;
		 }
	}																	 
	else if( (recordERROR[index]&PRO0_MASK)==PRO0_MASK)//�ȴ��ָ�
	{
		if((getMediaMSG[index].protect_code[0]&PRO0_DET)==0x00)
		{		
		 	GPRS_SendErrorCode(index,0);
			recordERROR[index]&=~PRO0_MASK;	
		}	
	}
																  
	if( (recordERROR[index]&PRO1_MASK)==0x00)
	{
		 if((getMediaMSG[index].protect_code[0]&PRO1_DET)==PRO1_DET)//˵���й���,�����ϱ�
		 {
		 	GPRS_SendErrorCode(index,PRO_1);
			recordERROR[index]|=PRO1_MASK;
		 }
	}																    
	else if( (recordERROR[index]&PRO1_MASK)==PRO1_MASK)
	{
		if((getMediaMSG[index].protect_code[0]&PRO1_DET)==0x00)
		{		
		 	GPRS_SendErrorCode(index,0);
			recordERROR[index]&=~PRO1_MASK;	
		}	
	}								  
															    
	if( (recordERROR[index]&PRO2_MASK)==0x00)
	{
		 if((getMediaMSG[index].protect_code[0]&PRO2_DET)==PRO2_DET)//˵���й���,�����ϱ�
		 {
		 	GPRS_SendErrorCode(index,PRO_2);
			recordERROR[index]|=PRO2_MASK;
		 }
	}																	  
	else if( (recordERROR[index]&PRO2_MASK)==PRO2_MASK)
	{
		if((getMediaMSG[index].protect_code[0]&PRO2_DET)==0x00)
		{		
		 	GPRS_SendErrorCode(index,0);
			recordERROR[index]&=~PRO2_MASK;	
		}	
	}
 																  
	if( (recordERROR[index]&PRO3_MASK)==0x00)
	{
		 if((getMediaMSG[index].protect_code[0]&PRO3_DET)==PRO3_DET)//˵���й���,�����ϱ�
		 {
		 	GPRS_SendErrorCode(index,PRO_3);
			recordERROR[index]|=PRO3_MASK;
		 }
	}																	   
	else if( (recordERROR[index]&PRO3_MASK)==PRO3_MASK)
	{
		if((getMediaMSG[index].protect_code[0]&PRO3_DET)==0x00)
		{		
		 	GPRS_SendErrorCode(index,0);
			recordERROR[index]&=~PRO3_MASK;	
		}	
	}
																   
	if( (recordERROR[index]&PRO4_MASK)==0x00)
	{
		 if((getMediaMSG[index].protect_code[0]&PRO4_DET)==PRO4_DET)//˵���й���,�����ϱ�
		 {
		 	GPRS_SendErrorCode(index,PRO_4);
			recordERROR[index]|=PRO4_MASK;
		 }
	}																	 
	else if( (recordERROR[index]&PRO4_MASK)==PRO4_MASK)
	{
		if((getMediaMSG[index].protect_code[0]&PRO4_DET)==0x00)
		{		
		 	GPRS_SendErrorCode(index,0);
			recordERROR[index]&=~PRO4_MASK;	
		}	
	}
															    
	if( (recordERROR[index]&PRO5_MASK)==0x00)
	{
		 if((getMediaMSG[index].protect_code[0]&PRO5_DET)==PRO5_DET)//˵���й���,�����ϱ�
		 {
		 	GPRS_SendErrorCode(index,PRO_5);
			recordERROR[index]|=PRO5_MASK;
		 }
	}																		  
	else if( (recordERROR[index]&PRO5_MASK)==PRO5_MASK)
	{
		if((getMediaMSG[index].protect_code[0]&PRO5_DET)==0x00)
		{		
		 	GPRS_SendErrorCode(index,0);
			recordERROR[index]&=~PRO5_MASK;	
		}	
	}
															    
	if( (recordERROR[index]&PRO6_MASK)==0x00)
	{
		 if((getMediaMSG[index].protect_code[0]&PRO6_DET)==PRO6_DET)//˵���й���,�����ϱ�
		 {
		 	GPRS_SendErrorCode(index,PRO_6);
			recordERROR[index]|=PRO6_MASK;

		 }
	}																	  
	else if( (recordERROR[index]&PRO6_MASK)==PRO6_MASK)
	{
		if((getMediaMSG[index].protect_code[0]&PRO6_DET)==0x00)
		{		
		 	GPRS_SendErrorCode(index,0);
			recordERROR[index]&=~PRO6_MASK;	
		}	
	}
																 
	if( (recordERROR[index]&PRO7_MASK)==0x00)
	{
		 if((getMediaMSG[index].protect_code[0]&PRO7_DET)==PRO7_DET)//˵���й���,�����ϱ�
		 {
		 	GPRS_SendErrorCode(index,PRO_7);
			recordERROR[index]|=PRO7_MASK;
		 }
	}																	  
	else if( (recordERROR[index]&PRO7_MASK)==PRO7_MASK)
	{
		if((getMediaMSG[index].protect_code[0]&PRO7_DET)==0x00)
		{		
		 	GPRS_SendErrorCode(index,0);
			recordERROR[index]&=~PRO7_MASK;	
		}	
	}
																 
	if( (recordERROR[index]&PRO8_MASK)==0x00)
	{
		 if((getMediaMSG[index].protect_code[1]&PRO8_DET)==PRO8_DET)//˵���й���,�����ϱ�
		 {
		 	GPRS_SendErrorCode(index,PRO_8);
			recordERROR[index]|=PRO8_MASK;
		 }
	}																		 
	else if( (recordERROR[index]&PRO8_MASK)==PRO8_MASK)
	{
		if((getMediaMSG[index].protect_code[1]&PRO8_DET)==0x00)
		{		
		 	GPRS_SendErrorCode(index,0);
			recordERROR[index]&=~PRO8_MASK;	
		}	
	}

																  
	if( (recordERROR[index]&PRO9_MASK)==0x00)
	{
		 if((getMediaMSG[index].protect_code[1]&PRO9_DET)==PRO9_DET)//˵���й���,�����ϱ�
		 {
		 	GPRS_SendErrorCode(index,PRO_9);
			recordERROR[index]|=PRO9_MASK;
		 }
	}																		 
	else if( (recordERROR[index]&PRO9_MASK)==PRO9_MASK)
	{
		if((getMediaMSG[index].protect_code[1]&PRO9_DET)==0x00)
		{		
		 	GPRS_SendErrorCode(index,0);
			recordERROR[index]&=~PRO9_MASK;	
		}	
	}
}


void RS485rs2Proc(void)
{
	u8 at;//Ӧ��ʱ��

 
	//�ж��Ƿ�ʱ����
	if(dargs.setIfAutoOnOff==1)//�����˶�ʱ�������ػ�����
	{		
		if( (timer.hour==dargs.setOnTime[0]) && (timer.min==dargs.setOnTime[1]) )//����
		{
			oc_status=0x02;
		}   		

		if( (timer.hour==dargs.setOffTime[0]) && (timer.min==dargs.setOffTime[1]) )//�ػ�
		{
			oc_status=0x08;
		}
	}
	//485������������
 	if(rd_idx>=32)
	{
		rd_idx=0;

		if(rs485_rs2_rxbuf[0]!=0xAA)//ͷ��Ϣ����
		{
			goto err_end;
		}
 
		if(rs485_rs2_rxbuf[31]!=0x55)//β��Ϣ����
		{

			goto err_end;
		}

	   if( (rs485_rs2_rxbuf[2]==0x00) && (rs485_rs2_rxbuf[3]==0xFF) )//�����㲥��Ϣ,Ӧ�� 
	   { 
			getMediaOutWaterTemper=(rs485_rs2_rxbuf[RS485_DAT+OFFSET_T5]/2)-15;//ȡˮ���¶�,����0.5
			getMediaEnvTemper=(rs485_rs2_rxbuf[RS485_DAT+OFFSET_T4]/2)-15;//ȡ�����¶�
			getMediaWaterHigh=rs485_rs2_rxbuf[RS485_DAT+OFFSET_WATER]; //ˮλ�߶�
	
			copy_data(0);
	   		if(dargs.rs485_adr1==0) //������
			{	
			}
	   		else//���඼�Ǵӻ� ,Ӧ���źţ������߿���
			{
				at=(dargs.rs485_adr1&0x0F);//1-15��ַ
				if(dargs.rs485_adr1>0x0F)//���ڵ�ַ15
				{
					at=16;
				}
				TimerStart(TT_RS485_RS2_ACK,at*100);//����Ӧ����ʱ�䣬���100msӦ����� 	

 			}
	   }
		   
	   if( (rs485_rs2_rxbuf[3]==0x00)&& (dargs.rs485_adr1==0xFF) )//��ͨ�ӻ���Ϣ,���������ӻ�����Ϣת�浽cache�洢
	   {
 	   	  copy_data(rs485_rs2_rxbuf[2]);	
	   }
err_end:
	   memset(&rs485_rs2_rxbuf[0],0,32);   
	}

	if(rs485_ack_flag)//��ҪӦ��
	{
		
		rs485_ack_flag=0;

		if(dargs.rs485_adr1==0xFF) //�߿���Ӧ��
		{
		 	rs485_rs2_send();
		}
		else//�����ȱôӻ�Ӧ��
		{
		  //�Ժ����
		}
 	}
}
