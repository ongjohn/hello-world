#include "tft_lcd.h"
#include "Delay.h"
#include "usart1.h"
#include "font.h"

#define LINE

u8 display_direction;
const u8 hzChar[]="设备置地址时间端口号一二路恢复出厂请输入位年月日分秒状态是否正在保存成功光电表进回量水使用环境故障温度代码箱位联线高运行机组连接断中央远程热泵控制系统查询台吨通讯密码上报心跳";



__inline void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
#ifdef LINE
  LCD_WR_CMD(R32, Ypos);
  LCD_WR_CMD(R33, 319-Xpos);
#else
  LCD_WR_CMD(R32, Xpos);
  LCD_WR_CMD(R33, Ypos);
#endif


} 


void LCD_OpenWindow(u16 x,u16 y,u16 len,u16 wid)
{                    
  LCD_WR_CMD(R80, y);
  LCD_WR_CMD(R81,y+wid-1);

  LCD_WR_CMD(R82, x);
  LCD_WR_CMD(R83, x+len-1);

  LCD_SetCursor(x,y); 
  LCD_PREPARE_WR();

 
}

void LCD_CloseWindow(void)
{
  LCD_WR_CMD(R80, 0x0000); //水平方向GRAM起始地址
  LCD_WR_CMD(R81, 0x00EF); //水平方向GRAM结束地址
  LCD_WR_CMD(R82, 0x0000); //垂直方向GRAM起始地址
  LCD_WR_CMD(R83, 0x013F); //垂直方向GRAM结束地址
}

void LCD_Point(u16 x,u16 y,u16 color)
{
  LCD_SetCursor(x,y); //设置位置
  LCD_PREPARE_WR();   //准备写0X22
  LCD_WR_Data(color);	 

} 

static u32 abs(s32 res)
{
    if(res < 0)
        return -res;
    else
        return res;
}

/*
 * 函数名：LCD_DrawPoint
 * 描述  ：在坐标(x,y)处画一个点
 * 输入  ：-x 0~239
           -y 0~319
 * 输出  ：无 
 */
void LCD_DrawPoint(u8 x,u16 y,u16 color)
{
    LCD_WR_CMD(0x0050, x); 
    LCD_WR_CMD(0x0051, 0x00EF);
    LCD_WR_CMD(0x0052, y); 
    LCD_WR_CMD(0x0053, 0x013F); 
    LCD_WR_CMD(0X20, x); 	
    LCD_WR_CMD(0X21, y); 
    LCD_WR_REG(0X22);
    
    LCD_WR_Data(color);	
} 

/*
 * 函数名：LCD_DrawLine
 * 描述  ：在液晶中以(x1,y1)为起点,(x2,y2)为终点画一直线
 * 输入  ：-x1 0~239
           -y1 0~319
					 -x2 0~239
					 -y2 0~319       
 * 输出  ：无 
 */ 
void LCD_DrawLine(u8 x1, u16 y1, u8 x2, u16 y2,u16 cc)
{
    u16 x, y, t;
    if( (x1 == x2) && (y1 == y2) )
       // LCD_DrawPoint(x1, y1,cc);
		LCD_Point(x1,y1,cc);
    else if( abs(y2 - y1) > abs(x2 - x1) )			//斜率大于1 
    {
        if(y1 > y2) 
        {
            t = y1;
            y1 = y2;
            y2 = t; 
            t = x1;
            x1 = x2;
            x2 = t; 
        }
        for(y=y1; y<y2; y++)									//以y轴为基准 
        {
            x = (u32)(y-y1)*(x2-x1) / (y2-y1) + x1;
            //LCD_DrawPoint(x, y,cc);  
			LCD_Point(x,y,cc);
        }
    }
    else     																	//斜率小于等于1 
    {
        if(x1 > x2)
        {
            t = y1;
            y1 = y2;
            y2 = t;
            t = x1;
            x1 = x2;
            x2 = t;
        }   
        for(x=x1; x<x2; x++)									//以x轴为基准 
        {
            y = (u32)(x-x1)*(y2-y1) / (x2-x1) + y1;
            //LCD_DrawPoint(x, y,cc);
			LCD_Point(x,y,cc); 
        }
    } 
}

void LCD_Rectangle(u16 x,u16 y,u16 len,u16 wid,u16 rgb565)
{                    
  u32 n, temp;

  LCD_OpenWindow(x,y,len,wid);   
  temp = (u32)len*wid;    
  for(n=0; n<temp; n++)
  {
    LCD_WR_Data( rgb565 );
  }	  
  LCD_CloseWindow();
}


void LCD_RectangleS(u16 x,u16 y,u16 len,u16 wid,u16 rgb565)
{                    
  u32 n, temp;

  LCD_WR_CMD(R80, x);
  LCD_WR_CMD(R81, x+len-1);
  
  LCD_WR_CMD(R82, y);
  LCD_WR_CMD(R83,y+wid-1);

  LCD_WR_CMD(R32, x);
  LCD_WR_CMD(R33, y);
  LCD_PREPARE_WR();
    
  temp = (u32)len*wid;    
  for(n=0; n<temp; n++)
  {
    LCD_WR_Data( rgb565 );
  }	  
  LCD_WR_CMD(R80, 0x0000); //水平方向GRAM起始地址
  LCD_WR_CMD(R81, 0x00EF); //水平方向GRAM结束地址
  LCD_WR_CMD(R82, 0x0000); //垂直方向GRAM起始地址
  LCD_WR_CMD(R83, 0x013F); //垂直方向GRAM结束地址

}

u16 LCD_RD_REG(__IO u16 regadr)
{
  LCD_WR_REG(regadr);
  return ( *(__IO u16 *) (Bank1_LCD_D) );
}

void LCD_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* 使能FSMC时钟*/
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

  /* 使能FSMC对应相应管脚时钟*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE , ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  /* 配置LCD背光控制管脚*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;		
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOB, GPIO_Pin_0);


  /* 配置LCD复位控制管脚*/
  //    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ; 	 
  //    GPIO_Init(GPIOE, &GPIO_InitStructure);  		   

  /* 配置FSMC相对应的数据线,FSMC-D0~D15: PD 14 15 0 1,PE 7 8 9 10 11 12 13 14 15,PD 8 9 10*/	
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | 
    GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
    GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
    GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure); 

  /* 配置FSMC相对应的控制线
   * PD4-FSMC_NOE  :LCD-RD
   * PD5-FSMC_NWE  :LCD-WR
   * PD7-FSMC_NE1  :LCD-CS
   * PD11-FSMC_A16 :LCD-DC
   */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);  

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 ; 	  
  GPIO_Init(GPIOD, &GPIO_InitStructure);


  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ;     
  GPIO_Init(GPIOD, &GPIO_InitStructure);    

  /* tft control gpio init */	 
  GPIO_SetBits(GPIOB, GPIO_Pin_0);		 // 背光打开
  //GPIO_ResetBits(GPIOE, GPIO_Pin_1);	 	 //	RST = 1   

  GPIO_SetBits(GPIOD, GPIO_Pin_4);		 // RD = 1  
  GPIO_SetBits(GPIOD, GPIO_Pin_5);		 // WR = 1 
  GPIO_SetBits(GPIOD, GPIO_Pin_7);		 //	CS = 1 

  GPIO_SetBits(GPIOD, GPIO_Pin_12);		  //?
  GPIO_ResetBits(GPIOD, GPIO_Pin_13);

  GPIO_ResetBits(GPIOD, GPIO_Pin_3);
  GPIO_SetBits(GPIOD, GPIO_Pin_6); 
}

void LCD_FSMC_Config(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  p; 

  p.FSMC_AddressSetupTime = 0x02;	 //地址建立时间
  p.FSMC_AddressHoldTime = 0x00;	 //地址保持时间
  p.FSMC_DataSetupTime = 0x05;		 //数据建立时间
  p.FSMC_BusTurnAroundDuration = 0x00;
  p.FSMC_CLKDivision = 0x00;
  p.FSMC_DataLatency = 0x00;
  p.FSMC_AccessMode = FSMC_AccessMode_B;	 // 一般使用模式B来控制LCD

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p; 

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

  /* 使能 FSMC Bank1_SRAM Bank */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  
}


void LCD_Init_Reg(void)
{
//  u16 devcode;

//  devcode = LCD_RD_REG(0x0000); 
  LCD_BG_ON();
  LCD_BG_OFF();

//#ifdef DBG_RS232
//  USART_printf(USART1,"id code is:%d\r\n",devcode); 
//#endif

  LCD_WR_CMD(0x00,0x0000);
  LCD_WR_CMD(0x01,0x0100);	//Driver Output Contral.
  LCD_WR_CMD(0x02,0x0700);	//LCD Driver Waveform Contral.
  LCD_WR_CMD(0x03,0x1030);//Entry Mode Set.

  LCD_WR_CMD(0x04,0x0000);	//Scalling Contral.
  LCD_WR_CMD(0x08,0x0202);	//Display Contral 2.(0x0207)
  LCD_WR_CMD(0x09,0x0000);	//Display Contral 3.(0x0000)
  LCD_WR_CMD(0x0a,0x0000);	//Frame Cycle Contal.(0x0000)
  LCD_WR_CMD(0x0c,(1<<0));	//Extern Display Interface Contral 1.(0x0000)
  LCD_WR_CMD(0x0d,0x0000);	//Frame Maker Position.
  LCD_WR_CMD(0x0f,0x0000);	//Extern Display Interface Contral 2.	    
  Delay(5); 
  LCD_WR_CMD(0x07,0x0101);	//Display Contral.
  Delay(5); 								  
  LCD_WR_CMD(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));	//Power Control 1.(0x16b0)
  LCD_WR_CMD(0x11,0x0007);								//Power Control 2.(0x0001)
  LCD_WR_CMD(0x12,(1<<8)|(1<<4)|(0<<0));				//Power Control 3.(0x0138)
  LCD_WR_CMD(0x13,0x0b00);								//Power Control 4.
  LCD_WR_CMD(0x29,0x0000);								//Power Control 7.

  LCD_WR_CMD(0x2b,(1<<14)|(1<<4));	    
  LCD_WR_CMD(0x50,0);	//Set X Star
  //水平GRAM终止位置Set X End.
  LCD_WR_CMD(0x51,239);	//Set Y Star
  LCD_WR_CMD(0x52,0);	//Set Y End.t.
  LCD_WR_CMD(0x53,319);	//

  LCD_WR_CMD(0x60,0x2700);	//Driver Output Control.
  LCD_WR_CMD(0x61,0x0001);	//Driver Output Control.
  LCD_WR_CMD(0x6a,0x0000);	//Vertical Srcoll Control.

  LCD_WR_CMD(0x80,0x0000);	//Display Position? Partial Display 1.
  LCD_WR_CMD(0x81,0x0000);	//RAM Address Start? Partial Display 1.
  LCD_WR_CMD(0x82,0x0000);	//RAM Address End-Partial Display 1.
  LCD_WR_CMD(0x83,0x0000);	//Displsy Position? Partial Display 2.
  LCD_WR_CMD(0x84,0x0000);	//RAM Address Start? Partial Display 2.
  LCD_WR_CMD(0x85,0x0000);	//RAM Address End? Partial Display 2.

  LCD_WR_CMD(0x90,(0<<7)|(16<<0));	//Frame Cycle Contral.(0x0013)
  LCD_WR_CMD(0x92,0x0000);	//Panel Interface Contral 2.(0x0000)
  LCD_WR_CMD(0x93,0x0001);	//Panel Interface Contral 3.
  LCD_WR_CMD(0x95,0x0110);	//Frame Cycle Contral.(0x0110)
  LCD_WR_CMD(0x97,(0<<8));	//
  LCD_WR_CMD(0x98,0x0000);	//Frame Cycle Contral.	   
  LCD_WR_CMD(0x07,0x0173);	//(0x0173)
}

 

void LCD_ClrFix(u16 x,u16 y,u16 x_len,u16 y_len)
{
  u16 i,j;

  LCD_BG_ON();	     
 
  //LCD_OpenWindow(x,y,x_len,y_len);
  for(i=x;i<(x+x_len);i++)
  {
  	for(j=y;j<(y+y_len);j++)
	{
		LCD_Point(i,j,RED);
	}		
  }
}

void LCD_Clr(void)
{
  u32 i;

  LCD_BG_ON();	     
  LCD_SetCursor(0x00,0x0000);//设置光标位置 
  LCD_PREPARE_WR(); 
  for(i=0; i<(320*240); i++)		 		
  {
    LCD_WR_Data(BLACK); 					    
  } 
}

void LCD_Initial(void)
{
  u32 i; 

  LCD_GPIO_Config();
  LCD_FSMC_Config();		
  LCD_Init_Reg();	

  LCD_BG_ON();	     
  LCD_PREPARE_WR();   
  for(i=0; i<(320*240); i++)		 		
  {
    LCD_WR_Data(RED); 					    
  }  
}

void LCD_Char_O(u16 x, u16 y, u8 acsii,u16 Color)
{ 
#ifdef LINE

#define MAX_CHAR_POSX (320-8)
#define MAX_CHAR_POSY (240-16)
  //#define MAX_CHAR_POSX 232
  //#define MAX_CHAR_POSY 304 

  u8 temp, t, pos;
  u16 x0=x;

  if(x > MAX_CHAR_POSX || y > MAX_CHAR_POSY)
    return;

  //LCD_OpenWindow(x, y,16,24);
  acsii-=' ';
  for (pos=0;pos<48;pos++) //16
  {
    temp=ascii_8x16[acsii][pos];  //	  

    for(t=0; t<8; t++)
    {
      if(temp & 0x01)
      {
	LCD_Point(x,y,Color);	
      }
 

      x++;
      if((x-x0)==16)
      {
	x=x0;
	y++;
	break;
      }

      temp >>= 1;	
    }
    //x=x0;
    //y++;

    //LCD_CloseWindow();
  }

#undef MAX_CHAR_POSX
#undef MAX_CHAR_POSY   
#else
#define MAX_CHAR_POSX (320-8)
#define MAX_CHAR_POSY (240-16)
  //#define MAX_CHAR_POSX 232
  //#define MAX_CHAR_POSY 304 

  u8 temp, t, pos;


  if(x > MAX_CHAR_POSX || y > MAX_CHAR_POSY)
    return;

  LCD_open_windows(x,y,8,16); 

  acsii-=' ';
  for (pos=0;pos<16;pos++)
  {
    temp=ascii_8x16[acsii][pos];  //	  

    for(t=0; t<8; t++)
    {
      if(temp & 0x01)
      {
	LCD_Point(x+t,y+pos,Color);	
      }

      temp >>= 1;	
    }

  }

  LCD_WR_CMD(R80, 0x0000); //水平方向GRAM起始地址
  LCD_WR_CMD(R81, 0x00EF); //水平方向GRAM结束地址
  LCD_WR_CMD(R82, 0x0000); //垂直方向GRAM起始地址
  LCD_WR_CMD(R83, 0x013F); //垂直方向GRAM结束地址

#undef MAX_CHAR_POSX
#undef MAX_CHAR_POSY  
#endif

}


s8 find_chn(const u8 *asc)
{
  u8 i;

  for(i=0;i<sizeof(hzChar);i+=2)
  {
    if( (*asc)==hzChar[i] )
      if( (*(asc+1))==hzChar[i+1] )
	return (i/2);
  }

  return -1;
}

void LCD_CHN(u16 x, u16 y,const u8* str,u16 Color)
{
#define MAX_CHAR_POSX (320-8)
#define MAX_CHAR_POSY (240-16)

  u8 temp, t, pos;
  u16 x0=x;
  u16 idx;

  //LCD_OpenWindow( x,  y,320,24);
  idx = find_chn(str);
  idx*=72;

  for (pos=0;pos<72;pos++)
  {

    temp= HZLib[idx+pos];

    for(t=0; t<8; t++)
    {
      if(temp & 0x01)
      {
	LCD_Point(x,y,Color);	
      }
      x++;
      if((x-x0)==24)
      {
	x=x0;
	y++;
	break;
      }
      temp>>=1;
    }
  }
  //LCD_CloseWindow();
}
void LCD_ShowString(u16 x, u16 y,const u8 *str,u16 Color)
{
  while(*str != '\0')
  {       
    if(*str<=0x80)
    {
      if(x > (320-12))//8 
      {	
	x = 0;
	y += 24;
      }
      if(y > (240-24)) //16
      {	
	x = y = 0;
      }
      LCD_Char_O(x , y, *str,Color);

      x += 16;//8
      str ++ ;
    }
    else
    {
      if(x > (320-24)) 
      {	
	x = 0;
	y += 24;
      }
      if(y > (240-24)) 
      {	
	x = y = 0;
      }
      LCD_CHN(x,y,str,Color);
      x+=24;
      str+=2;
    }
  }
}






