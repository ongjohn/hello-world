#ifndef STORAGE_H
#define	STORAGE_H

#include "stm32f10x.h"
#include "spi_flash.h"

#define MIDEA	
#define MAX_MIDEA_MACHINE	10
#define	RECORD_FORMAT		32	 


__packed typedef struct
{

  u8 devno[6];//设备地址
  u8 pwd[6]; //通信密码
  u8 rs485_adr1;//rs2地址，美的对接接口
  u8 g_hartheattime;//心跳包
  u8 ip[4];
  u16 port;
  u16 upload_min;//定时上报时间

  u32 rs485InWaterAddr;//进水表地址
  u32 rs485OutWaterAddr;//出水表rs485地址
  u32 rs485LoopWaterAddr;//回水表rs485地址
  u32 rs485ElecAddr;//电表rs485地址
  
   u8 setMideaDegree0;//PC机设定的水箱出水温度
  u8 setMideaLoopDegree;//PC机设定的循环水温度
  u8 setMideaWaterLevel;//PC机设定的水位设置
  u8 setMideaIfLoop;//PC机设定的是否管网循环	0-close 1-open
  u8 setMideaWaterOffset;//PC机设定的水位偏差
  u8 setMideaIfHot;//PC机设定的是否辅助加热
  u8 setMideaHandOpenNumbers;//PC机设定手动开启机组数目
  u8 setIfAutoOnOff;//PC机设定是否自动开关机 0-off  1-on
  u8 setOnTime[2];//PC机设定的开机时间
  u8 setOffTime[2];	//PC机设定的关机时间
  u8 setIfQuickOnOff;//设定的实时开机关机


}dev_args_t;





//其余用于存储记录
typedef struct
{
  u8  time[6];	//时间
  u32 inputw;		//进水
  u32 inpute;		//电量
  u32 outputw;	//出水
  u32 loopw;		//回水
  u8  wl;			//水位
}record_t;

//对齐32个字节
typedef union
{
  record_t record;
  u8 align[32];
}record_union_t;


//alias w25x16
#define	STORAGE_CHIP_EREASE()				SPI_FLASH_BulkErase()	
#define	STORAGE_SECTOR_EREASE(secadr)		SPI_FLASH_BulkErase(secadr)




//
void storage_init(void);
void storage_write_record(u8 *buf);


 extern u32 in_water;  //进水量
 extern u32 out_water; //出水量
 extern u32 loop_water;//循环水量
 extern u32 elec;		 //电量

extern dev_args_t dargs;

#endif
