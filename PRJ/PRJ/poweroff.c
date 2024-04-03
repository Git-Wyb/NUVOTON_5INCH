
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "sys.h"
#include "Command_all.h"
#include "rtc.h"
#include "Aprotocol.h"
#include "poweron.h"
#include "gpio.h"
#include "BSP_init.h"
#include "adc.h"
#include "ff.h"
#include "wwdt.h"
#include "poweroff.h"



extern int gs_usb_mount_flag;
extern uint8_t LOGO_DATA_OUT_FLAG;
extern AreaConfig gs_AreaInfo[16];
extern uint8_t  LOGO_DOUBLE_VISIT;
extern PARA_TYPE para;
extern uint32_t logodata_sdrambuffer_addr_arry[16];
extern uint16_t System_Run_Cycle ;
extern uint32_t  time1ms_count_forrun;
extern uint32_t System_Runtime_Lasttime ;
extern Dataclass4567_Info Fieldx_Info[16];
extern SYSTEMERRORSTATUS_TYPE  systerm_error_status;
extern TIME_TYPE timeandday_now_app,pwr_on_time_app;
extern uint8_t* BaseData_ARR;
extern uint32_t logodata_basedata_BUFFER;
extern LAYER_SDRAM_STR display_layer_sdram;
uint32_t AD_8V_timer;
extern uint32_t time1ms_count;
uint32_t get_timego(uint32_t x_data_his);
extern uint16_t flag_logo_change;
extern uint32_t logodata_sdrambuffer_addr_arry_bak[16];
uint8_t FLAG_WDT=0;
extern uint8_t LOGO_ERR;


void LVD_IRQHandler(void)
{
while(1);
	//	sysprintf("\r\n--------------LVD INIT------------------\r\n");
//	REG_OPERATE(REG_SYS_MISCISR,1,clear);
//	////////////////REG_OPERATE(REG_CLK_PCLKEN0,1<<1,set);//ENABLE WWDT
//	///////////////WWDT_Open(WWDT_PRESCALER_1,1,TRUE);
//	
//		//FLAG_WDT = 1;
//	//REG_OPERATE(REG_CLK_PCLKEN0,1<<1,set);//ENABLE WWDT
//	//	WWDT_Open(WWDT_PRESCALER_2048,0xf,TRUE);
//	AD_init_8V();
//	
//	while(1)
//	{
//			if((GPIO_ReadBit(GPIOH,BIT5)==1)&&(get_main_pwr_ad_value()>VOLT_WORK))
//		{
//		if(FLAG_WDT==0)
//		{
//			#ifdef  SYSUARTPRINTF 
//			sysprintf("wdt on\n\r");
//			#endif
//		FLAG_WDT = 1;	
//		REG_OPERATE(REG_CLK_PCLKEN0,1<<1,set);//ENABLE WWDT
//		WWDT_Open(WWDT_PRESCALER_2048,0xf,TRUE);
//		}
//		}
//	}
}

void LVD_disable(void)
{
	REG_OPERATE(REG_SYS_MISCISR,1,clear);
	REG_OPERATE(REG_SYS_LVRDCR,(1<<9)|(1<<8)|1,clear);
	//REG_OPERATE(REG_SYS_MISCISR,1,clear);
	REG_OPERATE(REG_SYS_MISCIER,1,clear);
	
	sysDisableInterrupt(LVD_IRQn);
}

void LVD_init_irq(void)
{
	REG_OPERATE(REG_SYS_LVRDCR,(1<<9)|(1<<8),set);
	REG_OPERATE(REG_SYS_MISCISR,1,set);
	REG_OPERATE(REG_SYS_MISCIER,1,set);
//	
	 sysInstallISR(IRQ_LEVEL_7, LVD_IRQn, (PVOID)LVD_IRQHandler);
   sysEnableInterrupt(LVD_IRQn);
   sysSetLocalInterrupt(ENABLE_IRQ);
	REG_OPERATE(REG_SYS_LVRDCR,1,clear);
}

void LVD_init_scan(void)
{
	REG_OPERATE(REG_SYS_LVRDCR,(1<<9)|(1<<8)|1,set);
	REG_OPERATE(REG_SYS_MISCISR,1,set);
	REG_OPERATE(REG_SYS_MISCIER,1,clear);
	
	 //sysInstallISR(IRQ_LEVEL_7, LVD_IRQn, (PVOID)LVD_IRQHandler);
    sysDisableInterrupt(LVD_IRQn);
   // sysSetLocalInterrupt(ENABLE_IRQ);
}

uint16_t get_main_pwr_ad_value(void)
{
   static uint32_t Tp_data_normal=0;
	static uint8_t TP_FLAG=0;
	static uint8_t Tp_i;
	static uint16_t Tp_his_data;
	//uint8_t Tp_isr;
	uint32_t  Tp_look;
         
		       //  delay_ms(2000);
		 for(Tp_i=0;Tp_i<100;Tp_i++)
	{
	       REG_OPERATE(REG_ADC_CTL,ADC_CTL_MST,set);
	Tp_look = 0;
							   while((inpw(REG_ADC_ISR)&ADC_ISR_MF)==0)  
								 {
									 
									 Tp_look++;// Tp_look<0x100
									 if(Tp_look>0x200)  break;
								 }
	       ///////   sysprintf("Tp_look=%x\n\r",Tp_look);
						outpw(REG_ADC_ISR,ADC_ISR_MF);
			     if((inpw(REG_ADC_ISR)&ADC_ISR_NACF))
	           {
			      if(TP_FLAG==0)
							 Tp_data_normal =inpw(REG_ADC_DATA);
						else
							Tp_data_normal =(inpw(REG_ADC_DATA)+Tp_data_normal)/2;
							
						TP_FLAG=1;
							 
			        outpw(REG_ADC_ISR,ADC_ISR_NACF);
							
							 
						// REG_OPERATE(REG_ADC_CTL,ADC_CTL_MST,set);
	         // outpw(REG_ADC_ISR,ADC_ISR_MF);
				           //	while((inpw(REG_ADC_ISR)&ADC_ISR_MF)==0);
	           
			
		       //REG_OPERATE(REG_ADC_ISR,ADC_ISR_MF,set);
		          
							 #ifdef  SYSUARTPRINTF
			        sysprintf("Tp_data_normal1=%x\n\r",Tp_data_normal);
							 #endif
						//	 Tp_data_normal =inpw(REG_ADC_DATA);
							// sysprintf("Tp_data_normal2=%x\n\r",Tp_data_normal);
							 //Tp_his_data = Tp_data_normal;
							//return (uint16_t)Tp_data_normal;

						 }
						 else
						 {
							 outpw(REG_ADC_ISR,inpw(REG_ADC_ISR));
						 }
						// outpw(REG_ADC_ISR,inpw(REG_ADC_ISR));
		}				
		return Tp_data_normal;
}


void power_save(void)
{
	//////////////////////////////////////// LVD_disable();
	 LED_POWER_OFF();
	 LED_LOGO_OFF();
	 LED_FLASHERR_OFF();
	 LED_POWERLOW_OFF();
	 
	 //while(1);
	 //free((void *)display_layer_sdram.LCD_CACHE_BUFFER);
	 LCD_PWR_OFF();
	 POWER_5V_OFF();
	 //Backlinght_Control_Init_HARDV4(255);
	
	
	 //REG_OPERATE(REG_CLK_HCLKEN,1<<25,clear);//LCD
	// REG_OPERATE(REG_CLK_HCLKEN,1<<28,clear);//G2ED
	 //REG_OPERATE(REG_CLK_HCLKEN,1<<18,clear);//USBHOST
   REG_OPERATE(REG_CLK_HCLKEN,(1<<25)|(1<<28)|(1<<18),clear);
	
	//////////////////	 ///////////////////REG_OPERATE(REG_CLK_PCLKEN0,1<<3,clear);//GPIO
	  /* enable UART0 clock */
  
   //REG_OPERATE(REG_CLK_PCLKEN0,1<<16,clear);//UART0  sysuart
	// REG_OPERATE(REG_CLK_PCLKEN0,1<<17,clear);//UART1
	// REG_OPERATE(REG_CLK_PCLKEN0,1<<7,clear);//TIM2 CLK DISABLE
		#ifdef  SYSUARTPRINTF
		REG_OPERATE(REG_CLK_PCLKEN0,(1<<7)|(1<<17),clear);
	#else
	REG_OPERATE(REG_CLK_PCLKEN0,(1<<7)|(1<<16)|(1<<17),clear);
	#endif
	
	
//	REG_OPERATE(REG_CLK_PCLKEN1,1<<27,clear);//PWM
//	REG_OPERATE(REG_CLK_PCLKEN1, 1<<24,clear);//AD
  REG_OPERATE(REG_CLK_PCLKEN1, (1<<24)|(1<<27) ,clear);
	
//////////////////	 
	 LED_POWERLOW_ON();
	 AUDIO_AMPLIFIER_SHUT_DOWN;
	AMP_POWER_OFF;
	AMP_DATA_0;
  //PB3 PWM 20221011
	REG_OPERATE(REG_SYS_GPB_MFPL,0X0000F000,clear);
	GPIO_OpenBit(GPIOB,BIT3, DIR_OUTPUT, PULL_UP);
	GPIO_Clr(GPIOB, BIT3);
	
	REG_OPERATE(REG_SYS_GPA_MFPH,0XFFFFFFFF,clear);
	REG_OPERATE(REG_SYS_GPA_MFPL,0XFFFFFFFF,clear);
	REG_OPERATE(REG_SYS_GPB_MFPL,0X000000FF,clear);
	REG_OPERATE(REG_SYS_GPB_MFPH,0X0000F000,clear);
	REG_OPERATE(REG_SYS_GPD_MFPH,0XFFFFFFFF,clear);
	REG_OPERATE(REG_SYS_GPE_MFPH,0X000000FF,clear);
	REG_OPERATE(REG_SYS_GPE_MFPL,0XF0000000,clear);
	REG_OPERATE(REG_SYS_GPG_MFPH,0X000000FF,clear);
	REG_OPERATE(REG_SYS_GPG_MFPL,0XFF000F00,clear);
//	GPIO_OpenBit(GPIOA,0XFFFF, DIR_INPUT, NO_PULL_UP);
//	GPIO_OpenBit(GPIOD,0XFF00, DIR_INPUT, NO_PULL_UP);
//	GPIO_OpenBit(GPIOB,BIT0|BIT1|BIT11, DIR_INPUT, NO_PULL_UP);
//	GPIO_OpenBit(GPIOE,BIT7|BIT8|BIT9, DIR_INPUT, NO_PULL_UP);
//	GPIO_OpenBit(GPIOG,BIT2|BIT6|BIT7|BIT8|BIT9, DIR_INPUT, NO_PULL_UP);
  GPIO_DisableTriggerType(GPIOE,BIT7);
	GPIO_DisableInt(GPIOE);
	
	GPIO_OpenBit(GPIOA,0XFFFF, DIR_OUTPUT, PULL_UP);
	GPIO_OpenBit(GPIOD,0XFF00, DIR_OUTPUT, PULL_UP);
	GPIO_OpenBit(GPIOB,BIT0|BIT1|BIT11, DIR_OUTPUT, PULL_UP);
	GPIO_OpenBit(GPIOE,BIT7|BIT8|BIT9, DIR_OUTPUT, PULL_UP);
	GPIO_OpenBit(GPIOG,BIT2|BIT6|BIT7|BIT8|BIT9, DIR_OUTPUT, PULL_UP);
	
	GPIO_Clr(GPIOA, 0XFFFF);
	GPIO_Clr(GPIOB, BIT0|BIT1|BIT11);
	GPIO_Clr(GPIOD, 0XFF00);
	GPIO_Clr(GPIOE, BIT7|BIT8|BIT9);
	GPIO_Clr(GPIOG, BIT2|BIT6|BIT7|BIT8|BIT9);
	
		LCD_PWR_OFF();
		
	///////////////////////	LVD_init_irq();
}


void LOW_POWER_cyw(void)
{
	 
	uint8_t Tp_field=0;
	uint32_t Tp_addr=0;
	uint32_t Tp_Time=0;
  uint32_t Tp_loop = 0;
	
	
	//HAL_PWR_DisablePVD();
	
	 //HAL_NVIC_DisableIRQ(PVD_IRQn); ////NVIC_DisableIRQ(PVD_IRQn);//h????????????????????PVD
    
	  //ADC_Poweroff_Init();
	
	 /////////////////  sysDisableInterrupt(LVD_IRQn);
	  power_save( );
	
	if(LOGO_ERR == 1) goto HDLEFLAG;
	
	//power_checkreset();
	
	//LVD_init_scan();
////////////////////	while(1)
////////////////////	{
////////////////////	      Tp_loop++;
////////////////////				if(Tp_loop>100000)
////////////////////				{
////////////////////					break;
////////////////////				}
////////////////////	}
	///////////////////	REG_OPERATE(REG_SYS_MISCISR,1,set);
	///////////////// sysEnableInterrupt(LVD_IRQn);
	
//REG_OPERATE(REG_CLK_PCLKEN0,1<<1,set);//ENABLE WWDT
//		WWDT_Open(WWDT_PRESCALER_2048,0xf,TRUE);
	
////	
////	if(gs_usb_mount_flag==1)
////	{
////	 f_mount(0,"3:",1);//20181217
////	}
    //read_rtc(&pwr_off_time);	
	  if(LOGO_DATA_OUT_FLAG==3)
		{
		
		//for(Tp_field = 15; Tp_field!=0;Tp_field--)//OK
			for(Tp_field = 1; Tp_field<16;Tp_field++)//NG
    {
			

//			while(get_Vrefint_ad_value()==0)
//			{
//				Tp_loop++;
//				if(Tp_loop>10000)
//				{
//					goto HDLEFLAG;
//				}
//			}

			 
			switch(gs_AreaInfo[Tp_field].type)
		  {
				case Base_DATACLASS:
					//if(Tp_field == 0)
				     
					   break;
				case ActionNumber_DATACLASS://ActionNumber_DATACLASS:
				
					   if(LOGO_DOUBLE_VISIT == Tp_field) continue;//????????????????????h?? SDRAM???????????????????????? 
			  
				if(para.dataclass_1_2_action_count >= MAX_LOGO_ACTION_MONTH) continue;//20170510
					 #ifdef  SYSUARTPRINTF  
				sysprintf("logodata_sdrambuffer_addr_arry[Tp_field]=%x",logodata_sdrambuffer_addr_arry[Tp_field]);
			#endif
			#ifdef  SYSUARTPRINTF 
				sysprintf("logodata_sdrambuffer_addr_arry[Tp_field]=%x,%x,%x,%x",*(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]),
				*(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+1),*(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+2),
				*(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+3));
				#endif
				
				Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + ((para.dataclass_1_2_action_count*4)/64)*64;		
       // NANDFLASH_TO_SDRAM_RANDOM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,logodata_ActionNumber_SIZE,4);//????????????????????
			       
				SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
			
			   //LED_POWER_ON();
					   //Tp_addr = FIELD_str->Field_Addr[Tp_field]/2048 + sdram_actionindex*4;
				    // SDRAM_TO_NANDFLASH_RANDOM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,logodata_ActionNumber_SIZE,4);
					  break;
				case ActionTimers_DATACLASS:
					  if(LOGO_DOUBLE_VISIT == Tp_field) continue;//????????????????????h?? SDRAM???????????????????????? 
					  if(para.dataclass_1_2_action_count >= MAX_LOGO_ACTION_MONTH) continue;//20170510
             Tp_Time = System_Run_Cycle*(0xffffffff/100) + time1ms_count_forrun/100;
				     Tp_Time = System_Runtime_Lasttime-Tp_Time;
				     memcpy((void *)(logodata_sdrambuffer_addr_arry[Tp_field]+(para.dataclass_1_2_action_count%16)*4*2048+4),(void *)(&Tp_Time),4);
////////////////				    *(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(para.dataclass_1_2_action_count%16)*4*2048+4)=System_Runtime_Lasttime-Tp_Time;
				   //???????????????????
				
					 Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + ((para.dataclass_1_2_action_count *4)/64)*64;		
            SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
			    	break;
				case UnitPara_DATACLASS://
					  //???h??????????????????????????
					 // SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],(gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048,1);//??????
					  break;
				case TestFinalData_DATACLASS:
					if(LOGO_DOUBLE_VISIT != Tp_field) //????????????????????h?? SDRAM???????????????????????? 
					{ 
    					Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Fieldx_Info[Tp_field].cycle*64;		
			      //??????????
			       SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
					}
					
				   Fieldx_Info[Tp_field].num = Fieldx_Info[Tp_field].num  + 1;
					 if(((Fieldx_Info[Tp_field].num +1)*MAX_LOGO_FINALTEST_ONEPACKET)>0x20000)//???????? +1?????j??? ???????h????????
					 {
						 Fieldx_Info[Tp_field].num  =0;
						 Fieldx_Info[Tp_field].cycle = Fieldx_Info[Tp_field].cycle +1;
						 if((Fieldx_Info[Tp_field].cycle * 0x20000)>=gs_AreaInfo[Tp_field].space)
						 {
							 Fieldx_Info[Tp_field].cycle = 0;
							 Fieldx_Info[Tp_field].loop = Fieldx_Info[Tp_field].loop  +1;
						 }
					 }
					  break;
				case TestAllData_DATACLASS:
			  case AnomalyRecord_DATACLASS:
				 if(LOGO_DOUBLE_VISIT == Tp_field) continue;//????????????????????h?? SDRAM???????????????????????? 
					  if((flag_logo_change&(1<<Tp_field))==0)  continue;
				
				Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Fieldx_Info[Tp_field].cycle*64;		
			      //??????????
			       SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
					break;
				case ActionRecord_DATACLASS:
				 	 
				if(LOGO_DOUBLE_VISIT == Tp_field) //???¶?????¼
				{
					Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Fieldx_Info[Tp_field].cycle*64;		
			    NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
				}
				
				    if(systerm_error_status.bits.lse_error==1)	//??j?RTCû???
						 {
					*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000)=0xEF;
					*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+1)=0;
			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+2)=0;
			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+3)=0;
			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+4)=0x99; 
			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+5)=0x99;
          *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+6)=0x99;
				 
						 }
						 else
						 {
				  *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+((Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000)) = 0xEF;
					*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+((Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+1))=(timeandday_now_app.year);
			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+((Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+2))=(timeandday_now_app.month);
			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+((Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+3))=(timeandday_now_app.day);
			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+((Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+4))=(timeandday_now_app.hour); 
			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+((Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+5))=(timeandday_now_app.minute);
          *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+((Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+6))=(timeandday_now_app.second);
						 }
					/*		 for(Tp_i =0; Tp_i<(FIELD_str->Field_Number[Tp_field]/2 + FIELD_str->Field_Number[Tp_field]%2) ;Tp_i++)
				 	   {
							 *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(sdram_TestFinalNumx*MAX_LOGO_ALLTEST_ONEPACKET)%0x20000+Tp_i+6) = 0xff;
						 }*/
						 //?????????  ???????
						
						
						 Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Fieldx_Info[Tp_field].cycle*64;		
			      SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
						
						 
						 Fieldx_Info[Tp_field].num = Fieldx_Info[Tp_field].num + 1;
					 if(((Fieldx_Info[Tp_field].num+1)*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))>0x20000)//???????? +1?????j??? ???????h????????
					 {
						 //????????????
						 
						 Fieldx_Info[Tp_field].num =0;
						 Fieldx_Info[Tp_field].cycle = Fieldx_Info[Tp_field].cycle +1;
						 if((Fieldx_Info[Tp_field].cycle * 0x20000)>=gs_AreaInfo[Tp_field].space)
						 {
							 Fieldx_Info[Tp_field].cycle = 0;
							 Fieldx_Info[Tp_field].loop = Fieldx_Info[Tp_field].loop   +1;
						 }
						// Clear_sdram(1<<(Tp_field+8));//AREANO
						  // Tp_field_addr = FIELD_str->Field_Addr[Tp_field]/2048 + sdram_TestFinalCyclex*64;		
			        // NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_field_addr,1);
					 }
					 
				  break;
				default:
					break;
					
		  }
			if(logodata_sdrambuffer_addr_arry[Tp_field]!=0)
			{
				free((void *)logodata_sdrambuffer_addr_arry_bak[Tp_field]);
				logodata_sdrambuffer_addr_arry[Tp_field]=0;
				logodata_sdrambuffer_addr_arry_bak[Tp_field]=0;
			}
			}
		  ///if(get_Vrefint_ad_value())
			//{
				 *(uint8_t *)(BaseData_ARR+BASE_data_PowerOffYear*9+8) = 8;
				 *(uint8_t *)(BaseData_ARR+BASE_data_PowerOffMonth*9+8) = 8;
			 if(systerm_error_status.bits.lse_error==1)
				 {
					 para.last_poweroff_year = 99;//20170924
									 para.last_poweroff_month = 99;//20170924
				 }
				 else
				 {
					 para.last_poweroff_year = pwr_on_time_app.year;//20170924
						       para.last_poweroff_month = pwr_on_time_app.month;//20170924
				 }
					 
				
			   	field_info_save();//??BASEDATA??????????? 
		      WriteAreaConfig_new();
				write_para_to_sdram();
				 basedata_ram_to_sdram();
				
	      
	     // if(get_Vrefint_ad_value())
	     //{
				SDRAM_TO_NANDFLASH(logodata_basedata_BUFFER,baseB_data__nandflash_start,1);
	     //}
			//}
	  }
		else
		{
//			while(1)
//			{
//			Tp_loop++;
//				if(Tp_loop>10000)
//				{
//					goto HDLEFLAG;
//				}
//			}
		}
		
		 HDLEFLAG:
	  
		//LED_POWERLOW_OFF();
		#ifdef  SYSUARTPRINTF  
		sysprintf("\r\n--------------logo save over------------------\r\n");
		#endif
		
		#ifdef POWER_INT_MODE
		power_checkreset();
		#endif
		 while(1)
	   	{
          
		      //if(get_main_pwr_ad_value()>=VOLT_WORK)  //;  //Ö±µ½Ã»µç
		      	{
                             //reset
                             //HAL_NVIC_SystemReset( );
							//REG_OPERATE(REG_CLK_PCLKEN0,1<<1,set);//ENABLE WWDT
							//WWDT_Open(WWDT_PRESCALER_2048,0x3f,TRUE);
							while(1);
		      	}

	   	}
}

vu32 GPIOHCallback(UINT32 status, UINT32 userData)
{
	// sysprintf("i am here,status=%d\r\n",status);
	if(status & BIT5)
	 {
	   //Çå³ýÖÐ¶Ï±êÖ¾Î»
    GPIO_ClrISRBit(GPIOH,status);
		 GPIO_DisableInt(GPIOH);
		 LOW_POWER_cyw();
	 }
	  //Çå³ýÖÐ¶Ï±êÖ¾Î»
    GPIO_ClrISRBit(GPIOH,status);
    return 0;
}



void main_pwr_process(void)
{
 
	static uint16_t Tp_data_all=0,Tp_vol,Tp_data_ave,Tp_data_his;
	static uint8_t  Tp_cnt=0;
   if(LOGO_DATA_OUT_FLAG != 3) return;
	if(get_timego(AD_8V_timer)>1)
	{
	Tp_vol = get_main_pwr_ad_value();
	AD_8V_timer=	time1ms_count;
	//if(Tp_vol< VOLT_SLEEP)   LOW_POWER_cyw();	
	}
	else
	{
		return;
	}
	
	
////////	/////////////////////sysprintf("Tp_data_9v=%x\r\n",Tp_vol);
////////	
////////	//if(Tp_vol< VOLT_SLEEP)   LOW_POWER_cyw();
	if(Tp_vol == 0)  return;
//////////////	sysprintf("%x\r\n",abs((int)Tp_data_his-(int)Tp_vol));
	if((abs((int)Tp_data_his-(int)Tp_vol)>0x400) )
	{
		Tp_data_all = 0;
		Tp_cnt = 0;
		Tp_data_his = Tp_vol;
		//while(1);
		//////////////////////////////////////sysprintf("err ad value,%d,%d\r\n",(int)Tp_data_his,(int)Tp_vol);
		return;
	}
	
	Tp_data_all =Tp_data_all+ Tp_vol;
	
	Tp_cnt++;
	if(Tp_cnt>=5) 
	{
		Tp_cnt = 5;
	
	  Tp_data_ave =Tp_data_all/Tp_cnt;
	  //sysprintf("Tp_data_ave=%x,\r\n",Tp_data_ave);
		
    if( Tp_data_ave<VOLT_SLEEP)  //||test_main_pwr_process)
    //if(time1ms_count==0)
	  {
	    LOW_POWER_cyw();
			sysprintf("AD_9V,ERR=%x,\r\n",Tp_data_ave);
		 
    }
		
		Tp_data_all = Tp_data_all - Tp_data_ave;
  }
	
	Tp_data_his = Tp_vol; 
}