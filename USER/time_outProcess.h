#ifndef	TIME_OUT_PROCESS_H
#define	TIME_OUT_PROCESS_H
#include "stm32f10x.h"

//GPRS操作时间		
#define	_50mS			(50) 
#define	_100mS			(100) 
#define	_500mS			(500) 
#define	_800mS			(800) 
#define	_1S			(1000) 
#define	_2S			(2000) 	
#define	_3S			(3000)
#define	_5S			(5000)
#define	_10S		(1000*10) 
#define	_30S		(3000*10)

//GPR操作类型 
#define	TT_GPRS_SEARCH_CARD	0 
#define	TT_GPRS_INIT		1 
#define	TT_GPRS_IN_MSG	        2	
#define	TT_GPRS_SND_MSG	        3	
#define	TT_GPRS_TCP	        4 
#define	TT_GPRS_TCP_COMMUNICATE	5
#define	TT_BUZZ			6
#define	TT_GPRS_HEART			7	
#define	TT_GPRS_NORAML_UP			8 
#define	TT_GPRS_ 			9		  
#define	TT_RS485_RS2_ACK			10


extern u8 flush_lcd_flag;
extern u8 state_tcp;
extern u8 TcpInitCt;
//function proto
extern void processUserTimerOut(u8 chIndex);
#endif


