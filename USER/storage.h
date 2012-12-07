#ifndef STORAGE_H
#define	STORAGE_H

#include "stm32f10x.h"
#include "spi_flash.h"

#define MIDEA	
#define MAX_MIDEA_MACHINE	10
#define	RECORD_FORMAT		32	 


__packed typedef struct
{

  u8 devno[6];//�豸��ַ
  u8 pwd[6]; //ͨ������
  u8 rs485_adr1;//rs2��ַ�����ĶԽӽӿ�
  u8 g_hartheattime;//������
  u8 ip[4];
  u16 port;
  u16 upload_min;//��ʱ�ϱ�ʱ��

  u32 rs485InWaterAddr;//��ˮ���ַ
  u32 rs485OutWaterAddr;//��ˮ��rs485��ַ
  u32 rs485LoopWaterAddr;//��ˮ��rs485��ַ
  u32 rs485ElecAddr;//���rs485��ַ
  
   u8 setMideaDegree0;//PC���趨��ˮ���ˮ�¶�
  u8 setMideaLoopDegree;//PC���趨��ѭ��ˮ�¶�
  u8 setMideaWaterLevel;//PC���趨��ˮλ����
  u8 setMideaIfLoop;//PC���趨���Ƿ����ѭ��	0-close 1-open
  u8 setMideaWaterOffset;//PC���趨��ˮλƫ��
  u8 setMideaIfHot;//PC���趨���Ƿ�������
  u8 setMideaHandOpenNumbers;//PC���趨�ֶ�����������Ŀ
  u8 setIfAutoOnOff;//PC���趨�Ƿ��Զ����ػ� 0-off  1-on
  u8 setOnTime[2];//PC���趨�Ŀ���ʱ��
  u8 setOffTime[2];	//PC���趨�Ĺػ�ʱ��
  u8 setIfQuickOnOff;//�趨��ʵʱ�����ػ�


}dev_args_t;





//�������ڴ洢��¼
typedef struct
{
  u8  time[6];	//ʱ��
  u32 inputw;		//��ˮ
  u32 inpute;		//����
  u32 outputw;	//��ˮ
  u32 loopw;		//��ˮ
  u8  wl;			//ˮλ
}record_t;

//����32���ֽ�
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


 extern u32 in_water;  //��ˮ��
 extern u32 out_water; //��ˮ��
 extern u32 loop_water;//ѭ��ˮ��
 extern u32 elec;		 //����

extern dev_args_t dargs;

#endif
