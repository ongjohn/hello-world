#include "time_malloc.h"

vu8  TimerStart0;
vu8  TimerStart1;
vu8  TimerStart2;
vu8  TimerStart3;
vu8  TimerStart4;
vu8  TimerStart5;
vu8  TimerStart6;
vu8  TimerStart7;
vu8  TimerStart8;
vu8  TimerStart9;  
vu8  TimerStart10;
vu8  TimerStart11; 
vu8  TimerStart12;
vu8  TimerStart13;

vu32  Timer0;
vu32  Timer1;
vu32  Timer2;
vu32  Timer3;
vu32  Timer4;
vu32  Timer5;
vu32  Timer6;
vu32  Timer7;
vu32  Timer8;
vu32  Timer9; 
vu32  Timer10; 
vu32  Timer11;
vu32  Timer12;
vu32  Timer13;


void time_init(void)
{
  Timer0=0;
  Timer1=0;
  Timer2=0;
  Timer3=0;
  Timer4=0;
  Timer5=0;
  Timer6=0;
  Timer7=0;
  Timer8=0;
  Timer9=0;	 
  Timer10=0; 
  Timer11=0;
  Timer12=0;
  Timer13=0;
  TimerStart0=0;
  TimerStart1=0;
  TimerStart2=0;
  TimerStart3=0;
  TimerStart4=0;
  TimerStart5=0;
  TimerStart6=0;
  TimerStart7=0;
  TimerStart8=0;
  TimerStart9=0; 
  TimerStart10=0;  
  TimerStart11=0;
  TimerStart12=0;
  TimerStart13=0;
}
int TimerStart(u8 chIndex, unsigned long ulMilliSecond)
{
   switch(chIndex)
  {
    case 0: //............................  0
      TimerStart0=1;
      Timer0=ulMilliSecond;
      break;
    case 1: //............................. 1
      TimerStart1=1;
      Timer1=ulMilliSecond;						 				   
      break;
    case 2: //..............................2
      TimerStart2=1;
      Timer2=ulMilliSecond;
      break;
    case 3: //..............................3
      TimerStart3=1;
      Timer3=ulMilliSecond;							   
      break;
    case 4: //..............................4
      TimerStart4=1;
      Timer4=ulMilliSecond;
      break;
    case 5://...............................5
      TimerStart5=1;
      Timer5=ulMilliSecond;             
      break;
    case 6://...............................6
      TimerStart6=1;
      Timer6=ulMilliSecond;          
      break;
    case 7://...............................7
      TimerStart7=1;
      Timer7=ulMilliSecond;           
      break;
    case 8://...............................8
      TimerStart8=1;
      Timer8=ulMilliSecond;          
      break;
    case 9://...............................9
      TimerStart9=1;
      Timer9=ulMilliSecond;         
      break;
	
	case 10://...............................9
      TimerStart10=1;
      Timer10=ulMilliSecond;         
      break;

	case 11://...............................9
      TimerStart11=1;
      Timer11=ulMilliSecond;         
      break;

	case 12://...............................9
      TimerStart12=1;
      Timer12=ulMilliSecond;         
      break;

	case 13://...............................9
      TimerStart13=1;
      Timer13=ulMilliSecond;         
      break;

    default :   break;	 						
  }

  return 0;
}

int TimerStop(u8 chIndex)
{
  switch(chIndex)
  {
    case 0: //............................  0
      TimerStart0=0;
      Timer0=0;
      break;
    case 1: //............................. 1
      TimerStart1=0;
      Timer1=0;						 				   
      break;
    case 2: //..............................2
      TimerStart2=0;
      Timer2=0;
      break;
    case 3: //..............................3
      TimerStart3=0;
      Timer3=0;							   
      break;
    case 4: //..............................4
      TimerStart4=0;
      Timer4=0;
      break;
    case 5://...............................5
      TimerStart5=0;
      Timer5=0;             
      break;
    case 6://...............................6
      TimerStart6=0;
      Timer6=0;          
      break;
    case 7://...............................7
      TimerStart7=0;
      Timer7=0;           
      break;
    case 8://...............................8
      TimerStart8=0;
      Timer8=0;          
      break;
    case 9://...............................9
      TimerStart9=0;
      Timer9=0;         
      break;
    case 10://...............................9
      TimerStart10=0;
      Timer10=0;         
      break;

	case 11://...............................9
      TimerStart11=0;
      Timer11=0;         
      break;

	case 12://...............................9
      TimerStart12=0;
      Timer12=0;         
      break;

	case 13://...............................9
      TimerStart13=0;
      Timer13=0;         
      break;


    default :   break;	 						
  }

  return 0;
}
