#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "gprs.h"
#include "delay.h"
#include "usart1.h"	
#include "time_malloc.h" 
#include "time_outProcess.h"
#include "storage.h"
#include "rtc.h"	  
#include "rs485_rs2.h"

u8 SendCt;
u8 recv_flag;

S_ATback g_sATback={0,0,"AT"};
S_AppContext g_sAppContext={{eGPRS_OFFLINE},{eInitStart},{eSimCardNotReady}};
MessageInfo g_sMessageInfo={"15013657293","#000#000#0000#","gsm ok"};  //����һ�����Žṹ�� 
//S_ConfigSetting g_setting={"CMNET","114.80.203.14","9090"};
S_ConfigSetting g_setting={"CMNET","219.134.171.108","9595"};
u8 send_st;
u8 strBuffer[512]={0};  
u8 TxBuffer[512]={0};   
u8 RxBuffer[512]={0}; 
u8 TxCache[128];
u8 countbuf[2];

u8 CipsendLen;

u8 TcpSendCt;//tcp���߷�������������
u8 TcpConnectCt;//tcp�����ظ��������


u8 getStartYear;
u8 getStartMon;
u8 getStartDate;  

u8 getEndYear;
u8 getEndMon;
u8 getEndDate;   

void Decry(unsigned char *buf,unsigned int buf_len,unsigned char *key,unsigned int key_len)
{	
  int i;

  for(i=0;i<buf_len;i++)
    buf[i]=(buf[i]-key[i%key_len]+256)%256;
}


void Encry(unsigned char *buf,unsigned int buf_len,unsigned char *key,unsigned int key_len)
{	
  int i;

  for(i=0;i<buf_len;i++)
    buf[i]=(buf[i]+key[i%key_len])%256;
}

//����: У���
unsigned char calcrc(u8 *ptr, int count) 
{ 
  u16 crc=0;
  u16 i;

  for(i=0;i<count;i++)
  {
    crc+=ptr[i];
  }
  crc&=0xFF;

  return crc;
} 

void  fill_data_nocrypto(u8 AFNdata,u8 COMdata,u8 *Gdata,u8 datarealen,u8* ctbuf)
{
  memset(strBuffer,'\0',512);
  TxBuffer[GINCLUDE]	=GSTX;			//��ʼ��
  TxBuffer[GLEN]	=datarealen+11;		//����
  TxBuffer[GINCLUDE_CHK]=GSTX;                 //��ʼ��ȷ�� 
  TxBuffer[GCOMMAND]	=COMdata;		//������
  memcpy(&TxBuffer[GADDRESS],&dargs.devno[0],GADR_LEN);//��ַ��A  00000000
  TxBuffer[GCOUNT]     =ctbuf[0];              //������
  TxBuffer[GCOUNT+1]   =ctbuf[1];              //������
  TxBuffer[GTYPE]      =AFNdata;		//AFN������
  memcpy(&TxBuffer[GDATA],Gdata,datarealen);	//�������ݵ�������
  TxBuffer[GCS]	=calcrc(&TxBuffer[GCOMMAND],TxBuffer[GLEN]);
  TxBuffer[G_ETX]	=GETX;
}
//����:���ҪgprsҪ���͵Ļ���������
void  fill_data(u8 AFNdata,u8 COMdata,u8 *Gdata,u8 datarealen,u8* ctbuf)
{
  memset(strBuffer,'\0',512);
  TxBuffer[GINCLUDE]	=GSTX;			//��ʼ��
  TxBuffer[GLEN]	=datarealen+11;		//����
  TxBuffer[GINCLUDE_CHK]=GSTX;                 //��ʼ��ȷ�� 
  TxBuffer[GCOMMAND]	=COMdata;		//������
  memcpy(&TxBuffer[GADDRESS],&dargs.devno[0],GADR_LEN);//��ַ��A  00000000
  TxBuffer[GCOUNT]     =ctbuf[0];              //������
  TxBuffer[GCOUNT+1]   =ctbuf[1];              //������
  TxBuffer[GTYPE]      =AFNdata;		//AFN������
  TxBuffer[GNULL]=0;
  memcpy(&TxBuffer[GDATA],Gdata,datarealen);	//�������ݵ�������
  TxBuffer[GCS]	=calcrc(&TxBuffer[GCOMMAND],TxBuffer[GLEN]);
  TxBuffer[G_ETX]	=GETX;
  Encry(&TxBuffer[GNULL],datarealen+2,&dargs.pwd[0],6);
}


//��ʱ�������� 
void GPRS_SendNormalData(void)
{
if(g_sAppContext.eGPRSState!=eGPRS_ONLINE)//�����ߣ�û�е�¼��gprs��
{
	return;	
}  
	TxCache[0]=DEV_SUCESS_DATA0;
	  TxCache[1]=((((timer.w_year-2000) / 10) << 4) | ((timer.w_year-2000) % 10));
	  TxCache[2]=((((timer.w_month) / 10) << 4) | ((timer.w_month) % 10));
	  TxCache[3]= ((((timer.w_date) / 10) << 4) | ((timer.w_date) % 10));
	  TxCache[4]=((((timer.hour) / 10) << 4) | ((timer.hour) % 10));
	  TxCache[5]= ((((timer.min) / 10) << 4) | ((timer.min) % 10));
	  TxCache[6]=((((timer.sec) / 10) << 4) | ((timer.sec) % 10));

	TxCache[7]=(in_water>>24)&0xFF;   
	TxCache[8]=(in_water>>16)&0xFF;
	TxCache[9]=(in_water>>8)&0xFF;
	TxCache[10]=(in_water)&0xFF;
	
	TxCache[11]=(out_water>>24)&0xFF;   
	TxCache[12]=(out_water>>16)&0xFF;
	TxCache[13]=(out_water>>8)&0xFF;
	TxCache[14]=(out_water)&0xFF;
	
	TxCache[15]=(loop_water>>24)&0xFF;   
	TxCache[16]=(loop_water>>16)&0xFF;
	TxCache[17]=(loop_water>>8)&0xFF;
	TxCache[18]=(loop_water)&0xFF;	 
	
	TxCache[19]=(elec>>24)&0xFF;   
	TxCache[20]=(elec>>16)&0xFF;
	TxCache[21]=(elec>>8)&0xFF;
	TxCache[22]=(elec)&0xFF;
	
	TxCache[23]=(getMediaOutWaterTemper);//�����Ļ����ȡ��ˮ���ˮ�¶�	 
	TxCache[24]=(getMediaEnvTemper);	 
	TxCache[25]=(getMediaWaterHigh);
	
	
	TxCache[26]=0; 
	TxCache[27]=0;
	TxCache[28]=0;
	TxCache[29]=0;
	TxCache[30]=0;
	TxCache[31]=0;
 

  countbuf[0]=0;
  countbuf[1]=1;
  fill_data(AFN_UP_NORMALDAT,CMD_LOG,TxCache,33,countbuf);
  TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
  TimerStart(TT_GPRS_NORAML_UP,dargs.upload_min*60*1000); //����Ϊ��λ	
	
}

//���͹��ϴ������
void GPRS_SendErrorCode(u8 id, u8 errCode)
{
  TxCache[0]=id;
  TxCache[1]=errCode;	 
 
  countbuf[0]=0;
  countbuf[1]=1;
  fill_data(AFN_MACHINE_ERR,CMD_LOG,TxCache,2,countbuf);
  TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
}

//��ʱ����������
void GPRS_SendHeart(void)
{
  TxCache[0]=dargs.pwd[0];
  TxCache[1]=dargs.pwd[1];	 
  TxCache[2]=dargs.pwd[2];
  TxCache[3]=dargs.pwd[3];
  TxCache[4]=dargs.pwd[4];
  TxCache[5]=dargs.pwd[5];  
  countbuf[0]=0;
  countbuf[1]=1;
  fill_data(AFN_HEART,CMD_LOG,TxCache,6,countbuf);
  TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
  TimerStart(TT_GPRS_HEART,dargs.g_hartheattime*60*1000);//����������������Ϊ��λ��	
}

u8 addresscmp(u8 *src_adr, u8 *rcv_adr)
{
  u8 i;

  for(i=0;i<6;i++)
  {
    if(src_adr[i]!=rcv_adr[i])
      return 1;
  }
  return 0;
}
//������������
void RxAnlyse(void)
{
  u8 i;
  u16 getYear;
  u8 getMon;
  u8 getDat;
  u8 getHour;
  u8 getMin;
  u8 getSec;



  //�ж�����ͷ�Ƿ���ȷ
  if(RxBuffer[GINCLUDE]!=GSTX)
  {
#ifdef DBG_RS232
    USART_printf(USART1,"Received Head Error.\r\n");	
#endif
    return;
  }	
  //�ж�����β�Ƿ���ȷ
  if(RxBuffer[RxBuffer[GLEN] + 4 ]!=GETX)
  {
#ifdef DBG_RS232
    USART_printf(USART1,"Received End Error.\r\n");	
#endif
    return;
  }	
  //�жϵ�ַ�Ƿ���ȷ
  if(addresscmp(&dargs.devno[0],&RxBuffer[GADDRESS]))
  {
#ifdef DBG_RS232
    USART_printf(USART1,"Received Address Error.\r\n");	
#endif
    return;
  }	

 
  //�ж�У����Ƿ���ȷ
  //������������
  if(RxBuffer[GTYPE]==AFN_PC_REQ_HEART)
  {
	  if( RxBuffer[RxBuffer[GLEN] + 3]!=calcrc(&RxBuffer[GCOMMAND],RxBuffer[GLEN]) )
	  {
	#ifdef DBG_RS232
	    USART_printf(USART1,"Received CRC Error.\r\n");	
	#endif
	    return;
	  }
  }
  //�������벻����
  else if(RxBuffer[GTYPE]==AFN_PC_SET_PWD)
  {
	  if( RxBuffer[RxBuffer[GLEN] + 3]!=calcrc(&RxBuffer[GCOMMAND],RxBuffer[GLEN]) )
	  {
	#ifdef DBG_RS232
	    USART_printf(USART1,"Received CRC Error.\r\n");	
	#endif
	    return;
	   }	
  }
  else
  {
  //�����Ľ������ݶ�����
  	Decry(&RxBuffer[13],RxBuffer[GLEN]-10,&(dargs.pwd[0]),6);  
  }
 
  //ת�����������ֵ
  countbuf[0] = RxBuffer[GCOMMAND+0]; 
  countbuf[1] = RxBuffer[GCOMMAND+1];

#ifdef DBG_RS232
    //USART_printf(USART1,"GTYPE:%d\r\n",RxBuffer[GTYPE]);	
#endif

//--2012.12.7  
  TimerStop(TT_GPRS_TCP_COMMUNICATE);//ֹͣ����
  send_st=0;//��λ
  TcpSendCt=0;//��λ

  //�жϹ�����
  switch(RxBuffer[GTYPE])
  {
    case AFN_PC_REQ_HEART:
      TxCache[0]=0x80;
      TxCache[1]=dargs.g_hartheattime;
      countbuf[0]=0;
      countbuf[1]=1;
      fill_data(AFN_PC_REQ_HEART,CMD_LOG,TxCache,2,countbuf);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break; 
    case AFN_PC_SET_HEART:
 	  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//��ȡ��������
	  SPI_FLASH_SectorErase(0);//��������
      dargs.g_hartheattime = RxBuffer[GDATA];//�޸�
	  SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));//д����������

      TxCache[0]=0x80;
      TxCache[1]=0x00;
      countbuf[0]=0;
      countbuf[1]=1;
      fill_data(AFN_PC_SET_HEART,CMD_LOG,TxCache,2,countbuf);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;
    case AFN_PC_REQ_CLK:
      TxCache[0]=0x80;									  
      TxCache[1]=((((timer.w_year-2000) / 10) << 4) | ((timer.w_year-2000) % 10));
      TxCache[2]=((((timer.w_month) / 10) << 4) | ((timer.w_month) % 10));
      TxCache[3]= ((((timer.w_date) / 10) << 4) | ((timer.w_date) % 10));
      TxCache[4]=((((timer.hour) / 10) << 4) | ((timer.hour) % 10));
      TxCache[5]= ((((timer.min) / 10) << 4) | ((timer.min) % 10));
      TxCache[6]=((((timer.sec) / 10) << 4) | ((timer.sec) % 10));
      fill_data(AFN_PC_REQ_CLK,CMD_LOG,TxCache,7,countbuf);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;
    case AFN_PC_SET_CLK:
	  getYear=(RxBuffer[GDATA+0]>>4)*10+(RxBuffer[GDATA+0]&0x0F);
	  getMon=(RxBuffer[GDATA+1]>>4)*10+(RxBuffer[GDATA+1]&0x0F);
	  getDat=(RxBuffer[GDATA+2]>>4)*10+(RxBuffer[GDATA+2]&0x0F);
	  getHour=(RxBuffer[GDATA+3]>>4)*10+(RxBuffer[GDATA+3]&0x0F);
	  getMin=(RxBuffer[GDATA+4]>>4)*10+(RxBuffer[GDATA+4]&0x0F);	
	  getSec=(RxBuffer[GDATA+5]>>4)*10+(RxBuffer[GDATA+5]&0x0F);

	  RTC_Set(getYear+2000,getMon,getDat,getHour,getMin,getSec);

      TxCache[0]=DEV_SUCESS_DATA0;
      TxCache[1]=DEV_SUCESS_DATA1;
      fill_data(AFN_PC_SET_CLK,CMD_LOG,TxCache,2,countbuf);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;
    case AFN_PC_REQ_PWD:
      TxCache[0]=DEV_SUCESS_DATA0;
      TxCache[1]=dargs.pwd[0];
      TxCache[2]=dargs.pwd[1];
      TxCache[3]=dargs.pwd[2];
      TxCache[4]=dargs.pwd[3];
      TxCache[5]=dargs.pwd[4];
      TxCache[6]=dargs.pwd[5];
      fill_data_nocrypto(AFN_PC_REQ_PWD,CMD_LOG,TxCache,7,countbuf);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;
    case AFN_PC_SET_PWD:
	  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//��ȡ��������
	  SPI_FLASH_SectorErase(0);//��������
      dargs.pwd[0]=RxBuffer[GDATA+0];//ת������,�޸�����
      dargs.pwd[1]=RxBuffer[GDATA+1];
      dargs.pwd[2]=RxBuffer[GDATA+2];
      dargs.pwd[3]=RxBuffer[GDATA+3];
      dargs.pwd[4]=RxBuffer[GDATA+4];
      dargs.pwd[5]=RxBuffer[GDATA+5];
	  SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));//д����������

	  #ifdef DBG_RS232
    USART_printf(USART1,"password:%d,%d,%d,%d,%d,%d\r\n",dargs.pwd[0],dargs.pwd[1],dargs.pwd[2],dargs.pwd[3],dargs.pwd[4],dargs.pwd[5]);	
#endif
      TxCache[0]=DEV_SUCESS_DATA0;
      TxCache[1]=DEV_SUCESS_DATA1;
      fill_data_nocrypto(AFN_PC_SET_PWD,CMD_LOG,TxCache,2,countbuf);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;
    case AFN_PC_REQ_CURDAT:
      TxCache[0]=DEV_SUCESS_DATA0;
      TxCache[1]=((((timer.w_year-2000) / 10) << 4) | ((timer.w_year-2000) % 10));
      TxCache[2]=((((timer.w_month) / 10) << 4) | ((timer.w_month) % 10));
      TxCache[3]= ((((timer.w_date) / 10) << 4) | ((timer.w_date) % 10));
      TxCache[4]=((((timer.hour) / 10) << 4) | ((timer.hour) % 10));
      TxCache[5]= ((((timer.min) / 10) << 4) | ((timer.min) % 10));
      TxCache[6]=((((timer.sec) / 10) << 4) | ((timer.sec) % 10));

      TxCache[7]=(in_water)&0xFF;
      TxCache[8]=(in_water>>8)&0xFF;
      TxCache[9]=(in_water>>16)&0xFF;
      TxCache[10]=(in_water>>24)&0xFF;

      TxCache[11]=(out_water)&0xFF;
      TxCache[12]=(out_water>>8)&0xFF;
      TxCache[13]=(out_water>>16)&0xFF;
      TxCache[14]=(out_water>>24)&0xFF;

      TxCache[15]=(loop_water)&0xFF;
      TxCache[16]=(loop_water>>8)&0xFF;
      TxCache[17]=(loop_water>>16)&0xFF;
      TxCache[18]=(loop_water>>24)&0xFF;

      TxCache[19]=(elec)&0xFF;
      TxCache[20]=(elec>>8)&0xFF;
      TxCache[21]=(elec>>16)&0xFF;
      TxCache[22]=(elec>>24)&0xFF;

      TxCache[23]=(getMediaOutWaterTemper)&0xFF;//���Ļ����ȡ�Ĳ���
      TxCache[24]=(getMediaEnvTemper)&0xFF;
      TxCache[25]=(getMediaWaterHigh)&0xFF;

      TxCache[26]=0;
      TxCache[27]=0;
      TxCache[28]=0;
      TxCache[29]=0;
      TxCache[30]=0;
      TxCache[31]=0;

      fill_data(AFN_PC_REQ_CURDAT,CMD_LOG,TxCache,32,countbuf);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;
    case AFN_PC_REQ_HISDAT://���ɼ���ʷ����
 	  getStartYear=((((RxBuffer[GDATA+0]-2000) / 10) << 4) | ((RxBuffer[GDATA+0]-2000) % 10)); 
	  getStartMon=((((RxBuffer[GDATA+1]) / 10) << 4) | ((RxBuffer[GDATA+1]) % 10));
	  getStartDate=((((RxBuffer[GDATA+2]) / 10) << 4) | ((RxBuffer[GDATA+2]) % 10));   

	  getEndYear=((((RxBuffer[GDATA+3]-2000) / 10) << 4) | ((RxBuffer[GDATA+3]-2000) % 10)); 
	  getEndMon=((((RxBuffer[GDATA+4]) / 10) << 4) | ((RxBuffer[GDATA+4]) % 10));
	  getEndDate=((((RxBuffer[GDATA+5]) / 10) << 4) | ((RxBuffer[GDATA+5]) % 10));
	 
      TxCache[0]=DEV_SUCESS_DATA0;
      TxCache[1]=DEV_SUCESS_DATA1;
      fill_data(AFN_PC_REQ_HISDAT,CMD_LOG,TxCache,2,countbuf);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
	  TimerStart(9,_5S);//5���������������
      break;
    case AFN_PC_REQ_ARGS://��ѯPC�·�������
      TxCache[0]=DEV_SUCESS_DATA0;
      TxCache[1]=dargs.setMideaDegree0;
      TxCache[2]=dargs.setMideaLoopDegree;
      TxCache[3]=dargs.setMideaWaterLevel;
      TxCache[4]=dargs.setMideaIfLoop;
      TxCache[5]=dargs.setMideaWaterOffset;
      TxCache[6]=dargs.setMideaIfHot;
      TxCache[7]=dargs.setMideaHandOpenNumbers;
      TxCache[8]=dargs.setIfAutoOnOff;
      TxCache[9]=dargs.setOnTime[0];
      TxCache[10]=dargs.setOnTime[1];
      TxCache[11]=dargs.setOffTime[0];
      TxCache[12]=dargs.setOffTime[1];
      fill_data(AFN_PC_REQ_ARGS,CMD_LOG,TxCache,13,countbuf);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;
    case AFN_PC_SET_ARGS://PC�·�������
	  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//��ȡ��������
	  SPI_FLASH_SectorErase(0);//��������
      dargs.setMideaDegree0=RxBuffer[GDATA+0]; //�޸�
      dargs.setMideaLoopDegree=RxBuffer[GDATA+1]; 
      dargs.setMideaWaterLevel=RxBuffer[GDATA+2]; 
      dargs.setMideaIfLoop=RxBuffer[GDATA+3]; 
      dargs.setMideaWaterOffset=RxBuffer[GDATA+4]; 
      dargs.setMideaIfHot=RxBuffer[GDATA+5]; 
      dargs.setMideaHandOpenNumbers=RxBuffer[GDATA+6]; 
      dargs.setIfAutoOnOff=RxBuffer[GDATA+7]; 
      dargs.setOnTime[0]=RxBuffer[GDATA+8]; 
      dargs.setOnTime[1]=RxBuffer[GDATA+9]; 
      dargs.setOffTime[0]=RxBuffer[GDATA+10];
      dargs.setOffTime[1]=RxBuffer[GDATA+11]; 
	  SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));//д����������

      TxCache[0]=DEV_SUCESS_DATA0;
      TxCache[1]=DEV_SUCESS_DATA1;
      fill_data(AFN_PC_SET_ARGS,CMD_LOG,TxCache,2,countbuf);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;
    case AFN_PC_REQ_UPTIME:
      TxCache[0]=DEV_SUCESS_DATA0;
      TxCache[1]=(dargs.upload_min>>8)&0xFF;
      TxCache[2]=dargs.upload_min&0xFF;
      fill_data(AFN_PC_REQ_UPTIME,CMD_LOG,TxCache,3,countbuf);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;
    case AFN_PC_SET_UPTIME:	  
	  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//��ȡ��������
	  SPI_FLASH_SectorErase(0);//��������
      dargs.upload_min = (RxBuffer[GDATA+0]<<8) | (RxBuffer[GDATA+1]);//�޸�  
	  SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));//д����������

      TxCache[0]=DEV_SUCESS_DATA0;
      TxCache[1]=DEV_SUCESS_DATA1;
      fill_data(AFN_PC_SET_UPTIME,CMD_LOG,TxCache,2,countbuf);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;
    case AFN_PC_REQ_STATUS:	//��ѯ����״̬
      TxCache[0]=DEV_SUCESS_DATA0;
      TxCache[1]=getMediaMSG[0].run_m; 
      TxCache[2]=getMediaMSG[0].online_m;  
      TxCache[3]=dargs.setMideaLoopDegree; 	//���õ�ѭ��ˮ�¶�
      TxCache[4]=oc_status;//���鿪�ػ�״̬

	  for(i=0;i<MAX_MIDEA_MACHINE;i++)
	  {
	      TxCache[5+i*9]=getMediaMSG[i].err_code[0+i*2];
	      TxCache[6+i*9]=getMediaMSG[i].err_code[1+i*2];
	      TxCache[7+i*9]=getMediaMSG[i].protect_code[0+i*2]; 
	      TxCache[8+i*9]=getMediaMSG[i].protect_code[1+i*2];
	      TxCache[9+i*9]=getMediaMSG[i].cool_t[0+i*2];	  
	      TxCache[10+i*9]=getMediaMSG[i].cool_t[1+i*2];	  	 
	      TxCache[11+i*9]=getMediaMSG[i].compress_c[0+i*2];	  	 
	      TxCache[12+i*9]=getMediaMSG[i].compress_c[1+i*2];	 	 
	      TxCache[13+i*9]=getMediaMSG[i].board_outwater_t;
	  }
	  
      fill_data(AFN_PC_REQ_STATUS,CMD_LOG,TxCache,95,countbuf);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);  
      break;
    case AFN_PC_SET_WLEV: //��˾�Դ�ˮλ̽ͷ������ˮλ
 	  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//��ȡ��������
	  SPI_FLASH_SectorErase(0);//��������
      dargs.setMideaWaterLevel=RxBuffer[GDATA+0]; //0-100%  1-75%  2-50%  3-25%	
	  SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));//д����������

      TxCache[0]=DEV_SUCESS_DATA0;
      TxCache[1]=DEV_SUCESS_DATA1;
      fill_data(AFN_PC_SET_WLEV,CMD_LOG,TxCache,2,countbuf);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;
    case AFN_PC_REQ_WDEGREE: //����ˮ��
      dargs.setMideaDegree0=RxBuffer[GDATA+0]; // �趨��ˮ���¶�
      TxCache[0]=DEV_SUCESS_DATA0;
      TxCache[1]=DEV_SUCESS_DATA1;
      fill_data(AFN_PC_REQ_WDEGREE,CMD_LOG,TxCache,2,countbuf);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;
    case AFN_PC_SET_OPENCLOSE: //�Զ����ػ�
 	  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//��ȡ��������
	  SPI_FLASH_SectorErase(0);//��������
      dargs.setIfAutoOnOff=RxBuffer[GDATA+0];  	
      dargs.setOnTime[0]=RxBuffer[GDATA+1];  
      dargs.setOnTime[1]=RxBuffer[GDATA+2];  
      dargs.setOffTime[0]=RxBuffer[GDATA+3];  
      dargs.setOffTime[1]=RxBuffer[GDATA+4]; 
	  SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));//д���������� 	  
	   
      TxCache[0]=DEV_SUCESS_DATA0;
      TxCache[1]=DEV_SUCESS_DATA1;
      fill_data(AFN_PC_SET_OPENCLOSE,CMD_LOG,TxCache,2,countbuf);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;
    case AFN_PC_SET_IOPENCLOSE:	  //ʵʱ���ػ� 
 	  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//��ȡ��������
	  SPI_FLASH_SectorErase(0);//��������
	  dargs.setIfQuickOnOff=RxBuffer[GDATA+0];  
	  SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));//д����������
      TxCache[0]=DEV_SUCESS_DATA0;
      TxCache[1]=DEV_SUCESS_DATA1;
      fill_data(AFN_PC_SET_IOPENCLOSE,CMD_LOG,TxCache,2,countbuf);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;
    case AFN_LOG://���յ���½��Ӧ��
     // TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break; 
    case AFN_HEART://����������
      //TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break; 
    case AFN_MACHINE_ERR://��������
      //TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;
    case AFN_UP_NORMALDAT://�ϴ���ʱ����
      //TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;
    case AFN_UP_ADOPTTDAT://�ϴ��ɼ�����
      //TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;

  }

}


void sim900b_send_tcp_data(void)
{
  u8 send_len;
  char command_send[128] = {0};

  if(send_st==0)//��������
  {
    send_len = TxBuffer[GLEN]+5;
#ifdef DBG_RS232
    USART_printf(USART1,"AT+CIPSEND=%d\r\n",send_len);	
#endif
    sprintf(command_send,"AT+CIPSEND=%d\r\n",send_len);	
    SendATCommand(command_send);
	  TimerStart(5, _1S);//	1������
  }
  else if(send_st==1)//��������
  {
#ifdef DBG_RS232
    USART_printf(USART1,"sending tcp data.\r\n");	
#endif
    USART_SendNByte(USART2,TxBuffer,TxBuffer[GLEN]+5);
	TimerStart(5, _2S); 
//	 if(++SendCt>=5)
//	 {
//	 	SendCt=0;
//		send_st=0;
//	      TimerStart(5, _1S);	
//	      if(++TcpSendCt>20)
//	      {
//			TcpSendCt=0;
//			send_st=0;
//			TimerStop(5);
//			SendATCommand("AT+CIPSHUT\r\n");
//			g_sAppContext.eGPRSState=eGPRS_OFFLINE;
//		    TimerStart(4, _2S);	
//	      }
//	 }
  }
}


int SendATCommand(char  *ATcom)
{
  while(*ATcom!='\0')
  {
    USART_SendByte(USART2,*ATcom++);	
  }
  return 0;
}

int ParseStringByToken(char **data, unsigned short *len, char token, char *v, unsigned short max_len)
{
  unsigned short i;
  int ret = TRUE;

  //data is empty
  if((*data)[0] == 0)
    return FALSE;

  for(i = 0; i < (*len); i++)
  { 
    if((*data)[i] == token)
    {
      break;
    }

    if(i == max_len - 1)
    {
      return FALSE;
    }
  } 
  //not have token
  if((*data)[i] != token)
    return FALSE;    
  memcpy(v, (*data), i);
  v[i] = 0;
  (*data) += i + 1;
  (*len) -= i + 1; 

  return ret;
}

int processATCnfData(char *pCmdResult)
{
  int result = FALSE;

  /* ��ȡ�������ݵĳ��� */
  while((*pCmdResult == '\r')||(*pCmdResult == '\n'))
  {
    ++pCmdResult;
  }

#ifdef DBG_RS232
  USART_printf(USART1, "\r\n");	

  USART_printf(USART1, (u8 *)pCmdResult);
  USART_printf(USART1, "\r\n");	
#endif


 
  if ((strstr(pCmdResult, "RDY")!= NULL)) //------- //ģ������������ַ���
 	{
#ifdef DBG_RS232
      USART_printf(USART1,"++MODULE:AT-Command Interpreter ready\r\n");	
#endif 

      TimerStart(TT_GPRS_SEARCH_CARD,_5S);
      result = TRUE;
    }
    else if((strstr(pCmdResult, "+IPR:")!= NULL))  //�忨
    {
#ifdef DBG_RS232
      USART_printf(USART1,"++MODULE:IPR\r\n");	
#endif
	  TimerStop(11); 
      TimerStart(TT_GPRS_SEARCH_CARD,_5S);
      result = TRUE;
    }
    else if ((strstr(pCmdResult, "+CPIN:") != NULL)) //------------------------ //��SIM��
    {
#ifdef DBG_RS232
      USART_printf(USART1,"++MODULE:SIM CARD READY\r\n");	
#endif
      g_sAppContext.eSimCardState = eSimCardReady;
      g_sAppContext.eInitState = eInitStart;
      TimerStop(TT_GPRS_SEARCH_CARD);
      TimerStart(TT_GPRS_INIT,_1S);
      result = TRUE;
    }
    else if((strstr(pCmdResult, "CONNECT OK") != NULL)) //-----------------------TCPIP���ӷ�����IP�˿ںųɹ�
    {
//#ifdef DBG_RS232
//      USART_printf(USART1,"++MODULE:CONNECT OK\r\n");	
//#endif
TcpInitCt=0;   TcpSendCt=0;
g_sAppContext.eGPRSState=eGPRS_ONLINE;  
      TimerStop(TT_GPRS_TCP);

      TxCache[0]=dargs.pwd[0];
      TxCache[1]=dargs.pwd[1];
      TxCache[2]=dargs.pwd[2];
      TxCache[3]=dargs.pwd[3];
      TxCache[4]=dargs.pwd[4];
      TxCache[5]=dargs.pwd[5];
      countbuf[0]=0;
      countbuf[1]=1;

#ifdef DBG_RS232
      USART_printf(USART1,"password:%d,%d,%d,%d,%d,%d\r\n",dargs.pwd[0],dargs.pwd[1],dargs.pwd[2],dargs.pwd[3],dargs.pwd[4],dargs.pwd[5]);	
#endif

      fill_data_nocrypto(AFN_LOG,CMD_LOG,TxCache,6,countbuf);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
	  TimerStart(6,_500mS);//������
	  TimerStart(8,dargs.upload_min*60*1000);//��ʱ�ϱ�����	,�ǲ���Ҫ�ƶ�����ʼ
	  TimerStart(TT_GPRS_HEART,dargs.g_hartheattime*60*1000);//����������������Ϊ��λ��
      result = TRUE; 	  	  
    }
    else if((strstr(pCmdResult, "CONNECT FAIL") != NULL)) //-----------------------TCPIP���ӷ�����IP�˿ںųɹ�
    {
      if(++TcpConnectCt>60)//ÿ5S����һ�Σ���5���ӻ���fail��������Ӫ���������ip
      {
		TcpConnectCt=0;
		g_sAppContext.eGPRSState=eGPRS_OFFLINE;
		TimerStop(TT_GPRS_TCP_COMMUNICATE);
		TimerStart(TT_GPRS_TCP,_1S);
      }
      result = TRUE; 	  	  
    }
    else if((strstr(pCmdResult, "ALREAY CONNECT") != NULL)) //-----------------------TCPIP���ӷ�����IP�˿ںųɹ�
    {
      result = TRUE; 	  	  
    }
    else if((strstr(pCmdResult, "SEND OK") != NULL)) //-- ���ͳɹ� 
    {
//#ifdef DBG_RS232
//      USART_printf(USART1,"++MODULE:IPSEND\r\n");	
//#endif
      send_st=0;
      //����
      //g_Senddata=time_set(TT_GPRS_TCP_OK,_1S);
      TcpSendCt=0;
      //if(TcpSendCt>0)
      //	--TcpSendCt;
      TimerStop(TT_GPRS_TCP_COMMUNICATE);
      result=TRUE;
    }
    else if( (strstr(pCmdResult, "+IPD,") != NULL)  ) //----------------------�յ�������һ��GPRS����
    {	
      // +IPD,2:12 
      char *p = strstr(pCmdResult, "+IPD,")+ strlen("+IPD,");
      //char *pDat;
      char szIndex[3 + 1] = {0};
      int nIndex = 0;
      int  Rssi=0;

//#ifdef DBG_RS232
//      USART_printf(USART1,"++MODULE:+IPD\r\n");	
//#endif
      while((*p != ':') &&(*p!=0))
      {
		if(*p <= '9' && *p >= '0')
		{
		  szIndex[nIndex] = *p;
		  nIndex++;
		}
		p++;
      } 
      ++p;//����":"
      szIndex[nIndex] = '\0';
      Rssi = atoi(&szIndex[0]);

      for(nIndex=0;nIndex<Rssi;nIndex++)
      {
		RxBuffer[nIndex]=*p++;					  	
 
      }

      RxAnlyse();
      result = TRUE;
    }

    else if((strstr(pCmdResult, "CLOSED") != NULL))//�ر�����  , ��������
    {
      TcpConnectCt=0;
      g_sAppContext.eGPRSState=eGPRS_OFFLINE;
      TimerStop(TT_GPRS_TCP_COMMUNICATE);
      TimerStart(TT_GPRS_INIT,_1S);
      result = TRUE; 	
    }
    else if((strstr(pCmdResult, "+CREG:") != NULL)) //------------------------//��ȡ��վ��Ϣ
    {
#ifdef DBG_RS232
      USART_printf(USART1,"++MODULE: CREG\r\n");	
#endif 
      result = TRUE; 	
    }
    else if((strstr(pCmdResult, ">") != NULL))        //------------------------//������Ϣ
    {
//#ifdef DBG_RS232
//      USART_printf(USART1,"++MODULE: >\r\n");	
//#endif 
      send_st=1;
      TimerStop(TT_GPRS_TCP_COMMUNICATE);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_50mS);//1s����������
      result = TRUE;
    }						  
    else if((strstr(pCmdResult, "+CMTI:") != NULL))	//------------------------//�յ�һ������
    {	  ////+CMTI: "SM", 1
      char *p = strstr(pCmdResult, "+CMTI:")+ strlen("+CMTI:");
      char szIndex[3 + 1] = {0};
      char szCommand[56] = {0};
      u8 nIndex = 0; 

#ifdef DBG_RS232
      USART_printf(USART1,"++MODULE:CMTI(MSG GET)\r\n");	
#endif
      //	TimerStop(4);
      while(*p != ',' && *p != '\0')
      {
	p++;
      }  
      if(*p == ',')
      {
	while(*p != '\0')
	{
	  if(*p <= '9' && *p >= '0')
	  {
	    szIndex[nIndex] = *p;
	    nIndex++;
	  }
	  p++;
	} 
	szIndex[nIndex] = '\0';
	//	SendATCommand("AT+CSDH=0\r");			
	//	SendATCommand("AT+CMGF=1\r");
	//	SendATCommand("AT+CSCS=\"GSM\"\r");
	sprintf(szCommand, "AT+CMGR=%d\r", atoi(szIndex));		
	SendATCommand(szCommand);
	result = TRUE;

      }        
    }	   
    else if((strstr(pCmdResult, "+CMGR:") != NULL))	 //------------------------ //����һ������
    {
      char *p = strstr(pCmdResult, "+CMGR:");
      //+CMGR: "REC READ", "+8613427925684", "", "10/08/09, 14:10:53+50" 
      //+CMGR: "REC UNREAD","8615013657293",   , "11/08/24, 15:11:10+32", 145,4,0,0,"8613800755500",145,3  MFK   AT+CSDH=1
#ifdef DBG_RS232
      USART_printf(USART1,"++MODULE: CMGR(MSG READ)\r\n");	
#endif

      if((strstr(p, "OK") != NULL))
      {	 
	if((strstr(p, "#") != NULL))
	{
	  memset(&g_sMessageInfo, 0x0, sizeof(g_sMessageInfo));
	  if(ParseReceiveMessage(p, &g_sMessageInfo))
	  {
	    //		           TimerStart(2, 1000);

	  }
	}

	result = TRUE;
      }
      else result = FALSE;
    }
    else if((strstr(pCmdResult, "+CGATT:") != NULL)) //GPRS�Ƿ���
    {
      char *p = strstr(pCmdResult, "+CGATT:")+ strlen("+CGATT:");
      char szIndex[3 + 1] = {0};
      char nIndex = 0;
      int  Rssi=0;

//#ifdef DBG_RS232
//      USART_printf(USART1,"++MODULE:+CGATT\r\n");	
//#endif
      while(*p != '\0')
      {
	if(*p <= '9' && *p >= '0')
	{
	  szIndex[nIndex] = *p;
	  nIndex++;
	}
	p++;
      } 
      szIndex[nIndex] = '\0';
      Rssi = atoi(&szIndex[0]);

      if(Rssi==1)//
      {
	if(g_sAppContext.eGPRSState==eGPRS_GTT)
	{
	  g_sAppContext.eGPRSState=eGPRS_APN;  	
	}
      }
      else if(Rssi==0)
      {
      }

    }
    else if((strstr(pCmdResult, "+CGREG:") != NULL)) //------------------------GPRS����ע��״̬
    {
      //+CGREG: 1,1     ��ע�ᣬ������
#ifdef DBG_RS232
      USART_printf(USART1,"++MODULE:+CGREG\r\n");	
#endif
      result = TRUE;
    }
    else if((strstr(pCmdResult, "%IPCLOSE:") != NULL)) //-----------------------��ѯ�Ƿ��˳�TCPIP����
    {
      //%IPCLOSE: 5    �˳�TCPIP���ܣ�GPRS����ע��
      //%IPCLOSE: 0,0,0    ���ӹر�
      //%IPCLOSE: 1,0,0   ����һ��TCPIP 
#ifdef DBG_RS232
      //USART_printf(USART1,"++MODULE:IPCLOSE\r\n");	
#endif
      result = TRUE;	    	  	  
    }
    else  if((strchr(pCmdResult, '.') != NULL))  //���뵽ip
    {
#ifdef DBG_RS232
      USART_printf(USART1,"--Get Active Ip.\r\n");	
#endif
      result = TRUE;
    }

    else  if((strstr(pCmdResult, "OK") != NULL))   //------------------------  AT OK  
    {
//#ifdef DBG_RS232
//      USART_printf(USART1,"++MODULE:OK\r\n");	
//#endif		
      if(g_sAppContext.eInitState != eInitEnd)
      { 
	if(g_sAppContext.eInitState == eInitStart)     //-----------------		eInitcreg,eInitcgreg,
	  {
	    g_sAppContext.eInitState = eIniteProductVer ;
	  }
	else if(g_sAppContext.eInitState == eIniteProductVer)     //-----------------		eInitcreg,eInitcgreg,
	  {
	    g_sAppContext.eInitState = eIniteModulVer ;
	  }
	else if(g_sAppContext.eInitState == eIniteModulVer)     //-----------------		eInitcreg,eInitcgreg,
	  {
	    g_sAppContext.eInitState = eInithead ;
	  }
	else if(g_sAppContext.eInitState == eInithead)     //-----------------		eInitcreg,eInitcgreg,
	  {
	    g_sAppContext.eInitState = eInitcreg ;
	  }
	else if(g_sAppContext.eInitState == eInitcreg )	   //-----------------
	{
	  g_sAppContext.eInitState = eInitcgreg;
	}
	else if(g_sAppContext.eInitState ==eInitcgreg )	   //-----------------
	{
	  g_sAppContext.eInitState = eInitImei;
	}
	else if(g_sAppContext.eInitState == eInitImei ) //-----------------
	{
	  g_sAppContext.eInitState = eInitImsi;
	}
	else if(g_sAppContext.eInitState == eInitImsi)   //-----------------
	{
	  g_sAppContext.eInitState = eInitSMS;	
	}
	else if(g_sAppContext.eInitState == eInitSMS) //-----------------
	{ 
	  g_sAppContext.eInitState = eInitSMS1;  
	}
	else if(g_sAppContext.eInitState == eInitSMS1) //-----------------
	{ 
	  g_sAppContext.eInitState =eInitSMS2;  
	}
	else if(g_sAppContext.eInitState == eInitSMS2) //-----------------
	{ 
	  g_sAppContext.eInitState = eInitSMS3;  
	}	 
	else if(g_sAppContext.eInitState == eInitSMS3) //-----------------                      
	{ 
	  g_sAppContext.eInitState = eInitGPRS;  
	}
	else if(g_sAppContext.eInitState == eInitGPRS) //-----------------                      
	{ 
	  g_sAppContext.eInitState = eIniteTCPIP;  
	}

	else if(g_sAppContext.eInitState == eIniteTCPIP) //-----------------                      
	{ 
	  g_sAppContext.eInitState = eInitEnd;  
	}
	else 
	{
	  g_sAppContext.eInitState =	eInitStart;
	}
      }
      else //��ʼ����������������tcp����
      {
	if(g_sAppContext.eGPRSState!=eGPRS_ONLINE)
	{
	  if(g_sAppContext.eGPRSState==eGPRS_OFFLINE)
	  {
	    g_sAppContext.eGPRSState=eGPRS_GTT;  
	  }
	  else   if(g_sAppContext.eGPRSState==eGPRS_APN)
	  {
	    g_sAppContext.eGPRSState=eGPRS_UP;  
	  }
	  else   if(g_sAppContext.eGPRSState==eGPRS_UP)
	  {
	    g_sAppContext.eGPRSState=eGPRS_GETIP;  
	  } 
	  else   if(g_sAppContext.eGPRSState==eGPRS_GETIP)
	  {
	  }
	  else   if(g_sAppContext.eGPRSState==eGPRS_CALL)
	  {
	    //g_sAppContext.eGPRSState=eGPRS_ONLINE;  
	  }
	}
      }

      result = TRUE;					
    }
    else  if((strstr(pCmdResult, "ERROR") != NULL))  //------------------------   AT FAIL!  
    {
#ifdef DBG_RS232
      USART_printf(USART1,"++MODULE:ERROR\r\n");	
#endif

      result = TRUE;
    }
    else 
    {	
    }
  return result;
}


void processATCnf(char * strAT, u16 len)
{
  //	char strBuffer[512]={0};  

  memset(strBuffer, 0x00, 512);

  if(len < 512)
  {
    memcpy(strBuffer, strAT, len);
    strBuffer[len] = '\0';
  }	 
  if (processATCnfData((char *)strBuffer))
  {
  }
  else
  {
  }
  return;
}


void SendMessage(char *szPhoneNumber, char *szContent)
{
  char szCommand[60] = {0};
  //	int bResult =  FALSE;

  if(szPhoneNumber == 0 ||*szPhoneNumber == '\0')
    return;

  sprintf(szCommand, "AT+CMGS=\"%s\"\r\n", szPhoneNumber);

  //	memset(g_szSendingMessage, 0x0, sizeof(g_szSendingMessage));
  //	strcpy(g_szSendingMessage, szContent);

  //    SendATCommand("AT+CMGF=1\r");
  //    SendATCommand("AT+CSCS=\"GSM\"\r");	
  // SendATCommand("AT+CSMP= 17,167,0,241\r");	
  SendATCommand(szCommand); 
}
//+CMGR: "REC READ", "+8613427925684", "", "10/08/09, 14:10:53+50"
//��������
//+CMGR: "REC UNREAD","+8615013657293","","11/06/26,13:32:59+50"

int ParseReceiveMessage(char *szText, MessageInfo *pMessageInfo)
{
  char *p = szText;
  u8 nIndex = 0;
  //   char S_time[14+1]={0};
  // char year[2 + 1] = {0};
  // char month[2 + 1] = {0};
  // char day[2 + 1] = {0};  
  // char hour[2 + 1] = {0};
  // char min[2 + 1] = {0};
  //  char sec[2 + 1] = {0};
  //skip
  while(*p != ',' && *p != '\0')
  {
    p++;
  }

  if(*p == '\0')
    return FALSE;
  else
    p++;

  while(*p != ',' && *p != '\0')
  {
    if(*p == '+' ||(*p >= '0' && *p <= '9'))
    {
      pMessageInfo->szRecNumber[nIndex] = *p;
      //  RecNumber[nIndex] = *p;    ////////////++++++++++++
      nIndex++;
    }

    p++;
  }

  pMessageInfo->szRecNumber[nIndex] = 0;
  // RecNumber[nIndex] = 0;    ////////////++++++++++++
  if(*p == '\0')
    return FALSE;
  else
    p++;
  //+CMGR: "REC UNREAD","+8615013657293","","11/06/26,13:32:59+50"
  //skip
  while(*p != ',' && *p != '\0')
  {
    p++;
  }

  if(*p == '\0')
    return FALSE;
  else
    p++;  
  //skip
  nIndex = 0;
  while(*p != '\n' && *p != '\0')
  {  
    //zhouqin
    if((*p >= '0' && *p <= '9'))
    {
      //			S_time[nIndex] = *p;
      nIndex++;
    }

    p++;
  }

  if(*p == '\0')
    return FALSE;
  else
    p++;

  nIndex = 0;


  while(*p != '\0')
  {
    if(*p != '\r' && *p != '\n')
    {
      pMessageInfo->szContent[nIndex] = *p;
      // Content[nIndex] = *p;	    ////////////++++++++++++
      nIndex++;
    }

    p++;
  }  
  //+CMGR: "REC UNREAD","+8615013657293","","11/06/26,13:32:59+50"

  //    year[0] =S_time[0];
  //	year[1] =S_time[1];

  //	month[0] =S_time[2];
  //	month[1] =S_time[3];

  //	day[0] =S_time[4];
  //	day[1] =S_time[5];

  //	hour[0] =S_time[6];
  //	hour[1] =S_time[7];

  //	min[0] =S_time[8];
  //	min[1] =S_time[9];

  //	sec[0] =S_time[10];
  //	sec[1] =S_time[11];

  //  pMessageInfo->sRecTime.nYear=2000 + atoi(year);
  //	pMessageInfo->sRecTime.nMonth=atoi(month);
  //	pMessageInfo->sRecTime.nDay=atoi(day);
  //	pMessageInfo->sRecTime.nHour=atoi(hour);
  //	pMessageInfo->sRecTime.nMin=atoi(min);
  //	pMessageInfo->sRecTime.nSec=atoi(sec);
  return TRUE;

}



int ProcessIncomingSMS(char *szSMSContent)
{
  char *p = g_sMessageInfo.szContent;
  //     char *p = Content;
  u16 nLen = strlen(g_sMessageInfo.szContent);
  // u16 nLen = strlen(Content);
  char szKey[3 + 1] = {0};
  int bResult = TRUE;	
  //skip '#'
  p++;
  nLen--;

  if(ParseStringByToken(&p, &nLen, '#', szKey, sizeof(szKey)))
  { 
    if(strncmp(szKey, "801", 3) == 0)
    {		 
      char szIP[MAX_IP_ADDRESS_LENGTH + 1] = {0};
      char szPortNumber[MAX_PORT_NUMBER_LENGTH + 1] = {0};  			
      char szPassword[MAX_PASSWORD_LENGTH + 1] = {0};				
      char szMessage[140] = {0};		
      if(ParseStringByToken(&p, &nLen, '#', szIP, sizeof(szIP)))
      { 
	if(ParseStringByToken(&p, &nLen, '#', szPortNumber, sizeof(szPortNumber)))
	{				
	  if(ParseStringByToken(&p, &nLen, '#', szPassword, sizeof(szPassword)))
	  {
	    //if(strcmp(szPassword, g_setting.szPassword) == 0)
	    //	{
	    strcpy(szMessage, "SET IP OK");
	    memset(g_setting.szIPAddress, 0x0, sizeof(g_setting.szIPAddress));
	    memset(g_setting.szPortNumber, 0x0, sizeof(g_setting.szPortNumber));

	    strcpy(g_setting.szIPAddress, szIP);
	    strcpy(g_setting.szPortNumber, szPortNumber);
	    SendATCommand("AT%IPCLOSE=1\r\n");
	    g_sAppContext.eGPRSState=eGPRS_OFFLINE;
	    //TimerStart(4, 10000);
	    //							SaveSettingConfig();
	    //	}
	    //	else
	    //	{
	    //	strcpy(szMessage, "PASSWORD ERROR");
	    //	}		
	  }
	  else
	    strcpy(szMessage, "COMMAND3 ERROR");
	}
	else
	  strcpy(szMessage, "COMMAND2 ERROR");
      }
      else
      {
	strcpy(szMessage, "COMMAND1 ERROR");
      }
      memset(g_sMessageInfo.szSendingData, 0x0, sizeof(g_sMessageInfo.szSendingData));
      strcpy(g_sMessageInfo.szSendingData, szMessage);
#ifdef GPRS_DEBUG
      DebugPrint(g_setting.szIPAddress);
      DebugPrint(g_setting.szPortNumber);
      DebugPrint(szPassword);
      DebugPrint(g_sMessageInfo.szSendingData);
#endif 
      //	TimerStart(3, 1000);
    }	
  }

  return bResult;
}


void GPRS_Initial(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);	 
}

void GPRS_StartUp(void)
{
  /*MIC29302_ON();
  delay_ms(1000);//
  GPRS_OPEN();
  delay_ms(500); //300
  GPRS_CLOSE();
  delay_ms(1000);//����SIM900B��������1s
  GPRS_OPEN(); */
  
  MIC29302_ON();
  Delay(100000);//
  GPRS_OPEN();
  Delay(50000); //300
  GPRS_CLOSE();
  Delay(100000);//����SIM900B��������1s
  GPRS_OPEN(); 
 
}

void GPRS_ShutDown(void)
{
  /*GPRS_CLOSE();
  delay_ms(1500);
  GPRS_OPEN();
  MIC29302_OFF();
  delay_ms(1000);*/

  GPRS_CLOSE();
  Delay(150000);
  GPRS_OPEN();
  MIC29302_OFF();
  Delay(100000);

}

void GprsProc(void)
{
  if(g_sATback.AT_RX_STA)
  {
    //g_sATback.AT_RX_STA=0;
    processATCnf(g_sATback.ATbackStr,g_sATback.AT_RX_GL);
    memset((u8*)&g_sATback.ATbackStr[0],0,g_sATback.AT_RX_GL); 
    g_sATback.AT_RX_GL=0;
    g_sATback.AT_RX_STA=0;  
  }
  else//��������
  {
    g_sATback.AT_RX_GL=0;
    g_sATback.AT_RX_STA=0;
  }
}
