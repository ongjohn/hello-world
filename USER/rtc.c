#include "rtc.h" 
#include "usart1.h"


tm timer;//时钟结构体 
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表	  
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};



void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  BKP_DeInit();

  /* Enable LSE */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}

  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Second */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec */
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}


u8 RTC_Init(void)
{
  NVIC_Configuration();

  if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    /* RTC Configuration */
    RTC_Configuration();

    BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
  }
  else
  {
    /* Check if the Power On Reset flag is set */
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
      //printf("\r\n\n Power On Reset occurred....");
    }
    /* Check if the Pin Reset flag is set */
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
      //printf("\r\n\n External Reset occurred....");
    }

    //printf("\r\n No need to configure RTC....");
    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    /* Enable the RTC Second */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
  }

  return 0;
}



u8 Is_Leap_Year(u16 year)
{			  
  if(year%4==0) //必须能被4整除
  { 
    if(year%100==0) 
    { 
      if(year%400==0)return 1;//如果以00结尾,还要能被400整除 	   
      else return 0;   
    }else return 1;   
  }else return 0;	
}	


u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{	
  u16 temp2;
  u8 yearH,yearL;

  yearH=year/100;	yearL=year%100; 
  // 如果为21世纪,年份数加100  
  if (yearH>19)yearL+=100;
  // 所过闰年数只算1900年之后的  
  temp2=yearL+yearL/4;
  temp2=temp2%7; 
  temp2=temp2+day+table_week[month-1];
  if (yearL%4==0&&month<3)temp2--;
  return(temp2%7);
} 

u8 RTC_Get(void)
{
  static u16 daycnt=0;
  u32 timecount=0; 
  u32 temp=0;
  u16 temp1=0;	  

  timecount=RTC_GetCounter();			 

  temp=timecount/86400;    
  if(daycnt!=temp)//超过一天了
  {	  
    daycnt=temp;
    temp1=1970;	//从1970年开始
    while(temp>=365)
    {				 
      if(Is_Leap_Year(temp1))//是闰年
      {
	if(temp>=366)temp-=366;//闰年的秒钟数
	else {temp1++;break;}  
      }
      else temp-=365;	  //平年 
      temp1++;  
    }   
    timer.w_year=temp1;//得到年份
    temp1=0;
    while(temp>=28)//超过了一个月
    {
      if(Is_Leap_Year(timer.w_year)&&temp1==1)//当年是不是闰年/2月份
      {
	if(temp>=29)temp-=29;//闰年的秒钟数
	else break; 
      }
      else 
      {
	if(temp>=mon_table[temp1])temp-=mon_table[temp1];//平年
	else break;
      }
      temp1++;  
    }
    timer.w_month=temp1+1;//得到月份
    timer.w_date=temp+1;  //得到日期 

  }
  temp=timecount%86400;     //得到秒钟数   	   
  timer.hour=temp/3600;     //小时
  timer.min=(temp%3600)/60; //分钟	
  timer.sec=(temp%3600)%60; //秒钟
  timer.week=RTC_Get_Week(timer.w_year,timer.w_month,timer.w_date);//获取星期  
 

  return 0;
}



u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
  u16 t;
  u32 seccount=0;
  if(syear<1970||syear>2099)return 1;	   
  for(t=1970;t<syear;t++)	//把所有年份的秒钟相加
  {
    if(Is_Leap_Year(t))seccount+=31622400;//闰年的秒钟数
    else seccount+=31536000;			  //平年的秒钟数
  }
  smon-=1;
  for(t=0;t<smon;t++)	   //把前面月份的秒钟数相加
  {
    seccount+=(u32)mon_table[t]*86400;//月份秒钟数相加
    if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//闰年2月份增加一天的秒钟数	   
  }
  seccount+=(u32)(sday-1)*86400;//把前面日期的秒钟数相加 
  seccount+=(u32)hour*3600;//小时秒钟数
  seccount+=(u32)min*60;	 //分钟秒钟数
  seccount+=sec;//最后的秒钟加上去

  //设置时钟
  RCC->APB1ENR|=1<<28;//使能电源时钟
  RCC->APB1ENR|=1<<27;//使能备份时钟
  PWR->CR|=1<<8;    //取消备份区写保护
  //上面三步是必须的!

  RTC_SetCounter(seccount);
  RTC_WaitForLastTask();

  return 0;	    
}

