#include "key.h"
#include "delay.h"
#include "usart1.h"
#include "ui.h"
#include "time_malloc.h"
#include "time_outProcess.h"
#include "buzz.h"
#include "gprs.h"

static BUTTON_T s_BtnReturn; 
static BUTTON_T s_BtnEnter; 
static BUTTON_T s_BtnDn;	 
static BUTTON_T s_BtnUp;	 
static BUTTON_T s_BtnSet;
static KEY_FIFO_T s_Key;	 

static uint8_t IsKeyReturn(void) 	{if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == Bit_SET) return 0; return 1;}
static uint8_t IsKeyEnter(void) 	{if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2) == Bit_SET) return 0; return 1;}
static uint8_t IsKeyDn(void) 		{if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == Bit_SET) return 0; return 1;}
static uint8_t IsKeyUp(void) 		{if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == Bit_SET) return 0; return 1;}
static uint8_t IsKeySet(void) 		{if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == Bit_SET) return 0; return 1;}

void PutKey(uint8_t _KeyCode)
{
  s_Key.Buf[s_Key.Write] = _KeyCode;

  if (++s_Key.Write  >= KEY_FIFO_SIZE)
  {
    s_Key.Write = 0;
  }
}

uint8_t GetKey(void)
{
  uint8_t ret;

  if (s_Key.Read == s_Key.Write)
  {
    return KEY_NONE;
  }
  else
  {
    ret = s_Key.Buf[s_Key.Read];

    if (++s_Key.Read >= KEY_FIFO_SIZE)
    {
      s_Key.Read = 0;
    }
    return ret;
  }
}


void DetectButton(BUTTON_T *_pBtn)
{
  if (_pBtn->IsKeyDownFunc())
  {
    if (_pBtn->Count < _pBtn->FilterTime)
    {
      _pBtn->Count = _pBtn->FilterTime;
    }
    else if(_pBtn->Count < 2 * _pBtn->FilterTime)
    {
      _pBtn->Count++;
    }
    else
    {
      if (_pBtn->State == 0)
      {
	_pBtn->State = 1;

	if (_pBtn->KeyCodeDown > 0)
	{
	  TimerStart(TT_BUZZ,_100mS);
	  BUZZ_ON();
	  PutKey(_pBtn->KeyCodeDown);

	}
      }
    }
  }
  else
  {
    if(_pBtn->Count > _pBtn->FilterTime)
    {
      _pBtn->Count = _pBtn->FilterTime;
    }
    else if(_pBtn->Count != 0)
    {
      _pBtn->Count--;
    }
    else
    {
      if (_pBtn->State == 1)
      {
	_pBtn->State = 0;
      }
    }
  }
}


void KEY_Scan(void)
{
  DetectButton(&s_BtnReturn);	 
  DetectButton(&s_BtnUp);
  DetectButton(&s_BtnSet);
  DetectButton(&s_BtnDn);
  DetectButton(&s_BtnEnter);	 	 	 	 
}

static void KeyInitMisc(void)
{
  s_Key.Read = 0;
  s_Key.Write = 0;

  s_BtnReturn.IsKeyDownFunc = IsKeyReturn;	 
  s_BtnReturn.FilterTime = BUTTON_FILTER_TIME;	 
  s_BtnReturn.Count = s_BtnReturn.FilterTime / 2; 
  s_BtnReturn.State = 0;					 
  s_BtnReturn.KeyCodeDown = KEY_RETURN;	 

  s_BtnEnter.IsKeyDownFunc = IsKeyEnter;	 
  s_BtnEnter.FilterTime = BUTTON_FILTER_TIME;	 
  s_BtnEnter.Count = s_BtnEnter.FilterTime / 2; 
  s_BtnEnter.State = 0;					 
  s_BtnEnter.KeyCodeDown = KEY_ENTER;

  s_BtnUp.IsKeyDownFunc = IsKeyUp;	 
  s_BtnUp.FilterTime = BUTTON_FILTER_TIME;	 
  s_BtnUp.Count = s_BtnUp.FilterTime / 2; 
  s_BtnUp.State = 0;					 
  s_BtnUp.KeyCodeDown = KEY_UP;

  s_BtnDn.IsKeyDownFunc = IsKeyDn;	 
  s_BtnDn.FilterTime = BUTTON_FILTER_TIME;	 
  s_BtnDn.Count = s_BtnDn.FilterTime / 2; 
  s_BtnDn.State = 0;					 
  s_BtnDn.KeyCodeDown = KEY_DN; 

  s_BtnSet.IsKeyDownFunc = IsKeySet;	 
  s_BtnSet.FilterTime = BUTTON_FILTER_TIME;	 
  s_BtnSet.Count = s_BtnSet.FilterTime / 2; 
  s_BtnSet.State = 0;					 
  s_BtnSet.KeyCodeDown = KEY_SET; 
}

void KEY_Initial(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);   

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  KeyInitMisc();
}


void KeyProc(void)
{
  u8 key_code;

  key_code = GetKey();
  if(key_code>KEY_RETURN)
  	return;
  if(key_code>0)
  {
    OnKey(key_code);						   
  }	
}

