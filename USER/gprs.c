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
MessageInfo g_sMessageInfo={"15013657293","#000#000#0000#","gsm ok"};  //定义一个短信结构体 
//S_ConfigSetting g_setting={"CMNET","114.80.203.14","9090"};
S_ConfigSetting g_setting={"CMNET","219.134.171.108","9595"};
u8 send_st;
u8 strBuffer[512]={0};  
u8 TxBuffer[512]={0};   
u8 RxBuffer[512]={0}; 
u8 TxCache[128];
u8 countbuf[2];

u8 CipsendLen;

u8 TcpSendCt;//tcp在线发送允许错误次数
u8 TcpConnectCt;//tcp连接重复允许次数


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

//功能: 校验和
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
  TxBuffer[GINCLUDE]	=GSTX;			//起始符
  TxBuffer[GLEN]	=datarealen+11;		//长度
  TxBuffer[GINCLUDE_CHK]=GSTX;                 //起始符确认 
  TxBuffer[GCOMMAND]	=COMdata;		//控制欲
  memcpy(&TxBuffer[GADDRESS],&dargs.devno[0],GADR_LEN);//地址域A  00000000
  TxBuffer[GCOUNT]     =ctbuf[0];              //计数器
  TxBuffer[GCOUNT+1]   =ctbuf[1];              //计数器
  TxBuffer[GTYPE]      =AFNdata;		//AFN功能码
  memcpy(&TxBuffer[GDATA],Gdata,datarealen);	//传输数据到数据域
  TxBuffer[GCS]	=calcrc(&TxBuffer[GCOMMAND],TxBuffer[GLEN]);
  TxBuffer[G_ETX]	=GETX;
}
//功能:填充要gprs要发送的缓冲区内容
void  fill_data(u8 AFNdata,u8 COMdata,u8 *Gdata,u8 datarealen,u8* ctbuf)
{
  memset(strBuffer,'\0',512);
  TxBuffer[GINCLUDE]	=GSTX;			//起始符
  TxBuffer[GLEN]	=datarealen+11;		//长度
  TxBuffer[GINCLUDE_CHK]=GSTX;                 //起始符确认 
  TxBuffer[GCOMMAND]	=COMdata;		//控制欲
  memcpy(&TxBuffer[GADDRESS],&dargs.devno[0],GADR_LEN);//地址域A  00000000
  TxBuffer[GCOUNT]     =ctbuf[0];              //计数器
  TxBuffer[GCOUNT+1]   =ctbuf[1];              //计数器
  TxBuffer[GTYPE]      =AFNdata;		//AFN功能码
  TxBuffer[GNULL]=0;
  memcpy(&TxBuffer[GDATA],Gdata,datarealen);	//传输数据到数据域
  TxBuffer[GCS]	=calcrc(&TxBuffer[GCOMMAND],TxBuffer[GLEN]);
  TxBuffer[G_ETX]	=GETX;
  Encry(&TxBuffer[GNULL],datarealen+2,&dargs.pwd[0],6);
}


//定时发送数据 
void GPRS_SendNormalData(void)
{
if(g_sAppContext.eGPRSState!=eGPRS_ONLINE)//不在线，没有登录在gprs上
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
	
	TxCache[23]=(getMediaOutWaterTemper);//由美的机组获取的水箱出水温度	 
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
  TimerStart(TT_GPRS_NORAML_UP,dargs.upload_min*60*1000); //分钟为单位	
	
}

//发送故障错误代码
void GPRS_SendErrorCode(u8 id, u8 errCode)
{
  TxCache[0]=id;
  TxCache[1]=errCode;	 
 
  countbuf[0]=0;
  countbuf[1]=1;
  fill_data(AFN_MACHINE_ERR,CMD_LOG,TxCache,2,countbuf);
  TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
}

//定时心跳包数据
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
  TimerStart(TT_GPRS_HEART,dargs.g_hartheattime*60*1000);//启动心跳包（分钟为单位）	
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
//分析接收数据
void RxAnlyse(void)
{
  u8 i;
  u16 getYear;
  u8 getMon;
  u8 getDat;
  u8 getHour;
  u8 getMin;
  u8 getSec;



  //判断数据头是否正确
  if(RxBuffer[GINCLUDE]!=GSTX)
  {
#ifdef DBG_RS232
    USART_printf(USART1,"Received Head Error.\r\n");	
#endif
    return;
  }	
  //判断数据尾是否正确
  if(RxBuffer[RxBuffer[GLEN] + 4 ]!=GETX)
  {
#ifdef DBG_RS232
    USART_printf(USART1,"Received End Error.\r\n");	
#endif
    return;
  }	
  //判断地址是否正确
  if(addresscmp(&dargs.devno[0],&RxBuffer[GADDRESS]))
  {
#ifdef DBG_RS232
    USART_printf(USART1,"Received Address Error.\r\n");	
#endif
    return;
  }	

 
  //判断校验和是否正确
  //心跳包不加密
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
  //下载密码不加密
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
  //其他的接收数据都加密
  	Decry(&RxBuffer[13],RxBuffer[GLEN]-10,&(dargs.pwd[0]),6);  
  }
 
  //转存计数器的数值
  countbuf[0] = RxBuffer[GCOMMAND+0]; 
  countbuf[1] = RxBuffer[GCOMMAND+1];

#ifdef DBG_RS232
    //USART_printf(USART1,"GTYPE:%d\r\n",RxBuffer[GTYPE]);	
#endif

//--2012.12.7  
  TimerStop(TT_GPRS_TCP_COMMUNICATE);//停止发送
  send_st=0;//复位
  TcpSendCt=0;//复位

  //判断功能码
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
 	  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//读取所有内容
	  SPI_FLASH_SectorErase(0);//擦除内容
      dargs.g_hartheattime = RxBuffer[GDATA];//修改
	  SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));//写入所有设置

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
	  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//读取所有内容
	  SPI_FLASH_SectorErase(0);//擦除内容
      dargs.pwd[0]=RxBuffer[GDATA+0];//转存密码,修改内容
      dargs.pwd[1]=RxBuffer[GDATA+1];
      dargs.pwd[2]=RxBuffer[GDATA+2];
      dargs.pwd[3]=RxBuffer[GDATA+3];
      dargs.pwd[4]=RxBuffer[GDATA+4];
      dargs.pwd[5]=RxBuffer[GDATA+5];
	  SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));//写入所有设置

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

      TxCache[23]=(getMediaOutWaterTemper)&0xFF;//美的机组获取的参数
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
    case AFN_PC_REQ_HISDAT://补采集历史数据
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
	  TimerStart(9,_5S);//5秒后启动补采数据
      break;
    case AFN_PC_REQ_ARGS://查询PC下发的命令
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
    case AFN_PC_SET_ARGS://PC下发的命令
	  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//读取所有内容
	  SPI_FLASH_SectorErase(0);//擦除内容
      dargs.setMideaDegree0=RxBuffer[GDATA+0]; //修改
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
	  SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));//写入所有设置

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
	  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//读取所有内容
	  SPI_FLASH_SectorErase(0);//擦除内容
      dargs.upload_min = (RxBuffer[GDATA+0]<<8) | (RxBuffer[GDATA+1]);//修改  
	  SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));//写入所有设置

      TxCache[0]=DEV_SUCESS_DATA0;
      TxCache[1]=DEV_SUCESS_DATA1;
      fill_data(AFN_PC_SET_UPTIME,CMD_LOG,TxCache,2,countbuf);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;
    case AFN_PC_REQ_STATUS:	//查询机组状态
      TxCache[0]=DEV_SUCESS_DATA0;
      TxCache[1]=getMediaMSG[0].run_m; 
      TxCache[2]=getMediaMSG[0].online_m;  
      TxCache[3]=dargs.setMideaLoopDegree; 	//设置的循环水温度
      TxCache[4]=oc_status;//机组开关机状态

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
    case AFN_PC_SET_WLEV: //公司自带水位探头，设置水位
 	  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//读取所有内容
	  SPI_FLASH_SectorErase(0);//擦除内容
      dargs.setMideaWaterLevel=RxBuffer[GDATA+0]; //0-100%  1-75%  2-50%  3-25%	
	  SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));//写入所有设置

      TxCache[0]=DEV_SUCESS_DATA0;
      TxCache[1]=DEV_SUCESS_DATA1;
      fill_data(AFN_PC_SET_WLEV,CMD_LOG,TxCache,2,countbuf);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;
    case AFN_PC_REQ_WDEGREE: //设置水温
      dargs.setMideaDegree0=RxBuffer[GDATA+0]; // 设定的水箱温度
      TxCache[0]=DEV_SUCESS_DATA0;
      TxCache[1]=DEV_SUCESS_DATA1;
      fill_data(AFN_PC_REQ_WDEGREE,CMD_LOG,TxCache,2,countbuf);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;
    case AFN_PC_SET_OPENCLOSE: //自动开关机
 	  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//读取所有内容
	  SPI_FLASH_SectorErase(0);//擦除内容
      dargs.setIfAutoOnOff=RxBuffer[GDATA+0];  	
      dargs.setOnTime[0]=RxBuffer[GDATA+1];  
      dargs.setOnTime[1]=RxBuffer[GDATA+2];  
      dargs.setOffTime[0]=RxBuffer[GDATA+3];  
      dargs.setOffTime[1]=RxBuffer[GDATA+4]; 
	  SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));//写入所有设置 	  
	   
      TxCache[0]=DEV_SUCESS_DATA0;
      TxCache[1]=DEV_SUCESS_DATA1;
      fill_data(AFN_PC_SET_OPENCLOSE,CMD_LOG,TxCache,2,countbuf);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;
    case AFN_PC_SET_IOPENCLOSE:	  //实时开关机 
 	  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//读取所有内容
	  SPI_FLASH_SectorErase(0);//擦除内容
	  dargs.setIfQuickOnOff=RxBuffer[GDATA+0];  
	  SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));//写入所有设置
      TxCache[0]=DEV_SUCESS_DATA0;
      TxCache[1]=DEV_SUCESS_DATA1;
      fill_data(AFN_PC_SET_IOPENCLOSE,CMD_LOG,TxCache,2,countbuf);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;
    case AFN_LOG://接收到登陆包应答
     // TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break; 
    case AFN_HEART://接收心跳包
      //TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break; 
    case AFN_MACHINE_ERR://机器故障
      //TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;
    case AFN_UP_NORMALDAT://上传定时数据
      //TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;
    case AFN_UP_ADOPTTDAT://上传采集数据
      //TimerStart(TT_GPRS_TCP_COMMUNICATE,_100mS);
      break;

  }

}


void sim900b_send_tcp_data(void)
{
  u8 send_len;
  char command_send[128] = {0};

  if(send_st==0)//启动发送
  {
    send_len = TxBuffer[GLEN]+5;
#ifdef DBG_RS232
    USART_printf(USART1,"AT+CIPSEND=%d\r\n",send_len);	
#endif
    sprintf(command_send,"AT+CIPSEND=%d\r\n",send_len);	
    SendATCommand(command_send);
	  TimerStart(5, _1S);//	1秒启动
  }
  else if(send_st==1)//发送数据
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

  /* 获取返回数据的长度 */
  while((*pCmdResult == '\r')||(*pCmdResult == '\n'))
  {
    ++pCmdResult;
  }

#ifdef DBG_RS232
  USART_printf(USART1, "\r\n");	

  USART_printf(USART1, (u8 *)pCmdResult);
  USART_printf(USART1, "\r\n");	
#endif


 
  if ((strstr(pCmdResult, "RDY")!= NULL)) //------- //模块起动主动输出字符串
 	{
#ifdef DBG_RS232
      USART_printf(USART1,"++MODULE:AT-Command Interpreter ready\r\n");	
#endif 

      TimerStart(TT_GPRS_SEARCH_CARD,_5S);
      result = TRUE;
    }
    else if((strstr(pCmdResult, "+IPR:")!= NULL))  //插卡
    {
#ifdef DBG_RS232
      USART_printf(USART1,"++MODULE:IPR\r\n");	
#endif
	  TimerStop(11); 
      TimerStart(TT_GPRS_SEARCH_CARD,_5S);
      result = TRUE;
    }
    else if ((strstr(pCmdResult, "+CPIN:") != NULL)) //------------------------ //有SIM卡
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
    else if((strstr(pCmdResult, "CONNECT OK") != NULL)) //-----------------------TCPIP连接服务器IP端口号成功
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
	  TimerStart(6,_500mS);//蜂鸣器
	  TimerStart(8,dargs.upload_min*60*1000);//定时上报数据	,是不是要移动到开始
	  TimerStart(TT_GPRS_HEART,dargs.g_hartheattime*60*1000);//启动心跳包（分钟为单位）
      result = TRUE; 	  	  
    }
    else if((strstr(pCmdResult, "CONNECT FAIL") != NULL)) //-----------------------TCPIP连接服务器IP端口号成功
    {
      if(++TcpConnectCt>60)//每5S连接一次，若5分钟还是fail重新向运营商申请分配ip
      {
		TcpConnectCt=0;
		g_sAppContext.eGPRSState=eGPRS_OFFLINE;
		TimerStop(TT_GPRS_TCP_COMMUNICATE);
		TimerStart(TT_GPRS_TCP,_1S);
      }
      result = TRUE; 	  	  
    }
    else if((strstr(pCmdResult, "ALREAY CONNECT") != NULL)) //-----------------------TCPIP连接服务器IP端口号成功
    {
      result = TRUE; 	  	  
    }
    else if((strstr(pCmdResult, "SEND OK") != NULL)) //-- 发送成功 
    {
//#ifdef DBG_RS232
//      USART_printf(USART1,"++MODULE:IPSEND\r\n");	
//#endif
      send_st=0;
      //测试
      //g_Senddata=time_set(TT_GPRS_TCP_OK,_1S);
      TcpSendCt=0;
      //if(TcpSendCt>0)
      //	--TcpSendCt;
      TimerStop(TT_GPRS_TCP_COMMUNICATE);
      result=TRUE;
    }
    else if( (strstr(pCmdResult, "+IPD,") != NULL)  ) //----------------------收到服务器一条GPRS数据
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
      ++p;//跳过":"
      szIndex[nIndex] = '\0';
      Rssi = atoi(&szIndex[0]);

      for(nIndex=0;nIndex<Rssi;nIndex++)
      {
		RxBuffer[nIndex]=*p++;					  	
 
      }

      RxAnlyse();
      result = TRUE;
    }

    else if((strstr(pCmdResult, "CLOSED") != NULL))//关闭连接  , 重新连接
    {
      TcpConnectCt=0;
      g_sAppContext.eGPRSState=eGPRS_OFFLINE;
      TimerStop(TT_GPRS_TCP_COMMUNICATE);
      TimerStart(TT_GPRS_INIT,_1S);
      result = TRUE; 	
    }
    else if((strstr(pCmdResult, "+CREG:") != NULL)) //------------------------//读取基站信息
    {
#ifdef DBG_RS232
      USART_printf(USART1,"++MODULE: CREG\r\n");	
#endif 
      result = TRUE; 	
    }
    else if((strstr(pCmdResult, ">") != NULL))        //------------------------//发送信息
    {
//#ifdef DBG_RS232
//      USART_printf(USART1,"++MODULE: >\r\n");	
//#endif 
      send_st=1;
      TimerStop(TT_GPRS_TCP_COMMUNICATE);
      TimerStart(TT_GPRS_TCP_COMMUNICATE,_50mS);//1s后启动发送
      result = TRUE;
    }						  
    else if((strstr(pCmdResult, "+CMTI:") != NULL))	//------------------------//收到一条短信
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
    else if((strstr(pCmdResult, "+CMGR:") != NULL))	 //------------------------ //读出一条短信
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
    else if((strstr(pCmdResult, "+CGATT:") != NULL)) //GPRS是否附着
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
    else if((strstr(pCmdResult, "+CGREG:") != NULL)) //------------------------GPRS网络注册状态
    {
      //+CGREG: 1,1     已注册，本地网
#ifdef DBG_RS232
      USART_printf(USART1,"++MODULE:+CGREG\r\n");	
#endif
      result = TRUE;
    }
    else if((strstr(pCmdResult, "%IPCLOSE:") != NULL)) //-----------------------查询是否退出TCPIP连接
    {
      //%IPCLOSE: 5    退出TCPIP功能，GPRS网络注销
      //%IPCLOSE: 0,0,0    连接关闭
      //%IPCLOSE: 1,0,0   连接一条TCPIP 
#ifdef DBG_RS232
      //USART_printf(USART1,"++MODULE:IPCLOSE\r\n");	
#endif
      result = TRUE;	    	  	  
    }
    else  if((strchr(pCmdResult, '.') != NULL))  //申请到ip
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
      else //初始化结束，进入连接tcp过程
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
//短信内容
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
  delay_ms(1000);//兼容SIM900B拉低至少1s
  GPRS_OPEN(); */
  
  MIC29302_ON();
  Delay(100000);//
  GPRS_OPEN();
  Delay(50000); //300
  GPRS_CLOSE();
  Delay(100000);//兼容SIM900B拉低至少1s
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
  else//其他误码
  {
    g_sATback.AT_RX_GL=0;
    g_sATback.AT_RX_STA=0;
  }
}
