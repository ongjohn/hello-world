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
unsigned char KeyFuncIndex        =0;       //������������
unsigned char MaxScreenDisp       =4;       //��Ļ�����ʾ��

//--------------------------------------------------------
unsigned char CurrentMenuIndex    =0;       //��ǰ�˵��������
unsigned char CurrentMenuItem     =0;       //��ǰ�˵���Ŀ
unsigned char CurrentMenuItemNum  =0;       //��ǰ�˵���Ŀ��
unsigned char **CurrentMenuString =0;       //��ǰ�˵���ʾ����ָ��
unsigned char CurrentMenuCur      =0;       //��ǰ�˵��������Ļ��λ��
unsigned char CurrentMenuPage     =0;       //��ǰ��ҳ�� һҳ4��ѡ��

//--------------------------------------------------------
unsigned char MenuDeep            =0;       //��ǰ�˵���ջ���

//-------------------------------------------------------- �û��˵���ʾ����
//--------��������
unsigned char * TopMenuString[]={"hello"};

//���ò˵�����
unsigned char * SetMenuString[]=
{
  "1.�����豸��ַ",
  "2.�����豸ʱ��",
  "3.����IP��ַ",
  "4.���ö˿ں�",
  "5.����һ·RS485��ַ",
  "6.���ý�ˮ���ַ",  
  "7.���ó�ˮ���ַ",
  "8.���û�ˮ���ַ",
  "9.���õ������ַ",
  "10.�ָ���������",	 
  "11.�ָ�ͨѶ����"
};

//��ַ�˵����� 
unsigned char * DevAddrMenuString[]   =
{
  "<������12λ�豸��ַ>" 
};


unsigned char * TimeMenuString[]   =
{
  "<������ʱ��>" 
}; 


unsigned char * IpMenuString[]   =
{
  "<������IP��ַ>" 
}; 

unsigned char * PortMenuString[]   =
{
  "<������4λ�˿ں�>" 
};


unsigned char * Rs485Addr1MenuString[]   =
{
  "<������һ·RS485��ַ>" 
}; 

unsigned char * Rs485Addr2MenuString[]   =
{
  "<�������ˮ��RS485��ַ>",
}; 

unsigned char * Rs485Addr3MenuString[]   =
{
  "<�������ˮ��RS485��ַ>",
}; 
unsigned char * Rs485Addr4MenuString[]   =
{
  "<�������ˮ��RS485��ַ>",
}; 
unsigned char * Rs485Addr5MenuString[]   =
{
  "<�����������RS485��ַ>",
}; 

unsigned char *ResetFacMenuString[] =
{
  "<�ָ���������״̬>",
  "1.��",
  "2.��"  
}; 

unsigned char *ResetCommunicateMenuString[] =
{
  "<�ָ�ͨѶ����>",
  "1.��",
  "2.��"  
};

//---------------------------------------------------------
//--------�˵��ṹ��
//--------�˵���ID, Ҳ���ǲ˵���������, �Ǹ��˵�˳�����ӵĽڵ�
//--------�˵��ı���,
//--------�˵�����Ŀ��
//--------�˵���ͼ��
//--------�˵�����ʾ����
//--------�˵�����ִ�к���
struct Menu
{
  unsigned char MenuID;
  unsigned char **MenuTitle;
  unsigned char MenuNum;
  unsigned char **MenuIco;
  unsigned char **MenuString;
  void* (*AddAction)();
};

//--------����˵�
struct Menu TopMenu=
{
  0,
  0,
  6,
  0,
  TopMenuString,
  (void*)0,
};

//--------���ò˵�
struct Menu SetMenu=
{
  6,
  0,
  11,//7
  0,
  SetMenuString,
  (void*)0,
};
//--------��ַ���ò˵�
struct Menu DevAdrMenu=
{
  12,
  0,
  1,
  0,
  DevAddrMenuString,
  (void*)0,
};

//--------ʱ�����ò˵�
struct Menu TimeMenu=
{
  18,
  0,
  1,
  0,
  TimeMenuString,
  (void*)0,
};

//--------IP���ò˵�
struct Menu IpMenu=
{
  24,
  0,
  1,
  0,
  IpMenuString,
  (void*)0,
};

//--------Port���ò˵�
struct Menu PortMenu=
{
  30,
  0,
  1,
  0,
  PortMenuString,
  (void*)0,
}; 
//--------Port���ò˵�
struct Menu Rs485addr1Menu=
{
  36,
  0,
  1,
  0,
  Rs485Addr1MenuString,
  (void*)0,
};

//--------��ˮ
struct Menu Rs485addr2Menu=
{
  42,
  0,
  1,
  0,
  Rs485Addr2MenuString,
  (void*)0,
}; 

//--------outˮ
struct Menu Rs485addr3Menu=
{
  48,//
  0,
  1,
  0,
  Rs485Addr3MenuString,
  (void*)0,
}; 

//--------��ˮ
struct Menu Rs485addr4Menu=
{
  54,//
  0,
  1,
  0,
  Rs485Addr4MenuString,
  (void*)0,
};

//--------����
struct Menu Rs485addr5Menu=
{
  60,//
  0,
  1,
  0,
  Rs485Addr5MenuString,
  (void*)0,
};

//--------�ָ������������ò˵�
struct Menu ResetFacMenu=
{
  66,
  0,
  3,
  0,
  ResetFacMenuString,
  (void*)0,
}; 

//--------�ָ�ͨѶ�������ò˵�
struct Menu ResetCommunicateMenu=
{
  72,
  0,
  3,
  0,
  ResetCommunicateMenuString,
  (void*)0,
}; 

//------------------------------------------------------------�˵��ں�
 

//--------�˵���ջ
//--------�˵�ID, Ҳ����������
//--------�˵���
//--------�˵���Ŀ��
//--------�˵���ʾ����
//--------���λ��
//--------�˵���������
struct MenuStack
{
  unsigned char MenuStackIndex;
  unsigned char MenuStackItem;
  unsigned char MenuStackItemNum;
  unsigned char **MenuStackString;
  unsigned char MenuStackCur;
  unsigned char MenuStackScroll;
};

 
//--------��ǰ�˵���ջ
 

 
//--------����˵�����
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

//--------��ǰ��˵���������
void (*KeyFuncPtr)();

typedef struct 
{
  unsigned char KeyStateIndex;			//����
  unsigned char KeySetState;			//����
  unsigned char KeyEnterState;			//ȷ��
  unsigned char KeyUpState;			//�Ϸ�
  unsigned char KeyDownState;			//�·�
  unsigned char KeyReturnState;		//����
  void (*CurrentOperate)();			//ִ��
}KeyFuncStruct;

//--------��������
KeyFuncStruct KeyTab[]=
{   
  { 0, 1, 2, 3, 4, 5, (DisMenuTopInit)}, //����˵� 
  { 1, 0, 0, 0, 0, 0, (DisMenuTopSet)},
  { 2, 0, 0, 0, 0, 0, (DisMenuTopEnter)},
  { 3, 0, 0, 0, 0, 0, (DisMenuTopUp)},
  { 4, 0, 0, 0, 0, 0, (DisMenuTopDown)},
  { 5, 0, 0, 0, 0, 0, (DisMenuTopReturn)},

  { 6, 7, 8, 9, 10,11,(DisMenuLevel0Init)},//��һ�����ò˵� 
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


  { 48, 49,50,51,52,53,(DisMenuDevRs485Adr3Init)}, //��
  { 49, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr3Set)},
  { 50, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr3Enter)},
  { 51, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr3Up)},
  { 52, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr3Down)},
  { 53, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr3Return)},	

  { 54, 55,56,57,58,59,(DisMenuDevRs485Adr4Init)}, //��
  { 55, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr4Set)},
  { 56, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr4Enter)},
  { 57, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr4Up)},
  { 58, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr4Down)},
  { 59, 0, 0, 0, 0, 0, (DisMenuDevRs485Adr4Return)},
  
  { 60, 61,62,63,64,65,(DisMenuDevRs485Adr5Init)}, //����
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

//--------������һ��˵�
void TurnToNextMenu(unsigned char menuid)
{
  KeyFuncPtr   = KeyTab[menuid].CurrentOperate;
  (*KeyFuncPtr)();
}

 

//--------��������˵���������
void AmendKeyIndex(unsigned char menuid)
{
  KeyFuncIndex = menuid;
}

  
 

//----------------------------------------------------------- �˵�����
//--------��ӭ����
void DispMenu(void)
{
}


//������
void DisMenuTopInit(void)
{
  //unsigned char i,MaxDisp=0;
    
	TimerStart(13,_5S);

  CurrentMenuIndex    = TopMenu.MenuID;         //������ʼ��
  CurrentMenuItem     = 1;
  CurrentMenuItemNum  = TopMenu.MenuNum;
  CurrentMenuString   = TopMenu.MenuString;
  CurrentMenuCur      = 1;
  CurrentMenuPage   = 0;


  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������
  LCD_Clr();
  
//  LCD_ShowString(28,0,"[����Զ���ȱÿ���ϵͳ]",GRED); 
//   
//  LCD_ShowString(0,30,"ˮ��:   ��",GRED);    LCD_ShowString(160,30,"��ˮ:    ��",GRED);
//  LCD_ShowString(0,60,"��ˮ:   ��",GRED);  	LCD_ShowString(160,60,"��ˮ:    ��",GRED);
//  LCD_ShowString(0,90,"����:   ��",GRED);	LCD_ShowString(160,90,"��ˮ:    ��",GRED);
//  LCD_ShowString(0,120,"����:  ̨",GRED);   LCD_ShowString(160,120,"����:    ��",GRED);
//  LCD_ShowString(0,150,"����:  ̨",GRED); 	LCD_ShowString(160,150,"����:",GRED);
//  LCD_ShowString(0,180,"GPRS:",GRED);  	LCD_ShowString(160,180,"ˮλ:   %",GRED);  
  
  LCD_ShowString(28,0,"[����Զ���ȱÿ���ϵͳ]",GRED); 
  LCD_ShowString(0,30,"ˮ��:   ��",GRED);    LCD_ShowString(160,30,"��ˮ:    ��",GRED);
  LCD_ShowString(0,60,"��ˮ:   ��",GRED);  	LCD_ShowString(160,60,"��ˮ:    ��",GRED);
  LCD_ShowString(0,90,"����:   ��",GRED);	LCD_ShowString(160,90,"��ˮ:    ��",GRED);
  LCD_ShowString(0,120,"����:   ̨",GRED);   LCD_ShowString(160,120,"����:    ��",GRED);
  LCD_ShowString(0,150,"����:   ̨",GRED); 	LCD_ShowString(160,150,"����:",GRED);
  LCD_ShowString(0,180,"GPRS:",GRED);  	LCD_ShowString(160,180,"ˮλ:   %",GRED);  

  ui_flush_idle_display();


  
//  LCD_ShowString(50,30,"ˮ���¶�:  ��",GRED);
//   LCD_ShowString(50,60,"��ˮ�¶�:  ��",GRED);  
//  LCD_ShowString(50,90,"�����¶�:  ��",GRED);
//  LCD_ShowString(50,120,"���л���:  ̨",GRED);
//  LCD_ShowString(50,150,"���߻���:  ̨",GRED); 
//  LCD_ShowString(50,180,"����״̬: ��",GRED);  
//  LCD_ShowString(50,210,"ˮλ�߶�:  %",GRED);  
////  LCD_ShowString(270,70,"ˮλ",GRED); 
////  LCD_Rectangle(4, 100,48,140,GRAY);	   
////  LCD_Rectangle(12,210,40,30,BLUE);
////  LCD_Rectangle(12,175,40,30,BLUE);
////  LCD_Rectangle(12,140,40,30,BLUE);
////  LCD_Rectangle(12,105,40,30,BLUE);
}
void DisMenuTopSet(void)
{
  AmendKeyIndex(CurrentMenuIndex);       //������ǰ�㰴������
  LCD_Clr();
  TurnToNextMenu(SetMenu.MenuID);
}
void DisMenuTopEnter(void)
{
  AmendKeyIndex(CurrentMenuIndex);       //������ǰ�㰴������




}
void DisMenuTopUp(void)
{
  s8 tmp_display[10];
  TimerStop(13);
  AmendKeyIndex(CurrentMenuIndex);              //������ǰ�㰴������
  LCD_Clr();

  LCD_ShowString(30,0,"---ϵͳ��ѯ---",GRED);
  LCD_ShowString(30,30,"�ϱ�ʱ��:",GRED);
  LCD_ShowString(30,60,"����ʱ��:",GRED);
															  

  sprintf( (char *)&tmp_display[0],"%03d",(u8)dargs.g_hartheattime);	 
  LCD_ShowString(150,30,(const u8*)&tmp_display[0],GREEN);	


  sprintf( (char *)&tmp_display[0],"%05d",dargs.upload_min);	 
  LCD_ShowString(150,60,(const u8*)&tmp_display[0],GREEN);	


  LCD_ShowString(260,30,"(��)",GRED);	
  LCD_ShowString(260,60,"(��)",GRED);
  
//  LCD_ShowString(30,90,"��ˮˮ��:",GRED);
//  LCD_ShowString(30,120,"ʹ�õ���:",GRED); 
//  LCD_ShowString(30,150,"���ϴ���:",GRED);  
}   
void DisMenuTopDown(void)
{
  AmendKeyIndex(CurrentMenuIndex);              //������ǰ�㰴������
        
 	
}
void DisMenuTopReturn(void)
{
  AmendKeyIndex(CurrentMenuIndex);              //������ǰ�㰴������

  TurnToNextMenu(TopMenu.MenuID);
}


//���ò�˵�
void DisMenuLevel0Init(void)
{
  unsigned char i,MaxDisp=0;

  CurrentMenuIndex    = SetMenu.MenuID;         //������ʼ��
  CurrentMenuItem     = 1;//1
  CurrentMenuItemNum  =  SetMenu.MenuNum; //max=10
  CurrentMenuString   = SetMenu.MenuString;
  CurrentMenuCur      = 1;//1
  CurrentMenuPage   	= 0;
  MaxScreenDisp	 	= SetMenu.MenuNum;//max=10
  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������
  TimerStop(13);

  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(50,i*30,*(CurrentMenuString+i),GREEN);    //��˵�
  }
}
void DisMenuLevel0Set(void)
{
  AmendKeyIndex(CurrentMenuIndex);       //������ǰ�㰴������
}
void DisMenuLevel0Enter(void)
{
  AmendKeyIndex(CurrentMenuIndex);       //������ǰ�㰴������
  switch(CurrentMenuItem)                //�����²�˵�
  {
    case 1: //���õ�ַ
      TurnToNextMenu(DevAdrMenu.MenuID); 
      break;
    case 2: //����ʱ��	
      TurnToNextMenu(TimeMenu.MenuID); 
      break; 	       
    case 3: //����ʱ��	
      TurnToNextMenu(IpMenu.MenuID); 
      break;	       
    case 4: //����ʱ��	
      TurnToNextMenu(PortMenu.MenuID); 
      break;	       
    case 5: //����ʱ��	
      TurnToNextMenu(Rs485addr1Menu.MenuID); 
      break;	       
    case 6: //���ý�ˮ
      TurnToNextMenu(Rs485addr2Menu.MenuID); 
      break;
    case 7: //���ó�ˮ
      TurnToNextMenu(Rs485addr3Menu.MenuID); 
      break;
    case 8: //���û�ˮ
      TurnToNextMenu(Rs485addr4Menu.MenuID); 
      break;
    case 9: //���õ���
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

  AmendKeyIndex(CurrentMenuIndex);              //������ǰ�㰴������

  CurrentMenuCur++;                             //�������
  CurrentMenuItem=CurrentMenuCur;

  if(CurrentMenuCur>CurrentMenuItemNum)
  {
    CurrentMenuItem=CurrentMenuCur=1;
  }  

   CurrentMenuPage=(CurrentMenuCur-1)/8;					//���㵱ǰ��ҳ����һҳ4��
   
  // if(CurrentMenuItemNum<MaxScreenDisp)           //��ǰ��˵���Ŀ��С����Ļ�����ʾ����
   if(CurrentMenuCur<9)
   {
      MaxDisp = 8;               //��ɨ������Ϊ��ǰ��˵�����
   }
   else
   {
     /* if((CurrentMenuItemNum-CurrentMenuPage*MaxScreenDisp)>MaxScreenDisp)//��ǰʣ����ʾ������������Ļ�����ʾ���� ��ɨ������Ϊ��Ļ�����ʾ����
      {
      		MaxDisp=MaxScreenDisp;
      }
      else
      {
      		MaxDisp=CurrentMenuItemNum-CurrentMenuPage*MaxScreenDisp;//��ʣ������С����Ļ��ʾ������ݣ���ֻ��ʾʣ�������
      }		*/

	  MaxDisp=3;
   }

  LCD_Clr();
  LCD_Rectangle(0,((CurrentMenuCur-1)%8)*30,320,30,RED);
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(50,i*30,*(CurrentMenuString+CurrentMenuPage*8+i),GREEN);    //��˵�
  }	
}   
void DisMenuLevel0Down(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);              //������ǰ�㰴������

  CurrentMenuCur--;                             //�������
  CurrentMenuItem=CurrentMenuCur;

  if(CurrentMenuCur<=0)             						//���ص�����ͷ�������ʼ��ʾ 
  {
    CurrentMenuItem=CurrentMenuCur=CurrentMenuItemNum;
  }  



   CurrentMenuPage=(CurrentMenuCur-1)/8;					//���㵱ǰ��ҳ����һҳ4��
   
//   if(CurrentMenuItemNum<MaxScreenDisp)           //��ǰ��˵���Ŀ��С����Ļ�����ʾ����
//   {
//      MaxDisp = CurrentMenuItemNum;               //��ɨ������Ϊ��ǰ��˵�����
//   }
//   else
//   {
//      if((CurrentMenuItemNum-CurrentMenuPage*MaxScreenDisp)>MaxScreenDisp)//��ǰʣ����ʾ������������Ļ�����ʾ���� ��ɨ������Ϊ��Ļ�����ʾ����
//      {
//      		MaxDisp=MaxScreenDisp;
//      }
//      else
//      {
//      		MaxDisp=CurrentMenuItemNum-CurrentMenuPage*MaxScreenDisp;//��ʣ������С����Ļ��ʾ������ݣ���ֻ��ʾʣ�������
//    	}
//   }

   if(CurrentMenuCur<9)
   {
      MaxDisp = 8;               //��ɨ������Ϊ��ǰ��˵�����
   }
   else
   {
	  MaxDisp=3;
   }

  LCD_Clr();
  LCD_Rectangle(0,((CurrentMenuCur-1)%8)*30,320,30,RED);
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(50,i*30,*(CurrentMenuString+CurrentMenuPage*8+i),GREEN);    //��˵�
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

  CurrentMenuIndex    = DevAdrMenu.MenuID;         //������ʼ��
  CurrentMenuItem     = 1;
  CurrentMenuItemNum  = DevAdrMenu.MenuNum;
  CurrentMenuString   = DevAdrMenu.MenuString;
  CurrentMenuCur      = 1;
  CurrentMenuPage     = 0;
  Column              = 0;
  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(DevAddrMenuString+i),GREEN);    //��˵�	  
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


  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(DevAddrMenuString+i),GREEN);    //��˵�	  
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
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(DevAddrMenuString+i),GREEN);    //��˵�	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)displaybuf,GREEN);
}   
void DisMenuDevAdrEnter(void)
{
  unsigned char i,MaxDisp=0;

  CurrentMenuIndex    = DevAdrMenu.MenuID;         //������ʼ��
  CurrentMenuItemNum  = DevAdrMenu.MenuNum;
  CurrentMenuString   = DevAdrMenu.MenuString;

  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  ++Column;
  if(Column>11)
  {
  	SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//��ȡ��������

    Column=0;
    dargs.devno[0]=(displaybuf[0]-'0')*10+(displaybuf[1]-'0');
    dargs.devno[1]=(displaybuf[2]-'0')*10+(displaybuf[3]-'0');
    dargs.devno[2]=(displaybuf[4]-'0')*10+(displaybuf[5]-'0');
    dargs.devno[3]=(displaybuf[6]-'0')*10+(displaybuf[7]-'0');
    dargs.devno[4]=(displaybuf[8]-'0')*10+(displaybuf[9]-'0');
    dargs.devno[5]=(displaybuf[10]-'0')*10+(displaybuf[11]-'0');
    SPI_FLASH_SectorErase(0);
    SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));

    AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(DevAddrMenuString+i),GREEN);    //��˵�	  
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
    AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(DevAddrMenuString+i),GREEN);    //��˵�	  
  }

  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[YEAR_START_POS],GREEN);
}


//-----
void DisMenuDevTimeInit(void)
{
  unsigned char i,MaxDisp=0;

  memset(displaybuf,'\0',sizeof(displaybuf));
  CurrentMenuIndex    = TimeMenu.MenuID;         //������ʼ��
  CurrentMenuItem     = 1;
  CurrentMenuItemNum  = TimeMenu.MenuNum;
  CurrentMenuString   = TimeMenu.MenuString;
  CurrentMenuCur      = 1;
  CurrentMenuPage     = 0;
  Column	      = 0;
  Row 		      = 0;
  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(TimeMenuString+i),GREEN);    //��˵�	  
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
    
    if(month>12)//������12��
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

    AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������
    TurnToNextMenu(SetMenu.MenuID); 
    return;
    //����ĩβ
  }

  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(TimeMenuString+i),GREEN);    //��˵�	  
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


  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(TimeMenuString+i),GREEN);    //��˵�	  
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

  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(TimeMenuString+i),GREEN);    //��˵�	  
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
    AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(TimeMenuString+i),GREEN);    //��˵�	  
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
  CurrentMenuIndex    = IpMenu.MenuID;         //������ʼ��
  CurrentMenuItem     = 1;
  CurrentMenuItemNum  = IpMenu.MenuNum;
  CurrentMenuString   = IpMenu.MenuString;
  CurrentMenuCur      = 1;
  CurrentMenuPage     = 0;
  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(IpMenuString+i),GREEN);    //��˵�	  
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
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
  	SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//��ȡ��������

    Column=0;//del
    //�����β
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


    AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }

  LCD_Clr();
  for(i=0;i<MaxDisp;i++)
  { 	
    LCD_ShowString(0,30,*(IpMenuString+i),GREEN);    //��˵�	  
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


  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(IpMenuString+i),GREEN);    //��˵�	  
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


  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(IpMenuString+i),GREEN);    //��˵�	  
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
    AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(IpMenuString+i),GREEN);    //��˵�	  
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
  CurrentMenuIndex    = PortMenu.MenuID;         //������ʼ��
  CurrentMenuItem     = 1;
  CurrentMenuItemNum  = PortMenu.MenuNum;
  CurrentMenuString   = PortMenu.MenuString;
  CurrentMenuCur      = 1;
  CurrentMenuPage   	= 0;
  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(PortMenuString+i),GREEN);    //��˵�	  
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
  	SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//��ȡ��������

    Column=0;
    dargs.port=(displaybuf[0]-'0')*10000 + (displaybuf[1]-'0') *1000 + (displaybuf[2]-'0')*100 + (displaybuf[3]-'0')*10 + (displaybuf[4]-'0');
    SPI_FLASH_SectorErase(0);
    SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));
    AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������
    TurnToNextMenu(SetMenu.MenuID); 
    return;
    //����ĩβ
  }
  AmendKeyIndex(CurrentMenuIndex); 
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(PortMenuString+i),GREEN);    //��˵�	  
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
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(PortMenuString+i),GREEN);    //��˵�	  
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

  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(PortMenuString+i),GREEN);    //��˵�	  
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
    //����ĩβ
    AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(PortMenuString+i),GREEN);    //��˵�	  
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
  CurrentMenuIndex    = Rs485addr1Menu.MenuID;         //������ʼ��
  CurrentMenuItem     = 1;
  CurrentMenuItemNum  = Rs485addr1Menu.MenuNum;
  CurrentMenuString   = Rs485addr1Menu.MenuString;
  CurrentMenuCur      = 1;
  CurrentMenuPage   	= 0;
  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(Rs485Addr1MenuString+i),GREEN);    //��˵�	  
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

  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  if(++Column>2)
  {
  	SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//��ȡ��������

    Column=0;
    res = (displaybuf[0]-'0')*100 + (displaybuf[1]-'0')*10 + (displaybuf[2]-'0');
    if(res>255)
    {
      res=255;
    } 
	dargs.rs485_adr1=(u8)res;
    SPI_FLASH_SectorErase(0);
    SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));
    AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(Rs485Addr1MenuString+i),GREEN);    //��˵�	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr1Up(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  if(++displaybuf[Column]>'9')
  {
    displaybuf[Column]='0';
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(Rs485Addr1MenuString+i),GREEN);    //��˵�	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
  AmendKeyIndex(CurrentMenuIndex); 
}
void DisMenuDevRs485Adr1Down(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  if(--displaybuf[Column]<'0')
  {
    displaybuf[Column]='9';
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(Rs485Addr1MenuString+i),GREEN);    //��˵�	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr1Return(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  if(--Column<0)
  {
    Column=0;
    AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(Rs485Addr1MenuString+i),GREEN);    //��˵�	  
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
  CurrentMenuIndex    = Rs485addr2Menu.MenuID;         //������ʼ��
  CurrentMenuItem     = 1;
  CurrentMenuItemNum  = Rs485addr2Menu.MenuNum;
  CurrentMenuString   = Rs485addr2Menu.MenuString;
  CurrentMenuCur      = 1;
  CurrentMenuPage   	= 0;
  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //��˵�	  
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

  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  if(++Column>=8)
  {
	SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//��ȡ��������
  
    Column=0;																	   
	res[0]=	((displaybuf[0]-'0')<<4) |  (displaybuf[1]-'0');	 					   
	res[1]=	((displaybuf[2]-'0')<<4) |  (displaybuf[3]-'0');					   
	res[2]=	((displaybuf[4]-'0')<<4) |  (displaybuf[5]-'0');					   
	res[3]=	((displaybuf[6]-'0')<<4) |  (displaybuf[7]-'0');
    dargs.rs485InWaterAddr =  ((u32)res[0]<<24) | ((u32)res[1]<<16) | ((u32)res[2]<<8) |((u32)res[3])  ;
 

    SPI_FLASH_SectorErase(0);
    SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));
    AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //��˵�	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr2Up(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  if(++displaybuf[Column]>'9')
  {
    displaybuf[Column]='0';
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //��˵�	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr2Down(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  if(--displaybuf[Column]<'0')
  {
    displaybuf[Column]='9';
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //��˵�	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr2Return(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  if(--Column<0)
  {
    Column=0;
    AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //��˵�	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
 }


//--------
void DisMenuDevRs485Adr3Init(void)
{
  unsigned char i,MaxDisp=0;
  memset(displaybuf,'\0',sizeof(displaybuf));
  CurrentMenuIndex    = Rs485addr3Menu.MenuID;         //������ʼ��
  CurrentMenuItem     = 1;
  CurrentMenuItemNum  = Rs485addr3Menu.MenuNum;
  CurrentMenuString   = Rs485addr3Menu.MenuString;
  CurrentMenuCur      = 1;
  CurrentMenuPage   	= 0;
  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //��˵�	  
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

  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  if(++Column>=8)
  {
	SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//��ȡ��������
  
    Column=0;																	   
	res[0]=	((displaybuf[0]-'0')<<4) |  (displaybuf[1]-'0');	 					   
	res[1]=	((displaybuf[2]-'0')<<4) |  (displaybuf[3]-'0');					   
	res[2]=	((displaybuf[4]-'0')<<4) |  (displaybuf[5]-'0');					   
	res[3]=	((displaybuf[6]-'0')<<4) |  (displaybuf[7]-'0');
    dargs.rs485OutWaterAddr =  ((u32)res[0]<<24) | ((u32)res[1]<<16) | ((u32)res[2]<<8) |((u32)res[3])  ;
  

    SPI_FLASH_SectorErase(0);
    SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));
    AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //��˵�	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr3Up(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  if(++displaybuf[Column]>'9')
  {
    displaybuf[Column]='0';
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //��˵�	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr3Down(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  if(--displaybuf[Column]<'0')
  {
    displaybuf[Column]='9';
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //��˵�	  
  }									
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr3Return(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  if(--Column<0)
  {
    Column=0;
    AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //��˵�	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
 }



//--------
void DisMenuDevRs485Adr4Init(void)
{
  unsigned char i,MaxDisp=0;
  memset(displaybuf,'\0',sizeof(displaybuf));
  CurrentMenuIndex    = Rs485addr4Menu.MenuID;         //������ʼ��
  CurrentMenuItem     = 1;
  CurrentMenuItemNum  = Rs485addr4Menu.MenuNum;
  CurrentMenuString   = Rs485addr4Menu.MenuString;
  CurrentMenuCur      = 1;
  CurrentMenuPage   	= 0;
  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //��˵�	  
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

  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  if(++Column>=8)
  {
	SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//��ȡ��������
  
    Column=0;																	   
	res[0]=	((displaybuf[0]-'0')<<4) |  (displaybuf[1]-'0');	 					   
	res[1]=	((displaybuf[2]-'0')<<4) |  (displaybuf[3]-'0');					   
	res[2]=	((displaybuf[4]-'0')<<4) |  (displaybuf[5]-'0');					   
	res[3]=	((displaybuf[6]-'0')<<4) |  (displaybuf[7]-'0');
    dargs.rs485LoopWaterAddr =  ((u32)res[0]<<24) | ((u32)res[1]<<16) | ((u32)res[2]<<8) |((u32)res[3])  ;

  

    SPI_FLASH_SectorErase(0);
    SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));
    AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //��˵�	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr4Up(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  if(++displaybuf[Column]>'9')
  {
    displaybuf[Column]='0';
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //��˵�	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr4Down(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  if(--displaybuf[Column]<'0')
  {
    displaybuf[Column]='9';
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //��˵�	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr4Return(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  if(--Column<0)
  {
    Column=0;
    AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //��˵�	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
 }


//--------
void DisMenuDevRs485Adr5Init(void)
{
  unsigned char i,MaxDisp=0;
  memset(displaybuf,'\0',sizeof(displaybuf));
  CurrentMenuIndex    = Rs485addr5Menu.MenuID;         //������ʼ��
  CurrentMenuItem     = 1;
  CurrentMenuItemNum  = Rs485addr5Menu.MenuNum;
  CurrentMenuString   = Rs485addr5Menu.MenuString;
  CurrentMenuCur      = 1;
  CurrentMenuPage   	= 0;
  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //��˵�	  
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

  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  if(++Column>=8)
  {
	SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//��ȡ��������
  
    Column=0;																	   
	res[0]=	((displaybuf[0]-'0')<<4) |  (displaybuf[1]-'0');	 					   
	res[1]=	((displaybuf[2]-'0')<<4) |  (displaybuf[3]-'0');					   
	res[2]=	((displaybuf[4]-'0')<<4) |  (displaybuf[5]-'0');					   
	res[3]=	((displaybuf[6]-'0')<<4) |  (displaybuf[7]-'0');
    dargs.rs485ElecAddr =  ((u32)res[0]<<24) | ((u32)res[1]<<16) | ((u32)res[2]<<8) |((u32)res[3])  ;
  

    SPI_FLASH_SectorErase(0);
    SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));
    AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //��˵�	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr5Up(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  if(++displaybuf[Column]>'9')
  {
    displaybuf[Column]='0';
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //��˵�	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr5Down(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  if(--displaybuf[Column]<'0')
  {
    displaybuf[Column]='9';
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //��˵�	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
}
void DisMenuDevRs485Adr5Return(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  if(--Column<0)
  {
    Column=0;
    AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������
    TurnToNextMenu(SetMenu.MenuID); 
    return;
  }
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,30,*(CurrentMenuString+i),GREEN);    //��˵�	  
  }
  LCD_ShowString(0+Column*16,60,"V",GRED);  
  LCD_ShowString(0,90,(const u8*)&displaybuf[0],GREEN);
 }



//--	  
void DisMenuResetFacInit(void)
{
  unsigned char i,MaxDisp=0;

  memset(displaybuf,'\0',sizeof(displaybuf));
  CurrentMenuIndex    = ResetFacMenu.MenuID;         //������ʼ��
  CurrentMenuItem     = 1;
  CurrentMenuItemNum  = ResetFacMenu.MenuNum;
  CurrentMenuString   = ResetFacMenu.MenuString;
  CurrentMenuCur      = 1;
  CurrentMenuPage     = 0;
  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,i*30,*(CurrentMenuString+i),GREEN);    //��˵�
  }
}
void DisMenuResetFacSet(void)
{
  AmendKeyIndex(CurrentMenuIndex); 
}

 
void DisMenuResetFacEnter(void)
{
  AmendKeyIndex(CurrentMenuIndex);       //������ǰ�㰴������
  switch(CurrentMenuItem)                //�����²�˵�
  {
    case 1: 
      LCD_Clr();
      LCD_ShowString(0,60,"���ڱ���...",GREEN);    //��˵�
      
      SPI_FLASH_SectorErase(0);
	  memset(&dargs.devno[0],0x00,sizeof(dargs));//ȫ������
	  memset(&dargs.pwd[0],0xFF,7);	//ͨ������+����ͨѶ��rs485��ַ
	  dargs.g_hartheattime=1;//������1����
	  dargs.upload_min=5;//Ĭ�϶�ʱ�ϱ�ʱ��5����
	  dargs.setMideaDegree0=40;//Ĭ�ϳ�ˮ�¶�50��C
	  dargs.setMideaLoopDegree=40;
 
      SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));
      delay_ms(1000); 
      LCD_Clr();
      LCD_ShowString(0,60,"����ɹ�",GREEN);    //��˵�
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

  AmendKeyIndex(CurrentMenuIndex);              //������ǰ�㰴������

  CurrentMenuCur++;                             //�������
  CurrentMenuItem=CurrentMenuCur;

  if(CurrentMenuCur>2)//CurrentMenuItemNum)
  {
    CurrentMenuItem=CurrentMenuCur=1;
  }  

  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,i*30,*(CurrentMenuString+i),GREEN);    //��˵�
  }	
}
void DisMenuResetFacDown(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);              //������ǰ�㰴������

  CurrentMenuCur--;                             //�������
  CurrentMenuItem=CurrentMenuCur;

  if(CurrentMenuCur<1)             						//���ص�����ͷ�������ʼ��ʾ 
  {
    CurrentMenuItem=CurrentMenuCur=2;//CurrentMenuItemNum;
  }  

  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,i*30,*(CurrentMenuString+i),GREEN);    //��˵�
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
  CurrentMenuIndex    = ResetCommunicateMenu.MenuID;         //������ʼ��
  CurrentMenuItem     = 1;
  CurrentMenuItemNum  = ResetCommunicateMenu.MenuNum;
  CurrentMenuString   = ResetCommunicateMenu.MenuString;
  CurrentMenuCur      = 1;
  CurrentMenuPage     = 0;
  AmendKeyIndex(CurrentMenuIndex);             //������ǰ�㰴������

  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));
  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,i*30,*(CurrentMenuString+i),GREEN);    //��˵�
  }
}
void DisMenuCommunicateSet(void)
{
  AmendKeyIndex(CurrentMenuIndex); 
}

 
void DisMenuCommunicateEnter(void)
{
  AmendKeyIndex(CurrentMenuIndex);       //������ǰ�㰴������
  switch(CurrentMenuItem)                //�����²�˵�
  {
    case 1: 
      LCD_Clr();
      LCD_ShowString(0,60,"���ڻָ�...",GREEN);    //��˵�
   	  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));//��ȡ��������
      SPI_FLASH_SectorErase(0);
 	  memset(&dargs.pwd[0],0xFF,6);	//ͨ������ 
      SPI_FLASH_BufferWrite(&dargs.devno[0],0,sizeof(dargs));
      delay_ms(500); 
      LCD_Clr();
      LCD_ShowString(0,60,"�ָ��ɹ�",GREEN);    //��˵�
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

  AmendKeyIndex(CurrentMenuIndex);              //������ǰ�㰴������

  CurrentMenuCur++;                             //�������
  CurrentMenuItem=CurrentMenuCur;

  if(CurrentMenuCur>2)//CurrentMenuItemNum)
  {
    CurrentMenuItem=CurrentMenuCur=1;
  }  

  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,i*30,*(CurrentMenuString+i),GREEN);    //��˵�
  }	
}
void DisMenuCommunicateDown(void)
{
  unsigned char i,MaxDisp=0;

  AmendKeyIndex(CurrentMenuIndex);              //������ǰ�㰴������

  CurrentMenuCur--;                             //�������
  CurrentMenuItem=CurrentMenuCur;

  if(CurrentMenuCur<1)             						//���ص�����ͷ�������ʼ��ʾ 
  {
    CurrentMenuItem=CurrentMenuCur=2;//CurrentMenuItemNum;
  }  

  if(CurrentMenuItemNum<MaxScreenDisp)         //��ǰ����Ŀ��С�������Ļ��ʾ��
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
    LCD_ShowString(0,i*30,*(CurrentMenuString+i),GREEN);    //��˵�
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
    case KEY_SET://���ü�
      KeyFuncIndex = KeyTab[KeyFuncIndex].KeySetState;
      KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
      (*KeyFuncPtr)();
      break;
    case KEY_ENTER://ȷ�ϼ�
      KeyFuncIndex = KeyTab[KeyFuncIndex].KeyEnterState;
      KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
      (*KeyFuncPtr)();
      break;
    case KEY_UP://�Ϸ����Ӻż�
      KeyFuncIndex = KeyTab[KeyFuncIndex].KeyUpState;
      KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
      (*KeyFuncPtr)();
      break;
    case KEY_DN://�·������ż�
      KeyFuncIndex = KeyTab[KeyFuncIndex].KeyDownState;
      KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
      (*KeyFuncPtr)();
      break;
    case KEY_RETURN://���ؼ�
      KeyFuncIndex = KeyTab[KeyFuncIndex].KeyReturnState;
      KeyFuncPtr   = KeyTab[KeyFuncIndex].CurrentOperate;
      (*KeyFuncPtr)();
      break;    		
    default :break;
  }
}


//������ʾ����
void ui_flush_idle_display(void)
{

 
//  LCD_ShowString(0,30,"ˮ��:   ��",GRED);    LCD_ShowString(160,30,"��ˮ:    ��",GRED);
//  LCD_ShowString(0,60,"��ˮ:   ��",GRED);  	LCD_ShowString(160,60,"��ˮ:    ��",GRED);
//  LCD_ShowString(0,90,"����:   ��",GRED);	LCD_ShowString(160,90,"��ˮ:    ��",GRED);
//  LCD_ShowString(0,120,"����:   ̨",GRED);   LCD_ShowString(160,120,"����:    ��",GRED);
//  LCD_ShowString(0,150,"����:   ̨",GRED); 	LCD_ShowString(160,150,"����:",GRED);
//  LCD_ShowString(0,180,"GPRS:",GRED);  	LCD_ShowString(160,180,"ˮλ:   %",GRED);  

////--�建��
//  display_tmp[0]=0;	
//  display_tmp[1]=0;
//  display_tmp[2]=0;
//  display_tmp[3]=0;
//  realError=0;
//  level_percent=0;

//void LCD_Rectangle(u16 x,u16 y,u16 len,u16 wid,u16 rgb565)

////--����  					////--�Ұ��
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

  if(	state_tcp==0 )   		LCD_ShowString(80,180,"����",GREEN);  
  else if(	state_tcp==1 )   	LCD_ShowString(80,180,"����",GREEN); 
  else if(	state_tcp==2 )   	LCD_ShowString(80,180,"����",GREEN);
  
//--�Ұ��   
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

