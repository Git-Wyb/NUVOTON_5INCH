#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "sys.h"
#include "Aprotocol.h"
#include "Command_TR.h"


extern uint16_t  RX_COUNT_IN,RX_COUNT_OUT;
extern uint8_t  UART2_RX_BUFF[RX_BUFF_MAX];

typedef enum biaoyanStatus
{
	Q_STEP,
	G_STEP,
	WAIT,
	DISPLAY_1,
	DISPLAY_2,
	DISPLAY_3,
	DISPLAY_4,
	DISPLAY_5,
	DISPLAY_6,
	DISPLAY_7,
	DISPLAY_8,
	DISPLAY_9,
	DISPLAY_10,
	IDEL,
}biaoyanEnum;

biaoyanEnum Biaoyan_Step = Q_STEP,Biaoyan_Step_His;
extern uint8_t  Flag_int;

void bianyan_scan(void)
{
	static uint8_t buf[64];//cyw
	if(RX_COUNT_IN==RX_COUNT_OUT)
	{
		switch(Biaoyan_Step)
		{
			case Q_STEP:
				 sprintf((char *)UART2_RX_BUFF,"%S","@34Q00630,0631,0632,0633,0634,0635,0636,0637,0638,0639*");
			 RX_COUNT_OUT = 0;   
			 RX_COUNT_IN =  55;
			  Biaoyan_Step = G_STEP;
			  Biaoyan_Step_His = G_STEP;
				 break;
			case G_STEP:
				 sprintf((char *)UART2_RX_BUFF,"%S","@05G2FF*");
			 RX_COUNT_OUT = 0;   
			 RX_COUNT_IN =  8;
			  Biaoyan_Step = WAIT;
			  Biaoyan_Step_His = WAIT;
				 break;
			case WAIT:
				
				if(Flag_int==1)
				{
					FT5x06_RD_Reg(0, buf, 42);
				 Biaoyan_Step = Biaoyan_Step_His+1;
			   if(Biaoyan_Step>=IDEL)
				 {
					 Biaoyan_Step = DISPLAY_1;
				 }
				 Biaoyan_Step_His = Biaoyan_Step;
				 Flag_int = 0;
			   }
				
				  break;
		 case DISPLAY_1:
				 sprintf((char *)UART2_RX_BUFF,"%S","@3ED000000010630,16000010630,32000010630,48000010630,64000010630*");
			 RX_COUNT_OUT = 0;   
			 RX_COUNT_IN =  65;
			  Biaoyan_Step = WAIT;
			  //Biaoyan_Step_His = WAIT;
				 break;
		 case DISPLAY_2:
				 sprintf((char *)UART2_RX_BUFF,"%S","@3ED000000010631,16000010631,32000010631,48000010631,64000010631*");
			 RX_COUNT_OUT = 0;   
			 RX_COUNT_IN =  65;
			  Biaoyan_Step = WAIT;
			 // Biaoyan_Step_His = WAIT;
				 break;
		 case DISPLAY_3:
				 sprintf((char *)UART2_RX_BUFF,"%S","@3ED000000010632,16000010632,32000010632,48000010632,64000010632*");
			 RX_COUNT_OUT = 0;   
			 RX_COUNT_IN =  65;
			  Biaoyan_Step = WAIT;
			//  Biaoyan_Step_His = WAIT;
				 break;
		 case DISPLAY_4:
				 sprintf((char *)UART2_RX_BUFF,"%S","@3ED000000010633,16000010633,32000010633,48000010633,64000010633*");
			 RX_COUNT_OUT = 0;   
			 RX_COUNT_IN =  65;
			  Biaoyan_Step = WAIT;
			//  Biaoyan_Step_His = WAIT;
				 break;
		 case DISPLAY_5:
				 sprintf((char *)UART2_RX_BUFF,"%S","@3ED000000010634,16000010634,32000010634,48000010634,64000010634*");
			 RX_COUNT_OUT = 0;   
			 RX_COUNT_IN =  65;
			  Biaoyan_Step = WAIT;
			  //Biaoyan_Step_His = WAIT;
				 break;
		 case DISPLAY_6:
				 sprintf((char *)UART2_RX_BUFF,"%S","@3ED000000010635,16000010635,32000010635,48000010635,64000010635*");
			 RX_COUNT_OUT = 0;   
			 RX_COUNT_IN =  65;
			  Biaoyan_Step = WAIT;
			 // Biaoyan_Step_His = WAIT;
				 break;
		 case DISPLAY_7:
				 sprintf((char *)UART2_RX_BUFF,"%S","@3ED000000010636,16000010636,32000010636,48000010636,64000010636*");
			 RX_COUNT_OUT = 0;   
			 RX_COUNT_IN =  65;
			  Biaoyan_Step = WAIT;
			 // Biaoyan_Step_His = WAIT;
				 break;
		 case DISPLAY_8:
				 sprintf((char *)UART2_RX_BUFF,"%S","@3ED000000010637,16000010637,32000010637,48000010637,64000010637*");
			 RX_COUNT_OUT = 0;   
			 RX_COUNT_IN =  65;
			  Biaoyan_Step = WAIT;
			//  Biaoyan_Step_His = WAIT;
				 break;
		 case DISPLAY_9:
				 sprintf((char *)UART2_RX_BUFF,"%S","@3ED000000010638,16000010638,32000010638,48000010638,64000010638*");
			 RX_COUNT_OUT = 0;   
			 RX_COUNT_IN =  65;
			  Biaoyan_Step = WAIT;
			//  Biaoyan_Step_His = WAIT;
				 break;
		 case DISPLAY_10:
				 sprintf((char *)UART2_RX_BUFF,"%S","@3ED000000010639,16000010639,32000010639,48000010639,64000010639*");
			 RX_COUNT_OUT = 0;   
			 RX_COUNT_IN =  65;
			  Biaoyan_Step = WAIT;
			//  Biaoyan_Step_His = WAIT;
				 break;
		 default:
			   break;
			
		}
	}
}
 	
	



















