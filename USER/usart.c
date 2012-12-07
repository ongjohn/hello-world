#include "usart1.h"
#include <stdarg.h>

 


static void USART_NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);

  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;//3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);	

  NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;//4
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);	

}

/*************************************************************************
 * 函数名：USART1_Config
 * 描述  ：USART1 GPIO 配置,工作模式配置。115200 8-N-1
 * 接口	 ：MAX232接口
 *  	   PA9(TX)  
 *  	   PA10(RX) 
 *		   RS1_INTERFAC
 *************************************************************************/
static void USART1_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  /* config USART1 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

  /* USART1 GPIO config */
  /* Configure USART1 Tx (PA.09) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);    
  /* Configure USART1 Rx (PA.10) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* USART1 mode config */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure); 
  USART_ITConfig(USART1,USART_IT_RXNE,ENABLE  );// 
  USART_ITConfig(USART1,USART_IT_PE,ENABLE );//
  USART_ClearFlag(USART1, USART_FLAG_TC); 
  USART_Cmd(USART1, ENABLE);
}

/*************************************************************************
 * 函数名：USART2_Config  - gprs
 * 描述  ：USART2 GPIO 配置,工作模式配置。9600 8-N-1
 * 接口	 ：GPRS接口
 * 输出  : 无
 *************************************************************************/
static void USART2_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);	  	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);	  

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);    

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = 9600;//9600 115200
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure); 

  USART_ITConfig(USART2,USART_IT_RXNE,ENABLE );// 
  USART_ITConfig(USART2,USART_IT_PE,ENABLE );//

  USART_ClearFlag(USART2, USART_FLAG_TC); 
  USART_Cmd(USART2, ENABLE);
}

/*************************************************************************
 * 函数名：USART3 - RS485 
 * 描述  ：USART3 GPIO 配置,工作模式配置。4800 8-N-1	 
 *		   PB10(TX)
 * 		   PB11(RX)
 *		   PB12(RS485EN)
 * 接口	 ：RS485接口 - RS2_INTERFACE
 *************************************************************************/
static void USART3_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 , ENABLE);	  	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);	  

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);    

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  

  USART_InitStructure.USART_BaudRate = 4800;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART3, &USART_InitStructure); 

  USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
  USART_Cmd(USART3, ENABLE);
  USART_ClearFlag(USART3, USART_FLAG_TC); 
}

/*************************************************************************
 * 函数名：USART4 - RS485 - RS3_INTERFACE
 * 描述  ：USART4 GPIO 配置,工作模式配置。9600 8-N-1	 
 *		   PC10(TX)
 *		   PC11(RX)
 *		   PC12(RS485EN)
 * 接口	 ：rs485接口
 *************************************************************************/
static void USART4_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4 , ENABLE);	  	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);	  

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);    

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//GPIO_Speed_2MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure);  

  USART_InitStructure.USART_BaudRate = 1200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(UART4, &USART_InitStructure); 


  USART_ITConfig(UART4,USART_IT_RXNE,ENABLE);
  USART_Cmd(UART4, ENABLE);
  USART_ClearFlag(UART4, USART_FLAG_TC); 
}


/*************************************************************************
 * 函数名：USART5 - TTL - RS3_INTERFACE
 * 描述  ：USART4 GPIO 配置,工作模式配置。9600 8-N-1	 
 * 接口	 ：TTL接口
 *************************************************************************/
static void USART5_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5 , ENABLE);	  	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOD, ENABLE);	  

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);    

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD, &GPIO_InitStructure);


  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(UART5, &USART_InitStructure); 

  USART_ITConfig(UART5,USART_IT_RXNE,ENABLE);
  USART_Cmd(UART5, ENABLE);
  USART_ClearFlag(UART5, USART_FLAG_TC); 
}



/*
 * 函数名：fputc
 * 描述  ：重定向c库函数printf到USART1
 * 输入  ：无
 * 输出  ：无
 * 调用  ：由printf调用
 */
int fputc(int ch, FILE *f)
{
  /* 将Printf内容发往串口 */
  USART_SendData(USART1, (unsigned char) ch);
  while (!(USART1->SR & USART_FLAG_TXE));

  return (ch);
}

/*
 * 函数名：itoa
 * 描述  ：将整形数据转换成字符串
 * 输入  ：-radix =10 表示10进制，其他结果为0
 *         -value 要转换的整形数
 *         -buf 转换后的字符串
 *         -radix = 10
 * 输出  ：无
 * 返回  ：无
 * 调用  ：被USART1_printf()调用
 */
static char *itoa(int value, char *string, int radix)
{
  int     i, d;
  int     flag = 0;
  char    *ptr = string;

  /* This implementation only works for decimal numbers. */
  if (radix != 10)
  {
    *ptr = 0;
    return string;
  }

  if (!value)
  {
    *ptr++ = 0x30;
    *ptr = 0;
    return string;
  }

  /* if this is a negative value insert the minus sign. */
  if (value < 0)
  {
    *ptr++ = '-';

    /* Make the value positive. */
    value *= -1;
  }

  for (i = 10000; i > 0; i /= 10)
  {
    d = value / i;

    if (d || flag)
    {
      *ptr++ = (char)(d + 0x30);
      value -= (d * i);
      flag = 1;
    }
  }

  /* Null terminate the string. */
  *ptr = 0;

  return string;

} /* NCL_Itoa */

/*
 * 函数名：USART1_printf
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库
 * 输入  ：-USARTx 串口通道，这里只用到了串口1，即USART1
 *		     -Data   要发送到串口的内容的指针
 *			   -...    其他参数
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用
 *         典型应用USART1_printf( USART1, "\r\n this is a demo \r\n" );
 *            		 USART1_printf( USART1, "\r\n %d \r\n", i );
 *            		 USART1_printf( USART1, "\r\n %s \r\n", j );
 */
void USART_printf(USART_TypeDef* USARTx, uint8_t *Data,...)
{
  const char *s;
  int d;   
  char buf[16];

  va_list ap;
  va_start(ap, Data);

  while ( *Data != 0)     // 判断是否到达字符串结束符
  {
    USART_ClearFlag(USARTx,USART_FLAG_TC); 

    if ( *Data == 0x5c )  //'\'
    {									  
      switch ( *++Data )
      {
	case 'r':							          //回车符
	  USART_SendData(USARTx, 0x0d);
	  Data ++;
	  break;

	case 'n':							          //换行符
	  USART_SendData(USARTx, 0x0a);	
	  Data ++;
	  break;

	default:
	  Data ++;
	  break;
      }			 
    }
    else if ( *Data == '%')
    {									  //
      switch ( *++Data )
      {				
	case 's':										  //字符串
	  s = va_arg(ap, const char *);
	  for ( ; *s; s++) 
	  {
	    USART_SendData(USARTx,*s);
	    while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	  }
	  Data++;
	  break;

	case 'd':										//十进制
	  d = va_arg(ap, int);
	  itoa(d, buf, 10);
	  for (s = buf; *s; s++) 
	  {
	    USART_SendData(USARTx,*s);
	    while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	  }
	  Data++;
	  break;
	default:
	  Data++;
	  break;
      }		 
    } /* end of else if */
    else USART_SendData(USARTx, *Data++);
    while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
  }
}



void USART_SendNByte(USART_TypeDef* USARTx,u8 *pbuf, u16 len)
{
  u16 i;

  for(i=0;i<len;i++)
  {
    USART_ClearFlag(USARTx,USART_FLAG_TC); 
    USART_SendData(USARTx,pbuf[i]);
    while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );	
  }	
}

 
void USART_SendByte(USART_TypeDef* USARTx,u16 dat)
{
  USART_ClearFlag(USARTx,USART_FLAG_TC); 
  USART_SendData(USARTx, dat);
  while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );	
}




void USART_Initial(void)
{
  USART1_Config();
  USART2_Config();  
  USART3_Config();
  USART4_Config();
  USART5_Config();
  USART_NVIC_Config();
}

