#ifndef __RS485_RS2_H
#define __RS485_RS2_H 			   
#include <stm32f10x.h>
 
//IO�ڶ��� 
#define RS485_RS2_EN()	 	GPIO_SetBits(GPIOB,GPIO_Pin_12)
#define	RS485_RS2_DIS()		GPIO_ResetBits(GPIOB,GPIO_Pin_12)


#define		RS485_START		0
#define		RS485_CMD		1
#define		RS485_SRC		2
#define		RS485_DEST		3
#define		RS485_DAT		4
 
//ˮ���¶�
#define		OFFSET_T5		18 
	
//�����¶�
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

#define	ERR_0			0xE0 //�ܳ�ˮ�¶ȴ���������
#define	ERR_1			0xE1 //���ˮ�¶ȴ���������
#define	ERR_2			0xE2 //������1�¶ȴ���������
#define	ERR_3			0xE3 //������2�¶ȴ���������
#define	ERR_4			0xE4 //���⻷���¶ȴ���������
#define	ERR_5			0xE5 //ˮ��������
#define	ERR_6			0xE6 //ͨѶ����
#define	ERR_7			0xE7 //��Դ�������
#define	ERR_8			0xE8 //ѭ��ˮ������


#define	PRO_0			0xF0//ϵͳ1��ѹ����
#define	PRO_1			0xF1//ϵͳ2��ѹ����
#define	PRO_2			0xF2//ϵͳ1��ѹ����
#define	PRO_3			0xF3//ϵͳ2��ѹ����
#define	PRO_4			0xF4//ϵͳ1ѹ�����¶ȱ���
#define	PRO_5			0xF5//ϵͳ2ѹ�����¶ȱ���
#define	PRO_6			0xF6//�����ᱣ��
#define	PRO_7			0xF7//���������±���
#define	PRO_8			0xF8//ϵͳ1��������
#define	PRO_9			0xF9//ϵͳ2��������

#define	RS485_LINE_ERR	0xD7;//485����


//���Ļ����������
__packed typedef struct
{
	u8 	board_outwater_t; 	//����ˮ������¶�*
	u8 	err_code[2];		//�������
	u8  protect_code[2];	//��������
	s8	cool_t[2];			//����ˮ�¶�*
	u8	compress_c[2];		//ѹ��������*
	u8	output_state;	    //�˿����״̬
	u8	outdoor_t;			//�����¶�
	u8  water_t;			//ˮ���¶�
	u8	online_m;			//���߻�����0-16
	u8	run_m;	   			//���л�����0-16
	u8  water_level;		//ˮλ�߶ȣ����Ĳ��ֻ����Դ��߶ȼ��
	u8	outwater_t;			//pc���趨�ĳ�ˮ�¶�,Ҫ�����Ļ��������һ��
	u8  water_offset;		//ˮλƫ��
}MEIDA_MSG_T;

extern void rs485_rs2_init(void);
extern void rs485_rs2_send (void);
extern void RS485rs2Proc(void);



//��������
extern u8 oc_status;
extern u8 rs485_ack_flag;
extern u8 rd_idx;
extern u8 rs485_rs2_rxbuf[32];
extern u8 rs485_rs2_txbuf[32]; 
extern MEIDA_MSG_T getMediaMSG[15];

extern u8 getMediaOutWaterTemper;//�����Ļ���������ȡ���ĳ�ˮˮ���¶�,��������ת�����¶�
extern u8 getMediaEnvTemper;//�����Ļ���������ȡ���Ļ����¶�,��������ת�����¶�
extern u8 getMediaWaterHigh;//�����Ļ��������õ���ˮλ�߶�

 		
#endif
