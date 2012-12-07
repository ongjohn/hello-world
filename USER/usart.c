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
 * ��������USART1_Config
 * ����  ��USART1 GPIO ����,����ģʽ���á�115200 8-N-1
 * �ӿ�	 ��MAX232�ӿ�
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
 * ��������USART2_Config  - gprs
 * ����  ��USART2 GPIO ����,����ģʽ���á�9600 8-N-1
 * �ӿ�	 ��GPRS�ӿ�
 * ���  : ��
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
 * ��������USART3 - RS485 
 * ����  ��USART3 GPIO ����,����ģʽ���á�4800 8-N-1	 
 *		   PB10(TX)
 * 		   PB11(RX)
 *		   PB12(RS485EN)
 * �ӿ�	 ��RS485�ӿ� - RS2_INTERFACE
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
 * ��������USART4 - RS485 - RS3_INTERFACE
 * ����  ��USART4 GPIO ����,����ģʽ���á�9600 8-N-1	 
 *		   PC10(TX)
 *		   PC11(RX)
 *		   PC12(RS485EN)
 * �ӿ�	 ��rs485�ӿ�
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
 * ��������USART5 - TTL - RS3_INTERFACE
 * ����  ��USART4 GPIO ����,����ģʽ���á�9600 8-N-1	 
 * �ӿ�	 ��TTL�ӿ�
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
 * ��������fputc
 * ����  ���ض���c�⺯��printf��USART1
 * ����  ����
 * ���  ����
 * ����  ����printf����
 */
int fputc(int ch, FILE *f)
{
  /* ��Printf���ݷ������� */
  USART_SendData(USART1, (unsigned char) ch);
  while (!(USART1->SR & USART_FLAG_TXE));

  return (ch);
}

/*
 * ��������itoa
 * ����  ������������ת�����ַ���
 * ����  ��-radix =10 ��ʾ10���ƣ��������Ϊ0
 *         -value Ҫת����������
 *         -buf ת������ַ���
 *         -radix = 10
 * ���  ����
 * ����  ����
 * ����  ����USART1_printf()����
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
 * ��������USART1_printf
 * ����  ����ʽ�������������C���е�printf��������û���õ�C��
 * ����  ��-USARTx ����ͨ��������ֻ�õ��˴���1����USART1
 *		     -Data   Ҫ���͵����ڵ����ݵ�ָ��
 *			   -...    ��������
 * ���  ����
 * ����  ���� 
 * ����  ���ⲿ����
 *         ����Ӧ��USART1_printf( USART1, "\r\n this is a demo \r\n" );
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

  while ( *Data != 0)     // �ж��Ƿ񵽴��ַ���������
  {
    USART_ClearFlag(USARTx,USART_FLAG_TC); 

    if ( *Data == 0x5c )  //'\'
    {									  
      switch ( *++Data )
      {
	case 'r':							          //�س���
	  USART_SendData(USARTx, 0x0d);
	  Data ++;
	  break;

	case 'n':							          //���з�
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
	case 's':										  //�ַ���
	  s = va_arg(ap, const char *);
	  for ( ; *s; s++) 
	  {
	    USART_SendData(USARTx,*s);
	    while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	  }
	  Data++;
	  break;

	case 'd':										//ʮ����
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

