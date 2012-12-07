
#ifndef	_GPRS_H
#define	_GPRS_H

#include "stm32f10x.h"


#define	GPRS_SIM900B 1
//#define	GPRS_GTM900C 1


#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif



#define	MIC29302_ON()		GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define	MIC29302_OFF()		GPIO_ResetBits(GPIOB,GPIO_Pin_6)


#define	GPRS_OPEN()		GPIO_SetBits(GPIOB,GPIO_Pin_7)
#define	GPRS_CLOSE()	GPIO_ResetBits(GPIOB,GPIO_Pin_7)



#define  AT_GL   128  //AT接收的最大长度

#define MAX_SMS_CONTENT_LENGTH        	64   //发送短信数据的最大长度
#define MAX_PHONE_NUMBER_LENGTH         20   //接收到信息电话号码字符长度
#define MAX_MESSAGE_CONTENT_LENGTH      64   //接收到信息内容最大字符长度


#define IMEI_NUMBER_LENGTH  15
#define IMSI_NUMBER_LENGTH  15
#define MAX_VERSION_LENGTH 5
#define MAX_GPRS_SENDING_DATA 512
#define MAX_GPRS_RECEIVING_DATA 1024


#define MAX_APN_LENGTH 31  //100
#define MAX_APN_USERNAME_LENGTH 31
#define MAX_APN_USERPASSWORD_LENGTH 31

#define MAX_IP_ADDRESS_LENGTH 31 //  24
#define MAX_PORT_NUMBER_LENGTH  31  //7 

#define MAX_PASSWORD_LENGTH 4 



#define GSTX    		0x69  //开始符
#define GETX  			0x16 //终止符
#define GINCLUDE		0    //包头
#define GLEN        		1    //长度(控制+地址+计数+功能+保留+数据)
#define GINCLUDE_CHK		2    //包头
#define GCOMMAND    		3    //控制域
#define GADDRESS    		4    //地址域  共6个字节
#define GCOUNT			10   //计数器
#define GTYPE       		12   //类型标识
#define GNULL			13   //保留
#define GDATA       		14   //数据
#define GCS  			TxBuffer[GLEN]+3       //校验索引值
#define G_ETX  			TxBuffer[GLEN]+4       //校验索引值
#define	GADR_LEN		6//地址域长度

//
#define DEV_SUCESS_DATA0	0x80
#define DEV_SUCESS_DATA1	0x00
#define DEV_ERROR_DATA0		0x81
#define DEV_ERROR_DATA1		

//PC主发给下位机的数据
#define AFN_PC_REQ_HEART	0x21
#define AFN_PC_SET_HEART	0x22
//#define AFN_RESERVE1
//
#define AFN_PC_REQ_CLK		0x25
#define AFN_PC_SET_CLK		0x26
#define AFN_PC_REQ_PWD		0x27
#define AFN_PC_SET_PWD		0x28
#define AFN_PC_REQ_CURDAT	0x29
#define AFN_PC_REQ_HISDAT	0x2A
#define AFN_PC_SET_ARGS		0x2B
#define AFN_PC_REQ_UPTIME	0x2C
#define AFN_PC_SET_UPTIME	0x2D
#define AFN_PC_REQ_STATUS	0x2E
#define AFN_PC_REQ_ARGS		0x2F
#define AFN_PC_SET_WLEV		0x30
#define AFN_PC_REQ_WDEGREE	0x31
#define AFN_PC_SET_OPENCLOSE	0x32
#define AFN_PC_SET_IOPENCLOSE	0x33

//下位机主发给PC的数据
#define AFN_LOG			0x82
#define AFN_HEART		0x83
#define AFN_MACHINE_ERR		0x89
#define AFN_UP_NORMALDAT	0x8A
#define AFN_UP_ADOPTTDAT	0x8C

#define CMD_LOG			0x91
#define CMD_PC2DEVLOG		0x90

__packed typedef struct S_ATbackTag
{
  char AT_RX_STA;         //接收状态标记
  u16 AT_RX_GL;	        //接收索引   
  char  ATbackStr[AT_GL];		//AT回复缓冲字符串
}S_ATback;


typedef enum AtOKTypeTag
{
  e_AT_None = 0,
  e_AT_Success,
  e_AT_Error
}E_ATOKType;


typedef enum MessageTypeTag
{
  e_Rec_UnRead = 0,
  e_Rec_Read,
  e_Sto_Unsent,
  e_Sto_Sent,
  e_All
}MessageType;

typedef enum E_InitStateTag
{
  eInitStart = 0,
  eIniteProductVer,
  eIniteModulVer,
  eInithead,
  eInitcreg,
  eInitcgreg,
  eInitImei,
  eInitImsi,
  eInitSMS,
  eInitSMS1,
  eInitSMS2,
  eInitSMS3,	
  eInitGPRS,
  eIniteTCPIP,
  eInitEnd
}E_InitState;
typedef enum E_InitResultTag
{
  eResultNone = 0,
  eInitSuccess,
  eInitError
}E_InitResult;


typedef enum E_SimCardStateTag
{
  eSimCardNotReady = 1,
  eSimCardReady    =2
}E_SimCardState;

typedef enum E_GPRSStateTag
{
  eGPRS_OFFLINE = 0,
  eGPRS_GTT,
  eGPRS_APN,
  eGPRS_UP,
  eGPRS_GETIP,
  eGPRS_CALL,
  eGPRS_OPEN,
  eGPRS_ONLINE,
  eGPRS_SEND,
  eGPRS_PUSH
}E_GPRSState;


__packed typedef struct S_DateTimeTag
{
  u16 nYear;
  u8 nMonth;
  u8 nDay;
  u8 nHour;
  u8 nMin;
  u8 nSec;
}S_DateTime;


//接收到短信相关信息
__packed typedef struct MessageInfoTag
{
  char szRecNumber[32];  //接收到电话号码
  char szContent[64];	    //接收到短信内容  
  char szSendingData[64];	 
}MessageInfo;

__packed typedef struct S_AppContextTag
{

  E_GPRSState eGPRSState;
  //	u8 sendingData[MAX_GPRS_SENDING_DATA];
  //	u16 nSendingLen;
  //	u8 receivingData[MAX_GPRS_RECEIVING_DATA];
  //	u16 nReceivingLen;

  //	BOOL bSuccess[eMaxGPRSCommand];
  //	E_GPRSCommand eCurrentGPRSCmd;
  //	E_ATOKType  eATOK;   
  E_InitState eInitState; //Init state
  //	char  eInitState;
  //	E_InitResult eInitResult; //init result
  //	char  eInitResult;
  //	char bLedHigh;

  //Add sim card state
  E_SimCardState eSimCardState;
  //	char bStartRead;

  //	U16 nLACValue; //current LAC
  //	U16 nCellIDValue; //current CellID value
}S_AppContext;

__packed typedef struct S_ConfigSettingTag
{


  char szAPN[32];	   //31 

  char szIPAddress[64];      //31
  char szPortNumber[32];	  // 31
}S_ConfigSetting;

 



extern u8 TxBuffer[512];  
extern u8 TxCache[128];
extern u8 countbuf[2];

extern S_AppContext g_sAppContext;
extern S_ConfigSetting g_setting;
extern MessageInfo g_sMessageInfo;
extern S_ATback g_sATback;





void GPRS_IOInitOPen(void);	
void GPRS_Open(void);
void GPRS_RST(void);

int SendATCommand(char *ATcom);
void processATCnf(char * strAT, u16 len);
int processATCnfData(char *pCmdResult);	
void SendMessage(char *szPhoneNumber, char *szContent);
int ParseReceiveMessage(char *szText, MessageInfo *pMessageInfo);
void GPRS_SendErrorCode(u8 id, u8 errCode);



int ProcessIncomingSMS(char *szSMSContent);


void  fill_data_nocrypto(u8 AFNdata,u8 COMdata,u8 *Gdata,u8 datarealen,u8* ctbuf);
void GPRS_StartUp(void);
void GPRS_ShutDown(void);
void GPRS_Initial(void);
void GprsProc(void);
void GPRS_SendHeart(void);
void GPRS_SendNormalData(void);


void sim900b_send_tcp_data(void);
extern u8 strBuffer[512];  

extern u8 send_st;
extern u8 TcpSendCt;
#endif
