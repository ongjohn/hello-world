#include <string.h>
#include <stdio.h>		 
#include <stdlib.h>
#include "key.h"
#include "ui.h"
#include "tft_lcd.h"
#include "spi_flash.h"
#include "usart1.h"
#include "rtc.h"
#include "storage.h"
#include "delay.h"	 
#include "rs485_rs2.h" 
#include "gprs.h" 
#include "time_outProcess.h"   
#include "time_malloc.h"

s8 Column;					 
s8 Row;
char displaybuf[30];
u16 tot_sector;


  char display_tmp[10];
  u8 realError;
  u8 level_percent;

#define YEAR_START_POS 0
#define MON_START_POS  5 
#define DATE_START_POS 8
#define HOUR_START_POS 11
#define MIN_START_POS  14
#define SEC_START_POS  17



//--------------------------------------------------------
unsigned char KeyFuncIndex        =0;       //按键动作索引
unsigned char MaxScreenDisp       =4;       //屏幕最大显示项

//--------------------------------------------------------
unsigned char CurrentMenuIndex    =0;       //当前菜单索引编号
unsigned char CurrentMenuItem     =0;       //当前菜单项目
unsigned char CurrentMenuItemNum  =0;       //当前菜单项目数
unsigned char **CurrentMenuString =0;       //当前菜单显示内容指针
unsigned char CurrentMenuCur      =0;       //当前菜单光标在屏幕的位置
unsigned char CurrentMenuPage     =0;       //当前菜页数 一页4个选项

//--------------------------------------------------------
unsigned char MenuDeep            =0;       //当前菜单堆栈深度

//-------------------------------------------------------- 用户菜单显示内容
//--------顶层内容
unsigned char * TopMenuString[]={"hello"};

//设置菜单内容
unsigned char * SetMenuString[]=
{
  "1.设置设备地址",
  "2.设置设备时间",
  "3.设置IP地址",
  "4.设置端口号",
  "5.设置一路RS485地址",
  "6.设置进水表地址",  
  "7.设置出水表地址",
  "8.设置回水表地址",
  "9.设置电量表地址",
  "10.恢复出厂设置",	 
  "11.恢复通讯密码"
};

//地址菜单内容 
unsigned char * DevAddrMenuString[]   =
{
  "<请输入12位设备地址>" 
};


unsigned char * TimeMenuString[]   =
{
  "<请输入时间>" 
}; 


unsigned char * IpMenuString[]   =
{
  "<请输入IP地址>" 
}; 

unsigned char * PortMenuString[]   =
{
  "<请输入4位端口号>" 
};


unsigned char * Rs485Addr1MenuString[]   =
{
  "<请输入一路RS485地址>" 
}; 

unsigned char * Rs485Addr2MenuString[]   =
{
  "<请输入进水表RS485地址>",
}; 

unsigned char * Rs485Addr3MenuString[]   =
{
  "<请输入出水表RS485地址>",
}; 
unsigned char * Rs485Addr4MenuString[]   =
{
  "<请输入回水表RS485地址>",
}; 
unsigned char * Rs485Addr5MenuString[]   =
{
  "<请输入电量表RS485地址>",
}; 

unsigned char *ResetFacMenuString[] =
{
  "<恢复出厂设置状态>",
  "1.是",
  "2.否"  
}; 

unsigned char *ResetCommunicateMenuString[] =
{
  "<恢复通讯密码>",
  "1.是",
  "2.否"  
};

//---------------------------------------------------------
//--------菜单结构体
//--------菜单的ID, 也就是菜单的索引号, 是各菜单顺利链接的节点
//--------菜单的标题,
//--------菜单的项目数
//--------菜单的图标
//--------菜单的显示内容
//--------菜单动作执行函数
struct Menu
{
  unsigned char MenuID;
  unsigned char **MenuTitle;
  unsigned char MenuNum;
  unsigned char **MenuIco;
  unsigned char **MenuString;
  void* (*AddAction)();
};

//--------顶层菜单
struct Menu TopMenu=
{
  0,
  0,
  6,
  0,
  TopMenuString,
  (void*)0,
};

//--------设置菜单
struct Menu SetMenu=
{
  6,
  0,
  11,//7
  0,
  SetMenuString,
  (void*)0,
};
//--------地址设置菜单
struct Menu DevAdrMenu=
{
  12,
  0,
  1,
  0,
  DevAddrMenuString,
  (void*)0,
};

//--------时间设置菜单
struct Menu TimeMenu=
{
  18,
  0,
  1,
  0,
  TimeMenuString,
  (void*)0,
};

//--------IP设置菜单
struct Menu IpMenu=
{
  24,
  0,
  1,
  0,
  IpMenuString,
  (void*)0,
};

//--------Port设置菜单
struct Menu PortMenu=
{
  30,
  0,
  1,
  0,
  PortMenuString,
  (void*)0,
}; 
//--------Port设置菜单
struct Menu Rs485addr1Menu=
{
  36,
  0,
  1,
  0,
  Rs485Addr1MenuString,
  (void*)0,
};

//--------进水
struct Menu Rs485addr2Menu=
{
  42,
  0,
  1,
  0,
  Rs485Addr2MenuString,
  (void*)0,
}; 

//--------out水
struct Menu Rs485addr3Menu=
{
  48,//
  0,
  1,
  0,
  Rs485Addr3MenuString,
  (void*)0,
}; 

//--------回水
struct Menu Rs485addr4Menu=
{
  54,//
  0,
  1,
  0,
  Rs485Addr4MenuString,
  (void*)0,
};

//--------电量
struct Menu Rs485addr5Menu=
{
  60,//
  0,
  1,
  0,
  Rs485Addr5MenuString,
  (void*)0,
};

//--------恢复出厂设置设置菜单
struct Menu ResetFacMenu=
{
  66,
  0,
  3,
  0,
  ResetFacMenuString,
  (void*)0,
}; 

//--------恢复通讯设置设置菜单
struct Menu ResetCommunicateMenu=
{
  72,
  0,
  3,
  0,
  ResetCommunicateMenuString,
  (void*)0,
}; 

//------------------------------------------------------------菜单内核
 

//--------菜单堆栈
//--------菜单ID, 也就是索引号
//--------菜单项
//--------菜单项目数
//--------菜单显示内容
//--------光标位置
//--------菜单滚动次数
struct MenuStack
{
  unsigned char MenuStackIndex;
  unsigned char MenuStackItem;
  unsigned char MenuStackItemNum;
  unsigned char **MenuStackString;
  unsigned char MenuStackCur;
  unsigned char MenuStackScroll;
};

 
//--------当前菜单入栈
 

 
//--------顶层菜单操作
void DisMenuTopInit(void);
void DisMenuTopSet(void);
void DisMenuTopEnter(void);
void DisMenuTopUp(void);
void DisMenuTopDown(void);
void DisMenuTopReturn(void);
//--------
void DisMenuLevel0Init(void);
void DisMenuLevel0Set(void);
void DisMenuLevel0Enter(void);
void DisMenuLevel0Up(void);
void DisMenuLevel0Down(void);
void DisMenuLevel0Return(void);
//--------
void DisMenuDevAdrInit(void);
void DisMenuDevAdrSet(void);
void DisMenuDevAdrEnter(void);
void DisMenuDevAdrUp(void);
void DisMenuDevAdrDown(void);
void DisMenuDevAdrReturn(void);
//--------
void DisMenuDevTimeInit(void);
void DisMenuDevTimeSet(void);
void DisMenuDevTimeEnter(void);
void DisMenuDevTimeUp(void);
void DisMenuDevTimeDown(void);
void DisMenuDevTimeReturn(void);
//--------
void DisMenuDevIPInit(void);
void DisMenuDevIPSet(void);
void DisMenuDevIPEnter(void);
void DisMenuDevIPUp(void);
void DisMenuDevIPDown(void);
void DisMenuDevIPReturn(void);
//--------
void DisMenuDevPortInit(void);
void DisMenuDevPortSet(void);
void DisMenuDevPortEnter(void);
void DisMenuDevPortUp(void);
void DisMenuDevPortDown(void);
void DisMenuDevPortReturn(void);
//--------
void DisMenuDevRs485Adr1Init(void);
void DisMenuDevRs485Adr1Set(void);
void DisMenuDevRs485Adr1Enter(void);
void DisMenuDevRs485Adr1Up(void);
void DisMenuDevRs485Adr1Down(void);
void DisMenuDevRs485Adr1Return(void);
//--------
void DisMenuDevRs485Adr2Init(void);
void DisMenuDevRs485Adr2Set(void);
void DisMenuDevRs485Adr2Enter(void);
void DisMenuDevRs485Adr2Up(void);
void DisMenuDevRs485Adr2Down(void);
void DisMenuDevRs485Adr2Return(void);

//--------
void DisMenuDevRs485Adr3Init(void);
void DisMenuDevRs485Adr3Set(void);
void DisMenuDevRs485Adr3Enter(void);
void DisMenuDevRs485Adr3Up(void);
void DisMenuDevRs485Adr3Down(void);
void DisMenuDevRs485Adr3Return(void);

//--------
void DisMenuDevRs485Adr4Init(void);
void DisMenuDevRs485Adr4Set(void);
void DisMenuDevRs485Adr4Enter(void);
void DisMenuDevRs485Adr4Up(void);
void DisMenuDevRs485Adr4Down(void);
void DisMenuDevRs485Adr4Return(void);

//--------
void DisMenuDevRs485Adr5Init(void);
void DisMenuDevRs485Adr5Set(void);
void DisMenuDevRs485Adr5Enter(void);
void DisMenuDevRs485Adr5Up(void);
void DisMenuDevRs485Adr5Down(void);
void DisMenuDevRs485Adr5Return(void);


//--------
void DisMenuResetFacInit(void);
void DisMenuResetFacSet(void);
void DisMenuResetFacEnter(void);
void DisMenuResetFacUp(void);
void DisMenuResetFacDown(void);
void DisMenuResetFacReturn(void);  

//--------
void DisMenuCommunicateInit(void);
void DisMenuCommunicateSet(void);
void DisMenuCommunicateEnter(void);
void DisMenuCommunicateUp(void);
void DisMenuCommunicateDown(void);
void DisMenuCommunicateReturn(void);

//--------当前层菜单操作函数
void (*KeyFuncPtr)();

typedef struct 
{
  unsigned char KeyStateIndex;			//索引
  unsigned char KeySetState;			//设置
  unsigned char KeyEnterState;			//确认
  unsigned char KeyUpState;			//上翻
  unsigned char KeyDownState;			//下翻
  unsigned char KeyReturnState;		//返回
  void (*CurrentOperate)();			//执行
}KeyFuncStruct;

//--------按键索引
KeyFuncStruct KeyTab[]=
{   
  { 0, 1, 2, 3, 4, 5, (DisMenuTopInit)}, //顶层菜单 
  { 1, 0, 0, 0, 0, 0, (DisMenuTopSet)},
  { 2, 0, 0, 0, 0, 0, (DisMenuTopEnter)},
  { 3, 0, 0, 0, 0, 0, (DisMenuTopUp)},
  { 4, 0, 0, 0, 0, 0, (DisMenuTopDown)},
  { 5, 0, 0, 0, 0, 0, (DisMenuTopReturn)},

  { 6, 7, 8, 9, 10,11,(DisMenuLevel0Init)},//第一层设置菜单 
  { 7, 0, 0, 0, 0, 0, (DisMenuLevel0Set)},
  { 8, 0, 0, 0, 0, 0, (DisMenuLevel0Enter)},
  { 9, 0, 0, 0, 0, 0, (DisMenuLevel0Up)},
  { 10, 0, 0, 0, 0,0, (DisMenuLevel0Down)},
  { 11, 0, 0, 0, 0,0, (DisMenuLevel0Return)},

  { 12, 13, 14,15, 16,17,(DisMenuDevAdrInit)}, 
  { 13, 0, 0, 0, 0, 0, (DisMenuDevAdrSet)},
  { 14, 0, 0, 0, 0, 0, (DisMenuDevAdrEnter)},
  { 15, 0, 0, 0, 0, 0, (DisMenuDevAdrUp)},
  { 16, 0, 0, 0, 0,0, (DisMenuDevAdrDown)},
  { 17, 0, 0, 0, 0,0, (DisMenuDevAdrReturn)},

  { 18, 19,20,21,22,23,(DisMenuDevTimeInit)}, 
  { 19, 0, 0, 0, 0, 0, (DisMenuDevTimeSet)},
  { 20, 0, 0, 0, 0, 0, (DisMenuDevTimeEnter)},
  { 21, 0, 0, 0, 0, 0, (DisMenuDevTimeUp)},
  { 22, 0, 0, 0, 0, 0, (DisMenuDevTimeDown)},
  { 23, 0, 0, 0, 0, 0, (DisMenuDevTimeReturn)},

  { 24, 25,26,27,28,29,(DisMenuDevIPInit)}, 
  { 25, 0, 0, 0, 0, 0, (DisMenuDevIPSet)},
  { 26, 0, 0, 0, 0, 0, (DisMenuDevIPEnter)},
  { 27, 0, 0, 0, 0, 0, (DisMenuDevIPUp)},
  { 28, 0, 0, 0, 0, 0, (DisMenuDevIPDown)},
  { 29, 0, 0, 0, 0, 0, (DisMenuDevIPReturn)},

  { 30, 31,32,33,34,35,(DisMenuDevPortInit)}, 
  { 31, 0, 0, 0, 0, 0, (DisMenuDevPortSet)},
  { 32, 0, 0, 0, 0, 0, (DisMenuDevPortEnter)},
  { 33, 0, 0, 0, 0, 0, (DisMenuDevPortUp)},
  { 34, 0, 0, 0, 0, 0, (DisMenuDevPortDown)},
  { 35, 0, 0, 0, 0, 0, (DisMenuDevPortReturn)},

  { 36, 37,38,39,40,41,(DisMenuDevRs485Adr1Init)}, 
  { 37, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr1Set)},
  { 38, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr1Enter)},
  { 39, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr1Up)},
  { 40, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr1Down)},
  { 41, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr1Return)},   

  { 42, 43,44,45,46,47,(DisMenuDevRs485Adr2Init)}, 
  { 43, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr2Set)},
  { 44, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr2Enter)},
  { 45, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr2Up)},
  { 46, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr2Down)},
  { 47, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr2Return)},


  { 48, 49,50,51,52,53,(DisMenuDevRs485Adr3Init)}, //出
  { 49, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr3Set)},
  { 50, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr3Enter)},
  { 51, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr3Up)},
  { 52, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr3Down)},
  { 53, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr3Return)},	

  { 54, 55,56,57,58,59,(DisMenuDevRs485Adr4Init)}, //回
  { 55, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr4Set)},
  { 56, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr4Enter)},
  { 57, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr4Up)},
  { 58, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr4Down)},
  { 59, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr4Return)},
  
  { 60, 61,62,63,64,65,(DisMenuDevRs485Adr5Init)}, //电量
  { 61, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr5Set)},
  { 62, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr5Enter)},
  { 63, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr5Up)},
  { 64, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr5Down)},
  { 65, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr5Return)},


  { 66, 67,68,69,70,71,(DisMenuResetFacInit)}, 
  { 67, 0, 0, 0, 0, 0, (DisMenuResetFacSet)},
  { 68, 0, 0, 0, 0, 0, (DisMenuResetFacEnter)},
  { 69, 0, 0, 0, 0, 0, (DisMenuResetFacUp)},
  { 70, 0, 0, 0, 0, 0, (DisMenuResetFacDown)},
  { 71, 0, 0, 0, 0, 0, (DisMenuResetFacReturn)},

  
  { 72, 73,74,75,76,77,(DisMenuCommunicateInit)}, 
  { 73, 0, 0, 0, 0, 0, (DisMenuCommunicateSet)},
  { 74, 0, 0, 0, 0, 0, (DisMenuCommunicateEnter)},
  { 75, 0, 0, 0, 0, 0, (DisMenuCommunicateUp)},
  { 76, 0, 0, 0, 0, 0, (DisMenuCommunicateDown)},
  { 77, 0, 0, 0, 0, 0, (DisMenuCommunicateReturn)}

};

//--------进入下一层菜单
void TurnToNextMenu(unsigned char menuid)
{
  KeyFuncPtr   = KeyTab[menuid].CurrentOperate;
  (*KeyFuncPtr)();
}

 

//--------修正本层菜单按键索引
void AmendKeyIndex(unsigned char menuid)
{
  KeyFuncIndex = menuid;
}

  
 

//----------------------------------------------------------- 菜单操作
//--------欢迎画面
void DispMenu(void)
{
}


//待机层
void DisMenuTopInit(void)
{
  //unsigned char i,MaxDisp=0;
    
	TimerStart(13,_5S);

  CurrentMenuIndex    = TopMenu.MenuID;         //变量初始化
  CurrentMenuItem     = 1;
  CurrentMenuItemNum  = TopMenu.MenuNum;
  CurrentMenuString   = TopMenu.MenuString;
  CurrentMenuCur      = 1;
  CurrentMenuPage   = 0;


  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引
  LCD_Clr();
  
//  LCD_ShowString(28,0,"[中央远程热泵控制系统]",GRED); 
//   
//  LCD_ShowString(0,30,"水箱:   度",GRED);    LCD_ShowString(160,30,"进水:    吨",GRED);
//  LCD_ShowString(0,60,"出水:   度",GRED);  	LCD_ShowString(160,60,"出水:    吨",GRED);
//  LCD_ShowString(0,90,"环境:   度",GRED);	LCD_ShowString(160,90,"回水:    吨",GRED);
//  LCD_ShowString(0,120,"运行:  台",GRED);   LCD_ShowString(160,120,"电量:    度",GRED);
//  LCD_ShowString(0,150,"在线:  台",GRED); 	LCD_ShowString(160,150,"故障:",GRED);
//  LCD_ShowString(0,180,"GPRS:",GRED);  	LCD_ShowString(160,180,"水位:   %",GRED);  
  
  LCD_ShowString(28,0,"[中央远程热泵控制系统]",GRED); 
  LCD_ShowString(0,30,"水箱:   度",GRED);    LCD_ShowString(160,30,"进水:    吨",GRED);
  LCD_ShowString(0,60,"出水:   度",GRED);  	LCD_ShowString(160,60,"出水:    吨",GRED);
  LCD_ShowString(0,90,"环境:   度",GRED);	LCD_ShowString(160,90,"回水:    吨",GRED);
  LCD_ShowString(0,120,"运行:   台",GRED);   LCD_ShowString(160,120,"电量:    度",GRED);
  LCD_ShowString(0,150,"在线:   台",GRED); 	LCD_ShowString(160,150,"故障:",GRED);
  LCD_ShowString(0,180,"GPRS:",GRED);  	LCD_ShowString(160,180,"水位:   %",GRED);  

  ui_flush_idle_display();


  
//  LCD_ShowString(50,30,"水箱温度:  度",GRED);
//   LCD_ShowString(50,60,"出水温度:  度",GRED);  
//  LCD_ShowString(50,90,"环境温度:  度",GRED);
//  LCD_ShowString(50,120,"运行机组:  台",GRED);
//  LCD_ShowString(50,150,"在线机组:  台",GRED); 
//  LCD_ShowString(50,180,"联线状态: 线",GRED);  
//  LCD_ShowString(50,210,"水位高度:  %",GRED);  
////  LCD_ShowString(270,70,"水位",GRED); 
////  LCD_Rectangle(4, 100,48,140,GRAY);	   
////  LCD_Rectangle(12,210,40,30,BLUE);
////  LCD_Rectangle(12,175,40,30,BLUE);
////  LCD_Rectangle(12,140,40,30,BLUE);
////  LCD_Rectangle(12,105,40,30,BLUE);
}
void DisMenuTopSet(void)
{
  AmendKeyIndex(CurrentMenuIndex);       //修正当前层按键索引
  LCD_Clr();
  TurnToNextMenu(SetMenu.MenuID);
}
void DisMenuTopEnter(void)
{
  AmendKeyIndex(CurrentMenuIndex);       //修正当前层按键索引




}
void DisMenuTopUp(void)
{
  s8 tmp_display[10];
  TimerStop(13);
  AmendKeyIndex(CurrentMenuIndex);              //修正当前层按键索引
  LCD_Clr();

  LCD_ShowString(30,0,"---系统查询---",GRED);
  LCD_ShowString(30,30,"上报时间:",GRED);
  LCD_ShowString(30,60,"心跳时间:",GRED);
															  

  sprintf( (char *)&tmp_display[0],"%03d",(u8)dargs.g_hartheattime);	 
  LCD_ShowString(150,30,(const u8*)&tmp_display[0],GREEN);	


  sprintf( (char *)&tmp_display[0],"%05d",dargs.upload_min);	 
  LCD_ShowString(150,60,(const u8*)&tmp_display[0],GREEN);	


  LCD_ShowString(260,30,"(分)",GRED);	
  LCD_ShowString(260,60,"(分)",GRED);
  
//  LCD_ShowString(30,90,"回水水量:",GRED);
//  LCD_ShowString(30,120,"使用电量:",GRED); 
//  LCD_ShowString(30,150,"故障代码:",GRED);  
}   
void DisMenuTopDown(void)
{
  AmendKeyIndex(CurrentMenuIndex);              //修正当前层按键索引
        
 	
}
void DisMenuTopReturn(void)
{
  AmendKeyIndex(CurrentMenuIndex);              //修正当前层按键索引

  TurnToNextMenu(TopMenu.MenuID);
}


//设置层菜单
void DisMenuLevel0Init(void)
{
  unsigned char i,MaxDisp=0;

  CurrentMenuIndex    = SetMenu.MenuID;         //变量初始化
  CurrentMenuItem     = 1;//1
  CurrentMenuItemNum  =  SetMenu.MenuNum; //max=10
  CurrentMenuString   = SetMenu.MenuString;
  CurrentMenuCur      = 1;//1
  CurrentMenuPage   	= 0;
  MaxScreenDisp	 	= SetMenu.MenuNum;//max=10
  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引
  TimerStop(13);

  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = 8;
  }
  else
  {
    MaxDisp = 8;
  }

  LCD_Clr();
  LCD_Rectangle(0,(CurrentMenuCur-1)*30,320,30,RED);
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(50,i*30,*(CurrentMenuString+i),GREEN);    //绘菜单
  }
}
void DisMenuLevel0Set(void)
{
  AmendKeyIndex(CurrentMenuIndex);       //修正当前层按键索引
}
void DisMenuLevel0Enter(void)
{
  AmendKeyIndex(CurrentMenuIndex);       //修正当前层按键索引
  switch(CurrentMenuItem)                //进入下层菜单
  {
    case 1: //设置地址
      TurnToNextMenu(DevAdrMenu.MenuID); 
      break;
    case 2: //设置时间	
      TurnToNextMenu(TimeMenu.MenuID); 
      break; 	       
    case 3: //设置时间	
      TurnToNextMenu(IpMenu.MenuID); 
      break;	       
    case 4: //设置时间	
      TurnToNextMenu(PortMenu.MenuID); 
      break;	       
    case 5: //设置时间	
      TurnToNextMenu(Rs485addr1Menu.MenuID); 
      break;	       
    case 6: //设置进水
      TurnToNextMenu(Rs485addr2Menu.MenuID); 
      break;
    case 7: //设置出水
      TurnToNextMenu(Rs485addr3Menu.MenuID); 
      break;
    case 8: //设置回水
      TurnToNextMenu(Rs485addr4Menu.MenuID); 
      break;
    case 9: //设置电量
      TurnToNextMenu(Rs485addr5Menu.MenuID); 
      break;
    case 10://	 
      TurnToNextMenu(ResetFacMenu.MenuID); 
      break;
    case 11://	 
      TurnToNextMenu(ResetCommunicateMenu.MenuID); 
      break;

  }
}
void DisMenuLevel0Up(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);              //修正当前层按键索引

  CurrentMenuCur++;                             //光标向下
  CurrentMenuItem=CurrentMenuCur;

  if(CurrentMenuCur>CurrentMenuItemNum)
  {
    CurrentMenuItem=CurrentMenuCur=1;
  }  

   CurrentMenuPage=(CurrentMenuCur-1)/8;					//计算当前的页数，一页4项
   
  // if(CurrentMenuItemNum<MaxScreenDisp)           //当前层菜单项目数小于屏幕最大显示项数
   if(CurrentMenuCur<9)
   {
      MaxDisp = 8;               //屏扫描项数为当前层菜单项数
   }
   else
   {
     /* if((CurrentMenuItemNum-CurrentMenuPage*MaxScreenDisp)>MaxScreenDisp)//当前剩余显示的容量大于屏幕最大显示类容 屏扫描项数为屏幕最大显示项数
      {
      		MaxDisp=MaxScreenDisp;
      }
      else
      {
      		MaxDisp=CurrentMenuItemNum-CurrentMenuPage*MaxScreenDisp;//当剩余类容小于屏幕显示最大类容，则只显示剩余的类容
      }		*/

	  MaxDisp=3;
   }

  LCD_Clr();
  LCD_Rectangle(0,((CurrentMenuCur-1)%8)*30,320,30,RED);
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(50,i*30,*(CurrentMenuString+CurrentMenuPage*8+i),GREEN);    //绘菜单
  }	
}   
void DisMenuLevel0Down(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);              //修正当前层按键索引

  CurrentMenuCur--;                             //光标向下
  CurrentMenuItem=CurrentMenuCur;

  if(CurrentMenuCur<=0)             						//光标回到了最头，从最后开始显示 
  {
    CurrentMenuItem=CurrentMenuCur=CurrentMenuItemNum;
  }  



   CurrentMenuPage=(CurrentMenuCur-1)/8;					//计算当前的页数，一页4项
   
//   if(CurrentMenuItemNum<MaxScreenDisp)           //当前层菜单项目数小于屏幕最大显示项数
//   {
//      MaxDisp = CurrentMenuItemNum;               //屏扫描项数为当前层菜单项数
//   }
//   else
//   {
//      if((CurrentMenuItemNum-CurrentMenuPage*MaxScreenDisp)>MaxScreenDisp)//当前剩余显示的容量大于屏幕最大显示类容 屏扫描项数为屏幕最大显示项数
//      {
//      		MaxDisp=MaxScreenDisp;
//      }
//      else
//      {
//      		MaxDisp=CurrentMenuItemNum-CurrentMenuPage*MaxScreenDisp;//当剩余类容小于屏幕显示最大类容，则只显示剩余的类容
//    	}
//   }

   if(CurrentMenuCur<9)
   {
      MaxDisp = 8;               //屏扫描项数为当前层菜单项数
   }
   else
   {
	  MaxDisp=3;
   }

  LCD_Clr();
  LCD_Rectangle(0,((CurrentMenuCur-1)%8)*30,320,30,RED);
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(50,i*30,*(CurrentMenuString+CurrentMenuPage*8+i),GREEN);    //绘菜单
  }
}
void DisMenuLevel0Return(void)
{
  AmendKeyIndex(CurrentMenuIndex); 
  TurnToNextMenu(TopMenu.MenuID); 
}

//--------
void DisMenuDevAdrInit(void)
{
  u8 i,MaxDisp=0;

  CurrentMenuIndex    = DevAdrMenu.MenuID;         //变量初始化
  CurrentMenuItem     = 1;
  CurrentMenuItemNum  = DevAdrMenu.MenuNum;
  CurrentMenuString   = DevAdrMenu.MenuString;
  CurrentMenuCur      = 1;
  CurrentMenuPage     = 0;
  Column              = 0;
  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(DevAddrMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0,60,"V",GRED);  
  
  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));

  if(dargs.devno[0]==0xff)
  if(dargs.devno[1]==0xff)
  if(dargs.devno[2]==0xff)
  if(dargs.devno[3]==0xff)
  if(dargs.devno[4]==0xff)
  if(dargs.devno[5]==0xff)
  {
    dargs.devno[0]=0;
    dargs.devno[1]=0;
    dargs.devno[2]=0;
    dargs.devno[3]=0;
    dargs.devno[4]=0;
    dargs.devno[5]=0;
  }
    
  sprintf((char *)&displaybuf[0],"%02d%02d%02d%02d%02d%02d",dargs.devno[0],dargs.devno[1],dargs.devno[2],dargs.devno[3],dargs.devno[4],dargs.devno[5]);

  LCD_ShowString(0,90,(const u8 *)&displaybuf[0],GREEN);
}
void DisMenuDevAdrSet(void)
{
  AmendKeyIndex(CurrentMenuIndex); 
} 
void DisMenuDevAdrUp(void)
{
  u8 i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex); 		
  
  ++displaybuf[Column];

  if( (displaybuf[Column]>'9') ) 
  {
    displaybuf[Column]='0';
  }  


  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(DevAddrMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
} 
void DisMenuDevAdrDown(void)
{
  u8 i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex); 		
  
  --displaybuf[Column];

  if( displaybuf[Column]<'0' ) 
  {
    displaybuf[Column]='9';
  }  
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(DevAddrMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)displaybuf,GREEN);
}   
void DisMenuDevAdrEnter(void)
{
  unsigned char i,MaxDisp=0;

  CurrentMenuIndex    = DevAdrMenu.MenuID;         //变量初始化
  CurrentMenuItemNum  = DevAdrMenu.MenuNum;
  CurrentMenuString   = DevAdrMenu.MenuString;

  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  ++Column;
  if(Column>11)
  {
  	SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//读取所有内容

    Column=0;
    dargs.devno[0]=(displaybuf[0]-'0')*10+(displaybuf[1]-'0');
    dargs.devno[1]=(displaybuf[2]-'0')*10+(displaybuf[3]-'0');
    dargs.devno[2]=(displaybuf[4]-'0')*10+(displaybuf[5]-'0');
    dargs.devno[3]=(displaybuf[6]-'0')*10+(displaybuf[7]-'0');
    dargs.devno[4]=(displaybuf[8]-'0')*10+(displaybuf[9]-'0');
    dargs.devno[5]=(displaybuf[10]-'0')*10+(displaybuf[11]-'0');
    SPI_FLASH_SectorErase(0);
    SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));

    AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(DevAddrMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
} 
void DisMenuDevAdrReturn(void)
{
  unsigned char i,MaxDisp=0;
  AmendKeyIndex(CurrentMenuIndex); 
  --Column;
  if(Column<0)
  {
    Column=0;
    AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }
  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(DevAddrMenuString+i),GREEN);    //绘菜单	  
  }

  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[YEAR_START_POS],GREEN);
}


//-----
void DisMenuDevTimeInit(void)
{
  unsigned char i,MaxDisp=0;

  memset(displaybuf,'\0',sizeof(displaybuf));
  CurrentMenuIndex    = TimeMenu.MenuID;         //变量初始化
  CurrentMenuItem     = 1;
  CurrentMenuItemNum  = TimeMenu.MenuNum;
  CurrentMenuString   = TimeMenu.MenuString;
  CurrentMenuCur      = 1;
  CurrentMenuPage     = 0;
  Column	      = 0;
  Row 		      = 0;
  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  //2010-11-12 18:29:56
  sprintf((char *)&displaybuf[YEAR_START_POS],"%04d-%02d-%02d %02d:%02d:%02d",(u16)timer.w_year,(u16)timer.w_month,(u16)timer.w_date,(u16)timer.hour,(u16)timer.min,(u16)timer.sec);

#ifdef DBG_RS232
  printf("%d\n",displaybuf[0]);
  printf("%d\n",displaybuf[1]);
  printf("%d\n",displaybuf[2]);
  printf("%d\n",displaybuf[3]);

  printf("%d\n",displaybuf[5]);
  printf("%d\n",displaybuf[6]);

  printf("%d\n",displaybuf[8]);
  printf("%d\n",displaybuf[9]);
  
#endif

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(TimeMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[YEAR_START_POS],GREEN);
}
void DisMenuDevTimeSet(void)
{
  AmendKeyIndex(CurrentMenuIndex); 
}
void DisMenuDevTimeEnter(void)
{
  unsigned char i,MaxDisp=0;
  u16 year;
  u8 month;
  u8 date;
  u8 hour;
  u8 min;
  u8 sec;

  AmendKeyIndex(CurrentMenuIndex); 

  ++Column;

  if( (Column==4) ||(Column==7) ||(Column==10) || (Column==13) || (Column==16) )
  {
    Column++;
  }
  if(Column>18)
  {
    Column=0;
    year=(displaybuf[0]-'0')*1000 + (displaybuf[1]-'0')*100 + (displaybuf[2]-'0')*10 + (displaybuf[3]-'0');
    month=(displaybuf[5]-'0')*10 + (displaybuf[6]-'0'); 
    date=(displaybuf[8]-'0')*10 + (displaybuf[9]-'0'); 
    
    hour=(displaybuf[HOUR_START_POS]-'0')*10 + (displaybuf[HOUR_START_POS+1]-'0');
    min=(displaybuf[MIN_START_POS]-'0')*10 + (displaybuf[MIN_START_POS+1]-'0');
    sec=(displaybuf[SEC_START_POS]-'0')*10 + (displaybuf[SEC_START_POS+1]-'0');
    
    if(month>12)//不超过12月
    {
      month=12;
    } 

    if(date>31)
    {
      date=31;
    }

    if(hour>24)
    {
      hour=24;
    }
    if(min>59)
    {
      min=59;
    }
    if(sec>59)
    {
      sec=59;
    }
    RTC_Set(year,month,date,hour, min, sec);

    AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引
    TurnToNextMenu(SetMenu.MenuID); 
    return;
    //到达末尾
  }

  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }
  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(TimeMenuString+i),GREEN);    //绘菜单	  
  }

  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[YEAR_START_POS],GREEN);
}
void DisMenuDevTimeUp(void)
{
  unsigned char i,MaxDisp=0;
  
  AmendKeyIndex(CurrentMenuIndex); 

  ++displaybuf[Column];

  if(displaybuf[Column]>'9')
  {
    displaybuf[Column]='0';
  }


  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }
  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(TimeMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevTimeDown(void)
{
  unsigned char i,MaxDisp=0;
  
  AmendKeyIndex(CurrentMenuIndex); 

  --displaybuf[Column];
  if(displaybuf[Column]<'0')
  {
    displaybuf[Column]='9';
  }

  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }
  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(TimeMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevTimeReturn(void)
{
  unsigned char i,MaxDisp=0;
  AmendKeyIndex(CurrentMenuIndex); 
  --Column;

  if( (Column==4) ||(Column==7) ||(Column==10) || (Column==13) || (Column==16) )
  {
    Column--;
  }
  if(Column<0)
  {
    Column=0;
    AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }
  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(TimeMenuString+i),GREEN);    //绘菜单	  
  }

  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[YEAR_START_POS],GREEN);
  //TurnToNextMenu(SetMenu.MenuID); 
}
//--------
void DisMenuDevIPInit(void)
{
  unsigned char i,MaxDisp=0;

  memset(displaybuf,'\0',sizeof(displaybuf));
  CurrentMenuIndex    = IpMenu.MenuID;         //变量初始化
  CurrentMenuItem     = 1;
  CurrentMenuItemNum  = IpMenu.MenuNum;
  CurrentMenuString   = IpMenu.MenuString;
  CurrentMenuCur      = 1;
  CurrentMenuPage     = 0;
  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  //sprintf(&displaybuf[0],"%03d.%03d.%03d.%03d",(u16)dargs.ip[0],(u16)dargs.ip[1],(u16)dargs.ip[2],(u16)dargs.ip[3]);
  sprintf(&displaybuf[0],"%03d.%03d.%03d.%03d",(u8)dargs.ip[0],(u8)dargs.ip[1],(u8)dargs.ip[2],(u8)dargs.ip[3]);
 
 
  
  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(IpMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevIPSet(void)
{
  AmendKeyIndex(CurrentMenuIndex); 
}
void DisMenuDevIPEnter(void)
{
  unsigned char i,MaxDisp=0;
  int ipres[4];
  AmendKeyIndex(CurrentMenuIndex); 
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }
  ++Column;
  if( (Column==3) || (Column==7) || (Column==11) )
  {
    Column++;
  }
  if(Column>14)
  {
  	SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//读取所有内容

    Column=0;//del
    //到达结尾
    ipres[0]=(displaybuf[0]-'0')*100 + (displaybuf[1]-'0')*10 + (displaybuf[2]-'0');
    ipres[1]=(displaybuf[4]-'0')*100 + (displaybuf[5]-'0')*10 + (displaybuf[6]-'0');
    ipres[2]=(displaybuf[8]-'0')*100 + (displaybuf[9]-'0')*10 + (displaybuf[10]-'0');
    ipres[3]=(displaybuf[12]-'0')*100 + (displaybuf[13]-'0')*10 + (displaybuf[14]-'0');

    if(ipres[0]>255)
    {
      ipres[0]=255;
    }

    if(ipres[1]>255)
    {
      ipres[1]=255;
    }
    if(ipres[2]>255)
    {
      ipres[2]=255;
    }
    if(ipres[3]>255)
    {
      ipres[3]=255;
    }

    dargs.ip[0]=(u8)ipres[0];
    dargs.ip[1]=(u8)ipres[1];
    dargs.ip[2]=(u8)ipres[2];
    dargs.ip[3]=(u8)ipres[3];

    SPI_FLASH_SectorErase(0);
    SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));

    sprintf((char *)&g_setting.szIPAddress,"%03d.%03d.%03d.%03d",(u16)dargs.ip[0],(u16)dargs.ip[1],(u16)dargs.ip[2],(u16)dargs.ip[3]);
    sprintf((char *)&g_setting.szPortNumber,"%05d",(u16)dargs.port);


    AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(IpMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevIPUp(void)
{
  unsigned char i,MaxDisp=0;
  AmendKeyIndex(CurrentMenuIndex); 
 
  ++displaybuf[Column];
  
  if(displaybuf[Column]>'9')
  {
    displaybuf[Column]='0';
  }


  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }
  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(IpMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevIPDown(void)
{
  unsigned char i,MaxDisp=0;
  AmendKeyIndex(CurrentMenuIndex); 
  --displaybuf[Column];
  
  if(displaybuf[Column]<'0')
  {
    displaybuf[Column]='9';
  }


  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }
  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(IpMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevIPReturn(void)
{
  unsigned char i,MaxDisp=0;
  AmendKeyIndex(CurrentMenuIndex); 
  --Column;

  if( (Column==3) ||(Column==7) ||(Column==11) )
  {
    Column--;
  }
  if(Column<0)
  {
    Column=0;
    AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }
  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(IpMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
  //TurnToNextMenu(SetMenu.MenuID); 
}
//--------
void DisMenuDevPortInit(void)
{
  unsigned char i,MaxDisp=0;

  memset(displaybuf,'\0',sizeof(displaybuf));
  CurrentMenuIndex    = PortMenu.MenuID;         //变量初始化
  CurrentMenuItem     = 1;
  CurrentMenuItemNum  = PortMenu.MenuNum;
  CurrentMenuString   = PortMenu.MenuString;
  CurrentMenuCur      = 1;
  CurrentMenuPage   	= 0;
  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  sprintf((char *)&displaybuf[0],"%05d",(u16)dargs.port);
  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(PortMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevPortSet(void)
{
  AmendKeyIndex(CurrentMenuIndex); 
}
void DisMenuDevPortEnter(void)
{
  unsigned char i,MaxDisp=0;

  ++Column;

  if(Column>4)
  {
  	SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//读取所有内容

    Column=0;
    dargs.port=(displaybuf[0]-'0')*10000 + (displaybuf[1]-'0') *1000 + (displaybuf[2]-'0')*100 + (displaybuf[3]-'0')*10 + (displaybuf[4]-'0');
    SPI_FLASH_SectorErase(0);
    SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));
    AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引
    TurnToNextMenu(SetMenu.MenuID); 
    return;
    //到达末尾
  }
  AmendKeyIndex(CurrentMenuIndex); 
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(PortMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevPortUp(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex); 

  ++displaybuf[Column];
  if(displaybuf[Column]>'9')
  {
    displaybuf[Column]='0';
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(PortMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevPortDown(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex); 

  --displaybuf[Column];
  if(displaybuf[Column]<'0')
  {
    displaybuf[Column]='9';
  }

  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(PortMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevPortReturn(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex); 
  --Column;

  if(Column<0)
  {
    Column=0;
    //到达末尾
    AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(PortMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
  //AmendKeyIndex(CurrentMenuIndex); 
  //TurnToNextMenu(SetMenu.MenuID); 
}
//--------
void DisMenuDevRs485Adr1Init(void)
{
  unsigned char i,MaxDisp=0;

  memset(displaybuf,'\0',sizeof(displaybuf));
  CurrentMenuIndex    = Rs485addr1Menu.MenuID;         //变量初始化
  CurrentMenuItem     = 1;
  CurrentMenuItemNum  = Rs485addr1Menu.MenuNum;
  CurrentMenuString   = Rs485addr1Menu.MenuString;
  CurrentMenuCur      = 1;
  CurrentMenuPage   	= 0;
  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  sprintf((char *)&displaybuf[0],"%03d",(u32)dargs.rs485_adr1);
  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(Rs485Addr1MenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr1Set(void)
{
  AmendKeyIndex(CurrentMenuIndex); 
}
void DisMenuDevRs485Adr1Enter(void)
{
  unsigned char i,MaxDisp=0;
  u16 res ;

  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  if(++Column>2)
  {
  	SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//读取所有内容

    Column=0;
    res = (displaybuf[0]-'0')*100 + (displaybuf[1]-'0')*10 + (displaybuf[2]-'0');
    if(res>255)
    {
      res=255;
    } 
	dargs.rs485_adr1=(u8)res;
    SPI_FLASH_SectorErase(0);
    SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));
    AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(Rs485Addr1MenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr1Up(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  if(++displaybuf[Column]>'9')
  {
    displaybuf[Column]='0';
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(Rs485Addr1MenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
  AmendKeyIndex(CurrentMenuIndex); 
}
void DisMenuDevRs485Adr1Down(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  if(--displaybuf[Column]<'0')
  {
    displaybuf[Column]='9';
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(Rs485Addr1MenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr1Return(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  if(--Column<0)
  {
    Column=0;
    AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(Rs485Addr1MenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
  //AmendKeyIndex(CurrentMenuIndex); 
  //TurnToNextMenu(SetMenu.MenuID); 
}
//--------
void DisMenuDevRs485Adr2Init(void)
{
  unsigned char i,MaxDisp=0;
  memset(displaybuf,'\0',sizeof(displaybuf));
  CurrentMenuIndex    = Rs485addr2Menu.MenuID;         //变量初始化
  CurrentMenuItem     = 1;
  CurrentMenuItemNum  = Rs485addr2Menu.MenuNum;
  CurrentMenuString   = Rs485addr2Menu.MenuString;
  CurrentMenuCur      = 1;
  CurrentMenuPage   	= 0;
  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  sprintf((char *)&displaybuf[0],"%08x",(u32)dargs.rs485InWaterAddr);
 																	    
  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
 

}
void DisMenuDevRs485Adr2Set(void)
{
  AmendKeyIndex(CurrentMenuIndex); 
}
void DisMenuDevRs485Adr2Enter(void)
{
  unsigned char i,MaxDisp=0;
u8 res[4];

  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  if(++Column>=8)
  {
	SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//读取所有内容
  
    Column=0;																	   
	res[0]=	((displaybuf[0]-'0')<<4) |  (displaybuf[1]-'0');	 					   
	res[1]=	((displaybuf[2]-'0')<<4) |  (displaybuf[3]-'0');					   
	res[2]=	((displaybuf[4]-'0')<<4) |  (displaybuf[5]-'0');					   
	res[3]=	((displaybuf[6]-'0')<<4) |  (displaybuf[7]-'0');
    dargs.rs485InWaterAddr =  ((u32)res[0]<<24) | ((u32)res[1]<<16) | ((u32)res[2]<<8) |((u32)res[3])  ;
 

    SPI_FLASH_SectorErase(0);
    SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));
    AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr2Up(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  if(++displaybuf[Column]>'9')
  {
    displaybuf[Column]='0';
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr2Down(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  if(--displaybuf[Column]<'0')
  {
    displaybuf[Column]='9';
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr2Return(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  if(--Column<0)
  {
    Column=0;
    AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
 }


//--------
void DisMenuDevRs485Adr3Init(void)
{
  unsigned char i,MaxDisp=0;
  memset(displaybuf,'\0',sizeof(displaybuf));
  CurrentMenuIndex    = Rs485addr3Menu.MenuID;         //变量初始化
  CurrentMenuItem     = 1;
  CurrentMenuItemNum  = Rs485addr3Menu.MenuNum;
  CurrentMenuString   = Rs485addr3Menu.MenuString;
  CurrentMenuCur      = 1;
  CurrentMenuPage   	= 0;
  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }
  																		 
    sprintf((char *)&displaybuf[0],"%08x",(u32)dargs.rs485OutWaterAddr);

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);

}
void DisMenuDevRs485Adr3Set(void)
{
  AmendKeyIndex(CurrentMenuIndex); 
}
void DisMenuDevRs485Adr3Enter(void)
{
  unsigned char i,MaxDisp=0;
u8 res[4];

  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  if(++Column>=8)
  {
	SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//读取所有内容
  
    Column=0;																	   
	res[0]=	((displaybuf[0]-'0')<<4) |  (displaybuf[1]-'0');	 					   
	res[1]=	((displaybuf[2]-'0')<<4) |  (displaybuf[3]-'0');					   
	res[2]=	((displaybuf[4]-'0')<<4) |  (displaybuf[5]-'0');					   
	res[3]=	((displaybuf[6]-'0')<<4) |  (displaybuf[7]-'0');
    dargs.rs485OutWaterAddr =  ((u32)res[0]<<24) | ((u32)res[1]<<16) | ((u32)res[2]<<8) |((u32)res[3])  ;
  

    SPI_FLASH_SectorErase(0);
    SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));
    AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr3Up(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  if(++displaybuf[Column]>'9')
  {
    displaybuf[Column]='0';
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr3Down(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  if(--displaybuf[Column]<'0')
  {
    displaybuf[Column]='9';
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //绘菜单	  
  }									
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr3Return(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  if(--Column<0)
  {
    Column=0;
    AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
 }



//--------
void DisMenuDevRs485Adr4Init(void)
{
  unsigned char i,MaxDisp=0;
  memset(displaybuf,'\0',sizeof(displaybuf));
  CurrentMenuIndex    = Rs485addr4Menu.MenuID;         //变量初始化
  CurrentMenuItem     = 1;
  CurrentMenuItemNum  = Rs485addr4Menu.MenuNum;
  CurrentMenuString   = Rs485addr4Menu.MenuString;
  CurrentMenuCur      = 1;
  CurrentMenuPage   	= 0;
  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }
 
  sprintf((char *)&displaybuf[0],"%08x",(u32)dargs.rs485LoopWaterAddr);	 
  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);

}
void DisMenuDevRs485Adr4Set(void)
{
  AmendKeyIndex(CurrentMenuIndex); 
}
void DisMenuDevRs485Adr4Enter(void)
{
  unsigned char i,MaxDisp=0;
u8 res[4];

  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  if(++Column>=8)
  {
	SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//读取所有内容
  
    Column=0;																	   
	res[0]=	((displaybuf[0]-'0')<<4) |  (displaybuf[1]-'0');	 					   
	res[1]=	((displaybuf[2]-'0')<<4) |  (displaybuf[3]-'0');					   
	res[2]=	((displaybuf[4]-'0')<<4) |  (displaybuf[5]-'0');					   
	res[3]=	((displaybuf[6]-'0')<<4) |  (displaybuf[7]-'0');
    dargs.rs485LoopWaterAddr =  ((u32)res[0]<<24) | ((u32)res[1]<<16) | ((u32)res[2]<<8) |((u32)res[3])  ;

  

    SPI_FLASH_SectorErase(0);
    SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));
    AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr4Up(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  if(++displaybuf[Column]>'9')
  {
    displaybuf[Column]='0';
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr4Down(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  if(--displaybuf[Column]<'0')
  {
    displaybuf[Column]='9';
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr4Return(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  if(--Column<0)
  {
    Column=0;
    AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
 }


//--------
void DisMenuDevRs485Adr5Init(void)
{
  unsigned char i,MaxDisp=0;
  memset(displaybuf,'\0',sizeof(displaybuf));
  CurrentMenuIndex    = Rs485addr5Menu.MenuID;         //变量初始化
  CurrentMenuItem     = 1;
  CurrentMenuItemNum  = Rs485addr5Menu.MenuNum;
  CurrentMenuString   = Rs485addr5Menu.MenuString;
  CurrentMenuCur      = 1;
  CurrentMenuPage   	= 0;
  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  sprintf((char *)&displaybuf[0],"%08x",(u32)dargs.rs485ElecAddr);
  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);

}
void DisMenuDevRs485Adr5Set(void)
{
  AmendKeyIndex(CurrentMenuIndex); 
}
void DisMenuDevRs485Adr5Enter(void)
{
  unsigned char i,MaxDisp=0;
u8 res[4];

  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  if(++Column>=8)
  {
	SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//读取所有内容
  
    Column=0;																	   
	res[0]=	((displaybuf[0]-'0')<<4) |  (displaybuf[1]-'0');	 					   
	res[1]=	((displaybuf[2]-'0')<<4) |  (displaybuf[3]-'0');					   
	res[2]=	((displaybuf[4]-'0')<<4) |  (displaybuf[5]-'0');					   
	res[3]=	((displaybuf[6]-'0')<<4) |  (displaybuf[7]-'0');
    dargs.rs485ElecAddr =  ((u32)res[0]<<24) | ((u32)res[1]<<16) | ((u32)res[2]<<8) |((u32)res[3])  ;
  

    SPI_FLASH_SectorErase(0);
    SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));
    AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr5Up(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  if(++displaybuf[Column]>'9')
  {
    displaybuf[Column]='0';
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr5Down(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  if(--displaybuf[Column]<'0')
  {
    displaybuf[Column]='9';
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr5Return(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  if(--Column<0)
  {
    Column=0;
    AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //绘菜单	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
 }



//--	  
void DisMenuResetFacInit(void)
{
  unsigned char i,MaxDisp=0;

  memset(displaybuf,'\0',sizeof(displaybuf));
  CurrentMenuIndex    = ResetFacMenu.MenuID;         //变量初始化
  CurrentMenuItem     = 1;
  CurrentMenuItemNum  = ResetFacMenu.MenuNum;
  CurrentMenuString   = ResetFacMenu.MenuString;
  CurrentMenuCur      = 1;
  CurrentMenuPage     = 0;
  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  LCD_Rectangle(0,(CurrentMenuCur)*30,320,30,RED);
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,i*30,*(CurrentMenuString+i),GREEN);    //绘菜单
  }
}
void DisMenuResetFacSet(void)
{
  AmendKeyIndex(CurrentMenuIndex); 
}

 
void DisMenuResetFacEnter(void)
{
  AmendKeyIndex(CurrentMenuIndex);       //修正当前层按键索引
  switch(CurrentMenuItem)                //进入下层菜单
  {
    case 1: 
      LCD_Clr();
      LCD_ShowString(0,60,"正在保存...",GREEN);    //绘菜单
      
      SPI_FLASH_SectorErase(0);
	  memset(&dargs.devno[0],0x00,sizeof(dargs));//全部清零
	  memset(&dargs.pwd[0],0xFF,7);	//通信密码+美的通讯的rs485地址
	  dargs.g_hartheattime=1;//心跳包1分钟
	  dargs.upload_min=5;//默认定时上报时间5分钟
	  dargs.setMideaDegree0=40;//默认出水温度50°C
	  dargs.setMideaLoopDegree=40;
 
      SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));
      delay_ms(1000); 
      LCD_Clr();
      LCD_ShowString(0,60,"保存成功",GREEN);    //绘菜单
      delay_ms(1000);
      break;
    case 2:
      break; 	       
  }
  TurnToNextMenu(SetMenu.MenuID); 
}
void DisMenuResetFacUp(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);              //修正当前层按键索引

  CurrentMenuCur++;                             //光标向下
  CurrentMenuItem=CurrentMenuCur;

  if(CurrentMenuCur>2)//CurrentMenuItemNum)
  {
    CurrentMenuItem=CurrentMenuCur=1;
  }  

  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  LCD_Rectangle(0,(CurrentMenuCur)*30,320,30,RED);
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,i*30,*(CurrentMenuString+i),GREEN);    //绘菜单
  }	
}
void DisMenuResetFacDown(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);              //修正当前层按键索引

  CurrentMenuCur--;                             //光标向下
  CurrentMenuItem=CurrentMenuCur;

  if(CurrentMenuCur<1)             						//光标回到了最头，从最后开始显示 
  {
    CurrentMenuItem=CurrentMenuCur=2;//CurrentMenuItemNum;
  }  

  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  LCD_Rectangle(0,(CurrentMenuCur)*30,320,30,RED);
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,i*30,*(CurrentMenuString+i),GREEN);    //绘菜单
  }
}
void DisMenuResetFacReturn(void)
{
  AmendKeyIndex(CurrentMenuIndex); 
  TurnToNextMenu(SetMenu.MenuID); 
} 

//--
void DisMenuCommunicateInit(void)
{
  unsigned char i,MaxDisp=0;

  memset(displaybuf,'\0',sizeof(displaybuf));
  CurrentMenuIndex    = ResetCommunicateMenu.MenuID;         //变量初始化
  CurrentMenuItem     = 1;
  CurrentMenuItemNum  = ResetCommunicateMenu.MenuNum;
  CurrentMenuString   = ResetCommunicateMenu.MenuString;
  CurrentMenuCur      = 1;
  CurrentMenuPage     = 0;
  AmendKeyIndex(CurrentMenuIndex);             //修正当前层按键索引

  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));
  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  LCD_Rectangle(0,(CurrentMenuCur)*30,320,30,RED);
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,i*30,*(CurrentMenuString+i),GREEN);    //绘菜单
  }
}
void DisMenuCommunicateSet(void)
{
  AmendKeyIndex(CurrentMenuIndex); 
}

 
void DisMenuCommunicateEnter(void)
{
  AmendKeyIndex(CurrentMenuIndex);       //修正当前层按键索引
  switch(CurrentMenuItem)                //进入下层菜单
  {
    case 1: 
      LCD_Clr();
      LCD_ShowString(0,60,"正在恢复...",GREEN);    //绘菜单
   	  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//读取所有内容
      SPI_FLASH_SectorErase(0);
 	  memset(&dargs.pwd[0],0xFF,6);	//通信密码 
      SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));
      delay_ms(500); 
      LCD_Clr();
      LCD_ShowString(0,60,"恢复成功",GREEN);    //绘菜单
      delay_ms(500);
      break;
    case 2:
      break; 	       
  }
  TurnToNextMenu(SetMenu.MenuID); 
}
void DisMenuCommunicateUp(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);              //修正当前层按键索引

  CurrentMenuCur++;                             //光标向下
  CurrentMenuItem=CurrentMenuCur;

  if(CurrentMenuCur>2)//CurrentMenuItemNum)
  {
    CurrentMenuItem=CurrentMenuCur=1;
  }  

  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  LCD_Rectangle(0,(CurrentMenuCur)*30,320,30,RED);
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,i*30,*(CurrentMenuString+i),GREEN);    //绘菜单
  }	
}
void DisMenuCommunicateDown(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);              //修正当前层按键索引

  CurrentMenuCur--;                             //光标向下
  CurrentMenuItem=CurrentMenuCur;

  if(CurrentMenuCur<1)             						//光标回到了最头，从最后开始显示 
  {
    CurrentMenuItem=CurrentMenuCur=2;//CurrentMenuItemNum;
  }  

  if(CurrentMenuItemNum<MaxScreenDisp)         //当前层项目数小于最大屏幕显示数
  {
    MaxDisp = CurrentMenuItemNum;
  }
  else
  {
    MaxDisp = MaxScreenDisp;
  }

  LCD_Clr();
  LCD_Rectangle(0,(CurrentMenuCur)*30,320,30,RED);
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,i*30,*(CurrentMenuString+i),GREEN);    //绘菜单
  }
}
void DisMenuCommunicateReturn(void)
{
  AmendKeyIndex(CurrentMenuIndex); 
  TurnToNextMenu(SetMenu.MenuID); 
} 

void OnKey(unsigned char key)
{ 
  switch(key)
  {
    case KEY_SET://设置键
      KeyFuncIndex = KeyTab[KeyFuncIndex].KeySetState;
      KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
      (*KeyFuncPtr)();
      break;
    case KEY_ENTER://确认键
      KeyFuncIndex = KeyTab[KeyFuncIndex].KeyEnterState;
      KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
      (*KeyFuncPtr)();
      break;
    case KEY_UP://上翻、加号键
      KeyFuncIndex = KeyTab[KeyFuncIndex].KeyUpState;
      KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
      (*KeyFuncPtr)();
      break;
    case KEY_DN://下翻、减号键
      KeyFuncIndex = KeyTab[KeyFuncIndex].KeyDownState;
      KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
      (*KeyFuncPtr)();
      break;
    case KEY_RETURN://返回键
      KeyFuncIndex = KeyTab[KeyFuncIndex].KeyReturnState;
      KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
      (*KeyFuncPtr)();
      break;    		
    default :break;
  }
}


//待机显示画面
void ui_flush_idle_display(void)
{

 
//  LCD_ShowString(0,30,"水箱:   度",GRED);    LCD_ShowString(160,30,"进水:    吨",GRED);
//  LCD_ShowString(0,60,"出水:   度",GRED);  	LCD_ShowString(160,60,"出水:    吨",GRED);
//  LCD_ShowString(0,90,"环境:   度",GRED);	LCD_ShowString(160,90,"回水:    吨",GRED);
//  LCD_ShowString(0,120,"运行:   台",GRED);   LCD_ShowString(160,120,"电量:    度",GRED);
//  LCD_ShowString(0,150,"在线:   台",GRED); 	LCD_ShowString(160,150,"故障:",GRED);
//  LCD_ShowString(0,180,"GPRS:",GRED);  	LCD_ShowString(160,180,"水位:   %",GRED);  

////--清缓冲
//  display_tmp[0]=0;	
//  display_tmp[1]=0;
//  display_tmp[2]=0;
//  display_tmp[3]=0;
//  realError=0;
//  level_percent=0;

//void LCD_Rectangle(u16 x,u16 y,u16 len,u16 wid,u16 rgb565)

////--左半边  					////--右半边
  LCD_ClrFix(64,30,48,24);      LCD_ClrFix(160+64,30,64,24); 
  LCD_ClrFix(64,60,48,24);		LCD_ClrFix(160+64,60,64,24); 
  LCD_ClrFix(64,90,48,24);		LCD_ClrFix(160+64,90,64,24); 
  LCD_ClrFix(64,120,48,24);		LCD_ClrFix(160+64,120,64,24); 
  LCD_ClrFix(64,150,48,24);		LCD_ClrFix(160+64,150,32,24); 
  LCD_ClrFix(80,180,48,24);	    LCD_ClrFix(160+64,180,48,24); 
  
////  LCD_RectangleS(30,208,24,48,RED);			LCD_RectangleS(30,32,24,64,RED);
////  LCD_RectangleS(60,208,24,48,RED);		    LCD_RectangleS(60,32,24,64,RED);
////  LCD_RectangleS(90,208,24,48,RED);			LCD_RectangleS(90,32,24,64,RED);
////  LCD_RectangleS(120,208,24,48,RED);			LCD_RectangleS(120,32,24,32,RED);
////  LCD_RectangleS(150,208,24,48,RED);	 		LCD_RectangleS(150,48,24,48,RED);
////  LCD_RectangleS(180,208-16,24,48,RED);		LCD_RectangleS(180,48,24,48,RED);
    
  sprintf( &display_tmp[0],"%03d",(u8)getMediaOutWaterTemper);	 
  LCD_ShowString(64,30,(const u8*)&display_tmp[0],GREEN);	
    
  sprintf( &display_tmp[0],"%03d",(u8)dargs.setMideaDegree0);	 
  LCD_ShowString(64,60,(const u8*)&display_tmp[0],GREEN);	

  sprintf( &display_tmp[0],"%03d",(u8)getMediaEnvTemper);	 
  LCD_ShowString(64,90,(const u8*)&display_tmp[0],GREEN);
  	 
  sprintf( &display_tmp[0],"%03d",(u8)getMediaMSG[0].run_m);	 
  LCD_ShowString(64,120,(const u8*)&display_tmp[0],GREEN);
  	
  sprintf( &display_tmp[0],"%03d",(u8)getMediaMSG[0].online_m);	 
  LCD_ShowString(64,150,(const u8*)&display_tmp[0],GREEN);

  if(	state_tcp==0 )   		LCD_ShowString(80,180,"断线",GREEN);  
  else if(	state_tcp==1 )   	LCD_ShowString(80,180,"连线",GREEN); 
  else if(	state_tcp==2 )   	LCD_ShowString(80,180,"在线",GREEN);
  
//--右半边   
  sprintf((char *)&display_tmp[0],"%04d",(in_water/100));	 
  LCD_ShowString(224,30,(const u8*)&display_tmp[0],GREEN);	

  sprintf((char *)&display_tmp[0],"%04d",(out_water/100));	 
  LCD_ShowString(224,60,(const u8*)&display_tmp[0],GREEN); 

  sprintf((char *)&display_tmp[0],"%04d",(loop_water/100));	 
  LCD_ShowString(224,90,(const u8*)&display_tmp[0],GREEN);
  
  sprintf((char *)&display_tmp[0],"%04d",elec);	 
  LCD_ShowString(224,120,(const u8*)&display_tmp[0],GREEN);


 
   if((getMediaMSG[0].err_code[0]&ERR0_DET)==ERR0_DET) realError=ERR_0;
  else if((getMediaMSG[0].err_code[0]&ERR1_DET)==ERR1_DET) realError=ERR_1 ; 
  else if((getMediaMSG[0].err_code[0]&ERR2_DET)==ERR2_DET) realError=ERR_2 ;
  else if((getMediaMSG[0].err_code[0]&ERR3_DET)==ERR3_DET) realError=ERR_3 ;
  else if((getMediaMSG[0].err_code[0]&ERR4_DET)==ERR4_DET) realError=ERR_4 ;
  else if((getMediaMSG[0].err_code[0]&ERR5_DET)==ERR5_DET) realError=ERR_5 ;
  else if((getMediaMSG[0].err_code[0]&ERR6_DET)==ERR6_DET) realError=ERR_6 ;
  else if((getMediaMSG[0].err_code[0]&ERR7_DET)==ERR7_DET) realError=ERR_7 ;
  else if((getMediaMSG[0].err_code[1]&ERR8_DET)==ERR8_DET) realError=ERR_8 ;

   if((getMediaMSG[0].protect_code[0]&PRO0_DET)==PRO0_DET) realError=PRO_1 ; 
  else if((getMediaMSG[0].protect_code[0]&PRO1_DET)==PRO1_DET) realError=PRO_1 ; 
  else if((getMediaMSG[0].protect_code[0]&PRO2_DET)==PRO2_DET) realError=PRO_2 ; 
  else if((getMediaMSG[0].protect_code[0]&PRO3_DET)==PRO3_DET) realError=PRO_3 ; 
  else if((getMediaMSG[0].protect_code[0]&PRO4_DET)==PRO4_DET) realError=PRO_4 ; 
  else if((getMediaMSG[0].protect_code[0]&PRO5_DET)==PRO5_DET) realError=PRO_5 ; 
  else if((getMediaMSG[0].protect_code[0]&PRO6_DET)==PRO6_DET) realError=PRO_6 ; 
  else if((getMediaMSG[0].protect_code[0]&PRO7_DET)==PRO7_DET) realError=PRO_7 ; 
  else if((getMediaMSG[0].protect_code[1]&PRO8_DET)==PRO8_DET) realError=PRO_8 ; 
  else if((getMediaMSG[0].protect_code[1]&PRO9_DET)==PRO9_DET) realError=PRO_9 ; 
 
  sprintf((char *)&display_tmp[0],"%02x",(u8)realError);	 
  LCD_ShowString(224,150,(const u8*)&display_tmp[0],GREEN);

 

   if(getMediaWaterHigh==0) level_percent=0;
   else if(getMediaWaterHigh==1) level_percent=25;
   else if(getMediaWaterHigh==2) level_percent=75;
   else if(getMediaWaterHigh==3) level_percent=100;

  sprintf((char *)&display_tmp[0],"%03d",(u8)level_percent);	 
  LCD_ShowString(224,180,(const u8*)&display_tmp[0],GREEN);
 	 
}

