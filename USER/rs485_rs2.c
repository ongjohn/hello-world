/*
 *RS2接口
 *使用串口3	,band:4800 8 1 N
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
u8 rs485_ack_time;//应答时间，以100ms为一次间隔
u8 rs485_ack_flag;//应答标志
u8 oc_status;
u8 rd_idx;
u8 rs485_ack_flag;
u16 rs485_send_time;

u8 getMediaOutWaterTemper;//从美的机组主机读取到的出水水箱温度,经过计算转化的温度
u8 getMediaEnvTemper;//从美的机组主机读取到的环境温度,经过计算转化的温度
u8 getMediaWaterHigh;//从美的机组主机得到的水位高度



MEIDA_MSG_T getMediaMSG[15];//美的机组+线控器一共可以带16台设备
u8 recordERROR[15];//记录故障上报标志位

static u8 crc_chksum(u8* dat)
{
	u8 i;
	u8 sum=0;
	
	for(i=1;i<29;i++)//命令码开始累加
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
	rs485_rs2_txbuf[0]	=0xAA;						//起始码
	rs485_rs2_txbuf[1]  =oc_status;					//命令码,是否让那个热泵机组开机或则关机
	rs485_rs2_txbuf[2]	=dargs.rs485_adr1;			//源地址
	rs485_rs2_txbuf[3]	=0x00;						//目的地址
	rs485_rs2_txbuf[4]	=0x00;						//第一目的地址
	rs485_rs2_txbuf[5]	=dargs.setMideaDegree0;		//出水温度设置 0-250度	  				*
	rs485_rs2_txbuf[6]	=dargs.setMideaLoopDegree;	//循环热水温度设置 1-250度				*
	rs485_rs2_txbuf[7]	=dargs.setMideaWaterLevel;	//水位设置,0-S1(4格) 1-S2(3格) 2-S3(1格)*
	rs485_rs2_txbuf[8]	=0;							//定时信息 0-不含定时信息 1-含定时信息
	rs485_rs2_txbuf[9]	=dargs.setMideaIfLoop;		//管网循环 1-有 0-无					*
	rs485_rs2_txbuf[10]	=dargs.setMideaWaterOffset;	//水位偏差 0-250						*
	rs485_rs2_txbuf[11]	=dargs.setMideaIfHot;		//设置电加热辅助设置	 				*	
	rs485_rs2_txbuf[12]	=dargs.setMideaHandOpenNumbers;			//设置手动开启机组台数	 				*
	rs485_rs2_txbuf[13]	=0;							//线控器状态改变     
	rs485_rs2_txbuf[14]	=0;							//线控器为1的时间值	
	rs485_rs2_txbuf[19]	=0;							//< 地暖设置温度0-250度 >
	rs485_rs2_txbuf[20]	=0;							//< 地暖信号 1-开 0-关  >
	rs485_rs2_txbuf[30] =crc_chksum(rs485_rs2_txbuf);
	rs485_rs2_txbuf[31]	=0x55;
	
	RS485_RS2_EN();
	USART_SendNByte(USART3,rs485_rs2_txbuf,32);
	RS485_RS2_DIS();
}


/*
 * 记录故障到缓冲区	, 有故障就上报
 */ 
void copy_data(u8 index)
{
 
	if(index>15) return;

	getMediaMSG[index].board_outwater_t=rs485_rs2_rxbuf[RS485_DAT+5-1]/2-15;	//板出水温度
	getMediaMSG[index].err_code[0]=rs485_rs2_rxbuf[RS485_DAT+7-1]; 		//故障代码
	getMediaMSG[index].err_code[1]=rs485_rs2_rxbuf[RS485_DAT+8-1];		//故障代码
	getMediaMSG[index].protect_code[0]=rs485_rs2_rxbuf[RS485_DAT+9-1];	//保护代码 
	getMediaMSG[index].protect_code[1]=rs485_rs2_rxbuf[RS485_DAT+10-1];	//保护代码 
	getMediaMSG[index].cool_t[0]=rs485_rs2_rxbuf[RS485_DAT+11-1]/2-15;		 //冷凝器温度
	getMediaMSG[index].cool_t[1]=rs485_rs2_rxbuf[RS485_DAT+12-1]/2-15;		 //冷凝器温度
	getMediaMSG[index].compress_c[0]=rs485_rs2_rxbuf[RS485_DAT+13-1];	    //压缩机电流
	getMediaMSG[index].compress_c[1]=rs485_rs2_rxbuf[RS485_DAT+14-1];	    //压缩机电流
	getMediaMSG[index].output_state=rs485_rs2_rxbuf[RS485_DAT+17-1];	    //输出状态
	getMediaMSG[index].water_t=rs485_rs2_rxbuf[RS485_DAT+19-1];				//水箱温度
	getMediaMSG[index].outdoor_t=rs485_rs2_rxbuf[RS485_DAT+20-1];		    //环境温度
	getMediaMSG[index].online_m=rs485_rs2_rxbuf[RS485_DAT+21-1];		    //在线机组数
	getMediaMSG[index].run_m=rs485_rs2_rxbuf[RS485_DAT+22-1];  			//运行机组数
	getMediaMSG[index].water_level=	rs485_rs2_rxbuf[RS485_DAT+24-1];	    //水位高度
	getMediaMSG[index].outwater_t=rs485_rs2_rxbuf[RS485_DAT+25-1]; 		//出水温度设置
	getMediaMSG[index].water_offset=rs485_rs2_rxbuf[RS485_DAT+26-1];	    //水位偏差
												 
	if(g_sAppContext.eGPRSState!=eGPRS_ONLINE) return;//未连接,不上报


//===================== E_xx故障代码上报 ==========================//
	if( (recordERROR[index]&ERR0_MASK)==0x00)//检测是否有故障
	{
		if((getMediaMSG[index].err_code[0]&ERR0_DET)==ERR0_DET)//E0故障
		{
			GPRS_SendErrorCode(index,ERR_0);
 			recordERROR[index]|=ERR0_MASK;
		}	
	}
 	else if( (recordERROR[index]&ERR0_MASK)==ERR0_MASK)//检测故障是否恢复
 	{
		if((getMediaMSG[index].err_code[0]&ERR0_DET)==0)//
		{
			GPRS_SendErrorCode(index,0);
 			recordERROR[index]&=~ERR0_MASK;
		}
	}
																	   
	if( (recordERROR[index]&ERR1_MASK)==0x00)
	{
		if((getMediaMSG[index].err_code[0]&ERR1_DET)==ERR1_DET)//E1故障
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
		if((getMediaMSG[index].err_code[0]&ERR2_DET)==ERR2_DET)//E2故障
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
		if((getMediaMSG[index].err_code[0]&ERR3_DET)==ERR3_DET)//E3故障
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
		if((getMediaMSG[index].err_code[0]&ERR4_DET)==ERR4_DET)//E4故障
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
		if((getMediaMSG[index].err_code[0]&ERR5_DET)==ERR5_DET)//e5故障
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
		if((getMediaMSG[index].err_code[0]&ERR6_DET)==ERR6_DET)//E0故障
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
		if((getMediaMSG[index].err_code[0]&ERR7_DET)==ERR7_DET)//E7故障
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
		if((getMediaMSG[index].err_code[1]&ERR8_DET)==ERR8_DET)//E8故障
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

//====================== P_xx保护代码上报 ==========================//
	if( (recordERROR[index]&PRO0_MASK)==0x00)
	{
		 if((getMediaMSG[index].protect_code[0]&PRO0_DET)==PRO0_DET)//说明有故障,进行上报
		 {
			GPRS_SendErrorCode(index,PRO_0);
			recordERROR[index]|=PRO0_MASK;
		 }
	}																	 
	else if( (recordERROR[index]&PRO0_MASK)==PRO0_MASK)//等待恢复
	{
		if((getMediaMSG[index].protect_code[0]&PRO0_DET)==0x00)
		{		
		 	GPRS_SendErrorCode(index,0);
			recordERROR[index]&=~PRO0_MASK;	
		}	
	}
																  
	if( (recordERROR[index]&PRO1_MASK)==0x00)
	{
		 if((getMediaMSG[index].protect_code[0]&PRO1_DET)==PRO1_DET)//说明有故障,进行上报
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
		 if((getMediaMSG[index].protect_code[0]&PRO2_DET)==PRO2_DET)//说明有故障,进行上报
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
		 if((getMediaMSG[index].protect_code[0]&PRO3_DET)==PRO3_DET)//说明有故障,进行上报
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
		 if((getMediaMSG[index].protect_code[0]&PRO4_DET)==PRO4_DET)//说明有故障,进行上报
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
		 if((getMediaMSG[index].protect_code[0]&PRO5_DET)==PRO5_DET)//说明有故障,进行上报
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
		 if((getMediaMSG[index].protect_code[0]&PRO6_DET)==PRO6_DET)//说明有故障,进行上报
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
		 if((getMediaMSG[index].protect_code[0]&PRO7_DET)==PRO7_DET)//说明有故障,进行上报
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
		 if((getMediaMSG[index].protect_code[1]&PRO8_DET)==PRO8_DET)//说明有故障,进行上报
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
		 if((getMediaMSG[index].protect_code[1]&PRO9_DET)==PRO9_DET)//说明有故障,进行上报
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
	u8 at;//应答时间

 
	//判断是否定时开机
	if(dargs.setIfAutoOnOff==1)//开启了定时开机、关机功能
	{		
		if( (timer.hour==dargs.setOnTime[0]) && (timer.min==dargs.setOnTime[1]) )//开机
		{
			oc_status=0x02;
		}   		

		if( (timer.hour==dargs.setOffTime[0]) && (timer.min==dargs.setOffTime[1]) )//关机
		{
			oc_status=0x08;
		}
	}
	//485数据正常接收
 	if(rd_idx>=32)
	{
		rd_idx=0;

		if(rs485_rs2_rxbuf[0]!=0xAA)//头信息不对
		{
			goto err_end;
		}
 
		if(rs485_rs2_rxbuf[31]!=0x55)//尾信息不对
		{

			goto err_end;
		}

	   if( (rs485_rs2_rxbuf[2]==0x00) && (rs485_rs2_rxbuf[3]==0xFF) )//主机广播信息,应答 
	   { 
			getMediaOutWaterTemper=(rs485_rs2_rxbuf[RS485_DAT+OFFSET_T5]/2)-15;//取水箱温度,精度0.5
			getMediaEnvTemper=(rs485_rs2_rxbuf[RS485_DAT+OFFSET_T4]/2)-15;//取环境温度
			getMediaWaterHigh=rs485_rs2_rxbuf[RS485_DAT+OFFSET_WATER]; //水位高度
	
			copy_data(0);
	   		if(dargs.rs485_adr1==0) //是主机
			{	
			}
	   		else//其余都是从机 ,应答信号，包括线控器
			{
				at=(dargs.rs485_adr1&0x0F);//1-15地址
				if(dargs.rs485_adr1>0x0F)//大于地址15
				{
					at=16;
				}
				TimerStart(TT_RS485_RS2_ACK,at*100);//启动应答发送时间，间隔100ms应答机制 	

 			}
	   }
		   
	   if( (rs485_rs2_rxbuf[3]==0x00)&& (dargs.rs485_adr1==0xFF) )//普通从机信息,接收其他从机的信息转存到cache存储
	   {
 	   	  copy_data(rs485_rs2_rxbuf[2]);	
	   }
err_end:
	   memset(&rs485_rs2_rxbuf[0],0,32);   
	}

	if(rs485_ack_flag)//需要应答
	{
		
		rs485_ack_flag=0;

		if(dargs.rs485_adr1==0xFF) //线控器应答
		{
		 	rs485_rs2_send();
		}
		else//其余热泵从机应答
		{
		  //以后添加
		}
 	}
}
