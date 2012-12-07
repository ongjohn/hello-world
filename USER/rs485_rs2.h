#ifndef __RS485_RS2_H
#define __RS485_RS2_H 			   
#include <stm32f10x.h>
 
//IO口定义 
#define RS485_RS2_EN()	 	GPIO_SetBits(GPIOB,GPIO_Pin_12)
#define	RS485_RS2_DIS()		GPIO_ResetBits(GPIOB,GPIO_Pin_12)


#define		RS485_START		0
#define		RS485_CMD		1
#define		RS485_SRC		2
#define		RS485_DEST		3
#define		RS485_DAT		4
 
//水箱温度
#define		OFFSET_T5		18 
	
//环境温度
#define		OFFSET_T4		19
#define		OFFSET_WATER	23
 

#define		ERR0_DET	(0x0080)
#define		ERR1_DET	(0x0040)
#define		ERR2_DET	(0x0020)
#define		ERR3_DET	(0x0010)
#define		ERR4_DET	(0x0008)
#define		ERR5_DET	(0x0004)
#define		ERR6_DET	(0x0002)
#define		ERR7_DET	(0x0001)
#define		ERR8_DET	(0x0080)

#define		PRO0_DET	(0x0080)
#define		PRO1_DET	(0x0040)
#define		PRO2_DET	(0x0020)
#define		PRO3_DET	(0x0010)
#define		PRO4_DET	(0x0008)
#define		PRO5_DET	(0x0004)
#define		PRO6_DET	(0x0002)
#define		PRO7_DET	(0x0001)
#define		PRO8_DET	(0x0080)
#define		PRO9_DET	(0x0040)


#define		ERR0_MASK	(0x00000001)
#define		ERR1_MASK	(0x00000002)
#define		ERR2_MASK	(0x00000004)
#define		ERR3_MASK	(0x00000008)
#define		ERR4_MASK	(0x00000010)
#define		ERR5_MASK	(0x00000020) 
#define		ERR6_MASK	(0x00000040)
#define		ERR7_MASK	(0x00000080)
#define		ERR8_MASK	(0x00000100)

#define		PRO0_MASK	(0x00000200)
#define		PRO1_MASK	(0x00000400)
#define		PRO2_MASK	(0x00000800)
#define		PRO3_MASK	(0x00001000)
#define		PRO4_MASK	(0x00002000)
#define		PRO5_MASK	(0x00004000)
#define		PRO6_MASK	(0x00008000)
#define		PRO7_MASK	(0x00010000)
#define		PRO8_MASK	(0x00020000)
#define		PRO9_MASK	(0x00040000) 

#define	ERR_0			0xE0 //总出水温度传感器故障
#define	ERR_1			0xE1 //板出水温度传感器故障
#define	ERR_2			0xE2 //冷凝器1温度传感器故障
#define	ERR_3			0xE3 //冷凝器2温度传感器故障
#define	ERR_4			0xE4 //室外环境温度传感器故障
#define	ERR_5			0xE5 //水流检测故障
#define	ERR_6			0xE6 //通讯故障
#define	ERR_7			0xE7 //电源相序故障
#define	ERR_8			0xE8 //循环水流故障


#define	PRO_0			0xF0//系统1高压保护
#define	PRO_1			0xF1//系统2高压保护
#define	PRO_2			0xF2//系统1低压保护
#define	PRO_3			0xF3//系统2低压保护
#define	PRO_4			0xF4//系统1压缩机温度保护
#define	PRO_5			0xF5//系统2压缩机温度保护
#define	PRO_6			0xF6//防冻结保护
#define	PRO_7			0xF7//冷凝器高温保护
#define	PRO_8			0xF8//系统1电流保护
#define	PRO_9			0xF9//系统2电流保护

#define	RS485_LINE_ERR	0xD7;//485断线


//美的机组基本参数
__packed typedef struct
{
	u8 	board_outwater_t; 	//机组水泵输出温度*
	u8 	err_code[2];		//错误代码
	u8  protect_code[2];	//保护代码
	s8	cool_t[2];			//冷凝水温度*
	u8	compress_c[2];		//压缩机电流*
	u8	output_state;	    //端口输出状态
	u8	outdoor_t;			//环境温度
	u8  water_t;			//水箱温度
	u8	online_m;			//在线机组数0-16
	u8	run_m;	   			//运行机组数0-16
	u8  water_level;		//水位高度，美的部分机器自带高度检测
	u8	outwater_t;			//pc机设定的出水温度,要和美的机组输出的一致
	u8  water_offset;		//水位偏差
}MEIDA_MSG_T;

extern void rs485_rs2_init(void);
extern void rs485_rs2_send (void);
extern void RS485rs2Proc(void);



//声明变量
extern u8 oc_status;
extern u8 rs485_ack_flag;
extern u8 rd_idx;
extern u8 rs485_rs2_rxbuf[32];
extern u8 rs485_rs2_txbuf[32]; 
extern MEIDA_MSG_T getMediaMSG[15];

extern u8 getMediaOutWaterTemper;//从美的机组主机读取到的出水水箱温度,经过计算转化的温度
extern u8 getMediaEnvTemper;//从美的机组主机读取到的环境温度,经过计算转化的温度
extern u8 getMediaWaterHigh;//从美的机组主机得到的水位高度

 		
#endif
