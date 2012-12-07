/*
 *  ds18b20_1  - pinA11
 *  ds18b20_2  - pinA12
 */
#include "ds18b20.h"
#include "delay.h"					 		   


void DS18B20_IO_OUT(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //��©���      
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void DS18B20_IO_IN()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}




//��λDS18B20
void DS18B20_Rst(void)	   
{                 
  DS18B20_IO_OUT(); //SET PA0 OUTPUT
  DS18B20_DQ_OUT_LOW; //����DQ
  delay_us(750);    //����750us
  DS18B20_DQ_OUT_HIGH; //DQ=1 
  delay_us(15);     //15US
}
//�ȴ�DS18B20�Ļ�Ӧ
//����1:δ��⵽DS18B20�Ĵ���
//����0:����
u8 DS18B20_Check(u8 cn) 	   
{   
  u8 retry=0;
  DS18B20_IO_IN();//SET PA0 INPUT	

  switch(cn)
  {
    case DS18B20_T_0:
      while (DS18B20_DQ_IN&&retry<200)
      {
	retry++;
	delay_us(1);
      };	 
      if(retry>=200)return 1;
      else retry=0;
      while (!DS18B20_DQ_IN&&retry<240)
      {
	retry++;
	delay_us(1);
      }
      break;

    case DS18B20_T_1:
      while (DS18B20_DQ_IN_CON16&&retry<200)
      {
	retry++;
	delay_us(1);
      };	 
      if(retry>=200)return 1;
      else retry=0;
      while (!DS18B20_DQ_IN_CON16&&retry<240)
      {
	retry++;
	delay_us(1);
      }
      break;

  } 

  if(retry>=240)return 1;	    
  return 0;
}

//
//�ڶ�·�¶�
//
u8 DS18B20_Check_Con16(void) 	   
{   
  u8 retry=0;
  DS18B20_IO_IN();//SET PA0 INPUT	 
  while (DS18B20_DQ_IN_CON16&&retry<200)
  {
    retry++;
    delay_us(1);
  };	 
  if(retry>=200)return 1;
  else retry=0;
  while (!DS18B20_DQ_IN_CON16&&retry<240)
  {
    retry++;
    delay_us(1);
  };
  if(retry>=240)return 1;	    
  return 0;
}


//��DS18B20��ȡһ��λ
//����ֵ��1/0
u8 DS18B20_Read_Bit(u8 tn) 			 // read one bit
{
  u8 data;
  DS18B20_IO_OUT();//SET PA0 OUTPUT
  DS18B20_DQ_OUT_LOW; 
  delay_us(2);
  DS18B20_DQ_OUT_HIGH; 
  DS18B20_IO_IN();//SET PA0 INPUT
  delay_us(12);

  switch(tn)
  {
    case DS18B20_T_0:
      if(DS18B20_DQ_IN)data=1; else data=0;
      break;
    case DS18B20_T_1:
      if(DS18B20_DQ_IN_CON16)data=1; else data=0;
      break;
  }


  delay_us(50);           
  return data;
}


//��DS18B20��ȡһ���ֽ�
//����ֵ������������
u8 DS18B20_Read_Byte(u8 n)    // read one byte
{        
  u8 i,j,dat;
  dat=0;
  for (i=1;i<=8;i++) 
  {
    j=DS18B20_Read_Bit(n);
    dat=(j<<7)|(dat>>1);
  }						    
  return dat;
}
//дһ���ֽڵ�DS18B20
//dat��Ҫд����ֽ�
void DS18B20_Write_Byte(u8 dat)     
{             
  u8 j;
  u8 testb;
  DS18B20_IO_OUT();//SET PA0 OUTPUT;
  for (j=1;j<=8;j++) 
  {
    testb=dat&0x01;
    dat=dat>>1;
    if (testb) 
    {
      DS18B20_DQ_OUT_LOW;// Write 1
      delay_us(2);                            
      DS18B20_DQ_OUT_HIGH;
      delay_us(60);             
    }
    else 
    {
      DS18B20_DQ_OUT_LOW;// Write 0
      delay_us(60);             
      DS18B20_DQ_OUT_HIGH;
      delay_us(2);                          
    }
  }
}
//��ʼ�¶�ת��
static void DS18B20_Start(u8 start_n)// ds1820 start convert
{   						               
  DS18B20_Rst();	   
  DS18B20_Check(start_n);	 
  DS18B20_Write_Byte(0xcc);// skip rom
  DS18B20_Write_Byte(0x44);// convert
} 





void DS18B20_Initial(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_Init(GPIOA, &GPIO_InitStructure);


}


//��ds18b20�õ��¶�ֵ
//���ȣ�0.1C
//����ֵ���¶�ֵ ��-550~1250��
//num: @arg DS18B20_T_0  DS18B20_T_1 
short DS18B20_Get_Temp(u8 num)
{
  u8 temp;
  u8 TL,TH;
  short tem;
  DS18B20_Start(num);                    // ds1820 start convert
  DS18B20_Rst();
  DS18B20_Check(num);	 
  DS18B20_Write_Byte(0xcc);// skip rom
  DS18B20_Write_Byte(0xbe);// convert	    
  TL=DS18B20_Read_Byte(num); // LSB   
  TH=DS18B20_Read_Byte(num); // MSB  

  if(TH>7)
  {
    TH=~TH;
    TL=~TL; 
    temp=0;//�¶�Ϊ��  
  }else temp=1;//�¶�Ϊ��	  	  
  tem=TH; //��ø߰�λ
  tem<<=8;    
  tem+=TL;//��õװ�λ
  tem=(float)tem*0.625;//ת��     
  if(temp)return tem; //�����¶�ֵ
  else return -tem;    
} 






