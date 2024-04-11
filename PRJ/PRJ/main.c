/*-----------------------NUC972����������----------------------------

����˵����
������ʵ��ͨ��GPIO�����ⲿLEDʵ��,����LED������˸��Ч��

Ӳ������
PB4 --> �û�LED0
PB5 --> �û�LED1

���뻷����Keil 5.24a
��ַ��qlqc.taobao.com

----------------���ƿƼ����ൺǧ�ڵ��ӿƼ����޹�˾��Ȩ����-----------*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "sys.h"
#include "gpio.h"
#include "lcd.h"
#include "display.h"
#include "2d.h"

#include "Aprotocol.h"
#include "BSP_Init.h"
#include "usbh_lib.h"
#include "Command_all.h"
#include "DIP_SW.h"
#include "Atime.h"
#include "poweroff.h"
#include "poweron.h"
#include "hard_function_test.h"
#include "wwdt.h"
#include "ff.h"
#include "W25Q128.h"

#ifdef BIAOYANJI
void bianyan_scan(void);
#endif

//__align(32) uint32_t u32CursorBuf[512];

void hard_function_test(void);

extern UINT32 volatile _sys_uTimer0Count ;
UINT32 volatile uTimer0Counthis =555;
extern PARA_TYPE para;

////UINT8 ColorPatternData[2][8*8*4] = { // 8*8*4
////    {
////#include "pat8x8-0.dat"
////    },
////    {
////#include "pat8x8-1.dat"
////    },
////};

////UINT8 picRGB565160x120[] = {
////#include "160x120RGB565.dat"
////};


extern uint32_t logodata_sdrambuffer_addr_arry[16];
extern const unsigned char image_bmp1[];
extern const unsigned char image_bmp4[];
extern const unsigned char image_bmp3[];
extern const unsigned char image_bmp2[];
extern const unsigned char image_bmp6[];
extern uint8_t pwr_on_cnt;
extern test_E test_e;
extern FATFS usb_fatfs;
extern LAYER_SDRAM_STR display_layer_sdram;
extern uint32_t Flag_Tim_1ms;
//��ʱ����
void delay_ms(uint32_t i)
{
uint32_t j;
	for(;i>0;i--)
		for(j=50000;j>0;j--);
}
void delay_us(int usec)
{
    volatile int  loop = 300 * usec;
    while (loop > 0) loop--;
}

uint32_t get_ticks(void)
{
    return sysGetTicks(TIMER0);
}


FRESULT TP_res;
extern uint32_t time1ms_count;
uint32_t Tp_data = 0;
#ifdef SYSUARTPRINTF_p

extern  uint8_t *bmpBuf_kkk,*bmpBuf_kkk_bak;
#endif

/******************************������************************/
int main(void)
{

	//	uint8_t *u8FrameBufPtr, *u8OSDFrameBufPtr,i; 
	//int cnt;	
	//void *_ColorSrcBufferPtr2;

	
	/**************************ϵͳ��ʼ����ʼ**********************/
    *((volatile unsigned int *)REG_AIC_MDCR)=0xFFFFFFFF;
    *((volatile unsigned int *)REG_AIC_MDCRH)=0xFFFFFFFF;
    sysDisableCache();
    sysFlushCache(I_D_CACHE);
    sysEnableCache(CACHE_WRITE_BACK);
	
	  REG_OPERATE(REG_CLK_HCLKEN,(1<<18)|(1<<19)|(1<<20)|(1<<21)|(1<<25)|(1<<28),clear);
	  outpw(REG_CLK_PCLKEN0,0);
   outpw(REG_CLK_PCLKEN1,0);
	  REG_OPERATE(REG_SYS_AHBIPRST,0XFFFFFFFF,set);
	   REG_OPERATE(REG_SYS_APBIPRST0,0XFFFFFFFF,set);
	 REG_OPERATE(REG_SYS_APBIPRST1,0XFFFFFFFF,set);
	//memset((void*)0x80000000,0,0x8000000);
	  
/**************************ϵͳ��ʼ������**********************/
	
	  
	
	  //ϵͳ���Դ��ڳ�ʼ�� ����0 115200
    sysInitializeUART();
	#ifdef  SYSUARTPRINTF 
    sysprintf("REG_CLK_PCLKEN0=%x\r\n",REG_CLK_PCLKEN0);
	  sysprintf("\r\n--------------GPIO_LEDʵ��------------------\r\n");
	  sysprintf("------------�ൺǧ�ڵ��ӿƼ����޹�˾---------\r\n");
	  sysprintf("--------------------------------------------\r\n");	
		sysprintf("SYS_UPLL=%d,SYS_APLL=%d,SYS_SYSTEM=%d,SYS_HCLK1=%d,SYS_HCLK234=%d,SYS_PCLK=%d,SYS_CPU=%d/n"
	             ,sysGetClock(SYS_UPLL),sysGetClock(SYS_APLL),sysGetClock(SYS_SYSTEM),sysGetClock(SYS_HCLK1),sysGetClock(SYS_HCLK234),
	             sysGetClock(SYS_PCLK),sysGetClock(SYS_CPU));
  #endif  
		
		
		
		init_gpio();
		UART_TOIO();
		POWER_5V_SETTING_initstart();
		
	#ifdef POWER_INT_MODE
	 power_checkon();
	 #endif
		
		
		//nandflash_init();
	
	
//	memset(u8FrameBufPtr,0x55,800*480*2);
//	delay_ms(5000);
 
		
	 //ˢȫ��Ϊ��ɫ
	//vpostVAStartTrigger();
   Timer_1ms_Init();
//Backlinght_Control_Init_HARDV4(128);
		display_init();
	 

//////////////	 AUDIO_AMPLIFIER_SHUT_DOWN;
//////////////	AMP_POWER_OFF;
//////////////	AMP_DATA_0;
//////////////		while(1);
		//AUDIO_AMPLIFIER_SHUT_DOWN;
		
		 FT5x06_Init();
		Touch_tim_init();
		
	//vpostVAStartTrigger();
	
//	 

//	
//	 delay_ms(500);
//	
//	ge2dFill_Solid(0, 0, 800, 480, 0xff00FF);
//	
//	
//	delay_ms(500);
//	
//	ge2dFill_Solid(0, 0, 800, 480, 0x00FFFF);
//	
//	delay_ms(500);
//	
//	ge2dFill_Solid(0, 0, 800, 480, 0x0000FF);
//	
//		
//	delay_ms(500);
//	
//	ge2dFill_Solid(0, 0, 800, 480, 0x00FF00);
//	
//		delay_ms(500);
//	
//	ge2dFill_Solid(0, 0, 800, 480, 0xFF0000);
//	
//	delay_ms(1500);
//	

//		
//		ge2dSpriteBlt_Screen(100,100,160,120,picRGB565160x120);
//		delay_ms(1500);
//		
//		ge2dSpriteBlt_Screen(0,0,800,480,(void *)image_bmp1);
//		delay_ms(1500);
//		
//		ge2dSpriteBlt_Screen(0,0,800,480,(void *)image_bmp2);
//		
//		delay_ms(1500);
//		
//		ge2dSpriteBlt_Screen(0,0,800,480,(void *)image_bmp3);
//		
//		delay_ms(1500);
//		
//		ge2dSpriteBlt_Screen(0,0,800,480,(void *)image_bmp4);
//		
//		delay_ms(1500);
//		ge2dSpriteBlt_Screen(0,0,800,480,(void *)image_bmp6);
//		
//		delay_ms(1500);
		

	#ifdef  SYSUARTPRINTF
	sysprintf("LCD_FRAME1_BUFFER=%X,%X,%X,%X,%X\r\n",
	*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER),*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+1), 
	*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+2), *(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+3),
	*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+4));
	#endif
	 
	  usb_deinit();
		

		#ifdef  SYSUARTPRINTF  
		sysprintf("REG_USBD_OPER=%X\r\n",REG_USBD_OPER);
	   #endif
	  protocol_uart_init();
	
		
		rtc_init();
		//AD_init();
		
		//#ifdef POWER_AD_MODE
    AD_init_8V();
		#ifdef  SYSUARTPRINTF  
		sysprintf("VOLT_WORK=%X\r\n",VOLT_WORK);
		#endif
	 while((get_main_pwr_ad_value()<VOLT_WORK));
	 LED_POWER_ON();
	 LED_POWERLOW_OFF();
	 //#endif
	

	
	  AD_init_vbat();
	  
		
		REG_OPERATE(REG_SYS_LVRDCR,1,clear);
		#ifdef POWER_INT_MODE
		
		power_int_init();
		#endif
		
		
    W25Q128_init();
    
		nandflash_init();
   	SDRAM_DATA_INIT();
		
	
  
		check_sw5();
		POWER_5V_SETTING_initover();
		
		//delay_ms(1500);
		#ifdef  SYSUARTPRINTF  
		sysprintf("para.lcd_back_light.brightness=%X\r\n",para.lcd_back_light.brightness);
		#endif
	   Backlinght_Control_Init_HARDV4(para.lcd_back_light.brightness);
	 
	 
			voice_tim_init();
		
//		//send_wt588h_init(0xFFE0);
//		//delay_ms(50);
//////////////////		send_wt588h_init(0xFFE0|5);
//////////////////		delay_ms(50);
//////////////////		send_wt588h_init(5);
//////////////////		delay_ms(500);

   //send_wt588h_init(0xFEFE);
	 //	delay_ms(500);
	 
	send_wt588h_init(0xFFFE);
		delay_ms(10);
		
		#ifdef  SYSUARTPRINTF  
		sysprintf("VOICE INIT END\r\n",para.lcd_back_light.brightness);
		#endif
		/////delay_ms(5000);
    LVD_init_irq();
		
		#ifdef  SYSUARTPRINTF  
		sysprintf("ALL INIT END\r\n",para.lcd_back_light.brightness);
		#endif
		
		//f_chdrive("3:"); 
   // TP_res = f_mount(&usb_fatfs, "3:", 1);
		
	//	TP_res=f_mkdir("3:/FUNCTION_MEMORY");
//		sysprintf("f_mkdir=%X\r\n",TP_res);
		////////REG_OPERATE(REG_CLK_PCLKEN0,1<<1,set);//ENABLE WWDT
		////////WWDT_Open(WWDT_PRESCALER_2048,0x3f,TRUE);
//		send_wt588h_init(0X05);
//		delay_ms(10);
//	  for(cnt=0;cnt<800*480;cnt++)
//	{
//	  ((uint16_t *)u8FrameBufPtr)[cnt]=0x7777;
//	
//	}
//delay_ms(10000);
   #ifdef BIAOYANJI
	 while(1)//sw must off
	 {
		  comm_handle();
      touch_dev_poll();
		  bianyan_scan();
	 }
	 #endif
	 
	#ifdef SYSUARTPRINTF_p
//	 while(1)
//	 {
//	 	bmpBuf_kkk = (uint8_t *)(((uint32_t )malloc(2048+64)));
//	 bmpBuf_kkk_bak = bmpBuf_kkk;
//	bmpBuf_kkk = 	(uint8_t *)(shift_pointer((uint32_t)bmpBuf_kkk,32)+32);
//	bmpBuf_kkk = (uint8_t *)((uint32_t)bmpBuf_kkk|0x80000000	);
//	 sysprintf("bmpBuf_kkk = %08X,bmpBuf_kkk_bak= %08X,mallco\r\n",bmpBuf_kkk,bmpBuf_kkk_bak);
//		 
//		if(bmpBuf_kkk_bak!=0) free(bmpBuf_kkk_bak);
//	 sysprintf("bmpBuf_kkk = %08X,bmpBuf_kkk_bak= %08X,free\r\n",bmpBuf_kkk,bmpBuf_kkk_bak);
//		 
//	 }
		 #endif
		
//				sysprintf("LCD_FRAME1_BUFFER=%X,%X,%X,%X,%X\r\n",
//	*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER),*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+1), 
//	*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+2), *(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+3),
//	*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+4));
	 
		//if(0)
	if(READ_WORKMODE==WORK_FUNCTION)
		{
		while(1)
		{
      TimeProcess( );
     if(pwr_on_cnt==0)
		{
			// sysFlushCache(I_D_CACHE);
			LOGO_handle();
	    comm_handle();
      usbh_pooling_hubs();
 			touch_dev_poll();
      SEND_data();
			tts_control();
			//if(time1ms_count%2==0) LED_POWER_ON();
			//if(time1ms_count%2==1) LED_POWER_OFF();
//////////			Tp_data++;
//////////			if(Tp_data%2000 == 0)LED_POWER_ON();
//////////			if(Tp_data%2000 == 1000)LED_POWER_OFF();
//////////			if(Tp_data == 2001) Tp_data = 0;
			#ifdef POWER_AD_MODE
			main_pwr_process( );
			#endif
			download_process();
			//////////////WWDT_RELOAD_COUNTER();
		//	delay_ms(5000);
		}
		   }
		}//WORK_FUNCTION
		
		//if(1)
		if(READ_WORKMODE==WORK_TEST)
		{	
			
		//REG_OPERATE(REG_CLK_PCLKEN0,1<<17,clear);	
			uart2_init_download(115200);
		while(1)
		{
     ///////////// if((test_e>SW_DIP6_OFF_wait))
		////////////	TimeProcess( );
     /////////////if(pwr_on_cnt==0)
		/////////////{
			////////////////////////LOGO_handle();
			if(get_timego(Flag_Tim_1ms)>=1)
	     {
		      Flag_Tim_1ms = time1ms_count;
		      tts_time();
		
	      }
			
			
	    comm_handle();
		
     usbh_pooling_hubs();
			
 			touch_dev_poll();
      //SEND_data();
			tts_control();
			///////////////////////main_pwr_process( );
			///////////////////////download_process();
		///////////////}
	    hard_function_test();
//			sysFlushCache(I_D_CACHE);
//			if(_sys_uTimer0Count!=uTimer0Counthis)
//			{
//				uTimer0Counthis = _sys_uTimer0Count;
//				if(uTimer0Counthis%2)
//				{
//						//����LED0  �ر�LED1
//			 GPIO_Clr(GPIOB, BIT4);
//		   GPIO_Set(GPIOB, BIT5);
//				}
//				else
//				{
//					 //����LED1  �ر�LED0
//			 GPIO_Clr(GPIOB, BIT5);
//		   GPIO_Set(GPIOB, BIT4);
//				}
//				
//			}
		   }
		 }//WORK_TEST
}


