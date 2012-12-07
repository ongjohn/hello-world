#include "storage.h"
#include "spi_flash.h"
#include "usart1.h"
#include "gprs.h"


u32 storage_pos;//记录存储笔数

dev_args_t dargs;
record_union_t ru;//记录结构体


  u32 in_water;  //进水量
  u32 out_water; //出水量
  u32 loop_water;//循环水量
  u32 elec;		 //电量

/*
   void SPI_FLASH_BufferWrite_InPage(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
   {
   u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

   Addr = WriteAddr % SPI_FLASH_PageSize;//得到页内偏移地址
   count = SPI_FLASH_PageSize - Addr;    //页还剩余的字节数   
   NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;//得到哪页
   NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;//

   if (Addr == 0)  
   {
   if (NumOfPage == 0)  
   {
   SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
   }
   else  
   {
   while (NumOfPage--)
   {
   SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
   WriteAddr +=  SPI_FLASH_PageSize;
   pBuffer += SPI_FLASH_PageSize;
   }

   SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
   }
   }
   else  
   {
   if (NumOfPage == 0)  
   {
   if (NumOfSingle > count)  
   {
   temp = NumOfSingle - count;

   SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
   WriteAddr +=  count;
   pBuffer += count;

   SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
   }
   else
   {
   SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
   }
   }
   else 
   {
   NumByteToWrite -= count;
   NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
   NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

   SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
   WriteAddr +=  count;
   pBuffer += count;

   while (NumOfPage--)
   {
   SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
   WriteAddr +=  SPI_FLASH_PageSize;
   pBuffer += SPI_FLASH_PageSize;
   }

   if (NumOfSingle != 0)
   {
   SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
   }
   }
   }
   }


u8 SPI_FLASH_BUF[4096];
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
  u32 secpos;
  u16 secoff;
  u16 secremain;	   
  u16 i;    

  secpos=WriteAddr/4096;//扇区地址 0~511 for w25x16
  secoff=WriteAddr%4096;//在扇区内的偏移
  secremain=4096-secoff;//扇区剩余空间大小   

  if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
  while(1) 
  {	
    SPI_Flash_Read(SPI_FLASH_BUF,secpos*4096,4096);//读出整个扇区的内容
    for(i=0;i<secremain;i++)//校验数据
    {
      if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;//需要擦除  	  
    }
    if(i<secremain)//需要擦除
    {
      SPI_Flash_Erase_Sector(secpos);//擦除这个扇区
      for(i=0;i<secremain;i++)	   //复制
      {
	SPI_FLASH_BUF[i+secoff]=pBuffer[i];	  
      }
      SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);//写入整个扇区  

    }else SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
    if(NumByteToWrite==secremain)break;//写入结束了
    else//写入未结束
    {
      secpos++;//扇区地址增1
      secoff=0;//偏移位置为0 	 

      pBuffer+=secremain;  //指针偏移
      WriteAddr+=secremain;//写地址偏移	   
      NumByteToWrite-=secremain;				//字节数递减
      if(NumByteToWrite>4096)secremain=4096;	//下一个扇区还是写不完
      else secremain=NumByteToWrite;			//下一个扇区可以写完了
    }	 
  };	 	 
}
*/
void storage_init(void)
{
  storage_pos=0;
  SPI_FLASH_BufferRead(dargs.devno,0,sizeof(dargs));

  sprintf((char *)&g_setting.szIPAddress,"%03d.%03d.%03d.%03d",(u16)dargs.ip[0],(u16)dargs.ip[1],(u16)dargs.ip[2],(u16)dargs.ip[3]);
  sprintf((char *)&g_setting.szPortNumber,"%05d",(u16)dargs.port);

  in_water=0;
  out_water=0;
  loop_water=0;
  elec=0;

}


void storage_write_record(u8 *buf)
{
  u32 i;
  u8  rdata;
  u32 record_nums;
  u32 erase_nums;

  //storage_pos记录笔数
  record_nums=storage_pos*RECORD_FORMAT;

  //一笔记录32个字节,查找空闲位置，用于存储记录
  for(i=record_nums;i<2*1024*1024;i+=RECORD_FORMAT) 
  {
    SPI_FLASH_BufferRead(&rdata,4096+i,1);	//从4096开始存放
    if(rdata==0xFF)
    {
      storage_pos++;	//记录笔数+1
      USART_printf(USART1, "Get Record position:%d\r\n",storage_pos);
      break;
    }
    else
    {
      USART_printf(USART1, "Get Record data:%d\r\n",rdata);
    }
  }

  //写一笔记录	
  SPI_FLASH_BufferWrite(buf,4096+record_nums,RECORD_FORMAT);

  //若记录数满，调整存储指针,(2M-4K)/32=65408笔记录
  if(storage_pos>=65408)
  {
    storage_pos=0;	
  }

  //延时擦除
  erase_nums=storage_pos*RECORD_FORMAT;

  //扇区边界，擦除一个扇区内容, 延时擦除
  if((erase_nums%4096)==0)
  {				 
    USART_printf(USART1, "erase a sector!\r\n");
    SPI_FLASH_SectorErase(record_nums);	
  }

}
