#include <string.h> 
#include "stm32f10x_it.h"
#include "usart1.h"
#include "time_malloc.h"
#include "time_outProcess.h"
#include "key.h"
#include "rtc.h"
#include "gprs.h"  
#include "rs485_rs2.h"	 
#include "rs485_rs3.h"	
#include "input_output.h"	
#include "storage.h"

 
u8 flag_500ms;
u8 res;
u8 data[4];
u32 rxAddr;

/** @addtogroup Template_Project
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 * @param  None
 * @retval : None
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval : None
 */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval : None
 */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval : None
 */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval : None
 */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval : None
 */
void SVC_Handler(void)
{
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval : None
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval : None
 */
void PendSV_Handler(void)
{
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval : None
 */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
 * @brief  This function handles PPP interrupt request.
 * @param  None
 * @retval : None
 */
/*void PPP_IRQHandler(void)
  {
  }*/

/**
 * @}
 */ 


//RS1-rs232
void USART1_IRQHandler(void)
{
 
  if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)!=RESET)
  {
 
  }
}

// GPRS
void USART2_IRQHandler(void)
{
  if(USART_GetFlagStatus(USART2,USART_FLAG_RXNE)!=RESET)
  {
    if(g_sATback.AT_RX_GL>AT_GL)g_sATback.AT_RX_GL=AT_GL;
    g_sATback.ATbackStr[g_sATback.AT_RX_GL]=USART_ReceiveData(USART2);
    g_sATback.AT_RX_GL++;
    if(((g_sATback.ATbackStr[g_sATback.AT_RX_GL-1])=='\r')||((g_sATback.ATbackStr[g_sATback.AT_RX_GL-1])=='\n'))
    {
      g_sATback.AT_RX_STA=1;
    }
  }	
}

//RS485_RS2
void USART3_IRQHandler(void)
{

  if(USART_GetFlagStatus(USART3,USART_FLAG_RXNE)!=RESET)
  {				 
		rs485_rs2_rxbuf[rd_idx]=USART_ReceiveData(USART3);
 
		++rd_idx;
		if(rs485_rs2_rxbuf[0]!=0xAA)//丢弃
		{
			rd_idx=0;
		}
  }					  
}

//光电表_RS3
void UART4_IRQHandler(void)
{
  if(USART_GetFlagStatus(UART4,USART_FLAG_RXNE)!=RESET)
  {				 
		rs485_rs3_rxbuf[rs485_rs3_rxIndex]=USART_ReceiveData(UART4);
 		//rs485_rs3_rxIndex=USART_ReceiveData(UART4);
		++rs485_rs3_rxIndex;


		
	   	if((rs485_rs3_rxbuf[FRAME_NULL+0]!=0xFE) ) 
		{
			rs485_rs3_rxIndex=0;
			CONTROL_12V_PA15_OUT_DIS;
			return;
		}
        
		if(rs485_rs3_rxIndex>=(rs485_rs3_rxbuf[FRAME_DLEN]+12+3))//长度匹配
		{
			
 			if((rs485_rs3_rxbuf[FRAME_START]!=0x68)||rs485_rs3_rxbuf[FRAME_RX_END]!=0x16) return;

//			rxcheckSum=crc(&rs485_rs3_rxbuf[FRAME_START],rs485_rs3_rxbuf[FRAME_DLEN]+11);
//			//校验和错误 
//			if(rxcheckSum!=rs485_rs3_rxbuf[FRAME_RX_CRC]) return;
			
			//判断是哪个表的地址
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
				return;			
			}
		//正确
			switch(rs485_rs3_rxbuf[FRAME_CTRL])
			{
				case 0x81://读表数据
					data[0]=bcd_to_hex(rs485_rs3_rxbuf[FRAME_DATA+3]);
					data[1]=bcd_to_hex(rs485_rs3_rxbuf[FRAME_DATA+4]);
					data[2]=bcd_to_hex(rs485_rs3_rxbuf[FRAME_DATA+5]);
					data[3]=bcd_to_hex(rs485_rs3_rxbuf[FRAME_DATA+6]);
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
			
			rs485_rs3_rxIndex=0;
			CONTROL_12V_PA15_OUT_DIS;	
		}  
  }	
}

//预留
void UART5_IRQHandler(void)
{
  if(USART_GetFlagStatus(UART5,USART_FLAG_RXNE)!=RESET)
  {
 
  }
}

void TIM2_IRQHandler(void)
{
  if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET ) 
  {	
    TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);    

    KEY_Scan();


    if(Timer0) { Timer0--;}//延时 
    if(Timer1) { Timer1--;}//延时 
    if(Timer2) { Timer2--;}//延时 
    if(Timer3) { Timer3--;}//延时 
    if(Timer4) { Timer4--;}//延时 
    if(Timer5) { Timer5--;}//延时 
    if(Timer6) { Timer6--;}//延时 
    if(Timer7) { Timer7--;}//延时 
    if(Timer8) { Timer8--;}//延时 
    if(Timer9) { Timer9--;}//延时  
    if(Timer10) { Timer10--;}//延时    
    if(Timer11) { Timer11--;}//延时 
    if(Timer12) { Timer12--;}//延时 
    if(Timer13) { Timer13--;}//延时 

    if((TimerStart0==1)&&(Timer0==0)){TimerStart0=0,processUserTimerOut(0);}
    else if((TimerStart1==1)&&(Timer1==0)){TimerStart1=0,processUserTimerOut(1);}
    else if((TimerStart2==1)&&(Timer2==0)){TimerStart2=0,processUserTimerOut(2);}
    else if((TimerStart3==1)&&(Timer3==0)){TimerStart3=0,processUserTimerOut(3);}
    else if((TimerStart4==1)&&(Timer4==0)){TimerStart4=0,processUserTimerOut(4);}
    else if((TimerStart5==1)&&(Timer5==0)){TimerStart5=0,processUserTimerOut(5);}
    else if((TimerStart6==1)&&(Timer6==0)){TimerStart6=0,processUserTimerOut(6);}
    else if((TimerStart7==1)&&(Timer7==0)){TimerStart7=0,processUserTimerOut(7);}
    else if((TimerStart8==1)&&(Timer8==0)){TimerStart8=0,processUserTimerOut(8);}
    else if((TimerStart9==1)&&(Timer9==0)){TimerStart9=0,processUserTimerOut(9);}  
    
	if((TimerStart10==1)&&(Timer10==0)){TimerStart10=0,processUserTimerOut(10);} 
	if((TimerStart11==1)&&(Timer11==0)){TimerStart10=0,processUserTimerOut(11);}
	if((TimerStart12==1)&&(Timer12==0)){TimerStart10=0,processUserTimerOut(12);}
	if((TimerStart13==1)&&(Timer13==0)){TimerStart10=0,processUserTimerOut(13);}

  }
}

void TIM4_IRQHandler(void)
{
  if ( TIM_GetITStatus(TIM4 , TIM_IT_Update) != RESET ) 
  {	
    //TIM_ClearITPendingBit(TIM4 , TIM_FLAG_Update);    
 	GprsProc();
  }
  TIM_ClearITPendingBit(TIM4 , TIM_FLAG_Update); 
}


void RTC_IRQHandler(void)
{
  if (RTC_GetITStatus(RTC_IT_SEC) != RESET)//秒钟中断
  {							
    RTC_Get();//更新时间   
  }
  if(RTC_GetITStatus(RTC_IT_ALR)!= RESET)//闹钟中断
  {
    RTC_ClearITPendingBit(RTC_IT_ALR);		//清闹钟中断	  	   
  } 				  								 
  RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);		//清闹钟中断
  RTC_WaitForLastTask();	
}

