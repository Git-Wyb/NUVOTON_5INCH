
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "gpio.h"
#include "sys.h"
#include "Command_all.h"
#include "BSP_Init.h"
#include "etimer.h"
#include "Aprotocol.h"


#define  AMP_DATA_PIN   			BIT11					  //PI8		AMP_DATA


//#define  AMP_DATA_TOGGLE 	 						HAL_GPIO_TogglePin(GPIOI,AMP_DATA_PIN)

#define   TIM_WAY   1  //1 ��ʾ�ö�ʱ��ʵ�֣�0 ��delayʵ��
#define  STOP_PLAY    0XFFFE//0xFFFE  //FFFEH ֹͣ�������� ֹͣ������������

#define TTS_DELAY 10

TTS_TYPE  tts={0};
volatile uint8_t STOP_FLAG_CYW = 0;

uint8_t wt588h_send_step=0;
volatile uint8_t flag_voice_end = 0;

static  uint8_t  wt588h_start_check=0;
volatile static  uint8_t  wt588h_no_ack=0;
static  uint8_t  wt588h_start_check_cnt=0;

static uint8_t wt588h_send_delay=0;
static  uint8_t  wt588h_voice_state=0;  //// ��ʾ1 �ڲ��� 0 ��ʾ�������
uint32_t get_timego(uint32_t x_data_his);

volatile static uint16_t  send_data_wt588h=0;
volatile static uint8_t    send_data_cnt=0;
volatile static uint8_t    send_data_flag=0;

extern SYSTEMERRORSTATUS_TYPE  systerm_error_status;

static  uint16_t  rev_command_delay=0;

void tim1ms_int_enable(void);
void tim1ms_int_disable(void);
extern UINT32 volatile time1ms_count;


void send_wt588h_init(uint16_t data)  //
{

#if TIM_WAY
	send_data_wt588h=((data>>8)&0x00ff)+((data<<8)&0xff00);
	send_data_cnt=0;
	send_data_flag=0;
	//voice_state=1;
	AMP_DATA_0;
	voice_tim_start(5000);
      //voice_tim_start();

	
	  
#else
	//start
	/*
	���е������ֽڿ��Ʒ�ʽ�����ȷ����ֽڵĵ�λ������ F5 01H������ʱ�ȷ� 1111 0101��F5H���ĵ�λ��
  �� 0000 0001��01H���ĵ�λ�������� 1010 1111 1000 0000��˳���͡�
//  */
//	uint8_t i;
//	uint16_t send_data=((data>>8)&0x00ff)+((data<<8)&0xff00);
//	
//	AMP_DATA_0;
//	delay_us(1666); //Delay_10us(500); //T3 ���� 5MS 500
//	for(i=0;i<16;i++)
//	{
//		//ÿ���ֽڷ���������� 2ms ������ 5ms
//		if(i==8)
//		{
//			AMP_DATA_1;
//			delay_us(666); //Delay_10us(200);
//			AMP_DATA_0;
//			delay_us(1666); //Delay_10us(500); //T3 ����
//		}
//		//
//		AMP_DATA_1; //�������ݴ����ߣ�׼����������
//		if((send_data&0x0001)==0)
//		{ 
//			/*��ʾ�߼���ƽ 0*/
//			delay_us(66); //Delay_10us(20); // ��ʱ 200us
//			AMP_DATA_0;
//			delay_us(200); //Delay_10us(60); // ��ʱ600us
//		}
//		else
//		{ 
//			/*��ʾ�߼���ƽ 1*/
//			delay_us(200); //Delay_10us(60); // ��ʱ 600us
//			AMP_DATA_0;
//			delay_us(66); //Delay_10us(20); // ��ʱ 200us
//		}
//		send_data = send_data>>1;
//		
//	}	
//	AMP_DATA_1;
#endif
     //start_heck_wt588h_exist( );

}


void stop_tts_play(void)
{
	
	//wt588h_send_delay=10;//VOICE_COMMAND_INTERVAL+25;
	//flag_voice_end =0;
	send_wt588h_init(STOP_PLAY); 
	tts.interval_cnt=0;
  tts.play_num=0;
	STOP_FLAG_CYW = 1;
//	AUDIO_AMPLIFIER_SHUT_DOWN;
	rev_command_delay=50;//200; //200ms
	//audio_amplifier_state=0;
}

void ETMR3_IRQHandler(void)
{
	  #if  1
			//	sysprintf("TOUCH TIM  COME");
	// ETIMER_ClearIntFlag(3);
			voice_tim_stop();	
			if(send_data_cnt==17)
			{
         
				  send_data_cnt=0;
				  send_data_flag=0;
				  AMP_DATA_1;
				 // if(wt588h_send_step ==2)
         // {						
				 //   wt588h_send_step = 0;
				//		AUDIO_AMPLIFIER_WORK;
				//	}
				 if(STOP_FLAG_CYW == 1)
				 {
					 STOP_FLAG_CYW = 0;
				 }
					
				if((wt588h_send_step ==4)||(wt588h_send_step ==5)||(wt588h_send_step ==3))
				{
					flag_voice_end = 1;
					Touch_int_enable();
					//sysEnableInterrupt(TMR0_IRQn);
					tim1ms_int_enable();
				}
				 // AUDIO_AMPLIFIER_WORK;//cyw
			}
			else
			{
			       
				//ÿ���ֽڷ���������� 2ms ������ 5ms
				if(send_data_cnt==8)
				{								
					 if(send_data_flag==0)
			                   {
			                     AMP_DATA_1;
					  voice_tim_start(2000);  //delay_us(50000); //Delay_10us(200); 2ms
					  
						send_data_flag=1;
			                   }
					else
					{
					    send_data_flag=0;
					    AMP_DATA_0;
					    voice_tim_start(5000); //delay_us(5000); //Delay_10us(500); //5ms
					    
						  send_data_cnt++;
					}	
				}
				else
				{
					 //�������ݴ����ߣ�׼����������
					if((send_data_wt588h&0x0001)==0)
					{ 
						/*��ʾ�߼���ƽ 0*/
				               if(send_data_flag==0)
				               {
				                     AMP_DATA_1;
						  	voice_tim_start(200);  //delay_us(200); //Delay_10us(20); // ��ʱ 200us
						  	
								send_data_flag=1;
				               }
						else
						{
						    send_data_flag=0;
						    AMP_DATA_0;
						    voice_tim_start(600); //delay_us(600); //Delay_10us(60); // ��ʱ600us
						   
							  send_data_cnt++;
						   send_data_wt588h = send_data_wt588h>>1;
						}	
						
					}
					else
					{ 
						/*��ʾ�߼���ƽ 1*/
						 if(send_data_flag==0)
				                   {
				                     AMP_DATA_1;
							  voice_tim_start(600);  //delay_us(600); //Delay_10us(60); // ��ʱ 600us
							  
								send_data_flag=1;
				                   }
						else
						{
						    send_data_flag=0;
						    AMP_DATA_0;
						    voice_tim_start(200); //delay_us(200); //Delay_10us(20); // ��ʱ 200us
						    
							  send_data_cnt++;
						   send_data_wt588h = send_data_wt588h>>1;
						}					
					}	
				}
				
				
			}
			#endif
			ETIMER_ClearIntFlag(3);
			
}

void start_heck_wt588h_exist(void)
{
	if(wt588h_start_check==0)
		{
			wt588h_start_check=1;
		       wt588h_no_ack=0;
			 wt588h_start_check_cnt=0;
		}
}

void send_tts_command_control(void)  //  run 1 ms ����֮���5ms
{
 static  int8_t bak_voice=0xff;  //����û�䣬ֻ��һ��
	static uint8_t STEP3_FLAG;
	
	if((wt588h_send_delay==0)&&(wt588h_send_step))
	{
	  #ifdef  SYSUARTPRINTF 
		sysprintf("wt588h_send_step = %d\r\n",wt588h_send_step);
		#endif
		switch( wt588h_send_step )
		{

		case 1:
			//if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_2)==GPIO_PIN_RESET)
//			if(GPIO_ReadBit(GPIOI,BIT12)==1)
//		  {
//				//sysprintf("wt588h_send_step = 5\r\n");
//				send_wt588h_init(STOP_PLAY);//send commad 5+2+5+0.8*16=24.8ms
//				wt588h_send_delay=TTS_DELAY;//VOICE_COMMAND_INTERVAL+25;
//				flag_voice_end =0;
//				wt588h_send_step=5;
//				
//			}
////////////////////			else if(bak_voice!=tts.voice)
////////////////////			{
////////////////////				//sysprintf("wt588h_send_step = 4\r\n");
////////////////////				//bak_voice=tts.voice;
////////////////////			send_wt588h_init(tts.voice|0xFFE0);//send commad 5+2+5+0.8*16=24.8ms
////////////////////				wt588h_send_delay=TTS_DELAY;//VOICE_COMMAND_INTERVAL+25;
////////////////////				flag_voice_end =0;
////////////////////				wt588h_send_step=4;
////////////////////			}
//			else
			{
			 // sysprintf("wt588h_send_step = 2\r\n");
				wt588h_send_delay=0;  //û����������ֱ������
				wt588h_send_step=2;
			}
		//	wt588h_send_step=2;
			break;
		case 2:
//			if(GPIO_ReadBit(GPIOI,BIT12)==1)
//			{
//				wt588h_send_step = 1;
//				return;
//			}
			 	if(bak_voice!=tts.voice)
				{
			send_wt588h_init(0XE0|(tts.voice)|0xff00); 	
			wt588h_send_step=4;
		  flag_voice_end=0;
			wt588h_send_delay=TTS_DELAY;
		 // bak_voice=tts.voice;//cyw move 
			start_heck_wt588h_exist();
				}
				else
				{
					sysprintf("flag_voice_end = 1\r\n");
				send_wt588h_init((tts.file));
				flag_voice_end =0;
				wt588h_send_delay=0;  //û����������ֱ������
				wt588h_send_step=3;
				bak_voice=tts.voice;
				wt588h_send_delay=TTS_DELAY;
		 // bak_voice=tts.voice;//cyw move 
			start_heck_wt588h_exist();
				}
			
			break;
		case 3:
			if(flag_voice_end == 1)
			{
				  flag_voice_end =0;
				  STEP3_FLAG = 1;
				 //AUDIO_AMPLIFIER_WORK;
			}
			if(STEP3_FLAG==1)
			{
			//if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_2)==GPIO_PIN_RESET)
			  //{
				    
				    wt588h_send_step = 0;
				   
				    STEP3_FLAG = 0;
				    //if(tts.ack_flag == 1)
						//{
							//code_protocol_ack(tts.fcs_code ,0,NULL,0); 
							
							//tts.ack_flag = 0;
						//}
			 // }
				#ifdef  SYSUARTPRINTF
				     sysprintf("wt588h_send_step = %d,voice send over\r\n",wt588h_send_step);
				    #endif
				
		  }
			break;
		case 4:
			sysprintf("wt588h_send_step = 4\r\n");
			if(flag_voice_end == 1)
			{
				sysprintf("flag_voice_end = 1\r\n");
				send_wt588h_init((tts.file));
				flag_voice_end =0;
				wt588h_send_delay=0;  //û����������ֱ������
				wt588h_send_step=3;
				bak_voice=tts.voice;
				wt588h_send_delay=TTS_DELAY;
		 // bak_voice=tts.voice;//cyw move 
			start_heck_wt588h_exist();
			}
			
			break;
		case 5:
			if(flag_voice_end == 1)
			{
				flag_voice_end =0;
				wt588h_send_delay=0;  //û����������ֱ������
				wt588h_send_step=1;
			}
			break;
		default:
			//wt588h_send_step=0;		
			//wt588h_send_delay=0;
			break;
		}

	}

}

void tts_control(void)
{
	uint8_t flag=0;


	 if(rev_command_delay)//�������ݼ����ʱ
	 	{
			return;
	 	}
	
	if( (tts.play_num>=1)&&(tts.play_num<0x0F)&&(tts.interval_cnt==0)&&(wt588h_send_step==0) )//��������û��  �������ʱ�䵽����flag
	{
		tts.play_num--;	
		
		//tts.interval_cnt=tts.interval;  //�Ƿ����жϲ�����Ÿ�ֵ	
		flag=1;		
			 									  					                          			  		
	}
   if((tts.play_num==0xf)&&(wt588h_send_step==0))  //�������޴εĲ�
	{
		if(tts.interval_cnt==0)//�������ʱ�䵽 - 100ms��
		{		
		  flag=1;				 			 									  					                          			  		
		}
	}

	if(flag)
		{
		//��������
		wt588h_send_step=1;
		wt588h_send_delay=0;
		wt588h_voice_state=1;  	
		tts.interval_cnt=tts.interval;//tts.interval;
    	
			Touch_int_disable();
			tim1ms_int_disable();
			//sysDisableInterrupt(TMR0_IRQn);
   // AMP_POWER_ON;
			
		}
    send_tts_command_control( );	
		
		
//   if((GPIO_ReadBit(GPIOI,BIT12)==1)&&(wt588h_send_step==3))
//	 {
//		 AUDIO_AMPLIFIER_SHUT_DOWN;
//	 }
   // if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_2)==GPIO_PIN_SET)
		//{
			//AUDIO_AMPLIFIER_SHUT_DOWN;
			//TOUCH_POWER_ON();
		//}
	
}

void tts_time(void)  // 1ms run one
{
	//sysprintf("void tts_time(void)\r\n");
	#ifdef  SYSUARTPRINTF 
	//sysprintf("void tts_time(void)\r\n");
	#endif
	if(wt588h_voice_state==0)
	{
  
		if(tts.interval_cnt)//����������кŲſ�ʼ��
		{
		   if(get_timego(tts.interval_time)>tts.interval_cnt)
			    tts.interval_cnt=0;
		   if(tts.interval_cnt==0)
		   	{
//	                // tts.interval_cnt=tts.interval;
//				;
		   	}
		}
	}
      //
	if(flag_voice_end==1)
	{
		
	if(wt588h_send_delay)
	{
		#ifdef  SYSUARTPRINTF 
		sysprintf("wt588h_send_delay=0x%x\r\n",wt588h_send_delay);
		#endif
		wt588h_send_delay--;
	}
  }
	
	if(rev_command_delay)
	 	{
			//sysprintf("rev_command_delay--\r\n");
			rev_command_delay--;
	 	}
	
}

void set_wt588h_voice_state(uint8_t a)
{
	wt588h_no_ack=1;
	
	#ifdef  SYSUARTPRINTF 
		 sysprintf("wt588h_send_step = 0x%x \r\n",wt588h_send_step);
	   sysprintf("flag_voice_end=0x%x\r\n",flag_voice_end);
	sysprintf("wt588h_send_delay=0x%x\r\n",wt588h_send_delay);
		 #endif
	
	if(wt588h_send_step==0)  //��ʾ�������
	{
		wt588h_voice_state=a;
//		AUDIO_AMPLIFIER_SHUT_DOWN;
		//audio_amplifier_state=0;
	}
}	

void check_wt588h_exist(void)   //1// 1 s run one
{

	
		
	if(wt588h_start_check)
		{
			wt588h_start_check_cnt++;
		      if(wt588h_start_check_cnt>15)   //����������жϣ����ҪΪ�����ʱ�����
                   	{
				wt588h_start_check=0;
				
				if(wt588h_no_ack==0)
				{
					
					systerm_error_status.bits.voice_error=1;
					AUDIO_AMPLIFIER_SHUT_DOWN;
				}
                   	}
		}
}


vu32 GPIOICallback(UINT32 status, UINT32 userData)
{
	// sysprintf("i am here,status=%d\r\n",status);
	if(status & BIT12)
	 {
		// Flag_int = 1;
		 //sTime = time1ms_count;
		 if(GPIO_ReadBit(GPIOI,BIT12))
		 {
		 set_wt588h_voice_state(0);
		 tts.interval_time=time1ms_count;
		 AUDIO_AMPLIFIER_SHUT_DOWN;
//		 AMP_POWER_OFF;
		 #ifdef  SYSUARTPRINTF 
		 sysprintf("VOICE END \r\n");
		 #endif
		 }
		 else
		 {
			 AUDIO_AMPLIFIER_WORK;
		 }
		 
	 }
	  //����жϱ�־λ
    GPIO_ClrISRBit(GPIOI,status);
    return 0;
}
