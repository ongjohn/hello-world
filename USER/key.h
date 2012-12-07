#ifndef	KEY_H
#define	KEY_H

#include "stm32f10x.h"

 
#define BUTTON_FILTER_TIME 	50

 
typedef struct
{
 	uint8_t (*IsKeyDownFunc)(void);  
	uint8_t Count;			
	uint8_t FilterTime;		
	uint8_t State;			
	uint8_t KeyCodeDown;
}BUTTON_T;

 
typedef enum
{
	KEY_NONE = 0,	 

 	KEY_SET, 	//key5
	KEY_UP,	 	//key4
 	KEY_DN,	 	//key3	 
	KEY_ENTER,	//key2 
 	KEY_RETURN	//key1
}KEY_ENUM;

 
#define KEY_FIFO_SIZE	20
typedef struct
{
	uint8_t Buf[KEY_FIFO_SIZE];	 
	uint8_t Read; 
	uint8_t Write; 
}KEY_FIFO_T;



void KEY_Scan(void);
void KEY_Initial(void);
void KeyProc(void);


#endif
