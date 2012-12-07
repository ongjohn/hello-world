#ifndef	TIME_MALLOC_H
#define	TIME_MALLOC_H

#include "stm32f10x.h" 

//variable proto 
extern vu8  TimerStart0;
extern vu8  TimerStart1;
extern vu8  TimerStart2;
extern vu8  TimerStart3;
extern vu8  TimerStart4;
extern vu8  TimerStart5;
extern vu8  TimerStart6;
extern vu8  TimerStart7;
extern vu8  TimerStart8;
extern vu8  TimerStart9;  
extern vu8  TimerStart10; 
extern vu8  TimerStart11;
extern vu8  TimerStart12;
extern vu8  TimerStart13;
extern vu32  Timer0;
extern vu32  Timer1;
extern vu32  Timer2;
extern vu32  Timer3;
extern vu32  Timer4;
extern vu32  Timer5;
extern vu32  Timer6;
extern vu32  Timer7;
extern vu32  Timer8;
extern vu32  Timer9;   
extern vu32  Timer10; 
extern vu32  Timer11; 
extern vu32  Timer12; 
extern vu32  Timer13;

//function proto
int TimerStart(u8 chIndex, unsigned long ulMilliSecond);
int TimerStop(u8 chIndex);
void time_init(void);
#endif


