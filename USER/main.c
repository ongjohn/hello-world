#include <string.h>
#include "stm32f10x.h"
#include "usart1.h"
#include "led.h"
#include "delay.h"
#include "key.h"
#include "ds18b20.h"
#include "buzz.h"
#include "input_output.h"
#include "spi_flash.h"
#include "tft_lcd.h"
#include "time_malloc.h" 
#include "time_outProcess.h"
#include "timer1.h"
#include "ui.h"
#include "storage.h"  
#include "rtc.h"    
#include "GPRS.h"   
#include "rs485_rs2.h"	 
#include "rs485_rs3.h"	 
#include "ADC.h"



u32 t;
u32 DeviceID,FlashID;


 
int main(void)
{ 
  SystemInit();			//配置系统时钟为 72M 
  delay_init(72);
  time_init();
  RTC_Init();
  BUZZ_Initial();               //蜂鸣器IO口初始化
  IO_Initial();                 //蜂鸣器IO口初始化
  DS18B20_Initial();
  LED_Initial();
  KEY_Initial();
  SPI_FLASH_Init();
  storage_init();
  LCD_Initial();
  DisMenuTopInit();
  GPRS_Initial();		
  USART_Initial();  		//串口初始化
  TIM_Initial();
  rs485_rs2_init();
  rs485_rs3_init();
  ADC1_Init();
  GPRS_StartUp();
  
  DeviceID = SPI_FLASH_ReadDeviceID();
  FlashID = SPI_FLASH_ReadID(); 
  printf("FlashID is 0x%X,  Manufacturer Device ID is 0x%X\r\n", FlashID, DeviceID);

  /* Check the SPI Flash ID */
  if (FlashID == 0xEF3015)  /* #define  sFLASH_ID  0xEF3015 */
  {
    printf("flash W25X16 OK!\r\n");
  }
  else
  {    
    printf("W25X16 ERROR!\r\n");
  }

  TimerStart(11,_5S);
  TimerStart(13,_5S);

  
  while (1)
  {
    KeyProc();
	
	RS485rs2Proc();//处理rs485事件	
	//RS485rs3Proc();  
  
	if(flush_lcd_flag)
	{
		flush_lcd_flag=0;
		ui_flush_idle_display();  
		TimerStart(13,_5S);

	}
   }
}
