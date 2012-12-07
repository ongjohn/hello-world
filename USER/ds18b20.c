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
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //开漏输出      
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




//复位DS18B20
void DS18B20_Rst(void)	   
{                 
  DS18B20_IO_OUT(); //SET PA0 OUTPUT
  DS18B20_DQ_OUT_LOW; //拉低DQ
  delay_us(750);    //拉低750us
  DS18B20_DQ_OUT_HIGH; //DQ=1 
  delay_us(15);     //15US
}
//等待DS18B20的回应
//返回1:未检测到DS18B20的存在
//返回0:存在
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
//第二路温度
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


//从DS18B20读取一个位
//返回值：1/0
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


//从DS18B20读取一个字节
//返回值：读到的数据
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
//写一个字节到DS18B20
//dat：要写入的字节
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
//开始温度转换
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


//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250）
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
    temp=0;//温度为负  
  }else temp=1;//温度为正	  	  
  tem=TH; //获得高八位
  tem<<=8;    
  tem+=TL;//获得底八位
  tem=(float)tem*0.625;//转换     
  if(temp)return tem; //返回温度值
  else return -tem;    
} 






