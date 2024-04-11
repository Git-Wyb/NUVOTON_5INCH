
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "sys.h"
#include "BSP_Init.h"
#include "gpio.h"
#include "DIP_SW.h"
#include "display.h"
#include "Command_all.h"
#include "Poweron.h"
#include "ff.h"
#include "Aprotocol.h"
#include "nand_nand.h"


uint32_t  Flag_Tim_100ms,Flag_Tim_500ms,Flag_Tim_50ms,Flag_Tim_1ms,Flag_Tim_1S;
extern uint32_t time1ms_count;
extern uint8_t LOGO_DATA_OUT_FLAG;
void tts_time(void) ;
uint8_t pwr_on_cnt=3;
extern char  USB_IMAGE_TYPE;
extern uint8_t  USB_HAS_USABLE_IMG;
extern uint8_t IMAGE_SEARCHED ;
extern  uint16_t  led_log_on_cnt;
extern uint8_t updata_hex_fromSW2;
extern uint16_t command_D8_D9_time;
uint8_t err_code_buff[9];
static uint8_t err_code_num=0;
static  uint8_t err_code_delay=0;
extern  SYSTEMERRORSTATUS_TYPE  systerm_error_status;
extern DOWNLOAD_TYPE  download;
E_INT_STATUS tim1ms_control=int_enable;
void sysTimer0ISR();
extern volatile uint8_t command_D8_D9_flag;
extern  uint8_t *BaseData_ARR;
extern uint32_t CHECK_SUM_NAND;
uint8_t checksum_flag=0;
extern uint8_t *bmpBuf_kkk,*bmpBuf_kkk_bak;
uint32_t shift_pointer(uint32_t ptr, uint32_t align);


void Display_checksum(void)
{
		char Tp_version[9]={0};
	     if(checksum_flag == 1)
			 {

	        SetZuobiao(10, 400 + 60);
		      lcd_printf_new("NAND Chechsum = 0x%08X         ",CHECK_SUM_NAND);
					SetZuobiao(10, 400 + 20);
					memcpy(Tp_version,(char *)(BaseData_ARR+BMP_Ver_index*9),8);
					lcd_printf_new("BMP VERSION   = %8s         ",Tp_version);
					SetZuobiao(10, 400 + 40);
					memcpy(Tp_version,(char *)(BaseData_ARR+Master_Ver_index*9),8);
					lcd_printf_new("MAIN VERSION   = %8s         ",Tp_version);
					SetZuobiao(10, 400);
					memcpy(Tp_version,(char *)(BaseData_ARR+LCD_Ver_index*9),8);
					lcd_printf_new("LCD VERSION   = %8s         ",Tp_version);
			 }
}


void NAND_BMP_Read_checksum(void)
{
	uint16_t Tp_i=0;
		uint32_t Tp_addr;
	  uint32_t Tp_bmp_TAB[3]={0};
		
		#ifdef SYSUARTPRINTF_p
	 sysprintf("Read_checksum_start\r\n");
	#endif
		
	CHECK_SUM_NAND = 0;
	
	if(bmpBuf_kkk==0)
	{
	#ifdef SYSUARTPRINTF_p
	 sysprintf("bmpBuf_kkk=%08X, malloc\r\n",bmpBuf_kkk);
	 #endif	
		
	bmpBuf_kkk = (uint8_t *)(((uint32_t )malloc(IMAGE_BUFFER_SIZE+64)));
	bmpBuf_kkk_bak = bmpBuf_kkk;
	bmpBuf_kkk = 	(uint8_t *)(shift_pointer((uint32_t)bmpBuf_kkk,32)+32);
	bmpBuf_kkk = (uint8_t *)((uint32_t)bmpBuf_kkk|0x80000000	);
	}
	else
	{
			#ifdef SYSUARTPRINTF_p
	 sysprintf("bmpBuf_kkk=%08X,bmpBuf_kkk_bak=%08X, not malloc\r\n",bmpBuf_kkk,bmpBuf_kkk_bak);
	 #endif	
	}
	
	if(bmpBuf_kkk==0)
	{
		#ifdef SYSUARTPRINTF_p
	 sysprintf("bmpBuf_kkk=%08X,bmpBuf_kkk_bak=%08X, malloc NG\r\n",bmpBuf_kkk,bmpBuf_kkk_bak);
	 #endif	
		return;
	}	
	else
	{
		#ifdef SYSUARTPRINTF_p
	 sysprintf("bmpBuf_kkk=%08X,bmpBuf_kkk_bak=%08X, malloc ok\r\n",bmpBuf_kkk,bmpBuf_kkk_bak);
	 #endif	
	}
		
	while(Tp_i<0XFF00)
	{
	   Tp_addr = READ_TAB_FROMSDRAM(Tp_i,1,Tp_bmp_TAB);
	   Tp_i++;
		 if(Tp_addr!=0Xffffffff)
		 check_sum_nand(Tp_addr*2048,Tp_bmp_TAB[0],Tp_bmp_TAB[1]);
	}
	
	//sprintf(display_checksum,"NAND Chechsum = 0x%08X",CHECK_SUM_NAND);
  //LCD_DisplayStringLine(30,display_checksum);
	
	if(bmpBuf_kkk) 
  {
		#ifdef SYSUARTPRINTF_p
	 sysprintf("bmpBuf_kkk=%08X,bmpBuf_kkk_bak=%08X, free\r\n",bmpBuf_kkk,bmpBuf_kkk_bak);
	 #endif	
	free(bmpBuf_kkk_bak);
	bmpBuf_kkk=0;
	}
	else
  {
		#ifdef SYSUARTPRINTF_p
	 sysprintf("bmpBuf_kkk=%08X,bmpBuf_kkk_bak=%08X, not free\r\n",bmpBuf_kkk,bmpBuf_kkk_bak);
	 #endif	
	}
	
	
   checksum_flag = 1;
	
	#ifdef SYSUARTPRINTF_p
	 sysprintf("Read_checksum_end\r\n");
	#endif
	
}


void code_protocol_error_and_start(int8_t type,uint8_t Value_1,uint16_t Value_2) 
{
      uint8_t  data_len = 0;
	//uint16_t n = 0;      
	
	err_code_buff[0]='@';
	data_len =6;
       convet_hex_to_hex_asccii(data_len,err_code_buff+1);

	err_code_buff[3]='U';
	err_code_buff[4]=type;

	err_code_buff[5]=convert_1_hex_to_hex_ascii(Value_1);

	err_code_buff[6]=convert_1_hex_to_hex_ascii(Value_2>>8);
  err_code_buff[7]=convert_1_hex_to_hex_ascii(Value_2);
	err_code_buff[8]='*';

	
	err_code_num=3;
	err_code_delay=0; //10;
     
}


void err_module_send_control(void)  // 1 ms run
{		
	uint8_t i =0;	
	static uint8_t err_code_13=0,err_code_6=0;	
	uint32_t m=0;
	//
	
	if(command_D8_D9_time)
	{
		return;
	}
	
	if(err_code_delay)
	{
	  err_code_delay--;
	}
		
      //�ж���û�з���
	if((err_code_num)&&(err_code_delay==0))
	{
	    err_code_num--;
	     err_code_delay=10;  
		   send_code_protocol_error();
	    
	     return;
	}
	//
	if((systerm_error_status.val!=0)&&(err_code_num == 0))//�����֧�ǿ��������Ĵ��������ʱ��
	{
		for(i=0;i<32;i++)
		{
	 if(err_code_num==3) break;//20220531
		m=systerm_error_status.val&(1<<i);	
		if(m)
		{
		  switch(i)
			{
				
			  case 0://CTP IIC ERROR
					
				  systerm_error_status.val&=~(1<<i); 
				  code_protocol_error_and_start(ERROR_TYPE_U1,(TOUCH_I2C_ERROR_101>>16),(int16_t)TOUCH_I2C_ERROR_101); //,ERROR_TYPE_SEND_3_TIMES);
				  
				break;
			case 2://LOGOIIC
					
				  systerm_error_status.val&=~(1<<i); 
				  code_protocol_error_and_start(ERROR_TYPE_U1,(LOG_I2C_ERROR_601>>16),(int16_t)LOG_I2C_ERROR_601);
					break;
			case 4://NANDFLASHд����
					
				  systerm_error_status.val&=~(1<<i); 
				  code_protocol_error_and_start(ERROR_TYPE_U1,(NANDFLASH_W_ERROR_702>>16),(int16_t)NANDFLASH_W_ERROR_702);
					break;
			case 6://�ⲿ���پ������ ������ȥ�Ĵ���									     
				     if(err_code_6==0)
					 {
					 //	code_protocol_error_and_start(ERROR_TYPE_U1,(RTC_CLOCK_ERROR_402>>16),(int16_t)RTC_CLOCK_ERROR_402); 
						err_code_6=1;
				     	}
					break;
			case 9://����IC�쳣
					
				  systerm_error_status.val&=~(1<<i); 
				  code_protocol_error_and_start(ERROR_TYPE_U1,(VOICE_ERROR_901>>16),(int16_t)VOICE_ERROR_901);
				  break;
				  
			case 13://��ص�ѹ�쳣 ������ȥ�Ĵ���					
					if(err_code_13==0)
					 {
						code_protocol_error_and_start(ERROR_TYPE_U1,(RTC_BAT_ERROR_403>>16),(int16_t)RTC_BAT_ERROR_403); 
						err_code_13=1;
					}
					break;
			case 15://USB��������
					
				  systerm_error_status.val&=~(1<<i); 
				  code_protocol_error_and_start(ERROR_TYPE_U1,(USB_COMM_ERROR_501>>16),(int16_t)USB_COMM_ERROR_501);
					break;
					
			case 16://�Ҳ������ϵ��ļ�
					
				 systerm_error_status.val&=~(1<<i); 
				  code_protocol_error_and_start(ERROR_TYPE_U1,(P_BMP_NO_RIGHT_FILEERROR_206>>16),(int16_t)P_BMP_NO_RIGHT_FILEERROR_206);
					break;
			case 17://�Ҳ���HEX��TXT
					if(download.task == 0)
					{
				  systerm_error_status.val&=~(1<<i); 
				  code_protocol_error_and_start(ERROR_TYPE_U1,(USB_W_ERROR1_503>>16),(int16_t)USB_W_ERROR1_503);
					}
					break;
				
			case 19://�軭���ļ�������
					
				  systerm_error_status.val&=~(1<<i); 
				  code_protocol_error_and_start(ERROR_TYPE_U1,(D_BMP_NO_FILE_ERROR_801>>16),(int16_t)D_BMP_NO_FILE_ERROR_801);
					break;
			case 20://�軭���ļ��������
				  
				  systerm_error_status.val&=~(1<<i); 
				  code_protocol_error_and_start(ERROR_TYPE_U1,(D_BMP_XY_ERROR_802>>16),(int16_t)D_BMP_XY_ERROR_802);
				  break;
				  
   			 case 21://ͼƬ�����ڴ�����
					
				  systerm_error_status.val&=~(1<<i); 
				  code_protocol_error_and_start(ERROR_TYPE_U1,(P_BMP_OVER_SIZE_ERROR_201>>16),(int16_t)P_BMP_OVER_SIZE_ERROR_201);
					break;
				case 22://ͼƬ����800*480��С
					
				  systerm_error_status.val&=~(1<<i); 
				  code_protocol_error_and_start(ERROR_TYPE_U1,(P_BMP_OVER_800X400_ERROR_202>>16),(int16_t)P_BMP_OVER_800X400_ERROR_202);
				  break;
				
			case 23://�ļ����ظ�
      	                      	
				 systerm_error_status.val&=~(1<<i); 
				  code_protocol_error_and_start(ERROR_TYPE_U1,(P_BMP_RENAME_FILE_ERROR_204>>16),(int16_t)P_BMP_RENAME_FILE_ERROR_204);
					break;
			case 24://�ļ����ǳߴ����
					
				 systerm_error_status.val&=~(1<<i); 
				  code_protocol_error_and_start(ERROR_TYPE_U1,(P_BMP_REWRITE_SIZE_ERROR_205>>16),(int16_t)P_BMP_REWRITE_SIZE_ERROR_205);
					break;

			case 25://����BMP�ļ�
					
				  systerm_error_status.val&=~(1<<i); 
				  code_protocol_error_and_start(ERROR_TYPE_U1,(P_BMP_NO_BMP_FILE_ERROR_203>>16),(int16_t)P_BMP_NO_BMP_FILE_ERROR_203);
					break;
					
				
			case 26://SDRAM������Χ
					
				 systerm_error_status.val&=~(1<<i); 
				  code_protocol_error_and_start(ERROR_TYPE_U1,(SDRAM_OVER_SIZE_ERROR_301>>16),(int16_t)SDRAM_OVER_SIZE_ERROR_301);
					break;
			case 27://��NANDFLASH���Ҳ���QҪ��ͼƬ
					
				  systerm_error_status.val&=~(1<<i); 
				  code_protocol_error_and_start(ERROR_TYPE_U1,(SDARM_NO_FILE_ERROR_302>>16),(int16_t)SDARM_NO_FILE_ERROR_302);
					break;
				
			case 28://USB���ʴ�����д�뵽USB
					
				  systerm_error_status.val&=~(1<<i); 
				  code_protocol_error_and_start(ERROR_TYPE_U1,(USB_W_ERROR_502>>16),(int16_t)USB_W_ERROR_502);
					break;
			case 31:
				   systerm_error_status.val&=~(1<<i); 
				  code_protocol_error_and_start(ERROR_TYPE_U1,(NANDFLASH_Q_ERROR_701>>16),(int16_t)NANDFLASH_Q_ERROR_701);
			    
			    break;				
				
				
				  
				default:
					//Tp_kong = 1;
					break;
		  	 }		
			}					
				
		}
	   }					   		
}




void led_log_wait_off(void ) // 1 ms run 
{	
	
	if(led_log_on_cnt)
		{
			led_log_on_cnt--;
			if(led_log_on_cnt==0)
			{
				LED_LOGO_OFF( );
			}
		}
}

void Time100msProcess(void)
{
	if(LOGO_DATA_OUT_FLAG==3)
	{
	check_sw234( );
	Display_checksum();	
	}
	//check_sw5( );
	//renesas_power_control( );
////////	add_action_timers_data_class_pwr_on( );

}
void Time500msProcess(void)
{
	Fresh_lcd_dis();//������ɱ�־
	power_on_logo();
}

void Time50msProcess(void)
{
//	check_tp_send(); 
}
void Time1msProcess(void)
{
	    
	  ppt_process( );
		// send_tts_command_control( ); 
	   tts_time();
	    led_log_wait_off( );
	   err_module_send_control();
	   if(command_D8_D9_flag == 1)
		 {
		 command_D8_D9_flag = 0;
			 code_protocol_error(ERROR_TYPE_U1,(U1A01_ERROR>>16),(int16_t)U1A01_ERROR,ERROR_TYPE_SEND_3_TIMES);
		 }
	  // judge_uart2_receive_end( );
	   
}




void OneSecondProcess(void)
{
	FRESULT res1=FR_OK,res2=FR_OK,res3=FR_OK;

  BmpFIleInfo Tp_info = {0,0,1,0};
  int width=0,height=0;
	//uint32_t pbuf=LCD_FRAME_BUFFER;
	
	check_wt588h_exist( );
	
	if(pwr_on_cnt)
		{
			pwr_on_cnt--;
			 if(pwr_on_cnt==2)
			 {
				 

			 }
			if(pwr_on_cnt==1)
				{
  					
				}
				
				
			if(pwr_on_cnt==0)
				{
					
				// ClearLayerData(3);//�����ʾ����
				// Backlinght_Control_Init_HARDV4(128);
				if((READ_PIN_SW1_3==SW_ON)&&(READ_PIN_SW1_6==SW_ON))
				{
					if((READ_PIN_SW1_1==SW_ON)&&(READ_PIN_SW1_2==SW_ON)&&(READ_PIN_SW1_3==SW_ON)&&(READ_PIN_SW1_4==SW_ON)&&(READ_PIN_SW1_6==SW_ON))
					{
						
					}
					else
					{
						SetZuobiao(10, 380);
						lcd_printf_new("ERROR: Incorrect DIP-SW settings");
					}
					NAND_BMP_Read_checksum();
	        Display_checksum();
	        while(1);
				}
					
				if(READ_PIN_SW1_2==SW_ON)
	      {
		       
					SetZuobiao(10, 400 + 20);
		       lcd_printf_new("Waiting        ");
		                                    
	      } 
	      if((READ_PIN_SW1_3==SW_ON)||(READ_PIN_SW1_6==SW_ON))
	      {
		      
					SetZuobiao(10, 400 + 40);
		       lcd_printf_new("Waiting        ");
					  USB_IMAGE_TYPE = '0';
		        USB_HAS_USABLE_IMG = 0;
		        IMAGE_SEARCHED =0;
		                                    
	      } 
	      if(READ_PIN_SW1_4==SW_ON)
	      {
		       
					SetZuobiao(10, 400 + 60);
		       lcd_printf_new("Waiting        ");
		                                    
	      } 
						
					
					
					//PVD_Init();
				 
			
				 
			//GetAreaAllDataByNand();
         
			//		fresh_rtc_coarse_value( );
					
			     //add_test_final_data_class_pwr_on( ); �ж�ADֵ������ŵ���
					
					LOGO_DATA_OUT_FLAG ++;//�����־��ʾ���ݵ���
				}
		}
}

void TimeProcess(void)
{
//	 LOGO_DATA_OUT_FLAG = 3;
	if(get_timego(Flag_Tim_1S)>=1000)
	{
		Flag_Tim_1S = time1ms_count;
		OneSecondProcess();
	}
		
	if(get_timego(Flag_Tim_1ms)>=1)
	{
		Flag_Tim_1ms = time1ms_count;
		Time1msProcess();
		
	}
	
	if(get_timego(Flag_Tim_100ms)>=100)
	{
		Flag_Tim_100ms = time1ms_count;
		Time100msProcess();
		//LED_POWER_TOGGLE();
	}
	
	if(get_timego(Flag_Tim_500ms)>=500)
	{
		Flag_Tim_500ms = time1ms_count;
		Time500msProcess();
	}
	
	if(get_timego(Flag_Tim_50ms)>=50)
	{
		Flag_Tim_50ms = time1ms_count;
		Time50msProcess();
	}
	
	
}


void set_tim1msint(E_INT_STATUS x_status)
{
	tim1ms_control = x_status;
}

void tim1ms_int_enable(void)
{
	///////////////////sysEnableInterrupt(ETMR2_IRQn);
	//////////////////sysEnableInterrupt(TMR0_IRQn);
	// ETIMER_EnableInt(2);
	//sysSetLocalInterrupt(ENABLE_IRQ);
	if(tim1ms_control== int_wait) 
	{
		set_tim1msint(int_enable);
		sysTimer0ISR();
		//check_tp_send();
	}
	set_tim1msint(int_enable);
}
void tim1ms_int_disable(void)
{
	set_tim1msint(int_disable);
	//////////////////sysDisableInterrupt(ETMR2_IRQn);
	/////////////////sysDisableInterrupt(TMR0_IRQn);
//	ETIMER_DisableInt(2);
	//sysSetLocalInterrupt(DISABLE_IRQ);
}




