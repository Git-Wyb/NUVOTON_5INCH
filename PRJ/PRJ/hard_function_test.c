#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "sys.h"
#include "gpio.h"
#include "hard_function_test.h"
#include "Command_all.h"
#include "BSP_Init.h"
#include "display.h"
#include "DIP_SW.h"
#include "2d.h"
#include "lcd.h"
#include "nand_nand.h"
#include "usbh_lib.h"
#include "rtc.h"
#include "adc.h"
#include "Aprotocol.h"
#include "poweron.h"
#include "poweroff.h"
#include "wwdt.h"
#include "W25Q128.h"

extern LAYER_SDRAM_STR display_layer_sdram;
test_E test_e=test_Reset;//test_Reset;//download_pin_test;//test_Reset;
extern volatile uint8_t Flag_int ;
extern UINT32 _ClipTL, _ClipBR;
extern BOOL _ClipEnable;
extern tpInfo gs_tpInfo[16];
extern int gs_usb_mount_flag;
extern uint8_t IMAGE_SEARCHED;
RTC_TIME_DATA_T time_T;
extern uint8_t touch_send_imm ;
extern uint32_t Tp_bad;
extern BMP_RPOTOCOL_TYPE  bmp_protocol[3];
extern BADMANAGE_TAB_TYPE_U badmanage_str[1];
extern uint16_t CHECK_SUM_USB,CHECK_SUM_NAND;
extern char  USB_IMAGE_TYPE ;
extern volatile uint8_t  *UART2_RX_BUFF;//[RX_BUFF_MAX];
extern volatile uint16_t  RX_COUNT_IN ;
extern volatile uint16_t  RX_COUNT_OUT ;
uint8_t FLAG_DISPLAY=0;
extern TTS_TYPE  tts;
extern uint8_t STOP_FLAG_CYW;
extern uint8_t screen_reverse_bit;
uint8_t TEMP_COUNT=0;
static uint8_t Q_Num=0;
static uint32_t Q_ADDR[4]={0};
extern uint8_t *BaseData_ARR;
extern uint8_t wt588h_send_step;

void AD_init_8V(void)
{
	adcOpen();
	
	REG_OPERATE(REG_ADC_CONF,(1<<3)|(1<<4)|(1<<5),clear);
	REG_OPERATE(REG_ADC_CONF,(1<<3),set);
	
	
	REG_OPERATE(REG_ADC_CONF,ADC_CONF_VBATEN,clear);
////////////	
	REG_OPERATE(REG_ADC_CONF,ADC_CONF_HSPEED,set);
////////////	//REG_OPERATE(REG_ADC_CONF,ADC_CONF_HSPEED,clear);
////////////	
	REG_OPERATE(REG_ADC_CONF,(1<<6)|(1<<7),set);//vref
////////////	
	REG_OPERATE(REG_ADC_CONF,ADC_CONF_NACEN,set);
	
	REG_OPERATE(REG_ADC_CONF,0xf<<16,set);//REFCNT

////////////	
	REG_OPERATE(REG_ADC_ISR,ADC_ISR_NACF/*ADC_ISR_MF|ADC_ISR_VBF|ADC_ISR_SELFTF|ADC_ISR_NACF*/,set);
////////////	
	REG_OPERATE(REG_ADC_CTL,ADC_CTL_VBGEN,clear);
////////////	/* Enable ADC Power */
  outpw(REG_ADC_CTL, ADC_CTL_ADEN);
////////////	
	REG_OPERATE(REG_ADC_CTL,ADC_CTL_MST,set);
	while((inpw(REG_ADC_ISR)&ADC_ISR_MF)==0);
////////////		       //REG_OPERATE(REG_ADC_ISR,ADC_ISR_MF,set);
	outpw(REG_ADC_ISR,inpw(REG_ADC_ISR));
	
}
void AD_init_vbat(void)
{

	uint8_t complete;
	//adcOpen();
//	REG_OPERATE(REG_CLK_PCLKEN1, (1<<24),set);
//	REG_OPERATE(REG_ADC_CTL,ADC_CTL_ADEN|ADC_CTL_VBGEN,set);
//	REG_OPERATE(REG_ADC_CONF,ADC_CONF_VBATEN,set);
//	REG_OPERATE(REG_ADC_CONF,ADC_CONF_NACEN,set);
//	REG_OPERATE(REG_ADC_CONF,(1<<3)|(1<<4)|(1<<5),clear);
//	REG_OPERATE(REG_ADC_CONF,(1<<6)|(1<<7),set);
//	REG_OPERATE(REG_ADC_CONF,ADC_CONF_HSPEED,set);
//	REG_OPERATE(REG_ADC_ISR,ADC_ISR_MF|ADC_ISR_NACF,set);
//	REG_OPERATE(REG_ADC_IER,ADC_IER_MIEN,set);
//	
//	do{
//complete = 0;
//REG_OPERATE(REG_ADC_CTL,ADC_CTL_MST,set);
//sysprintf("Waiting for Normal mode Interrupt\r\n");
//while((inpw(REG_ADC_ISR) & ADC_ISR_MF)==0);
////rREG_ISR = ADC_ISR_MF; //Clear MF flag
//		REG_OPERATE(REG_ADC_ISR,ADC_ISR_MF,set);
////while((inpw(REG_ADC_ISR) & ADC_ISR_NACF)==0);
////if(inpw(REG_ADC_ISR) & ADC_ISR_NACF)
//{

//	Tp_data=inpw(REG_ADC_DATA);
//	REG_OPERATE(REG_ADC_ISR,ADC_ISR_NACF,set);
////n=(33*data*100)>>12;
////d1=n/1000;
////d2=n%1000;
//sysprintf("DATA=0x%08x\r\n",Tp_data);
//}
////else
////sysprintf("interrupt error\r\n");
//}while(1);
//adcOpen();
	//REG_OPERATE(REG_CLK_PCLKEN1, (1<<24),set);
	//adcIoctl(VBPOWER_ON,0,0);
	
	//adcIoctl(VBAT_ON,(UINT32)BattreyConvCallback,0);
	//REG_OPERATE(REG_ADC_CONF,(1<<6)|(1<<7),set);
	//adcChangeChannel(AIN0);
	//adcIoctl(START_MST,0,0);
	
	adcOpen();
	
	REG_OPERATE(REG_ADC_CONF,ADC_CONF_VBATEN,set);
	REG_OPERATE(REG_ADC_CONF,ADC_CONF_HSPEED,set);
	
	REG_OPERATE(REG_ADC_CONF,(1<<6)|(1<<7),clear);//vref
	REG_OPERATE(REG_ADC_CONF,(1<<3)|(1<<4)|(1<<5),clear);//000-bat
	REG_OPERATE(REG_ADC_CONF,(1<<16)|(1<<17)|(1<<18)|(1<<19),set);
	REG_OPERATE(REG_ADC_CTL,ADC_CTL_VBGEN,set);
	
	////////////////////REG_OPERATE(REG_ADC_CONF,ADC_CONF_SELFTEN,set);
	REG_OPERATE(REG_ADC_CONF,ADC_CONF_NACEN,clear);
	REG_OPERATE(REG_ADC_ISR,ADC_ISR_VBF|ADC_ISR_NACF/*ADC_ISR_MF|ADC_ISR_VBF|ADC_ISR_SELFTF|ADC_ISR_NACF*/,set);
	
	/* Enable ADC Power */
    outpw(REG_ADC_CTL, ADC_CTL_ADEN);
	
		REG_OPERATE(REG_ADC_CTL,ADC_CTL_MST,set);
				   while((inpw(REG_ADC_ISR)&ADC_ISR_MF)==0);
		       //REG_OPERATE(REG_ADC_ISR,ADC_ISR_MF,set);
		         // outpw(REG_ADC_ISR,ADC_ISR_MF);
						 outpw(REG_ADC_ISR,inpw(REG_ADC_ISR));
	//outpw(REG_ADC_CONF,ADC_CONF_VBATEN);
	///////////////REG_OPERATE(REG_ADC_IER,ADC_IER_MIEN,set);
sysprintf("AD INIT END\n\r");
	//do
	//{

	//}
	//while(1);
}


void hard_function_test(void)
{
	static uint8_t Tp_sw_count;
	static uint8_t buf[64];
	//static UINT32 _ClipTL, _ClipBR;
	static UINT32 width,height,x,y;
	static uint32_t Tp_i ;
	static uint32_t sdram_total = 0,sdram_new_addr;
	static	uint32_t Tp_data_vbat,Tp_data_normal;
	static  float Tp_advalue;
	
	switch(test_e)
	{
		case test_Reset:
			  if(READ_PRODUCTTYPE==PORDUCT_5INCH)
				{
					SCREEN_CONTROL133_1;
				  SCREEN_CONTROL134_0;
				}
				AD_init_8V();
				 Backlinght_Control_Init_HARDV4(0);
			   screen_reverse_bit=1;
				 RTC_CLKOUT();
			   test_e = SW_DIP1_ON_dis;
			   break;
		case SW_DIP1_ON_dis:
			    memset((void *)(display_layer_sdram.LCD_CACHE_BUFFER&(~0x80000000)),0xff,800*480*2);
		      SetZuobiao(10, 400 + 40);
		      lcd_printf_new("ONLY-DIP-SW1-ON ");
		
		      test_e = SW_DIP1_ON_wait;
		      break;
		case SW_DIP1_ON_wait:
			    //gs_usb_mount_flag = 0;
			    Tp_sw_count = 0;
			    if(READ_PIN_SW1_1==SW_ON)
					{
						Tp_sw_count++;
						LED_POWER_ON();
					}
					else
					{
						LED_POWER_OFF();
					}
					if(READ_PIN_SW1_2==SW_ON)
					{
						Tp_sw_count++;
						LED_LOGO_ON();
					}
					else
					{
						LED_LOGO_OFF();
					}
					if(READ_PIN_SW1_3==SW_ON)
					{
						Tp_sw_count++;
						LED_POWERLOW_ON();
					}
					else
					{
						LED_POWERLOW_OFF();
					}
						if(READ_PIN_SW1_4==SW_ON)
					{
						Tp_sw_count++;
						LED_FLASHERR_ON();
					}
					else
					{
						LED_FLASHERR_OFF();
					}
					if(READ_PIN_SW5==SW_ON)
					{
						Tp_sw_count++;
					}
					if(READ_PIN_SW1_6==SW_ON)
					{
						Tp_sw_count++;
					}
					if((Tp_sw_count==1)&&(READ_PIN_SW1_1==SW_ON))
					{
						test_e = SW_DIP1_OFF_dis;
					}
					
			    break;
	   	case SW_DIP1_OFF_dis:
				   SetZuobiao(10, 400 + 40);
				   lcd_printf_new("ONLY-DIP-SW1-OFF");
			     test_e = SW_DIP1_OFF_wait;
				   break;
		  case SW_DIP1_OFF_wait:
				  // gs_usb_mount_flag = 0;
				   Tp_sw_count = 0;
			    if(READ_PIN_SW1_1==SW_ON)
					{
						
						LED_POWER_ON();
					}
					else
					{
						Tp_sw_count++;
						LED_POWER_OFF();
					}
					if(READ_PIN_SW1_2==SW_ON)
					{
						
						LED_LOGO_ON();
					}
					else
					{
						Tp_sw_count++;
						LED_LOGO_OFF();
					}
					if(READ_PIN_SW1_3==SW_ON)
					{
						
						LED_POWERLOW_ON();
					}
					else
					{
						Tp_sw_count++;
						LED_POWERLOW_OFF();
					}
						if(READ_PIN_SW1_4==SW_ON)
					{
						
						LED_FLASHERR_ON();
					}
					else
					{
						Tp_sw_count++;
						LED_FLASHERR_OFF();
					}
					if(READ_PIN_SW5==SW_OFF)
					{
						Tp_sw_count++;
					}
					if(READ_PIN_SW1_6==SW_OFF)
					{
						Tp_sw_count++;
					}
					if(Tp_sw_count==6)
					{
						test_e = SW_DIP2_ON_dis;
					}
					
				   break;
			case SW_DIP2_ON_dis:
			    //memset((void *)(display_layer_sdram.LCD_CACHE_BUFFER&(~0x80000000)),0xff,800*480*2);
		      SetZuobiao(10, 400 + 40);
		      lcd_printf_new("ONLY-DIP-SW2-ON ");
		
		      test_e = SW_DIP2_ON_wait;
		      break;
		case SW_DIP2_ON_wait:
			    //gs_usb_mount_flag = 0;
			    Tp_sw_count = 0;
			    if(READ_PIN_SW1_1==SW_ON)
					{
						Tp_sw_count++;
						LED_POWER_ON();
					}
					else
					{
						LED_POWER_OFF();
					}
					if(READ_PIN_SW1_2==SW_ON)
					{
						Tp_sw_count++;
						LED_LOGO_ON();
					}
					else
					{
						LED_LOGO_OFF();
					}
					if(READ_PIN_SW1_3==SW_ON)
					{
						Tp_sw_count++;
						LED_POWERLOW_ON();
					}
					else
					{
						LED_POWERLOW_OFF();
					}
						if(READ_PIN_SW1_4==SW_ON)
					{
						Tp_sw_count++;
						LED_FLASHERR_ON();
					}
					else
					{
						LED_FLASHERR_OFF();
					}
					if(READ_PIN_SW5==SW_ON)
					{
						Tp_sw_count++;
					}
					if(READ_PIN_SW1_6==SW_ON)
					{
						Tp_sw_count++;
					}
					if((Tp_sw_count==1)&&(READ_PIN_SW1_2==SW_ON))
					{
						test_e = SW_DIP2_OFF_dis;
					}
					
			    break;
	   	case SW_DIP2_OFF_dis:
				   SetZuobiao(10, 400 + 40);
				   lcd_printf_new("ONLY-DIP-SW2-OFF");
			     test_e = SW_DIP2_OFF_wait;
				   break;
		  case SW_DIP2_OFF_wait:
				  // gs_usb_mount_flag = 0;
				   Tp_sw_count = 0;
			    if(READ_PIN_SW1_1==SW_ON)
					{
						
						LED_POWER_ON();
					}
					else
					{
						Tp_sw_count++;
						LED_POWER_OFF();
					}
					if(READ_PIN_SW1_2==SW_ON)
					{
						
						LED_LOGO_ON();
					}
					else
					{
						Tp_sw_count++;
						LED_LOGO_OFF();
					}
					if(READ_PIN_SW1_3==SW_ON)
					{
						
						LED_POWERLOW_ON();
					}
					else
					{
						Tp_sw_count++;
						LED_POWERLOW_OFF();
					}
						if(READ_PIN_SW1_4==SW_ON)
					{
						
						LED_FLASHERR_ON();
					}
					else
					{
						Tp_sw_count++;
						LED_FLASHERR_OFF();
					}
					if(READ_PIN_SW5==SW_OFF)
					{
						Tp_sw_count++;
					}
					if(READ_PIN_SW1_6==SW_OFF)
					{
						Tp_sw_count++;
					}
					if(Tp_sw_count==6)
					{
						test_e = SW_DIP3_ON_dis;
					}
					
				   break;
			case SW_DIP3_ON_dis:
			   // memset((void *)(display_layer_sdram.LCD_CACHE_BUFFER&(~0x80000000)),0xff,800*480*2);
		      SetZuobiao(10, 400 + 40);
		      lcd_printf_new("ONLY-DIP-SW3-ON ");
		
		      test_e = SW_DIP3_ON_wait;
		      break;
		case SW_DIP3_ON_wait:
			    //gs_usb_mount_flag = 0;
			    Tp_sw_count = 0;
			    if(READ_PIN_SW1_1==SW_ON)
					{
						Tp_sw_count++;
						LED_POWER_ON();
					}
					else
					{
						LED_POWER_OFF();
					}
					if(READ_PIN_SW1_2==SW_ON)
					{
						Tp_sw_count++;
						LED_LOGO_ON();
					}
					else
					{
						LED_LOGO_OFF();
					}
					if(READ_PIN_SW1_3==SW_ON)
					{
						Tp_sw_count++;
						LED_POWERLOW_ON();
					}
					else
					{
						LED_POWERLOW_OFF();
					}
						if(READ_PIN_SW1_4==SW_ON)
					{
						Tp_sw_count++;
						LED_FLASHERR_ON();
					}
					else
					{
						LED_FLASHERR_OFF();
					}
					if(READ_PIN_SW5==SW_ON)
					{
						Tp_sw_count++;
					}
					if(READ_PIN_SW1_6==SW_ON)
					{
						Tp_sw_count++;
					}
					if((Tp_sw_count==1)&&(READ_PIN_SW1_3==SW_ON))
					{
						test_e = SW_DIP3_OFF_dis;
					}
					
			    break;
	   	case SW_DIP3_OFF_dis:
				   SetZuobiao(10, 400 + 40);
				   lcd_printf_new("ONLY-DIP-SW3-OFF");
			     test_e = SW_DIP3_OFF_wait;
				   break;
		  case SW_DIP3_OFF_wait:
				  // gs_usb_mount_flag = 0;
				   Tp_sw_count = 0;
			    if(READ_PIN_SW1_1==SW_ON)
					{
						
						LED_POWER_ON();
					}
					else
					{
						Tp_sw_count++;
						LED_POWER_OFF();
					}
					if(READ_PIN_SW1_2==SW_ON)
					{
						
						LED_LOGO_ON();
					}
					else
					{
						Tp_sw_count++;
						LED_LOGO_OFF();
					}
					if(READ_PIN_SW1_3==SW_ON)
					{
						
						LED_POWERLOW_ON();
					}
					else
					{
						Tp_sw_count++;
						LED_POWERLOW_OFF();
					}
						if(READ_PIN_SW1_4==SW_ON)
					{
						
						LED_FLASHERR_ON();
					}
					else
					{
						Tp_sw_count++;
						LED_FLASHERR_OFF();
					}
					if(READ_PIN_SW5==SW_OFF)
					{
						Tp_sw_count++;
					}
					if(READ_PIN_SW1_6==SW_OFF)
					{
						Tp_sw_count++;
					}
					if(Tp_sw_count==6)
					{
						test_e = SW_DIP4_ON_dis;
					}
					
				   break;
				case SW_DIP4_ON_dis:
			   // memset((void *)(display_layer_sdram.LCD_CACHE_BUFFER&(~0x80000000)),0xff,800*480*2);
		      SetZuobiao(10, 400 + 40);
		      lcd_printf_new("ONLY-DIP-SW4-ON ");
		
		      test_e = SW_DIP4_ON_wait;
		      break;
		case SW_DIP4_ON_wait:
			    //gs_usb_mount_flag = 0;
			    Tp_sw_count = 0;
			    if(READ_PIN_SW1_1==SW_ON)
					{
						Tp_sw_count++;
						LED_POWER_ON();
					}
					else
					{
						LED_POWER_OFF();
					}
					if(READ_PIN_SW1_2==SW_ON)
					{
						Tp_sw_count++;
						LED_LOGO_ON();
					}
					else
					{
						LED_LOGO_OFF();
					}
					if(READ_PIN_SW1_3==SW_ON)
					{
						Tp_sw_count++;
						LED_POWERLOW_ON();
					}
					else
					{
						LED_POWERLOW_OFF();
					}
						if(READ_PIN_SW1_4==SW_ON)
					{
						Tp_sw_count++;
						LED_FLASHERR_ON();
					}
					else
					{
						LED_FLASHERR_OFF();
					}
					if(READ_PIN_SW5==SW_ON)
					{
						Tp_sw_count++;
					}
					if(READ_PIN_SW1_6==SW_ON)
					{
						Tp_sw_count++;
					}
					if((Tp_sw_count==1)&&(READ_PIN_SW1_4==SW_ON))
					{
						test_e = SW_DIP4_OFF_dis;
					}
					
			    break;
	   	case SW_DIP4_OFF_dis:
				   SetZuobiao(10, 400 + 40);
				   lcd_printf_new("ONLY-DIP-SW4-OFF");
			     test_e = SW_DIP4_OFF_wait;
				   break;
		  case SW_DIP4_OFF_wait:
				  // gs_usb_mount_flag = 0;
				   Tp_sw_count = 0;
			    if(READ_PIN_SW1_1==SW_ON)
					{
						
						LED_POWER_ON();
					}
					else
					{
						Tp_sw_count++;
						LED_POWER_OFF();
					}
					if(READ_PIN_SW1_2==SW_ON)
					{
						
						LED_LOGO_ON();
					}
					else
					{
						Tp_sw_count++;
						LED_LOGO_OFF();
					}
					if(READ_PIN_SW1_3==SW_ON)
					{
						
						LED_POWERLOW_ON();
					}
					else
					{
						Tp_sw_count++;
						LED_POWERLOW_OFF();
					}
						if(READ_PIN_SW1_4==SW_ON)
					{
						
						LED_FLASHERR_ON();
					}
					else
					{
						Tp_sw_count++;
						LED_FLASHERR_OFF();
					}
					if(READ_PIN_SW5==SW_OFF)
					{
						Tp_sw_count++;
					}
					if(READ_PIN_SW1_6==SW_OFF)
					{
						Tp_sw_count++;
					}
					if(Tp_sw_count==6)
					{
						test_e = SW_DIP5_ON_dis;
					}
					
				   break;
			case SW_DIP5_ON_dis:
			   // memset((void *)(display_layer_sdram.LCD_CACHE_BUFFER&(~0x80000000)),0xff,800*480*2);
		      SetZuobiao(10, 400 + 40);
		      lcd_printf_new("ONLY-DIP-SW5-ON ");
		
		      test_e = SW_DIP5_ON_wait;
		      break;
		case SW_DIP5_ON_wait:
			    //gs_usb_mount_flag = 0;
			    Tp_sw_count = 0;
			    if(READ_PIN_SW1_1==SW_ON)
					{
						Tp_sw_count++;
						LED_POWER_ON();
					}
					else
					{
						LED_POWER_OFF();
					}
					if(READ_PIN_SW1_2==SW_ON)
					{
						Tp_sw_count++;
						LED_LOGO_ON();
					}
					else
					{
						LED_LOGO_OFF();
					}
					if(READ_PIN_SW1_3==SW_ON)
					{
						Tp_sw_count++;
						LED_POWERLOW_ON();
					}
					else
					{
						LED_POWERLOW_OFF();
					}
						if(READ_PIN_SW1_4==SW_ON)
					{
						Tp_sw_count++;
						LED_FLASHERR_ON();
					}
					else
					{
						LED_FLASHERR_OFF();
					}
					if(READ_PIN_SW5==SW_ON)
					{
						Tp_sw_count++;
					}
					if(READ_PIN_SW1_6==SW_ON)
					{
						Tp_sw_count++;
					}
					if((Tp_sw_count==1)&&(READ_PIN_SW5==SW_ON))
					{
						test_e = SW_DIP5_OFF_dis;
					}
					
			    break;
	   	case SW_DIP5_OFF_dis:
				   SetZuobiao(10, 400 + 40);
				   lcd_printf_new("ONLY-DIP-SW5-OFF");
			     test_e = SW_DIP5_OFF_wait;
				   break;
		  case SW_DIP5_OFF_wait:
				  // gs_usb_mount_flag = 0;
				   Tp_sw_count = 0;
			    if(READ_PIN_SW1_1==SW_ON)
					{
						
						LED_POWER_ON();
					}
					else
					{
						Tp_sw_count++;
						LED_POWER_OFF();
					}
					if(READ_PIN_SW1_2==SW_ON)
					{
						
						LED_LOGO_ON();
					}
					else
					{
						Tp_sw_count++;
						LED_LOGO_OFF();
					}
					if(READ_PIN_SW1_3==SW_ON)
					{
						
						LED_POWERLOW_ON();
					}
					else
					{
						Tp_sw_count++;
						LED_POWERLOW_OFF();
					}
						if(READ_PIN_SW1_4==SW_ON)
					{
						
						LED_FLASHERR_ON();
					}
					else
					{
						Tp_sw_count++;
						LED_FLASHERR_OFF();
					}
					if(READ_PIN_SW5==SW_OFF)
					{
						Tp_sw_count++;
					}
					if(READ_PIN_SW1_6==SW_OFF)
					{
						Tp_sw_count++;
					}
					if(Tp_sw_count==6)
					{
						test_e = SW_DIP6_ON_dis;
					}
					
				   break;
			case SW_DIP6_ON_dis:
			   // memset((void *)(display_layer_sdram.LCD_CACHE_BUFFER&(~0x80000000)),0xff,800*480*2);
		      SetZuobiao(10, 400 + 40);
		      lcd_printf_new("ONLY-DIP-SW6-ON ");
		
		      test_e = SW_DIP6_ON_wait;
		      break;
		case SW_DIP6_ON_wait:
			    //gs_usb_mount_flag = 0;
			    Tp_sw_count = 0;
			    if(READ_PIN_SW1_1==SW_ON)
					{
						Tp_sw_count++;
						LED_POWER_ON();
					}
					else
					{
						LED_POWER_OFF();
					}
					if(READ_PIN_SW1_2==SW_ON)
					{
						Tp_sw_count++;
						LED_LOGO_ON();
					}
					else
					{
						LED_LOGO_OFF();
					}
					if(READ_PIN_SW1_3==SW_ON)
					{
						Tp_sw_count++;
						LED_POWERLOW_ON();
					}
					else
					{
						LED_POWERLOW_OFF();
					}
						if(READ_PIN_SW1_4==SW_ON)
					{
						Tp_sw_count++;
						LED_FLASHERR_ON();
					}
					else
					{
						LED_FLASHERR_OFF();
					}
					if(READ_PIN_SW5==SW_ON)
					{
						Tp_sw_count++;
					}
					if(READ_PIN_SW1_6==SW_ON)
					{
						Tp_sw_count++;
					}
					if((Tp_sw_count==1)&&(READ_PIN_SW1_6==SW_ON))
					{
						test_e = SW_DIP6_OFF_dis;
					}
					
			    break;
	   	case SW_DIP6_OFF_dis:
				   SetZuobiao(10, 400 + 40);
				   lcd_printf_new("ONLY-DIP-SW6-OFF");
			     test_e = SW_DIP6_OFF_wait;
				   break;
		  case SW_DIP6_OFF_wait:
				  // gs_usb_mount_flag = 0;
				   Tp_sw_count = 0;
			    if(READ_PIN_SW1_1==SW_ON)
					{
						
						LED_POWER_ON();
					}
					else
					{
						Tp_sw_count++;
						LED_POWER_OFF();
					}
					if(READ_PIN_SW1_2==SW_ON)
					{
						
						LED_LOGO_ON();
					}
					else
					{
						Tp_sw_count++;
						LED_LOGO_OFF();
					}
					if(READ_PIN_SW1_3==SW_ON)
					{
						
						LED_POWERLOW_ON();
					}
					else
					{
						Tp_sw_count++;
						LED_POWERLOW_OFF();
					}
						if(READ_PIN_SW1_4==SW_ON)
					{
						
						LED_FLASHERR_ON();
					}
					else
					{
						Tp_sw_count++;
						LED_FLASHERR_OFF();
					}
					if(READ_PIN_SW5==SW_OFF)
					{
						Tp_sw_count++;
					}
					if(READ_PIN_SW1_6==SW_OFF)
					{
						Tp_sw_count++;
					}
					if(Tp_sw_count==6)
					{
						test_e = BACK_light_dis;
					}
					
				   break;		
			case BACK_light_dis:
				   SetZuobiao(10, 400 + 40);
				   lcd_printf_new("LCD_BACKLIGHT=100%");
			     Backlinght_Control_Init_HARDV4(0);
			     test_e = BACK_light_100_1;
			     Flag_int = 0;
			     break;
			case BACK_light_100_1:
				   if(Flag_int==1)
					 {
						 sysprintf("BACK_light_50_re\r\n");
						 Flag_int = 0;
						  test_e = BACK_light_50_re;
					 }
				   break;
			case BACK_light_50_re:
				   if(touch_send_imm==0)
					 {
						 sysprintf("BACK_light_50\r\n");
						 test_e = BACK_light_50;
						 Flag_int = 0;
					 }
				   break;
			case BACK_light_50:
				   if(Flag_int==1)
					 {
						 sysprintf("BACK_light_0_re\r\n");
						 Flag_int = 0;
						 FT5x06_RD_Reg(0, buf, 42);
						 SetZuobiao(10, 400 + 40);
						 lcd_printf_new(" LCD_BACKLIGHT=50%");
						 Backlinght_Control_Init_HARDV4(128);
						 test_e = BACK_light_0_re;
					 }
				   break;
			case BACK_light_0_re:
				    if(touch_send_imm==0)
					 {
						 test_e = BACK_light_0;
						 Flag_int = 0;
					 }
				   break;
			case BACK_light_0:
				 if(Flag_int==1)
					 {
						 Flag_int = 0;
						 FT5x06_RD_Reg(0, buf, 42);
						 SetZuobiao(10, 400 + 40);
						 lcd_printf_new("  LCD_BACKLIGHT=0%");
						 Backlinght_Control_Init_HARDV4(255);
						 test_e = BACK_light_50_2_re;
					 }
				   break;
			case BACK_light_50_2_re:
				    if(touch_send_imm==0)
					 {
						 test_e = BACK_light_50_2;
						 Flag_int = 0;
					 }
				   break;
			case BACK_light_50_2:
				    if(Flag_int==1)
					 {
						 Flag_int = 0;
						 FT5x06_RD_Reg(0, buf, 42);
						 SetZuobiao(10, 400 + 40);
						 lcd_printf_new(" LCD_BACKLIGHT=50%");
						 Backlinght_Control_Init_HARDV4(128);
						 test_e = BACK_light_100_2_re;
					 }
				   break;
			case BACK_light_100_2_re:
				   if(touch_send_imm==0)
					 {
						 test_e = BACK_light_100_2;
						 Flag_int = 0;
					 }
				   break;
			case BACK_light_100_2:
				if(Flag_int==1)
					 {
						 Flag_int = 0;
						 FT5x06_RD_Reg(0, buf, 42);
						 SetZuobiao(10, 400 + 40);
						 lcd_printf_new("LCD_BACKLIGHT=100%");
						 Backlinght_Control_Init_HARDV4(0);
						 test_e = LCD_forward_direction;
					 }
				    break;
			case LCD_forward_direction:
				   if(touch_send_imm==0)
					 {
					SetZuobiao(10, 400 + 40);
				   lcd_printf_new("LCD_forward_direction");
			     SCREEN_CONTROL133_0;
			     SCREEN_CONTROL134_1;
			     screen_reverse_bit=0;
						 Flag_int = 0;
			     test_e = LCD_forward_wait;
					 }
				   break;
		  case LCD_forward_wait:
				   if(Flag_int==1)
					 {
						 Flag_int = 0;
						 FT5x06_RD_Reg(0, buf, 42);
						 SetZuobiao(10, 400 + 40);
						 lcd_printf_new("LCD_positive_direction");
						 SCREEN_CONTROL133_1;
				     SCREEN_CONTROL134_0;
				      screen_reverse_bit=1;
						  test_e =  LCD_positive_wait;
					 }
				   break;
			case LCD_positive_wait:
				    if(touch_send_imm==0)
						{
							Flag_int = 0;
							test_e =LCD_positive_direction ;
						}
				   break;
			case LCD_positive_direction:
				    if(Flag_int==1)
					 {
						 Flag_int = 0;
						 FT5x06_RD_Reg(0, buf, 42);
						 test_e =  pre_Touch_middle;
					 }
				   break;
			case  pre_Touch_middle:
				    if(touch_send_imm==0)
					 {
						 test_e = Touch_middle;
					 }
				    break;
			case Touch_middle:
				    memset((void *)(display_layer_sdram.LCD_CACHE_BUFFER&(~0x80000000)),0xff,800*480*2);
				     x=378;
			       y=215;
			       width = 25;
			       height = 25;
				   	_ClipEnable=TRUE;
	          _ClipTL=x+(y<<16);
	          _ClipBR=x+width+((y+height)<<16);
	          ge2dSpriteBlt_Screen(x,y,width, height,(uint8_t *)(bmp_layer1_BUFFER ), (uint8_t *)(bmp_LCD_BUFFER));
            _ClipEnable = FALSE;
			      test_e = Touch_middle_re;
			      
							//touch_dev_poll();
						
						
				    break;
			case Touch_middle_re:
				   	for(Tp_i=0;Tp_i<10;Tp_i++)
							{
//								sysprintf("gs_tpInfo[Tp_i].x=%d,gs_tpInfo[Tp_i].y=%d",gs_tpInfo[Tp_i].x,gs_tpInfo[Tp_i].y);
								if((gs_tpInfo[Tp_i].x>378)&&(gs_tpInfo[Tp_i].x<425)&&(gs_tpInfo[Tp_i].y>215)&&(gs_tpInfo[Tp_i].y<265))
								{
									test_e = Touch_Left_Up;
									sysprintf("go to left up\r\n");
									break;
								}
							}
			     break;
			case Touch_Left_Up:
				  // Clear_sdram(0x40);
			    memset((void *)(display_layer_sdram.LCD_CACHE_BUFFER&(~0x80000000)),0xff,800*480*2);
				   x=78;
			       y=35;
			       width = 25;
			       height = 25;
				   	_ClipEnable=TRUE;
	          _ClipTL=x+(y<<16);
	          _ClipBR=x+width+((y+height)<<16);
	          ge2dSpriteBlt_Screen(x,y,width, height,(uint8_t *)(bmp_layer1_BUFFER ), (uint8_t *)(bmp_LCD_BUFFER));
            _ClipEnable = FALSE;
			      test_e = Touch_Left_Up_re;
			     // while(1)
						//{
							//touch_dev_poll();
							
						//}
				  break;
			case Touch_Left_Up_re:
				  for(Tp_i=0;Tp_i<10;Tp_i++)
							{
								if((gs_tpInfo[Tp_i].x>78)&&(gs_tpInfo[Tp_i].x<125)&&(gs_tpInfo[Tp_i].y>15)&&(gs_tpInfo[Tp_i].y<65))
								{
									test_e = Touch_Right_Up;
									break;
								}
							}
         	break;			
			case Touch_Right_Up:
				//Clear_sdram(0x40);
			    memset((void *)(display_layer_sdram.LCD_CACHE_BUFFER&(~0x80000000)),0xff,800*480*2);
				   x=678;
			       y=35;
			       width = 25;
			       height = 25;
				   	_ClipEnable=TRUE;
	          _ClipTL=x+(y<<16);
	          _ClipBR=x+width+((y+height)<<16);
	          ge2dSpriteBlt_Screen(x,y,width, height,(uint8_t *)(bmp_layer1_BUFFER ), (uint8_t *)(bmp_LCD_BUFFER));
            _ClipEnable = FALSE;
			      test_e = Touch_Right_Up_re;
			      //while(1)
						//{
							//touch_dev_poll();
							
						//}
				  break;
				case Touch_Right_Up_re:
					   for(Tp_i=0;Tp_i<10;Tp_i++)
							{
								if((gs_tpInfo[Tp_i].x>678)&&(gs_tpInfo[Tp_i].x<725)&&(gs_tpInfo[Tp_i].y>15)&&(gs_tpInfo[Tp_i].y<65))
								{
									test_e = Touch_Left_Down;
									break;
								}
							}
					  break;
				case Touch_Left_Down:
					  //Clear_sdram(0x40);
				   memset((void *)(display_layer_sdram.LCD_CACHE_BUFFER&(~0x80000000)),0xff,800*480*2);
				   x=78;
			       y=415;
			       width = 25;
			       height = 25;
				   	_ClipEnable=TRUE;
	          _ClipTL=x+(y<<16);
	          _ClipBR=x+width+((y+height)<<16);
	          ge2dSpriteBlt_Screen(x,y,width, height,(uint8_t *)(bmp_layer1_BUFFER ), (uint8_t *)(bmp_LCD_BUFFER));
            _ClipEnable = FALSE;
				   test_e =Touch_Left_Down_re ;
			     // while(1)
						//{
					//		touch_dev_poll();
						
						//}
				  break;	
				case Touch_Left_Down_re:
						for(Tp_i=0;Tp_i<10;Tp_i++)
							{
								if((gs_tpInfo[Tp_i].x>78)&&(gs_tpInfo[Tp_i].x<125)&&(gs_tpInfo[Tp_i].y>415)&&(gs_tpInfo[Tp_i].y<465))
								{
									test_e = Touch_Right_Down;
									break;
								}
							}
					break;
					case Touch_Right_Down:
						//Clear_sdram(0x40);
					  memset((void *)(display_layer_sdram.LCD_CACHE_BUFFER&(~0x80000000)),0xff,800*480*2);
				   x=678;
			       y=415;
			       width = 25;
			       height = 25;
				   	_ClipEnable=TRUE;
	          _ClipTL=x+(y<<16);
	          _ClipBR=x+width+((y+height)<<16);
	          ge2dSpriteBlt_Screen(x,y,width, height,(uint8_t *)(bmp_layer1_BUFFER ), (uint8_t *)(bmp_LCD_BUFFER));
            _ClipEnable = FALSE;
					  test_e = Touch_Right_Down_re;
					case Touch_Right_Down_re:
							for(Tp_i=0;Tp_i<10;Tp_i++)
							{
								if((gs_tpInfo[Tp_i].x>678)&&(gs_tpInfo[Tp_i].x<725)&&(gs_tpInfo[Tp_i].y>415)&&(gs_tpInfo[Tp_i].y<465))
								{
									test_e = AD_BAT_INIT;
									break;
								}
							}
						 break;
			case 	AD_BAT_INIT:
				 memset((void *)(display_layer_sdram.LCD_CACHE_BUFFER&(~0x80000000)),0xff,800*480*2);
			   if(touch_send_imm==0)
			    {
				    AD_init_vbat();
						test_e = AD_BAT;
			    }
				  // test_e = AD_BAT;
					 break;
			case AD_BAT_wait:
				   if(Flag_int==1)
					 {
						 Flag_int = 0;
						 FT5x06_RD_Reg(0, buf, 42);
						 test_e = AD_BAT_wait1;
					 }
           break;		
			case 	AD_BAT_wait1:
				    if(touch_send_imm==0)
			    {
						test_e = AD_BAT;
					}
           break;				
			case AD_BAT:
				      Tp_advalue = get_vbat_ad_value();
			        Tp_advalue = Tp_advalue*64/3;
							Tp_advalue = Tp_advalue*2.5;
    					Tp_advalue = Tp_advalue/4095;		 
    					Tp_advalue = 	(Tp_advalue*10)/2.5;	
			        SetZuobiao(10, 400 + 40);
  						 lcd_printf_new("                  ");
               lcd_printf_new("VBAT=%fV",Tp_advalue);
			        delay_ms(2000);
			
////						REG_OPERATE(REG_ADC_CTL,ADC_CTL_MST,set);
////				    while((inpw(REG_ADC_ISR)&ADC_ISR_MF)==0);
////		       
////		          outpw(REG_ADC_ISR,ADC_ISR_MF);
////			       delay_ms(2000);
////		      
////		       if(inpw(REG_ADC_ISR)&ADC_ISR_VBF)
////		      {
////			        Tp_data_vbat =inpw(REG_ADC_VBADATA) ;
////			      
////			        outpw(REG_ADC_ISR,ADC_ISR_VBF);
////			         sysprintf("Tp_data_vbat=%x\n\r",Tp_data_vbat);
////				
////		       }
////					
////					 

////					Tp_advalue = Tp_data_vbat*2.5;
////					Tp_advalue = Tp_advalue/4095;		 
////					Tp_advalue = 	(Tp_advalue*10)/2.5;	 
////					 SetZuobiao(10, 400 + 40);
////						 lcd_printf_new("                  ");
////						 lcd_printf_new("VBAT=%fV",Tp_advalue);
						if(Flag_int==1)
					 {
						 Flag_int = 0;
						 FT5x06_RD_Reg(0, buf, 42);
						 test_e = AD_8V_INIT;
					 }						 
				   break;
				 
		  case  AD_8V_INIT:
				   if(touch_send_imm==0)
			      {
							AD_init_8V();
							
							test_e = AD_8V;
					  }
				    break;
			case AD_8V_wait:
				   if(Flag_int==1)
					 {
						 Flag_int = 0;
						 FT5x06_RD_Reg(0, buf, 42);
						 test_e = AD_8V_wait1;
					 }
				    break;
			case AD_8V_wait1:
				   if(touch_send_imm==0)
			      {
							test_e = AD_8V;
						}
           break;				
      case 	AD_8V:
				  if(touch_send_imm==0)
					{
						
//				    REG_OPERATE(REG_ADC_CTL,ADC_CTL_MST,set);
//				    while((inpw(REG_ADC_ISR)&ADC_ISR_MF)==0);
//		       
//		          outpw(REG_ADC_ISR,ADC_ISR_MF);
//		         delay_ms(2000);
//		      	REG_OPERATE(REG_ADC_CONF,ADC_CONF_NACEN,set);
//			
//			     if(inpw(REG_ADC_ISR)&ADC_ISR_NACF)
//	           {
//			       Tp_data_normal =inpw(REG_ADC_DATA);
//			        outpw(REG_ADC_ISR,ADC_ISR_NACF);
//			        sysprintf("Tp_data_normal1=%x\n\r",Tp_data_normal);
//						
//						 }
//				   
//						 
//						 Tp_advalue = Tp_data_normal*3.3;
//					Tp_advalue = Tp_advalue/4095;		 
//					Tp_advalue = 	(Tp_advalue*133)/33;	 
						Tp_advalue = get_main_pwr_ad_value();
             Tp_advalue = Tp_advalue*0.003247;
  					//Tp_advalue = Tp_advalue/4095;		 
  					//Tp_advalue = 	(Tp_advalue*100)/33;	
					 SetZuobiao(10, 400 + 40);
						 lcd_printf_new("                  ");
						 lcd_printf_new("VEXT=%fV",Tp_advalue);
						 delay_ms(2000);
						if(Flag_int==1)
					 {
						 Flag_int = 0;
						 FT5x06_RD_Reg(0, buf, 42 );
						  power_int_disable();
			      SetZuobiao(10, 400 + 40);
						lcd_printf_new("                  ");
						lcd_printf_new("U9 OUTPUT LOW,6V INPUT");
						 test_e = POWER_8V_LEVEL_LOW_wait;
					 }	
				 }					 
           break;
      case AD_33V:
				   
				   test_e = POWER_8V_LEVEL_LOW_wait;
           break;	
			case POWER_8V_LEVEL_LOW_wait:
				    if(touch_send_imm==0)
			      {
							test_e = POWER_8V_LEVEL_LOW;
						}
				   break;
			case POWER_8V_LEVEL_HIGH_wait:
				 if(touch_send_imm==0)
			      {
							 SetZuobiao(10, 400 + 40);
						lcd_printf_new("                  ");
						lcd_printf_new("U9 OUTPUT HIGH,9V INPUT");
							test_e = POWER_8V_LEVEL_HIGH;
						}
      case POWER_8V_LEVEL_HIGH:
				   
				   power_checkon();
			     TEMP_COUNT = 0;
			     while(TEMP_COUNT)
					 {
			      TEMP_COUNT++;
						 delay_ms(1);
			       if(GPIO_ReadBit(GPIOH,BIT5)==0)
					   {
						 SetZuobiao(10, 400 + 40);
						  lcd_printf_new("                  ");
						  lcd_printf_new("U9 ERR");
						 while(1);
					  }
				   }
					 power_int_init();
					 delay_ms(3000);
					  //SetZuobiao(10, 400 + 40);
						//lcd_printf_new("                  ");
						//lcd_printf_new("U9 OUTPUT LOW,5V INPUT");
				   test_e = Sdram_check;
           break;	
      case 	POWER_8V_LEVEL_LOW:
				   power_checkoff();
			     TEMP_COUNT = 0;
			     while(TEMP_COUNT)
					 {
			      TEMP_COUNT++;
						 delay_ms(1);
			       if(GPIO_ReadBit(GPIOH,BIT5)==0)
					   {
						 SetZuobiao(10, 400 + 40);
						  lcd_printf_new("                  ");
						  lcd_printf_new("U9 ERR");
						 while(1);
					  }
				   }
					
					 test_e = POWER_8V_LEVEL_HIGH_wait;           
					 break;				
			case Sdram_check:
				   if(touch_send_imm==0)
					 {
				   SetZuobiao(10, 400 + 40);
				  
			      sdram_total = 5*800*480*2;
			     	memset((void *)display_layer_sdram.LCD_FRAME_BUFFER,0x00,800*480*2);
	          memset((void *)display_layer_sdram.LCD_FRAME1_BUFFER,0x55,800*480*2);
	          memset((void *)display_layer_sdram.LCD_FRAME2_BUFFER,0xaa,800*480*2);
	          memset((void *)display_layer_sdram.LCD_FRAME3_BUFFER,0x77,800*480*2);
			      memset((void *)display_layer_sdram.LCD_CACHE_BUFFER,0x11,800*480*2);
						 
						
						 
			      for(Tp_i=0;Tp_i<800*480*2;Tp_i++)
			     {
						 if(*(uint8_t *)(display_layer_sdram.LCD_FRAME_BUFFER+Tp_i)!=0x00)
						 {
						   SetZuobiao(10, 400 + 40); 
							 lcd_printf_new("                       ");
				        lcd_printf_new("sdram err,err addr=0x%x,LCD_FRAME_BUFFER\r\n",display_layer_sdram.LCD_FRAME_BUFFER);
							 while(1);
							 
						 }
						  if(*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+Tp_i)!=0x55)
						 {
						   SetZuobiao(10, 400 + 40); 
							 lcd_printf_new("                       ");
				        lcd_printf_new("sdram err,err addr=0x%x,LCD_FRAME1_BUFFER\r\n",display_layer_sdram.LCD_FRAME1_BUFFER);
							 while(1);
							 
						 }
						   if(*(uint8_t *)(display_layer_sdram.LCD_FRAME2_BUFFER+Tp_i)!=0xaa)
						 {
						   SetZuobiao(10, 400 + 40); 
							 lcd_printf_new("                       ");
				        lcd_printf_new("sdram err,err addr=0x%x,LCD_FRAME2_BUFFER\r\n",display_layer_sdram.LCD_FRAME2_BUFFER);
							 while(1);
							 
						 }
						   if(*(uint8_t *)(display_layer_sdram.LCD_FRAME3_BUFFER+Tp_i)!=0x77)
						 {
               SetZuobiao(10, 400 + 40);						   
							 lcd_printf_new("                       ");
				        lcd_printf_new("sdram err,err addr=0x%x,LCD_FRAME3_BUFFER\r\n",display_layer_sdram.LCD_FRAME3_BUFFER);
							 while(1);
							 
						 }
						   if(*(uint8_t *)(display_layer_sdram.LCD_CACHE_BUFFER+Tp_i)!=0x11)
						 {
						   SetZuobiao(10, 400 + 40); 
							 lcd_printf_new("                       ");
				        lcd_printf_new("sdram err,err addr=0x%x,LCD_CACHE_BUFFER\r\n",display_layer_sdram.LCD_CACHE_BUFFER);
							 while(1);
							 
						 }
					 }
					   lcd_printf_new("                       ");
				   lcd_printf_new("sdram test");
			     while(1)
					 {
						 sdram_new_addr = (uint32_t )malloc((800 * 480 * 2)+64);
						 #ifdef  SYSUARTPRINTF 
						 sysprintf("sdram_new_addr=0x%x\r\n",sdram_new_addr);
						 #endif
						 if(Q_Num<4) Q_ADDR[Q_Num]=sdram_new_addr;
						 Q_Num++;
						 //sdram_new_addr = 32+shift_pointer((uint32_t)sdram_new_addr,32);
						 //sysprintf("sdram_new_addr=%X\r\n",sdram_new_addr);
						 if(sdram_new_addr==NULL)
						 {
							 SetZuobiao(10, 400 + 40); 
							 lcd_printf_new("                       ");
				        lcd_printf_new("sdram pass, size = 0x%x",sdram_total);
							 for(Q_Num=0;Q_Num<4;Q_Num++)
							 {
								 free((void *)(Q_ADDR[Q_Num]));
							 }
							  delay_ms(5000);
							 test_e = Nand_check;
//							  while(1)
//								{
//									Flag_int = 0;
//									if(Flag_int==1)
//									{
//										Flag_int = 0;
//										FT5x06_RD_Reg(0, buf, 42);
//										test_e = Nand_check;
//										break;
//									}
//								}
								break;
						 }
						 else
						 {
							 sdram_new_addr = 32+shift_pointer(sdram_new_addr,32);
							 sdram_new_addr = sdram_new_addr|0x80000000;
							 memset((void *)sdram_new_addr,0x55,800*480*2);
							  for(Tp_i=0;Tp_i<800*480*2;Tp_i++)
							 {
							   if(*(uint8_t *)(sdram_new_addr+Tp_i)!=0x55)
							   {
								    SetZuobiao(10, 400 + 40);  
									 lcd_printf_new("                       ");
				             lcd_printf_new("sdram err,err_addr=0x%x,sdram_total=0x%x",sdram_new_addr,sdram_total);
							       while(1);
							 
							   }
						   }
							 sdram_total = sdram_total + 800*480*2;
							// free((void *)sdram_new_addr);
						 }
					 }
				   }
				   break;
			case Nand_check:
				 if(touch_send_imm==0)
					 {
			SetZuobiao(10, 400 + 40);	    
			lcd_printf_new("                                               ");
				    lcd_printf_new("test nandflash");
				   TEST_NANDFLASH();
						 
						 SetZuobiao(10, 400 + 40);	    
			      lcd_printf_new("                                               ");
				    lcd_printf_new("nandflash pass,bad = %d",Tp_bad);
           if(Tp_bad>10) 
					 {
						 lcd_printf_new("nandflash err,bad = %d",Tp_bad);
						 while(1);
					 }
						 delay_ms(3000);
						 test_e = W25Q128_check;//Nand_check;//Updata_image;
//			     Flag_int = 0;
//			      while(1)
//								{
//									if(Flag_int==1)
//									{
//										Flag_int = 0;
//										FT5x06_RD_Reg(0, buf, 42);
//										test_e = Updata_image;
//										break;
//									}
//								}
						}
				   break;
			case W25Q128_check:
				    Tp_i=0;
				    while(1)
						{
							
				    memset((void *)(BaseData_ARR),Tp_i,164*9);
			      W25Q128_Write();
							memset((void *)(BaseData_ARR),0,164*9);
							W25Q128_Read();
							if((*(uint8_t *)(BaseData_ARR+3*9)!=Tp_i)||
								(*(uint8_t *)(BaseData_ARR+4*9)!=Tp_i)||
							   (*(uint8_t *)(BaseData_ARR+12*9)!=Tp_i)||
							   (*(uint8_t *)(BaseData_ARR+13*9)!=Tp_i))
							{
								SetZuobiao(10, 400 + 40);  
				         lcd_printf_new("W25Q128 err");
								#ifdef  SYSUARTPRINTF 
								sysprintf("W25Q128 err=%X\r\n",Tp_i);
								#endif
								 while(1);
							}
							else
							{
								SetZuobiao(10, 400 + 40);  
				         lcd_printf_new("W25Q128 ok");
								#ifdef  SYSUARTPRINTF 
								sysprintf("W25Q128 ok=%X\r\n",Tp_i);
								#endif
							}
							Tp_i = (Tp_i+1)%256;
							if(Tp_i == 0) break;
							
				    }
				    test_e = Updata_image;
				   break;
			case Updata_image:
				if(touch_send_imm==0)
				{
				  SetZuobiao(10, 400 + 40);     
						lcd_printf_new("                                                                         ");
					SetZuobiao(10, 400 + 40);  
				         lcd_printf_new("Write image FF00 FF01 FF02");
					badmanage_str->BAD_MANAGE_str.flag=0;
					NANDFLASH_BADMANAGE_INIT();
					usb_init();
					while(gs_usb_mount_flag==0)//U?????????
	        {
		        
						SetZuobiao(10, 400 + 40);     
						lcd_printf_new("                       ");
						SetZuobiao(10, 400 + 40);  
				         lcd_printf_new("USB NO LOAD");
						delay_ms(3000);
						usbh_pooling_hubs();
						break;
	        }
					//SDRAM_DATA_INIT();
						CHECK_SUM_USB = 0;
			 CHECK_SUM_NAND = 0;
				USB_IMAGE_TYPE='A';
				
			 badmanage_str->BAD_MANAGE_str.NANDFLASH_CUSTOMER_INX = (uint32_t)(image_user_nandflash_start*2048);
					UsbWriteNandFlash(U2N_PA, NULL, 0);
					if(IMAGE_SEARCHED==1)
					{
					//  ppt_find_file_and_Q(N2S_Q0);
						DisPlayLayer(LCD_G2, 0, 0);
						Flag_int = 0;
						test_e = Display_FF00;
					}
					else
					{
						SetZuobiao(10, 400 + 40);  
						lcd_printf_new("                       ");
				         lcd_printf_new("USB NO BMP ");
						delay_ms(3000);
						
					}
				}
//				   while(1)
//					 {
//						 //usbh_pooling_hubs();
//						 
//					 }
				   break;
			case Display_FF00:
				if(touch_send_imm==0)
				{
				  if(FLAG_DISPLAY==0)
					{
					 //DisPlayLayer(LCD_G0,1,9);
							#ifdef  SYSUARTPRINTF
					 sysprintf("\r\nFF00 DISPLAY\r\n");
						#endif
					 ///sprintf((char *)UART2_RX_BUFF,"%S",(char *)"@0ED00000001FF00*");
						memcpy((unsigned char *)UART2_RX_BUFF,(unsigned char *)"@0ED00000001FF00*",17);
						 #ifdef  SYSUARTPRINTF
					 sysprintf("\r\nUART2_RX_BUFF=%d\r\n",UART2_RX_BUFF[0]);
						 sysprintf("\r\nUART2_RX_BUFF=%d\r\n",UART2_RX_BUFF[1]);
						sysprintf("\r\nUART2_RX_BUFF addr=0x%x\r\n",UART2_RX_BUFF);
						#endif
			     RX_COUNT_OUT = 0;   
			     RX_COUNT_IN =  17;
					 FLAG_DISPLAY = 1;
						test_e = Display_FF00_WAIT;
					}
//					DisPlayPptBmp(0xff00);
//					bmp_protocol[0].num = 1;
//					bmp_protocol[0].bmp->layer = 1;
//					bmp_protocol[0].bmp->name = 0XFF00;
//					bmp_protocol[0].bmp->x = 0;
//					bmp_protocol[0].bmp->y = 0;
//					bmp_protocol[1].num = 0;
//					bmp_protocol[2].num = 0;
//					if(BMP_judge(1,&bmp_protocol[0].bmp[0])!=1)
//					{
//							SetZuobiao(10, 400 + 40);  
//						lcd_printf_new("                       ");
//				         lcd_printf_new("CAN'T FOUND BMP FF00");
//						while(1);
//					}
//					 DisCmdDisplay(LCD_D0,bmp_protocol[0].bmp,bmp_protocol[0].num,bmp_protocol[1].bmp,bmp_protocol[1].num,bmp_protocol[2].bmp,bmp_protocol[2].num);
			     //Flag_int = 0;
			     // while(1)
						//		{
									
					//			}
				}
				break;
			case Display_FF00_WAIT:
				
				sysprintf("\r\nRX_COUNT_OUT=%d\r\n",RX_COUNT_OUT);
				if(RX_COUNT_OUT==17)
				{
					Flag_int = 0;
					while(1)		
					{						
					   if(Flag_int==1)  
						 {							 
						        FLAG_DISPLAY = 0;
					          Flag_int = 0;
					          RX_COUNT_OUT = 0;
										FT5x06_RD_Reg(0, buf, 42);
										test_e = Display_FF01;
										break;
						 }
					}
				}
				   break;
			case Display_FF01:
				if(touch_send_imm==0)
				{
				  if(FLAG_DISPLAY==0)
					{
					 sysprintf("\r\nFF01 DISPLAY\r\n");
					 sprintf((char *)UART2_RX_BUFF,"%S","@0ED00000001FF01*");
			     RX_COUNT_OUT = 0;   
			     RX_COUNT_IN =  17;
					 FLAG_DISPLAY = 1;
						test_e = Display_FF01_WAIT;
					}
//					DisPlayPptBmp(0xff00);
//					bmp_protocol[0].num = 1;
//					bmp_protocol[0].bmp->layer = 1;
//					bmp_protocol[0].bmp->name = 0XFF00;
//					bmp_protocol[0].bmp->x = 0;
//					bmp_protocol[0].bmp->y = 0;
//					bmp_protocol[1].num = 0;
//					bmp_protocol[2].num = 0;
//					if(BMP_judge(1,&bmp_protocol[0].bmp[0])!=1)
//					{
//							SetZuobiao(10, 400 + 40);  
//						lcd_printf_new("                       ");
//				         lcd_printf_new("CAN'T FOUND BMP FF00");
//						while(1);
//					}
//					 DisCmdDisplay(LCD_D0,bmp_protocol[0].bmp,bmp_protocol[0].num,bmp_protocol[1].bmp,bmp_protocol[1].num,bmp_protocol[2].bmp,bmp_protocol[2].num);
			     //Flag_int = 0;
			     // while(1)
						//		{
									
					//			}
				}
				break;
			case Display_FF01_WAIT:
				if(RX_COUNT_OUT==17)
				{
					Flag_int = 0;
					while(1)		
					{						
					   if(Flag_int==1)  
						 {							 
						        FLAG_DISPLAY = 0;
					          Flag_int = 0;
					          RX_COUNT_OUT = 0;
										FT5x06_RD_Reg(0, buf, 42);
										test_e = Display_FF02;
										break;
						 }
					}
				}
				   break;
		   case Display_FF02:
				 
					if(touch_send_imm==0)
				{
				  if(FLAG_DISPLAY==0)
					{
					 sysprintf("\r\nFF02 DISPLAY\r\n");
					 sprintf((char *)UART2_RX_BUFF,"%S","@0ED00000001FF02*");
			     RX_COUNT_OUT = 0;   
			     RX_COUNT_IN =  17;
					 FLAG_DISPLAY = 1;
					test_e = Display_FF02_WAIT;
					}
//					DisPlayPptBmp(0xff00);
//					bmp_protocol[0].num = 1;
//					bmp_protocol[0].bmp->layer = 1;
//					bmp_protocol[0].bmp->name = 0XFF00;
//					bmp_protocol[0].bmp->x = 0;
//					bmp_protocol[0].bmp->y = 0;
//					bmp_protocol[1].num = 0;
//					bmp_protocol[2].num = 0;
//					if(BMP_judge(1,&bmp_protocol[0].bmp[0])!=1)
//					{
//							SetZuobiao(10, 400 + 40);  
//						lcd_printf_new("                       ");
//				         lcd_printf_new("CAN'T FOUND BMP FF00");
//						while(1);
//					}
//					 DisCmdDisplay(LCD_D0,bmp_protocol[0].bmp,bmp_protocol[0].num,bmp_protocol[1].bmp,bmp_protocol[1].num,bmp_protocol[2].bmp,bmp_protocol[2].num);
			     //Flag_int = 0;
			     // while(1)
						//		{
									
					//			}
				}
				break;
			 case Display_FF02_WAIT:
				if(RX_COUNT_OUT==17)
				{
					Flag_int = 0;
					while(1)		
					{						
					   if(Flag_int==1)  
						 {							 
						        FLAG_DISPLAY = 0;
					          Flag_int = 0;
					          RX_COUNT_OUT = 0;
										FT5x06_RD_Reg(0, buf, 42);
										test_e = Voice_out;
										break;
						 }
					}
				}
				   break;
			 case Voice_out:
				 if(touch_send_imm==0)
				 {
				 sysprintf("\r\nVOICE OUT\r\n");
					 SetZuobiao(10, 400 + 40);  
						lcd_printf_new("                       ");
					 SetZuobiao(10, 400 + 40); 
				         lcd_printf_new("VOICE OUT");
					 
					 tts.file = 9;
					 tts.voice = 0XF;
					 tts.interval=1000;
					 tts.play_num = 0xa;
					  tts.interval_cnt=0; 
					 tts.ack_flag = 1;
					 // sprintf((char *)UART2_RX_BUFF,"%S","@07BAF905*");
//					 RX_COUNT_IN=  0;
//					 UART2_RX_BUFF[RX_COUNT_IN++]='@';
//					 UART2_RX_BUFF[RX_COUNT_IN++]='0';
//					 UART2_RX_BUFF[RX_COUNT_IN++]='7';
//					 UART2_RX_BUFF[RX_COUNT_IN++]='B';
//					 UART2_RX_BUFF[RX_COUNT_IN++]='A';
//					 UART2_RX_BUFF[RX_COUNT_IN++]='F';
//					 UART2_RX_BUFF[RX_COUNT_IN++]='9';
//					 UART2_RX_BUFF[RX_COUNT_IN++]='0';
//					 UART2_RX_BUFF[RX_COUNT_IN++]='5';
//					 UART2_RX_BUFF[RX_COUNT_IN++]='*';
//					 // lcd_printf_new("VOICE OUT");
//			     RX_COUNT_OUT = 0;   
			     //RX_COUNT_IN =  10;
					 test_e = Voice_out_wait;
				 }
					// FLAG_DISPLAY = 1;
//				  send_wt588h_init(0xFFE0|9);
//		      delay_ms(50);
//		      send_wt588h_init(3);
//		      delay_ms(500);
				 break;
			 case Voice_out_wait:
				 //if(RX_COUNT_OUT==10)
				 //{
			   //  Flag_int = 0;
			    //  while(1)
			      if(wt588h_send_step == 0)
						{
									if(Flag_int==1)
									{
										Flag_int = 0;
										FT5x06_RD_Reg(0, buf, 42);
										test_e = Voice_stop;
										//sysprintf("\r\nVOICE OUT\r\n");
										//break;
									}
						}
					//}
				 
				  break;
				case Voice_stop:
				 if(touch_send_imm==0)
				 {
				 sysprintf("\r\nVOICE STOP\r\n");
					  SetZuobiao(10, 400 + 40);  
						lcd_printf_new("                       ");
					 SetZuobiao(10, 400 + 40); 
				         lcd_printf_new("VOICE STOP");
					 // sprintf((char *)UART2_RX_BUFF,"%S","@07BF09FF*");
					 	tts.play_num=0;
					//
					stop_tts_play( );
				//	while(STOP_FLAG_CYW);
						
			    // RX_COUNT_IN =  10;
					 test_e = Voice_stop_wait;
				 }
					// FLAG_DISPLAY = 1;
//				  send_wt588h_init(0xFFE0|9);
//		      delay_ms(50);
//		      send_wt588h_init(3);
//		      delay_ms(500);
				 break;
				 case Voice_stop_wait:
				 //if(RX_COUNT_OUT==10)
				// {
			   //  Flag_int = 0;
			   //   while(1)
								{
									if(Flag_int==1)
									{
										Flag_int = 0;
										FT5x06_RD_Reg(0, buf, 42);
										test_e = RTC_check;
										break;
									}
								}
					//}
				 
				  break;
			 case RTC_check:
				  if(touch_send_imm==0)
				 {
				 sysprintf("\r\nRTC CLOCK OUT\r\n");
			  SetZuobiao(10, 400 + 40);  
						lcd_printf_new("                       ");
					 SetZuobiao(10, 400 + 40); 
				         lcd_printf_new("RTC CLOCK OUT");
			       RTC_CLKOUT();
					 test_e = RTC_check_wait;
				 }
				// RTC_Read(RTC_CURRENT_TIME,&time_T);
			 //while(1);
				  break;
			 case RTC_check_wait:
				  	if(Flag_int==1)
							{
										Flag_int = 0;
										FT5x06_RD_Reg(0, buf, 42);
								    RTC_CLKSTOP();
										test_e = powersave;
										
							}
				 break;
			 case powersave:
				  if(touch_send_imm==0)
				 {
				  sysprintf("\r\npower save\r\n");
			  SetZuobiao(10, 400 + 40);  
						lcd_printf_new("                       ");
					 SetZuobiao(10, 400 + 40); 
				         lcd_printf_new("POWERSAVE");
					 test_e =  powersave_wait;
			   }
				 break;
			 case powersave_wait:
				 if(Flag_int==1)
							{
										Flag_int = 0;
										FT5x06_RD_Reg(0, buf, 42);
								    //RTC_CLKSTOP();
										//test_e = powersave;
								    test_e =   powersave_end;
								    
										
							}
				 break;
			 case powersave_end:
				 power_save();
			  REG_OPERATE(REG_CLK_PCLKEN0,1<<1,set);//ENABLE WWDT
				WWDT_Open(WWDT_PRESCALER_2048,0x3f,TRUE);
				 break;
			 case download_pin_test:
				 download_gpio_set_output();
			 while(1)
			 {
			 DOWNLOAD_TOOL_CLR();
			 DOWNLOAD_RESET_CLR();
			//////// POWER_5V_OFF();
			   	delay_us1(23000);
			//////    POWER_5V_ON();
			 	delay_us1(23000);
			 DOWNLOAD_TOOL_SET();
			 	delay_us1(23000);
			  DOWNLOAD_RESET_SET();
			 	delay_us1(23000);
			  DOWNLOAD_RESET_CLR();
			 	delay_us1(23000);
			 DOWNLOAD_TOOL_CLR();
			 	delay_us1(23000);
			/////////// POWER_5V_OFF();
			 }
			// while(1);
				 break;
			default:break;
		
	}
	
}
