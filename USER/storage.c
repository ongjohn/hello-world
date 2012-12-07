#include "storage.h"
#include "spi_flash.h"
#include "usart1.h"
#include "gprs.h"


u32 storage_pos;//��¼�洢����

dev_args_t dargs;
record_union_t ru;//��¼�ṹ��


  u32 in_water;  //��ˮ��
  u32 out_water; //��ˮ��
  u32 loop_water;//ѭ��ˮ��
  u32 elec;		 //����

/*
   void SPI_FLASH_BufferWrite_InPage(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
   {
   u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

   Addr = WriteAddr % SPI_FLASH_PageSize;//�õ�ҳ��ƫ�Ƶ�ַ
   count = SPI_FLASH_PageSize - Addr;    //ҳ��ʣ����ֽ���   
   NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;//�õ���ҳ
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

  secpos=WriteAddr/4096;//������ַ 0~511 for w25x16
  secoff=WriteAddr%4096;//�������ڵ�ƫ��
  secremain=4096-secoff;//����ʣ��ռ��С   

  if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
  while(1) 
  {	
    SPI_Flash_Read(SPI_FLASH_BUF,secpos*4096,4096);//������������������
    for(i=0;i<secremain;i++)//У������
    {
      if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
    }
    if(i<secremain)//��Ҫ����
    {
      SPI_Flash_Erase_Sector(secpos);//�����������
      for(i=0;i<secremain;i++)	   //����
      {
	SPI_FLASH_BUF[i+secoff]=pBuffer[i];	  
      }
      SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);//д����������  

    }else SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
    if(NumByteToWrite==secremain)break;//д�������
    else//д��δ����
    {
      secpos++;//������ַ��1
      secoff=0;//ƫ��λ��Ϊ0 	 

      pBuffer+=secremain;  //ָ��ƫ��
      WriteAddr+=secremain;//д��ַƫ��	   
      NumByteToWrite-=secremain;				//�ֽ����ݼ�
      if(NumByteToWrite>4096)secremain=4096;	//��һ����������д����
      else secremain=NumByteToWrite;			//��һ����������д����
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

  //storage_pos��¼����
  record_nums=storage_pos*RECORD_FORMAT;

  //һ�ʼ�¼32���ֽ�,���ҿ���λ�ã����ڴ洢��¼
  for(i=record_nums;i<2*1024*1024;i+=RECORD_FORMAT) 
  {
    SPI_FLASH_BufferRead(&rdata,4096+i,1);	//��4096��ʼ���
    if(rdata==0xFF)
    {
      storage_pos++;	//��¼����+1
      USART_printf(USART1, "Get Record position:%d\r\n",storage_pos);
      break;
    }
    else
    {
      USART_printf(USART1, "Get Record data:%d\r\n",rdata);
    }
  }

  //дһ�ʼ�¼	
  SPI_FLASH_BufferWrite(buf,4096+record_nums,RECORD_FORMAT);

  //����¼�����������洢ָ��,(2M-4K)/32=65408�ʼ�¼
  if(storage_pos>=65408)
  {
    storage_pos=0;	
  }

  //��ʱ����
  erase_nums=storage_pos*RECORD_FORMAT;

  //�����߽磬����һ����������, ��ʱ����
  if((erase_nums%4096)==0)
  {				 
    USART_printf(USART1, "erase a sector!\r\n");
    SPI_FLASH_SectorErase(record_nums);	
  }

}
