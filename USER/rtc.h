#ifndef __RTC_H
#define __RTC_H	    
 
#include "stm32f10x.h"
//ʱ��ṹ��
__packed typedef struct 
{
  vu8 hour;
  vu8 min;
  vu8 sec;			
  //������������
  vu16 w_year;
  vu8  w_month;
  vu8  w_date;
  vu8  week;		 
}tm;	
				 
extern tm timer; 
extern u8 const mon_table[12];//�·��������ݱ�

u8 RTC_Init(void);
u8 RTC_Get(void);         //����ʱ��   
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);//����ʱ��	  

#endif




























 
