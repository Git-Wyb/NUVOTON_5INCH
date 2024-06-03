#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "sys.h"
#include "BSP_Init.h"
#include "gpio.h"
#include "DIP_SW.h"
#include "ff.h"
#include "Command_All.h"
#include "display.h"


extern int gs_usb_mount_flag;
uint8_t LOGO_DATA_OUT_FLAG=0;
uint8_t FLAG_SW_FINISH=0;//这个是由SW触发的升级要一直显示
extern  uint32_t CHECK_SUM_USB,CHECK_SUM_NAND;
extern char  USB_IMAGE_TYPE;
extern uint16_t FLAG_SW3_BEGIN;
//extern uint32_t NANDFLASH_CUSTOMER_INX;
extern CopyFileInfo gs_CpyInfo ;
extern FATFS  usb_fatfs;
FRESULT res;
extern  BADMANAGE_TAB_TYPE_U badmanage_str[1];
STATE_TYPE state;
DOWNLOAD_TYPE  download={0};
uint8_t screen_reverse_bit;
extern uint8_t updata_hex_fromSW2;
extern Download_ERR_U Download_code;
extern uint8_t TYPE_PRODUCT;


void POWER_5V_SETTING_initover(void)
{
	uint8_t Tp_key1;
	Tp_key1 = READ_PIN_SW1_1;
	if(Tp_key1==0)//ON ????
	{
		//POWER_5V_Init();
	  POWER_5V_ON();
//		delay_1us(500);
//		DOWNLOAD_RESET_SET();
//		delay_1us(500);
	
		
	}
	
}


//开机时扫苗一次5V是否需要供电
void POWER_5V_SETTING_initstart(void)
{
	uint8_t Tp_key1;
	Tp_key1 = READ_PIN_SW1_1;
	if(Tp_key1!=0)//OFF不待机
	{
		//POWER_5V_Init();
	  POWER_5V_ON();
		sysprintf("+5v POWERON once\r\n");
	//	delay_1us(500);
		//DOWNLOAD_RESET_SET();
	//	delay_1us(500);
	
		
	}
}


void check_sw5(void)
{
	static uint8_t sw5_cnt=0;
	
      if(sw5_cnt==0)  //开机只检测一次
      	{
		sw5_cnt=1;
		if(READ_PIN_SW5==SW_OFF)
			{                 
			   SCREEN_CONTROL133_0;
			   SCREEN_CONTROL134_1;
			   screen_reverse_bit=0;
			}
		else
			{
				SCREEN_CONTROL133_1;
				SCREEN_CONTROL134_0;
				screen_reverse_bit=1;
			}
      	}	
}


void check_sw234(void) //100ms run one 
{
	static uint8_t sw1_4_cnt=0;
	static uint8_t sw1_3_cnt=0;
	static uint8_t sw1_2_cnt=0;
	static uint8_t sw1_6_cnt=0;
//	static uint8_t sw1_1_cnt=0;

	//if(pwr_on_cnt)//没加有些执行不到
	//	{
  if((READ_PIN_SW1_3==SW_ON)&&(READ_PIN_SW1_6==SW_ON))
	 {
		 SetZuobiao(10, 400 + 60);
		 lcd_printf_new("ERROR: Incorrect DIP-SW settings");
		 return;
	 }

		if((READ_PIN_SW1_2==SW_ON)||(READ_PIN_SW1_3==SW_ON)||(READ_PIN_SW1_4==SW_ON)||(READ_PIN_SW1_6==SW_ON))
				{
					usb_init();
					if(gs_usb_mount_flag == 1)//U盘已经加载
					{
						 SetZuobiao(10, 400);
		         lcd_printf_new("USB FOUND        ");
					}
					if(LOGO_DATA_OUT_FLAG == 3)
					{
						 SetZuobiao(10, 380);
		         lcd_printf_new("LOGO INIT FINISH     ");
					}
				}	

	//	return;
	
//}
	if(gs_usb_mount_flag!=1)//U盘已经加载
	{
		 return;
	}
	
//	if(LOGO_DATA_OUT_FLAG!=2)//LOGO数据已经导入
//	{
//		return;
//	}
	

	if(READ_PIN_SW1_4==SW_ON)
	{
		sw1_4_cnt++;		
		if(sw1_4_cnt>5)
		{
		     sw1_4_cnt=5;
			if(state.sw1_4==0)
			{
				state.sw1_4=1;
				SetZuobiao(10, 400 + 60);
				lcd_printf_new("Download BackupData");
				 if(TYPE_PRODUCT==PORDUCT_7INCH)
				 {
			     BackupDeviceData_7( );
					  BackupDeviceData_5( );
				 }
				  if(TYPE_PRODUCT==PORDUCT_5INCH)
					{
				    BackupDeviceData_5( );
					}
				//SetZuobiao(10, 400 + 60);
				//lcd_printf_new("Backup End         ");
				FLAG_SW_FINISH = FLAG_SW_FINISH|0x08;
				//gs_usb_mount_flag=0;
				//USBH_USR_DeviceDisconnected();
			}
	   	}		   
	}
      else
      	{
			
		if(sw1_4_cnt)
		{
		       sw1_4_cnt--;	
		  	if(sw1_4_cnt==0)

		  	{
			if(state.sw1_4)
			{
				//state.sw1_4=0;  //DIP2,DIP3,DIP4 ON直用一次即上电检查一次；
			}
		  	}	
		}		   
	}
//
  if(READ_PIN_SW1_3==SW_ON)
	{
		sw1_3_cnt++;		
		if(sw1_3_cnt>5)
		{
		     sw1_3_cnt=5;
			if(state.sw1_3==0)
			{
				state.sw1_3=1;
				SetZuobiao(10, 400 + 40);
				lcd_printf_new("Rewrite All Image                  ");
				FLAG_SW3_BEGIN = 0;
				
					CHECK_SUM_USB = 0;
			 CHECK_SUM_NAND = 0;
				USB_IMAGE_TYPE='0';
				
			 badmanage_str->BAD_MANAGE_str.NANDFLASH_CUSTOMER_INX = (uint32_t)(image_file_nandflash_start*2048);
				//nandflash_format_disk0();
				 UsbWriteNandFlash(U2N_P0, NULL, 0);
				SetZuobiao(10, 400 + 40);
//				 lcd_printf_new("copy bmp file end,total %d success %d failed %d",gs_CpyInfo.total,gs_CpyInfo.okCnt,gs_CpyInfo.failedCnt);               
       // lcd_printf_new("Rewrite End      ,total %d success %d failed %d",gs_CpyInfo.total,gs_CpyInfo.okCnt,gs_CpyInfo.failedCnt);
				//USBH_USR_DeviceDisconnected();
				FLAG_SW_FINISH = FLAG_SW_FINISH|0x04;
				//gs_usb_mount_flag = 0;
			}
	   	}		   
	}
  else
  {
			
		if(sw1_3_cnt)
		{
		       sw1_3_cnt--;	
		  	if(sw1_3_cnt==0)
		  	{
			if(state.sw1_3)
			{
				//state.sw1_3=0; //DIP2,DIP3,DIP4 ON直用一次即上电检查一次；
			}
		  	}	
		}		   
	}
//

	
	
	if(READ_PIN_SW1_6==SW_ON)
	{
		sw1_6_cnt++;		
		if(sw1_6_cnt>5)
		{
		     sw1_6_cnt=5;
			if(state.sw1_6==0)
			{
				state.sw1_6=1;
				SetZuobiao(10, 400 + 40);
				lcd_printf_new("Rewrite All Image                        ");
				
				if(gs_usb_mount_flag!=1)
				{
					gs_CpyInfo.total =0;
					gs_CpyInfo.okCnt=0;
					gs_CpyInfo.failedCnt =0;
					FLAG_SW_FINISH = FLAG_SW_FINISH|0x04;
					return;
				}
					//f_mount(0, "3:", 1);
				//res = f_mount(&usb_fatfs, "3:", 1); 
			
				if(check_u_disk_img( )==0)
				{
					gs_CpyInfo.total =0;
					gs_CpyInfo.okCnt=0;
					gs_CpyInfo.failedCnt =0;
					FLAG_SW_FINISH = FLAG_SW_FINISH|0x04;
					return;
				}
				
				FLAG_SW3_BEGIN = 0;
				USB_IMAGE_TYPE='0';
					CHECK_SUM_USB = 0;
			 CHECK_SUM_NAND = 0;
			 badmanage_str->BAD_MANAGE_str.NANDFLASH_CUSTOMER_INX = (uint32_t)(image_file_nandflash_start*2048);
				//nandflash_format_disk0();
				//nandflash_format();
			//	badmanage_str->flag = 0;
			//	NANDFLASH_BABMANAGE_INIT();
				nandflash_format();
				 UsbWriteNandFlash(U2N_P0, NULL, 0);
				SetZuobiao(10, 400 + 40);
//				 lcd_printf_new("copy bmp file end,total %d success %d failed %d",gs_CpyInfo.total,gs_CpyInfo.okCnt,gs_CpyInfo.failedCnt);               
       // lcd_printf_new("Rewrite End      ,total %d success %d failed %d",gs_CpyInfo.total,gs_CpyInfo.okCnt,gs_CpyInfo.failedCnt);
				//USBH_USR_DeviceDisconnected();
				FLAG_SW_FINISH = FLAG_SW_FINISH|0x04;
				//gs_usb_mount_flag = 0;
			}
	   	}		   
	}
  else
  {
			
		if(sw1_6_cnt)
		{
		       sw1_6_cnt--;	
		  	if(sw1_6_cnt==0)
		  	{
			if(state.sw1_6)
			{
				//state.sw1_3=0; //DIP2,DIP3,DIP4 ON直用一次即上电检查一次；
			}
		  	}	
		}		   
	}
	     if(READ_PIN_SW1_2==SW_ON)
	{
		sw1_2_cnt++;		
		if(sw1_2_cnt>5)
		{
		     sw1_2_cnt=5;
			if(state.sw1_2==0)
			{
				state.sw1_2=1;
				SetZuobiao(10, 400 + 20);
				lcd_printf_new("Update Software                                                    ");
				set_download_task(1); 
				updata_hex_fromSW2 = 1;
			}
	   	}		   
	}
   else
  {
			
		if(sw1_2_cnt)
		{
		       sw1_2_cnt--;	
		  	if(sw1_2_cnt==0)

		  	{
			if(state.sw1_2)
			{
				//state.sw1_2=0;  DIP2,DIP3,DIP4 ON直用一次即上电检查一次；
			}
		  	}	
		}		   
	}
}

void Fresh_lcd_dis(void)
{
	static uint8_t led_count=0;
	
	
	
	
		
	
	if((download.task==1)||(download.start == 1))
	{
		return;
	}
	
	if(FLAG_SW_FINISH & 0x01)
	{
		 SetZuobiao(10, 400 + 20);
		 lcd_printf_new("Update Success     ");
	
	}
	
	if(FLAG_SW_FINISH & 0x02)
	{
		 SetZuobiao(10, 400 + 20);
		 lcd_printf_new("Update Failure         Code=0x%02x    ",Download_code.val);
		 
		 
	}
	
	if(FLAG_SW_FINISH & 0x04)
	{
		 SetZuobiao(10, 400 + 40);
		 lcd_printf_new("copy bmp file end,total %d success %d failed %d",gs_CpyInfo.total,gs_CpyInfo.okCnt,gs_CpyInfo.failedCnt);              
     SetZuobiao(10, 360);
		if((CHECK_SUM_USB == CHECK_SUM_NAND)&&(gs_CpyInfo.total==gs_CpyInfo.okCnt)&&(gs_CpyInfo.total!=0))
		{
		 lcd_printf_new("PASS,PASS,PASS,PASS,PASS,PASS,PASS,PASS,USB=0x%08X,NAND=0x%08X",CHECK_SUM_USB,CHECK_SUM_NAND);
		
		}
		else
		{
		 lcd_printf_new("NG,NG,NG,NG,NG,NG,NG,NG,NG,NG,NG,NG,NG,USB=0x%08X,NAND=0x%08X",CHECK_SUM_USB,CHECK_SUM_NAND);	
			
		}
		
	}
	
	
	if(FLAG_SW_FINISH & 0x08)
	{
		  SetZuobiao(10, 400 + 60);
		 lcd_printf_new("Backup End         ");
	}
	
	if((FLAG_SW_FINISH==0x05)||(FLAG_SW_FINISH==0x06))
	{
		if((CHECK_SUM_USB == CHECK_SUM_NAND)&&(gs_CpyInfo.total==gs_CpyInfo.okCnt)&&(gs_CpyInfo.total!=0)&&(FLAG_SW_FINISH==5))//right
		{
			  LED_POWER_ON();
				LED_LOGO_ON();
				LED_POWERLOW_ON();
				LED_FLASHERR_ON();
		}
		else
		{
			  if((led_count%2)==1) 
			{
					LED_POWER_OFF();
				LED_LOGO_OFF();
				LED_POWERLOW_OFF();
				LED_FLASHERR_OFF();
			
			}	
			if((led_count%2)==0)
			{
				LED_POWER_ON();
				LED_LOGO_ON();
				LED_POWERLOW_ON();
				LED_FLASHERR_ON();
			}
		}
		
	}
	
	
	led_count++;
}


