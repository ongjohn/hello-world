#include "buzz.h"
#include "led.h"
#include "usart1.h"
#include "time_malloc.h"
#include "time_outProcess.h"
#include "gprs.h"
#include "storage.h"  
#include "rs485_rs2.h" 
#include "rs485_rs3.h" 
#include "ui.h"	  
#include "tft_lcd.h"


u8 TcpInitCt;
u8 flush_lcd_flag;
u8 state_tcp;


void processUserTimerOut(u8 chIndex)
{
  switch(chIndex)
  {
    case 0: //寻卡
	  state_tcp=0;
#ifdef DBG_RS232
      USART_printf(USART1,"AT+CPIN?\r\n");	
#endif
      g_sAppContext.eInitState = eInitStart;
      SendATCommand("AT+CPIN?\r");
      TimerStart(TT_GPRS_SEARCH_CARD, _5S);
      break;
    case 1: //初始化流程
		TcpInitCt=0;
		state_tcp=1;
      if(g_sAppContext.eInitState == eInitStart)     //-----------------
      {
#ifdef DBG_RS232
	USART_printf(USART1,"ATE0\r\n");	
#endif
    
	SendATCommand("ATE0\r");
      }
      else if(g_sAppContext.eInitState == eIniteProductVer)
      {
#ifdef DBG_RS232
	USART_printf(USART1,"AT+CGSN\r\n");	
#endif
	SendATCommand("AT+CGSN\r");
      }
      else if(g_sAppContext.eInitState == eIniteModulVer)
      {
#ifdef DBG_RS232
	USART_printf(USART1,"AT+CGMR\r\n");	
#endif
	SendATCommand("AT+CGMR\r");
      }
      else if(g_sAppContext.eInitState == eInithead)
      {
#ifdef DBG_RS232
	USART_printf(USART1,"AT+CIPHEAD=1\r\n");	
#endif
	SendATCommand("AT+CIPHEAD=1\r");
      }
      else if(g_sAppContext.eInitState == eInitcreg)	   //-----------------
      { 
#ifdef DBG_RS232
	USART_printf(USART1,"AT+CREG=1\r\n");	
#endif
	SendATCommand("AT+CREG=1\r");
      }
      else if(g_sAppContext.eInitState == eInitcgreg)	   //-----------------
      { 
#ifdef DBG_RS232
	USART_printf(USART1,"AT+CGREG\r\n");	
#endif
	SendATCommand("AT+CGREG=1\r"); 
      }
      else if(g_sAppContext.eInitState == eInitImei)	   //-----------------
      { 
#ifdef DBG_RS232
	USART_printf(USART1,"AT+CIPSPRT=1\r\n");	
#endif
	SendATCommand("AT+CIPSPRT=1\r");//AT+CIPSNED成功不提示>,提示send ok
      }
      else if(g_sAppContext.eInitState == eInitImsi)   //-----------------
      {
#ifdef DBG_RS232
	USART_printf(USART1,"AT+CNMI=2,1\r\n");	
#endif
	SendATCommand("AT+CNMI=2,1\r");	
      }
      else if(g_sAppContext.eInitState == eInitSMS)   //-----------------
      {
#ifdef DBG_RS232
	USART_printf(USART1,"AT+CSDH=0\r\n");	
#endif
	SendATCommand("AT+CSDH=0\r"); 	

      }
      else if(g_sAppContext.eInitState == eInitSMS1)   //-----------------
      {
#ifdef DBG_RS232
	USART_printf(USART1,"AT+CMGF=1\r\n");	
#endif
	SendATCommand("AT+CMGF=1\r");
      }
      else if(g_sAppContext.eInitState == eInitSMS2)   //----------------- 
      {
#ifdef DBG_RS232
	USART_printf(USART1,"AT+CSCS=\"GSM\"\r\n");	
#endif
	SendATCommand("AT+CSCS=\"GSM\"\r");
      }
      else if(g_sAppContext.eInitState == eInitSMS3)   //----------------- 
      {
#ifdef DBG_RS232
	USART_printf(USART1,"AT+CSMP= 17,167,0,241\r\n");	
#endif
	SendATCommand("AT+CSMP= 17,167,0,241\r");
      }
      else if(g_sAppContext.eInitState == eInitGPRS) //-----------------
      {	 					
#ifdef DBG_RS232
	USART_printf(USART1,"AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n");	
#endif
	SendATCommand("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n");	
      } 
      else if(g_sAppContext.eInitState == eIniteTCPIP)   //----------------- 
      {
#ifdef DBG_RS232
	USART_printf(USART1,"AT+CGATT\r\n");	
#endif
	SendATCommand("AT+CGATT=1\r");	 
      }
      else if(g_sAppContext.eInitState == eInitEnd)
      {
#ifdef DBG_RS232
	USART_printf(USART1,"--SIM900B init end.\r\n");	
#endif
	g_sAppContext.eGPRSState = eGPRS_OFFLINE;
	TimerStop(TT_GPRS_INIT);
	TimerStart(TT_GPRS_TCP,_2S);				   
	break;
      }
      else 
      {
	g_sAppContext.eInitState = eInitStart;
      }
      TimerStart(TT_GPRS_INIT, _1S);	 				   
      break;
    case 2: //............................................................................2
      ProcessIncomingSMS(NULL); 
      break;
    case 3: //............................................................................3
      SendMessage(g_sMessageInfo.szRecNumber, "0");
      break;
    case 4: //............................................................................4
	 	state_tcp=1;
      if(g_sAppContext.eGPRSState==eGPRS_OFFLINE) 
      {
#ifdef DBG_RS232
	USART_printf(USART1,"AT+CIPSHUT\r\n");	
#endif
	SendATCommand("AT+CIPSHUT\r"); 
      }
      if(g_sAppContext.eGPRSState==eGPRS_GTT)
      {
#ifdef DBG_RS232
	USART_printf(USART1,"AT+CGATT?\r\n");	
#endif
	SendATCommand("AT+CGATT?\r");
      }
      else if(g_sAppContext.eGPRSState==eGPRS_APN)
      {
#ifdef DBG_RS232
	USART_printf(USART1,"AT+CSTT=CMNET\r\n");	
#endif
	SendATCommand("AT+CSTT=\"CMNET\"\r");
      }
      else if(g_sAppContext.eGPRSState==eGPRS_UP)
      {
#ifdef DBG_RS232
	USART_printf(USART1,"AT+CIICR\r\n");	
#endif
	SendATCommand("AT+CIICR\r");	
      } 
      else if(g_sAppContext.eGPRSState==eGPRS_GETIP)
      {
#ifdef DBG_RS232
	USART_printf(USART1,"AT+CIFSR\r\n");	
#endif
	SendATCommand("AT+CIFSR\r");
	g_sAppContext.eGPRSState=eGPRS_CALL;		   
      }
      else if(g_sAppContext.eGPRSState==eGPRS_CALL)	 //进入拨号
      {
	char szCommand[128] = {0};
#ifdef DBG_RS232
	USART_printf(USART1,"AT+CIPSTART=\"TCP\",\"%s\",\"%s\"\r\n",g_setting.szIPAddress,g_setting.szPortNumber);	
#endif
	sprintf(szCommand,"AT+CIPSTART=\"TCP\",\"%s\",\"%s\"\r\n",g_setting.szIPAddress,g_setting.szPortNumber);	
	SendATCommand(szCommand);
      }
      else if(g_sAppContext.eGPRSState==eGPRS_OPEN)//gprs连接上平台
      {
#ifdef DBG_RS232
	USART_printf(USART1,"g_sAppContext.eGPRSState=eGPRS_ONLINE");	
#endif
	g_sAppContext.eGPRSState=eGPRS_ONLINE;
	break;
      }
      else  
      {
		g_sAppContext.eGPRSState=eGPRS_OFFLINE;
      }

      if(++TcpInitCt>100)//重新启动GPRS模块
	  {
	  	TcpInitCt=0;   
		GPRS_ShutDown();
		GPRS_StartUp();							 
		TimerStop(4);
		TimerStart(11, _10S);//启动查询卡片
		break;	
	  }
	  TimerStop(7);//停止心跳包	
	  TimerStop(9);//停止补采数据
      TimerStart(4, _5S);	
      break;
    case 5: //TT_GPRS_TCP_COMMUNICATE       
	  	state_tcp=2;
      sim900b_send_tcp_data();
       //TimerStart(5, _800mS);	
      if(++TcpSendCt>30)
      {
		TcpSendCt=0;
		send_st=0;
		TimerStop(5);
		SendATCommand("AT+CIPSHUT\r\n");
		g_sAppContext.eGPRSState=eGPRS_OFFLINE;
	    TimerStart(4, _2S);	
      }
 
      break;
    case 6: // 蜂鸣器
      BUZZ_OFF();
      break;
    case 7: //心跳包
	  GPRS_SendHeart();
      break;
    case 8: // 定时上报数据
	 GPRS_SendNormalData();
      break;
    case 9: //补采数据 
	 //添加代码
 
	  TimerStart(9,_3S);//每3秒发送一次数据
      break;
	case 10: //rs485 rs2 对接接口开启应答信号
	rs485_ack_flag=1;
	 break;  
	
	case 11://正式启动GPRS 
     SendATCommand("AT+IPR?\r\n"); 
	 TimerStart(11,_5S); 
 	 break; 	
	case 12: //读取光电表数据??
	 if(ct_readDate==0)
	 {
	 	rs485_rs3_GetWaterData(dargs.rs485InWaterAddr);
	 }
	 else if(ct_readDate==1)
	 {
	 	rs485_rs3_GetWaterData(dargs.rs485OutWaterAddr);
	 }
	 else if(ct_readDate==3)
	 {
	 	rs485_rs3_GetWaterData(dargs.rs485LoopWaterAddr);
	 }
	 else if(ct_readDate==4)
	 {
	 	rs485_rs3_GetWaterData(dargs.rs485ElecAddr);
	 }
	 if(++ct_readDate>4)
	 {
	 	ct_readDate=0;
	 }
	 TimerStart(12,_30S); 
 	 break; 	
	case 13:		//5s刷新显示一次
	flush_lcd_flag=1;
  	 break; 
	 						   
    default :   break;	 						
  }
  return ;
}

