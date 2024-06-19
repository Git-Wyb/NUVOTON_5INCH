#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "sys.h"
#include "Command_all.h"
#include "ff.h"
#include "nand_nand.h"
#include "Aprotocol.h"
#include "BSP_init.h"
#include "RTC.h"
#include "config.h"


#define Basedata_cs_stm7                    0x308 //»ù±¾Êý¾ÝµÄ±¸·ÝÖµ
//#define Basedata_f7end            0x400 //Í¨Ñ¶Ö¸ÁîF7ÄÜ·ÃÎÊµ½µÄ×î¸ßµØÖ·
#define Basedata_copy0_stm7             0x05000//»ù±¾Êý¾ÝµÄ±¸·ÝÇø0
#define Basedata_copy1_stm7             0x10000//»ù±¾Êý¾ÝµÄ±¸·ÝÇø1
#define Basedata_copy2_stm7             0x15000//»ù±¾Êý¾ÝµÄ±¸·ÝÇø2
#define logodata_Basedata_SIZE_stm7 		0x320

#define image_tab__nandflash_start_stm7  (765*64)
#define baseB_data__nandflash_start_stm7 (768*64)
#define backupdata_nandflash_start_stm7  (1004*64)

extern TIME_TYPE       pwr_on_time_app;

extern FATFS usb_fatfs;
FIL file;
extern  SYSTEMERRORSTATUS_TYPE  systerm_error_status;
extern uint8_t LOGO_DATA_OUT_FLAG;
extern AreaConfig gs_AreaInfo[16],Tp_gs_AreaInfo[16];
extern PARA_TYPE para;
extern uint32_t logodata_sdrambuffer_addr_arry[16];
extern uint16_t System_Run_Cycle ;
extern uint32_t  time1ms_count_forrun;
extern uint32_t System_Runtime_Lasttime;
extern Dataclass4567_Info Fieldx_Info[16];
extern uint8_t *BaseData_ARR;

extern uint32_t logodata_basedata_BUFFER;
/*__attribute__((aligned(8))) */uint8_t *Image_Buf=0,*Image_Buf_bak=0;
uint32_t shift_pointer(uint32_t ptr, uint32_t align);
extern USBH_T     *_ohci;
extern TIME_TYPE   timeandday_now_app;
static uint32_t logodata_basedata_stm7_BUFFER=0,logodata_basedata_stm7_BUFFER_bak=0;
extern LAYER_SDRAM_STR display_layer_sdram;

uint8_t  write_data_to_stm7buffer(void)
{
	uint8_t Tp_i,Tp_j,Tp_k,Tp_long;
	uint16_t  offset1,offset2;
	uint32_t Tp_check=0;
	
	if(logodata_basedata_stm7_BUFFER_bak==0)
	{
	logodata_basedata_stm7_BUFFER = (uint32_t )malloc((0x20000)+64);
	logodata_basedata_stm7_BUFFER_bak = logodata_basedata_stm7_BUFFER;
	}
	
	if(logodata_basedata_stm7_BUFFER_bak==NULL) return 0;
	//logodata_sdrambuffer_addr_arry_bak[0]=logodata_basedata_BUFFER;
	logodata_basedata_stm7_BUFFER = 32+shift_pointer(logodata_basedata_BUFFER,32);
	logodata_basedata_stm7_BUFFER = logodata_basedata_BUFFER|0X80000000;
	
	
	
	memset((void *)(logodata_basedata_stm7_BUFFER),0xFF,0x20000);
	
	for(Tp_i=0;Tp_i<BASE_data_4byte_end;Tp_i++)
	{
		if(Tp_i<=31) 
		{
			sysprintf("Tp_i=%x\r\n",Tp_i);
			Tp_long = *(uint8_t *)(BaseData_ARR+Tp_i*9+8);
			if(Tp_i== 0)  Tp_long=8;
			if(Tp_long>8) Tp_long=8;
			sysprintf("Tp_long=%x\r\n",Tp_long);
			Tp_k = 0;
			
			for(Tp_j=0;Tp_j<Tp_long;Tp_j++)
			{
				*(uint8_t *)(logodata_basedata_stm7_BUFFER+Tp_i*8+Tp_j) = *(uint8_t *)(BaseData_ARR+Tp_i*9+Tp_j);
				Tp_k++;
			}
			sysprintf("Tp_k_0=%x\r\n",Tp_k);
			while(Tp_k<8)
			{
				*(uint8_t *)(logodata_basedata_stm7_BUFFER+Tp_i*8+Tp_k) = 0xFF;
				Tp_k++;
			}
			sysprintf("Tp_k_1=%x\r\n",Tp_k);
			//memcpy((void *)(logodata_basedata_stm7_BUFFER+Tp_i*8),(void *)(BaseData_ARR+Tp_i*9),8);
		}
		
		if(Tp_i==32)//START ADDR
		{
			for(Tp_j=0;Tp_j<16;Tp_j++)
			{
				for(Tp_k = 0; Tp_k<4;Tp_k++)
				{
					offset1=(32*8)+(Tp_j*4)+Tp_k;
					offset2=(32*9)+((4*Tp_j)*9)+((3-Tp_k)*2);
					*(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1) = (uint8_t)CHANGE_HEXDATA_2BYTE((uint8_t  *)(BaseData_ARR+offset2));
					
				}
			}
			
		}
	
		
		if(Tp_i==48)//SIZE
		{
			for(Tp_j=0;Tp_j<16;Tp_j++)
			{
				for(Tp_k = 0; Tp_k<4;Tp_k++)
				{
					
					offset1 = (32*8)+(16*4)+(Tp_j*4)+Tp_k;
					offset2 = (32*9)+(1*9)+((4*Tp_j)*9)+((3-Tp_k)*2);
					sysprintf("data1=%x,data2=%x\r\n",*(uint8_t  *)(BaseData_ARR+offset2),*(uint8_t  *)(BaseData_ARR+offset2+1));
					
					*(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1) = (uint8_t)CHANGE_HEXDATA_2BYTE((uint8_t  *)(BaseData_ARR+offset2));
				}
			}	
			
			
			
		}
	
		if(Tp_i==64)//NUMBER
		{
			
			for(Tp_j=0;Tp_j<16;Tp_j++)
			{
				for(Tp_k = 0; Tp_k<4;Tp_k++)
				{
					offset1 = (32*8)+(32*4)+(Tp_j*4)+Tp_k;
					offset2 = (32*9)+(3*9)+((4*Tp_j)*9)+((3-Tp_k)*2);
					*(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1) = (uint8_t)CHANGE_HEXDATA_2BYTE((uint8_t  *)(BaseData_ARR+offset2));
				}
			}
			
		}
		
		
		
		if(Tp_i==80)//DATACLASS 
		{
			
		  for(Tp_j=0;Tp_j<16;Tp_j++)
			{
				for(Tp_k = 0; Tp_k<4;Tp_k++)
				{
					offset1 = (32*8)+(48*4)+(Tp_j*4)+Tp_k;
					offset2 = (32*9)+(2*9)+((4*Tp_j)*9)+((3-Tp_k)*2);
					*(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1) = (uint8_t)CHANGE_HEXDATA_2BYTE((uint8_t  *)(BaseData_ARR+offset2));
				  if(*(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1)==0xff) *(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1) = 0;
				}
			}
			
		}
		
		if(Tp_i==98)//LCD BACKLIGHT
		{
			offset1 = 8*32+4*66;
			*(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1)=para.lcd_back_light.brightness;
			 *(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1+1)=~para.lcd_back_light.brightness;
		}
		if(Tp_i==99)//TOUCH 
		{
			offset1 = 8*32+4*67;
			*(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1)=para.touch_sensibility;
			 *(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1+1)=~para.touch_sensibility;
		}
		
		
		
		if(Tp_i==112)//count
		{
			for(Tp_j=0;Tp_j<48;Tp_j++)
			{
				for(Tp_k = 0; Tp_k<4;Tp_k++)
			  {
					offset1 = (32*8) + (80*4)+ (Tp_j*4) +Tp_k;
					offset2 = (112*9) + (Tp_j*9)+((3-Tp_k)*2);
					*(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1) = (uint8_t)CHANGE_HEXDATA_2BYTE((uint8_t  *)(BaseData_ARR+offset2));
				}
			}
		}
		
		if(Tp_i==162)//checksum
		{
			for(offset1=0;offset1<Basedata_cs_stm7;offset1++)
			{
				Tp_check = Tp_check + *(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1);
			}
			*(uint32_t*)(logodata_basedata_stm7_BUFFER+Basedata_cs_stm7) = Tp_check;
			
				memcpy((uint8_t*)(logodata_basedata_stm7_BUFFER +Basedata_copy0_stm7),(uint8_t*)(logodata_basedata_stm7_BUFFER),logodata_Basedata_SIZE_stm7);
        memcpy((uint8_t*)(logodata_basedata_stm7_BUFFER +Basedata_copy1_stm7),(uint8_t*)(logodata_basedata_stm7_BUFFER),logodata_Basedata_SIZE_stm7);
        memcpy((uint8_t*)(logodata_basedata_stm7_BUFFER +Basedata_copy2_stm7),(uint8_t*)(logodata_basedata_stm7_BUFFER),logodata_Basedata_SIZE_stm7);
		}
		
	}
	
	
	if(logodata_basedata_stm7_BUFFER_bak==NULL) return 0;
	else 
	{
		//free((void *)logodata_basedata_stm7_BUFFER_bak);
		//logodata_basedata_stm7_BUFFER=0;
		//logodata_basedata_stm7_BUFFER_bak=0;
	}
	return 1;
}

void read_data_from_stm7buffer(void)
{
	uint8_t Tp_i,Tp_j,Tp_k,Tp_long,Tp_x;
	uint16_t  offset1,offset2;
	uint8_t Tp_data;
	
	sysprintf("read_data_from_stm7buffer\r\n");
	for(Tp_i=0;Tp_i<BASE_data_4byte_end;Tp_i++)
	{
		if(Tp_i<=31) 
		{
			Tp_long = 0;
			memset((char  *)(BaseData_ARR+Tp_i*9),'0',9);
			for(Tp_j=0;Tp_j<8;Tp_j++)
			{
				if(*(uint8_t *)(logodata_basedata_stm7_BUFFER+Tp_i*8+Tp_j)!=0xff)
				{
					Tp_long++;
				}
				//
				*(uint8_t *)(BaseData_ARR+Tp_i*9+Tp_j) = *(uint8_t *)(logodata_basedata_stm7_BUFFER+Tp_i*8+Tp_j);
				#ifdef SYSUARTPRINTF_ActionTimers
				sysprintf("no=%d,data1=%x\r\n",Tp_i,*(uint8_t *)(BaseData_ARR+Tp_i*9+Tp_j));
				#endif
			}
		//	sysprintf("Tp_long=%x\r\n",Tp_long);
			*(uint8_t *)(BaseData_ARR+Tp_i*9+8) = Tp_long;
	   }
		 
		 if(Tp_i==32)//START ADDR
		{
			for(Tp_j=0;Tp_j<16;Tp_j++)
			{
				offset2=(32*9)+((4*Tp_j)*9);
				memset((char  *)(BaseData_ARR+offset2),'0',8);
				
				for(Tp_k = 0; Tp_k<4;Tp_k++)
				{
					offset1=(32*8)+(Tp_j*4)+Tp_k;
					offset2=(32*9)+((4*Tp_j)*9)+((3-Tp_k)*2);
					//*(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1) = (uint8_t)CHANGE_HEXDATA_2BYTE((uint8_t  *)(BaseData_ARR+offset2));
					
					Tp_data = *(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1);
					
				//	sprintf((char  *)(BaseData_ARR+offset2),"%X",*(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1));
					//*(char  *)(BaseData_ARR+offset2) = '0'+Tp_data/16;
					//*(char  *)(BaseData_ARR+offset2+1) = '0'+Tp_data%16;
					convet_hex_to_hex_asccii(Tp_data,(uint8_t   *)(BaseData_ARR+offset2));
					 
					#ifdef SYSUARTPRINTF_ActionTimers
				  sysprintf("no=%d,offset1=%x\r\n",Tp_i+Tp_j,*(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1));
				  #endif
				}
				
////				offset2=(32*9)+((4*Tp_j)*9);
////				for(Tp_x=0;Tp_x<8;Tp_x++)
////				{
////					//if(*(char  *)(BaseData_ARR+offset2+Tp_x)==0x20) *(char  *)(BaseData_ARR+offset2+Tp_x)='0';
////					#ifdef SYSUARTPRINTF_ActionTimers
////					sysprintf("no=%d,offset2=%x\r\n",Tp_i+Tp_j,*(char  *)(BaseData_ARR+offset2+Tp_x));
////					#endif
////				}
////				offset2=(32*9)+((4*Tp_j)*9);
////				for(Tp_k = 0; Tp_k<8;Tp_k++)
////				{
////					if(*(uint8_t  *)(BaseData_ARR+offset2+Tp_k)==0)
////					{
////						*(uint8_t  *)(BaseData_ARR+offset2+Tp_k) = '0';
////					}
////				}
			}
			
		}
	
		
		if(Tp_i==48)//SIZE
		{
			for(Tp_j=0;Tp_j<16;Tp_j++)
			{
				offset2 = (32*9)+(1*9)+((4*Tp_j)*9);
				memset((char  *)(BaseData_ARR+offset2),'0',8);
				
				for(Tp_k = 0; Tp_k<4;Tp_k++)
				{
					
					offset1 = (32*8)+(16*4)+(Tp_j*4)+Tp_k;
					offset2 = (32*9)+(1*9)+((4*Tp_j)*9)+((3-Tp_k)*2);
					
					Tp_data = *(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1);
					//*(char  *)(BaseData_ARR+offset2) = '0'+Tp_data/16;
					//*(char  *)(BaseData_ARR+offset2+1) = '0'+Tp_data%16;
					convet_hex_to_hex_asccii(Tp_data,(uint8_t   *)(BaseData_ARR+offset2));
					
					//sysprintf("data1=%x,data2=%x\r\n",*(uint8_t  *)(BaseData_ARR+offset2),*(uint8_t  *)(BaseData_ARR+offset2+1));
					//sprintf((char  *)(BaseData_ARR+offset2),"%X",*(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1));
					//*(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1) = (uint8_t)CHANGE_HEXDATA_2BYTE((uint8_t  *)(BaseData_ARR+offset2));
				  #ifdef SYSUARTPRINTF_ActionTimers
				  sysprintf("no=%d,offset1=%x\r\n",Tp_i+Tp_j,*(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1));
				  #endif
				}
				
//				offset2 = (32*9)+(1*9)+((4*Tp_j)*9);
//				for(Tp_x=0;Tp_x<8;Tp_x++)
//				{
//					//if(*(char  *)(BaseData_ARR+offset2+Tp_x)==0x20) *(char  *)(BaseData_ARR+offset2+Tp_x)='0';
//					#ifdef SYSUARTPRINTF_ActionTimers
//					sysprintf("offset2=%x\r\n",*(char  *)(BaseData_ARR+offset2+Tp_x));
//					#endif
//				}
			}	
			
			
			
		}
	
		if(Tp_i==64)//NUMBER
		{
			
			for(Tp_j=0;Tp_j<16;Tp_j++)
			{
				offset2 = (32*9)+(3*9)+((4*Tp_j)*9);
				memset((char  *)(BaseData_ARR+offset2),'0',8);
				
				if(Tp_j==0) continue;
				
				for(Tp_k = 0; Tp_k<4;Tp_k++)
				{
					offset1 = (32*8)+(32*4)+(Tp_j*4)+Tp_k;
					offset2 = (32*9)+(3*9)+((4*Tp_j)*9)+((3-Tp_k)*2);
					//*(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1) = (uint8_t)CHANGE_HEXDATA_2BYTE((uint8_t  *)(BaseData_ARR+offset2));
					//sprintf((char  *)(BaseData_ARR+offset2),"%d",*(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1));
					Tp_data = *(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1);
					//*(char  *)(BaseData_ARR+offset2) = '0'+Tp_data/16;
					//*(char  *)(BaseData_ARR+offset2+1) = '0'+Tp_data%16;
					convet_hex_to_hex_asccii(Tp_data,(uint8_t   *)(BaseData_ARR+offset2));
					#ifdef SYSUARTPRINTF_ActionTimers
				  sysprintf("no=%d,offset1=%x\r\n",Tp_i+Tp_j,*(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1));
				  #endif
				}
				
//				offset2 = (32*9)+(3*9)+((4*Tp_j)*9);
//				for(Tp_x=0;Tp_x<8;Tp_x++)
//				{
//					//if(*(char  *)(BaseData_ARR+offset2+Tp_x)==0x20) *(char  *)(BaseData_ARR+offset2+Tp_x)='0';
//					#ifdef SYSUARTPRINTF_ActionTimers
//					sysprintf("offset2=%x\r\n",*(char  *)(BaseData_ARR+offset2+Tp_x));
//					#endif
//				}
			}
			
		}
		
		
		
		if(Tp_i==80)//DATACLASS 
		{
			
		  for(Tp_j=0;Tp_j<16;Tp_j++)
			{
				offset2 = (32*9)+(2*9)+((4*Tp_j)*9);
				memset((char  *)(BaseData_ARR+offset2),'0',8);
				
				if(Tp_j==0)  continue;
				
				for(Tp_k = 0; Tp_k<4;Tp_k++)
				{
					offset1 = (32*8)+(48*4)+(Tp_j*4)+Tp_k;
					offset2 = (32*9)+(2*9)+((4*Tp_j)*9)+((3-Tp_k)*2);
					//*(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1) = (uint8_t)CHANGE_HEXDATA_2BYTE((uint8_t  *)(BaseData_ARR+offset2));
					
					//sprintf((char  *)(BaseData_ARR+offset2),"%d",*(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1));
					Tp_data = *(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1);
					//*(char  *)(BaseData_ARR+offset2) = '0'+Tp_data/16;
					//*(char  *)(BaseData_ARR+offset2+1) = '0'+Tp_data%16;
					convet_hex_to_hex_asccii(Tp_data,(uint8_t   *)(BaseData_ARR+offset2));
					#ifdef SYSUARTPRINTF_ActionTimers
				  sysprintf("no=%d,offset1=%x\r\n",Tp_i+Tp_j,*(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1));
				  #endif
					
					if(Tp_k==0)
					{
					  if(Tp_data == 0) //*(char *)(BaseData_ARR+offset2+1)=0xff;
						{
							convet_hex_to_hex_asccii(0xff,(uint8_t   *)(BaseData_ARR+offset2));
						}
					}
					
//					offset2 = (32*9)+(2*9)+((4*Tp_j)*9);
//					for(Tp_x=0;Tp_x<8;Tp_x++)
//				  {
//					   // if(*(char  *)(BaseData_ARR+offset2+Tp_x)==0x20) *(char  *)(BaseData_ARR+offset2+Tp_x)='0';
//						  #ifdef SYSUARTPRINTF_ActionTimers
//					    sysprintf("offset2=%x\r\n",*(char  *)(BaseData_ARR+offset2+Tp_x));
//					    #endif
//				  }
					
					
				 // if(*(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1)==0xff)// *(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1) = 0;
				}
			}
			
		}
		
		if(Tp_i==98)//LCD BACKLIGHT
		{
			offset1 = 8*32+4*66;
			Tp_data = *(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1);
			para.lcd_back_light.brightness = Tp_data;
		}
		if(Tp_i==99)//TOUCH 
		{
			offset1 = 8*32+4*67;
			Tp_data = *(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1);
			para.touch_sensibility = Tp_data;
		}
		
		
		
		if(Tp_i==112)//count
		{
			for(Tp_j=0;Tp_j<48;Tp_j++)
			{
				offset2 = (112*9) + (Tp_j*9);
				memset((char  *)(BaseData_ARR+offset2),'0',8);
				
				for(Tp_k = 0; Tp_k<4;Tp_k++)
			  {
					offset1 = (32*8) + (80*4)+ (Tp_j*4) +Tp_k;
					offset2 = (112*9) + (Tp_j*9)+((3-Tp_k)*2);
					Tp_data = *(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1);
					//*(char  *)(BaseData_ARR+offset2) = '0'+Tp_data/16;
					//*(char  *)(BaseData_ARR+offset2+1) = '0'+Tp_data%16;
					convet_hex_to_hex_asccii(Tp_data,(uint8_t   *)(BaseData_ARR+offset2));
					//sprintf((char  *)(BaseData_ARR+offset2),"%02X",*(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1));
					//*(uint8_t *)(logodata_basedata_stm7_BUFFER+offset1) = (uint8_t)CHANGE_HEXDATA_2BYTE((uint8_t  *)(BaseData_ARR+offset2));
				}
				
//				offset2 = (112*9) + (Tp_j*9);
//				for(Tp_x=0;Tp_x<8;Tp_x++)
//				{
//					//if(*(char  *)(BaseData_ARR+offset2+Tp_x)==0) *(char  *)(BaseData_ARR+offset2+Tp_x)='0';
//				}
			}
		}
		
		 
	}
	
	write_para_to_sdram();
	basedata_ram_to_sdram();
	
}


void write_para_to_sdram(void)
{
	sprintf((char *)(BaseData_ARR+99*9),"%08X", para.touch_sensibility);
	sprintf((char *)(BaseData_ARR+98*9),"%08X",para.lcd_back_light.brightness);
	sprintf((char *)(BaseData_ARR+103*9),"%08X",para.lcd_back_light.state);
	sprintf((char *)(BaseData_ARR+102*9),"%08X",para.rtc_coarse_value);
	sprintf((char *)(BaseData_ARR+BASE_data_Dataclass12RTCPowerOffTimer*9),"%08X",para.no_vbat_pwr_on_cnt );
	sprintf((char *)(BaseData_ARR+BASE_data_actioncount*9),"%08X",para.dataclass_1_2_action_count );
	sprintf((char *)(BaseData_ARR+BASE_data_PowerOffYear*9),"%08X",para.last_poweroff_year );
	sprintf((char *)(BaseData_ARR+BASE_data_PowerOffMonth*9),"%08X",para.last_poweroff_month );
	sprintf((char *)(BaseData_ARR+BASE_data_SW3_6_TIMES*9),"%08X",para.sw36_count);
	sprintf((char *)(BaseData_ARR+BASE_data_ParaFlag*9),"%08X",para.flag);
}

void	Save_Logo(void)
{
//	  uint16_t  x_delay;
//	static uint8_t Tp_Buff[2048]={0};
	uint8_t Tp_field=0;
	uint32_t Tp_addr=0;
	uint32_t Tp_Time=0;//??????
//	uint32_t Tp_checksum2;
//	uint16_t Tp_i;
	
	//  LOGOSAVE_FLAG  = 1;//logo??????
		
	if(LOGO_DATA_OUT_FLAG!=3)//??LOGO????????????????
		 return;
	
	for(Tp_field = 1; Tp_field<16;Tp_field++)//NG
    {
			
			
			 
			switch(gs_AreaInfo[Tp_field].type)
		  {
				case Base_DATACLASS:
					//if(Tp_field == 0)
				     
					   break;
				case ActionNumber_DATACLASS://ActionNumber_DATACLASS:
				
					  // if(LOGO_DOUBLE_VISIT == Tp_field) continue;//????????????????????h?? SDRAM???????????????????????? 
			  
				if(para.dataclass_1_2_action_count >= MAX_LOGO_ACTION_MONTH) continue;//20170510
					
		
				Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + ((para.dataclass_1_2_action_count*4)/64)*64;		
       // NANDFLASH_TO_SDRAM_RANDOM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,logodata_ActionNumber_SIZE,4);//????????????????????
			       
				SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
			
			   //LED_POWER_ON();
					   //Tp_addr = FIELD_str->Field_Addr[Tp_field]/2048 + sdram_actionindex*4;
				    // SDRAM_TO_NANDFLASH_RANDOM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,logodata_ActionNumber_SIZE,4);
					  break;
				case ActionTimers_DATACLASS:
					//  if(LOGO_DOUBLE_VISIT == Tp_field) continue;//????????????????????h?? SDRAM???????????????????????? 
					  if(para.dataclass_1_2_action_count >= MAX_LOGO_ACTION_MONTH) continue;//20170510
			    	Tp_Time = System_Run_Cycle*(0xffffffff/100) + time1ms_count_forrun/100;
				    Tp_Time = System_Runtime_Lasttime-Tp_Time;
				    memcpy((void*)(logodata_sdrambuffer_addr_arry[Tp_field]+(para.dataclass_1_2_action_count%16)*4*2048+4),(void *)(&Tp_Time),4);
				   // *(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(para.dataclass_1_2_action_count%16)*4*2048+4)=System_Runtime_Lasttime-Tp_Time;
				   //???????????????????
				
					 Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + ((para.dataclass_1_2_action_count *4)/64)*64;		
            SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
			    	break;
				case UnitPara_DATACLASS://
					  //???h??????????????????????????
					  SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],(gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048,1);//??????
					  SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],unit_data__B_nandflash_start,1);
				    break;
				case TestFinalData_DATACLASS:
//					if(LOGO_DOUBLE_VISIT != Tp_field) //????????????????????h?? SDRAM???????????????????????? 
					{ 
    					Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Fieldx_Info[Tp_field].cycle*64;		
			      //??????????
			       SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
					}
					
//				   Fieldx_Info[Tp_field].num = Fieldx_Info[Tp_field].num  + 1;
//					 if(((Fieldx_Info[Tp_field].num +1)*MAX_LOGO_FINALTEST_ONEPACKET)>0x20000)//???????? +1?????j??? ???????h????????
//					 {
//						 Fieldx_Info[Tp_field].num  =0;
//						 Fieldx_Info[Tp_field].cycle = Fieldx_Info[Tp_field].cycle +1;
//						 if((Fieldx_Info[Tp_field].cycle * 0x20000)>=gs_AreaInfo[Tp_field].space)
//						 {
//							 Fieldx_Info[Tp_field].cycle = 0;
//							 Fieldx_Info[Tp_field].loop = Fieldx_Info[Tp_field].loop  +1;
//						 }
//					 }
					  break;
				case TestAllData_DATACLASS:
			  case AnomalyRecord_DATACLASS:
//				 if(LOGO_DOUBLE_VISIT == Tp_field) continue;//????????????????????h?? SDRAM???????????????????????? 
					 Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Fieldx_Info[Tp_field].cycle*64;		
			      //??????????
			       SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
					break;
				case ActionRecord_DATACLASS:
				 	 
//				if(LOGO_DOUBLE_VISIT == Tp_field) //???�?????�
//				{
//					Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Fieldx_Info[Tp_field].cycle*64;		
//			    NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
//				}
//				
//				    if(systerm_error_status.bits.lse_error==1)	//??j?RTC�???
//						 {
//					*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000)=0xEF;
//					*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+1)=0;
//			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+2)=0;
//			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+3)=0;
//			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+4)=0x99; 
//			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+5)=0x99;
//          *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+6)=0x99;
//				 
//						 }
//						 else
//						 {
//				  *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+((Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000)) = 0xEF;
//					*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+((Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+1))=(timeandday_now.year);
//			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+((Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+2))=(timeandday_now.month);
//			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+((Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+3))=(timeandday_now.day);
//			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+((Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+4))=(timeandday_now.hour); 
//			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+((Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+5))=(timeandday_now.minute);
//          *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+((Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+6))=(timeandday_now.second);
//						 }
					/*		 for(Tp_i =0; Tp_i<(FIELD_str->Field_Number[Tp_field]/2 + FIELD_str->Field_Number[Tp_field]%2) ;Tp_i++)
				 	   {
							 *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(sdram_TestFinalNumx*MAX_LOGO_ALLTEST_ONEPACKET)%0x20000+Tp_i+6) = 0xff;
						 }*/
						 //?????????  ???????
						
						
						 Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Fieldx_Info[Tp_field].cycle*64;		
			      SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
						
						 
//						 Fieldx_Info[Tp_field].num = Fieldx_Info[Tp_field].num + 1;
//					 if(((Fieldx_Info[Tp_field].num+1)*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))>0x20000)//???????? +1?????j??? ???????h????????
//					 {
//						 //????????????
//						 
//						 Fieldx_Info[Tp_field].num =0;
//						 Fieldx_Info[Tp_field].cycle = Fieldx_Info[Tp_field].cycle +1;
//						 if((Fieldx_Info[Tp_field].cycle * 0x20000)>=gs_AreaInfo[Tp_field].space)
//						 {
//							 Fieldx_Info[Tp_field].cycle = 0;
//							 Fieldx_Info[Tp_field].loop = Fieldx_Info[Tp_field].loop   +1;
//						 }
//						// Clear_sdram(1<<(Tp_field+8));//AREANO
//						  // Tp_field_addr = FIELD_str->Field_Addr[Tp_field]/2048 + sdram_TestFinalCyclex*64;		
//			        // NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_field_addr,1);
//					 }
//					 
				  break;
				default:
					break;
					
		  }
			}
		
			
				 *(uint8_t *)(BaseData_ARR+BASE_data_PowerOffYear*9 +8) = 8;
				 *(uint8_t *)(BaseData_ARR+BASE_data_PowerOffMonth*9 +8) = 8;
			   if(systerm_error_status.bits.lse_error==1)
				 {
					 para.last_poweroff_year = 0X99;//20170924
									 para.last_poweroff_month = 0X99;//20170924
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
				SDRAM_TO_NANDFLASH(logodata_basedata_BUFFER,baseA_data__nandflash_start,1);
				 SDRAM_TO_NANDFLASH(logodata_basedata_BUFFER,baseB_data__nandflash_start,1);
//		for(Tp_field = 0; Tp_field < 16;Tp_field++)
//    {
//			switch(gs_AreaInfo[Tp_field].type)
//		  {
//				case Base_DATACLASS:
//					//if(Tp_field == 0)
//				     
//					   break;
//				case ActionNumber_DATACLASS:
//					   Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + ((para.dataclass_1_2_action_count*4)/64)*64;		
//       // NANDFLASH_TO_SDRAM_RANDOM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,logodata_ActionNumber_SIZE,4);//???????????
//			       SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
//			
//				
//					   //Tp_addr = FIELD_str->Field_Addr[Tp_field]/2048 + sdram_actionindex*4;
//				    // SDRAM_TO_NANDFLASH_RANDOM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,logodata_ActionNumber_SIZE,4);
//					  break;
//				case ActionTimers_DATACLASS:
//					 
//			    	Tp_Time = System_Run_Cycle*(0xffffffff/100) + time1ms_count_forrun/100;
//				    *(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(para.dataclass_1_2_action_count%16)*4*2048+4)=System_Runtime_Lasttime-Tp_Time;
//				   //????????????
//				
//					 Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + ((para.dataclass_1_2_action_count*4)/64)*64;		
//            SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
//			    	break;
//				case UnitPara_DATACLASS:
//					  SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],(gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048,1);//??????
//					  break;
//				case TestFinalData_DATACLASS:
//					   Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Fieldx_Info[Tp_field].cycle*64;		
//			      //??????
//			       SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
//					  break;
//				case TestAllData_DATACLASS:
//			  case AnomalyRecord_DATACLASS:
//				case ActionRecord_DATACLASS:
//					 Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Fieldx_Info[Tp_field].cycle*64;		
//			      //??????
//			       SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
//					break;
//				default:
//					break;
//					
//		  }
//		}	
//		//	 Base_DATACLASEE
//		
//		
//		memcpy((uint8_t*)(logodata_field0_BUFFER +Basedata_copy0),(uint8_t*)(logodata_field0_BUFFER),logodata_Basedata_SIZE);
//		memcpy((uint8_t*)(logodata_field0_BUFFER +Basedata_copy1),(uint8_t*)(logodata_field0_BUFFER),logodata_Basedata_SIZE);
//		memcpy((uint8_t*)(logodata_field0_BUFFER +Basedata_copy2),(uint8_t*)(logodata_field0_BUFFER),logodata_Basedata_SIZE);
//		
//		SDRAM_TO_NANDFLASH(logodata_field0_BUFFER,base_data__nandflash_start,1);//??????
//	
	
	//	LOGOSAVE_FLAG = 0;
}

static void Creste_new_backup_file_name_5(uint8_t *x_name)
{
  static uint8_t file_sub=0;
//	static uint8_t Tp_i=0;
//	uint8_t *Tp_point;
	
	file_sub = 0;
	memset(x_name,0,60);
	x_name[file_sub++]='3';
	x_name[file_sub++]=':';
	x_name[file_sub++]='/';
	x_name[file_sub++]='B';
	x_name[file_sub++]='E';
	x_name[file_sub++]='L';
	x_name[file_sub++]='M';
	x_name[file_sub++]='O';
	x_name[file_sub++]='N';
	x_name[file_sub++]='T';
	x_name[file_sub++]='_';
	x_name[file_sub++]='B';
	x_name[file_sub++]='A';
	x_name[file_sub++]='C';
	x_name[file_sub++]='K';
	x_name[file_sub++]='U';
	x_name[file_sub++]='P';
	x_name[file_sub++]='/';
	x_name[file_sub++]='B';
	x_name[file_sub++]='A';
	x_name[file_sub++]='C';
	x_name[file_sub++]='K';
	x_name[file_sub++]='U';
	x_name[file_sub++]='P';
//	x_name[file_sub++]='_';
//	for(Tp_i = 0;Tp_i<8;Tp_i++)
//	{
//						if(((*(uint8_t*)(logodata_sdrambuffer_addr_arry[0]+IDNumber_High_index*9+1+Tp_i)>='A')&&
//							 (*(uint8_t*)(logodata_sdrambuffer_addr_arry[0]+IDNumber_High_index*9+1+Tp_i)<='Z'))||
//						   ((*(uint8_t*)(logodata_sdrambuffer_addr_arry[0]+IDNumber_High_index*9+1+Tp_i)>='0')&&
//							 (*(uint8_t*)(logodata_sdrambuffer_addr_arry[0]+IDNumber_High_index*9+1+Tp_i)<='9')))
//						{
//							x_name[file_sub++] = *(uint8_t*)(logodata_sdrambuffer_addr_arry[0]+IDNumber_High_index*9+1+Tp_i);
//						}
//						else
//						{
//							break;
//						}
//	}
//	for(Tp_i = 0;Tp_i<8;Tp_i++)
//	{
//						if(((*(uint8_t*)(logodata_sdrambuffer_addr_arry[0]+IDNumber_Low_index*9+1+Tp_i)>='A')&&
//							 (*(uint8_t*)(logodata_sdrambuffer_addr_arry[0]+IDNumber_Low_index*9+1+Tp_i)<='Z'))||
//						   ((*(uint8_t*)(logodata_sdrambuffer_addr_arry[0]+IDNumber_Low_index*9+1+Tp_i)>='0')&&
//							 (*(uint8_t*)(logodata_sdrambuffer_addr_arry[0]+IDNumber_Low_index*9+1+Tp_i)<='9')))
//						{
//							x_name[file_sub++] = *(uint8_t*)(logodata_sdrambuffer_addr_arry[0]+IDNumber_Low_index*9+1+Tp_i);
//						}
//						else
//						{
//							break;
//						}
//	}
//	x_name[file_sub++]='_';
//	
//	Tp_point = (uint8_t *)&(timeandday_now);
//	for(Tp_i = 0;Tp_i<6;Tp_i++)
//	{
//			DIVIDED_DATA_HEX2BYTE(*(Tp_point++)  ,(uint8_t *)(x_name+file_sub+Tp_i*2));
//						
//	}
//	
//	file_sub = file_sub + 12;
	x_name[file_sub++]='.';
	x_name[file_sub++]='T';
	x_name[file_sub++]='X';
	x_name[file_sub++]='T';
}


static void Creste_new_backup_file_name_7(uint8_t *x_name)
{
  static uint8_t file_sub=0;
  uint8_t Tp_i=0;
	uint8_t *Tp_point;
	
	file_sub = 0;
	memset(x_name,0,60);
	x_name[file_sub++]='3';
	x_name[file_sub++]=':';
	x_name[file_sub++]='/';
	x_name[file_sub++]='B';
	x_name[file_sub++]='E';
	x_name[file_sub++]='L';
	x_name[file_sub++]='M';
	x_name[file_sub++]='O';
	x_name[file_sub++]='N';
	x_name[file_sub++]='T';
	x_name[file_sub++]='_';
	x_name[file_sub++]='B';
	x_name[file_sub++]='A';
	x_name[file_sub++]='C';
	x_name[file_sub++]='K';
	x_name[file_sub++]='U';
	x_name[file_sub++]='P';
	x_name[file_sub++]='/';
	x_name[file_sub++]='B';
	x_name[file_sub++]='A';
	x_name[file_sub++]='C';
	x_name[file_sub++]='K';
	x_name[file_sub++]='U';
	x_name[file_sub++]='P';
	x_name[file_sub++]='_';
	for(Tp_i = 0;Tp_i<8;Tp_i++)
	{
						if(((*(uint8_t*)(logodata_sdrambuffer_addr_arry[0]+IDNumber_High_index*9+Tp_i)>='A')&&
							 (*(uint8_t*)(logodata_sdrambuffer_addr_arry[0]+IDNumber_High_index*9+Tp_i)<='Z'))||
						   ((*(uint8_t*)(logodata_sdrambuffer_addr_arry[0]+IDNumber_High_index*9+Tp_i)>='0')&&
							 (*(uint8_t*)(logodata_sdrambuffer_addr_arry[0]+IDNumber_High_index*9+Tp_i)<='9')))
						{
							x_name[file_sub++] = *(uint8_t*)(logodata_sdrambuffer_addr_arry[0]+IDNumber_High_index*9+Tp_i);
						}
						else
						{
							break;
						}
	}
	for(Tp_i = 0;Tp_i<8;Tp_i++)
	{
						if(((*(uint8_t*)(logodata_sdrambuffer_addr_arry[0]+IDNumber_Low_index*9+Tp_i)>='A')&&
							 (*(uint8_t*)(logodata_sdrambuffer_addr_arry[0]+IDNumber_Low_index*9+Tp_i)<='Z'))||
						   ((*(uint8_t*)(logodata_sdrambuffer_addr_arry[0]+IDNumber_Low_index*9+Tp_i)>='0')&&
							 (*(uint8_t*)(logodata_sdrambuffer_addr_arry[0]+IDNumber_Low_index*9+Tp_i)<='9')))
						{
							x_name[file_sub++] = *(uint8_t*)(logodata_sdrambuffer_addr_arry[0]+IDNumber_Low_index*9+Tp_i);
						}
						else
						{
							break;
						}
	}
	x_name[file_sub++]='_';
//	
	Tp_point = (uint8_t *)&(timeandday_now_app);
	for(Tp_i = 0;Tp_i<6;Tp_i++)
	{
			DIVIDED_DATA_HEX2BYTE(*(Tp_point++)  ,(uint8_t *)(x_name+file_sub+Tp_i*2));
						
	}
//	
	file_sub = file_sub + 12;
	
	x_name[file_sub++]='.';
	x_name[file_sub++]='T';
	x_name[file_sub++]='X';
	x_name[file_sub++]='T';
}

int BackupDeviceData_5(void)
{
	FRESULT Tp_res;
	DIR tdir;
	static FATFS Tp_fatfs[2];
	static char filename_new[60]={0};
	NAND_ADDRESS_STR WriteReadAddr;
	static uint32_t Tp_i = 0;
	uint16_t  bytesToWrite;
	FRESULT res;
  static uint16_t Tp_Delay;
	uint8_t Tp_data[9]={0};
	
	//??BACKUPDATA 
//	#ifdef PROMPT_ENABLE 
//	LCD_DisplayStringLine( 30, "DOWNLOAD BACKUPDATA");
//	#endif
//	
	if(Image_Buf==0)
	{
	Image_Buf_bak = (uint8_t *)(((uint32_t )malloc(2048+64)));
	Image_Buf = Image_Buf_bak;//(uint8_t *)(((uint32_t )malloc(2048)+32)|0x80000000);
	Image_Buf = (uint8_t *)(shift_pointer((uint32_t)Image_Buf,32)+32);
	Image_Buf = (uint8_t *)((uint32_t)Image_Buf|0x80000000);
	}
	if(Image_Buf_bak==0)
	{
		return 1;
	}
	
	f_mount(0, "3:", 1);
	res = f_mount(&usb_fatfs, "3:", 1); 
	Tp_res =	f_opendir(&tdir,"3:/BELMONT_BACKUP");
	if(Tp_res == FR_NO_PATH)//?????? ??????
	{
		f_mkdir("3:/BELMONT_BACKUP");
		f_opendir(&tdir,"3:/BELMONT_BACKUP");  //?????????
	}
	
	f_mount(&Tp_fatfs[0],"",0);
	Creste_new_backup_file_name_5((uint8_t *)filename_new);
	 if( f_open(&file, (const char *)filename_new ,FA_CREATE_ALWAYS | FA_WRITE)!= FR_OK)
	//if(f_open(&file, "0:BASEDATA.CSV",FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
	  {
		      //SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
			    
			    systerm_error_status.bits.usb_canot_write_error=1;
     			return 0;
						//???????USB???
	  }
		Save_Logo();//20171019 ???SDRAM????????
		
		f_lseek(&file,0);
		// Printf_One_Line(".....BACKUPDATA.....",(uint8_t *)logodata_sdrambuffer_addr_arry[0],0,1);
		
		for(Tp_i = baseB_data__nandflash_start;Tp_i<backupdata_nandflash_start;Tp_i++)
		{
			 //IWDG_Reset();//?? ?????30S
			WriteReadAddr.Zone = 0x00;
      WriteReadAddr.Block = 0x00;
	    WriteReadAddr.Page = Tp_i;
			
			for(Tp_Delay=0;Tp_Delay<540;Tp_Delay++);//????????
			
			NAND_ReadPage( WriteReadAddr.Page ,0,Image_Buf,2048);
			res = f_write(&file,Image_Buf,2048,(void *)&bytesToWrite);
			if(res||(bytesToWrite != 2048))//????
	    {
			//	SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
				sysprintf("X command f_write=%X,bytesToWrite=%X\r\n",res,bytesToWrite);
				systerm_error_status.bits.usb_canot_write_error=1;
				break;
				//??????
			}
			
		}
		if( f_close(&file)!=FR_OK)
		{
				// SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
				systerm_error_status.bits.usb_canot_write_error = 1;
			  return 0;
		}
		
		sprintf(filename_new,"3:/BELMONT_BACKUP/0.fil");
		
		if( f_open(&file, (const char *)filename_new ,FA_CREATE_ALWAYS | FA_WRITE)!= FR_OK)
	  {
		      
			    systerm_error_status.bits.usb_canot_write_error=1;
     			return 0;
		}
		
		f_lseek(&file,0);
		
			memset(Image_Buf,0,2048);
			memcpy(Tp_data,BaseData_ARR,8);
		sprintf((char *)Image_Buf,"%03d:%02d/%02d/%02d,%02d:%02d:%02d%c%c",
			       0,HEXTODEC(Tp_data[0]),HEXTODEC(Tp_data[1]),HEXTODEC(Tp_data[2]),HEXTODEC(Tp_data[3]),
		         HEXTODEC(Tp_data[4]),HEXTODEC(Tp_data[5]),13,10);
			res = f_write(&file,Image_Buf,strlen((char *)Image_Buf),(void *)&bytesToWrite);
		
		for(Tp_i=1;Tp_i<164;Tp_i++)
		{
			memset(Image_Buf,0,2048);
			memcpy(Tp_data,BaseData_ARR+Tp_i*9,8);
			sprintf((char *)Image_Buf,"%03d:%8s%c%c",Tp_i,Tp_data,13,10);
			res = f_write(&file,Image_Buf,strlen((char *)Image_Buf),(void *)&bytesToWrite);
		}
		
		if( f_close(&file)!=FR_OK)
		{
				// SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
				systerm_error_status.bits.usb_canot_write_error = 1;
			  return 0;
		}
		
		
//	f_lseek(&file,0);//????????????
//	  f_close(&file);
//	  f_mount(NULL,"",0);
//		
//	UINT bytesWritten;
//	 f_mount(&Tp_fatfs[1],"",0);
//	f_lseek(&file,0);
//	f_open(&file,"CYW.~TXT",FA_CREATE_ALWAYS | FA_WRITE);
//	 f_write (&file, writeTextBuff, 1, (void *)&bytesWritten);
//	 f_close(&file);
//	f_unlink("CYW.~TXT");
//	 f_mount(NULL,"",0);
//		
//		delay_us(500000);//1?0kb???????
//		if(Tp_i == backupdata_nandflash_start)	
//		{
//			#ifdef PROMPT_ENABLE 
//		  LCD_DisplayStringLine( 30, "BACKUP FINISH      ");
//			#endif
//			if(flag_backup == 1)
//			{
//				LCD_DisplayStringLine( 30, "BACKUP FINISH      ");
//				flag_backup = 0;
//			}
//		}
//		else
//		{
//			#ifdef PROMPT_ENABLE 
//			LCD_DisplayStringLine( 30, "BACKUP OUTPUT ERR  ");
//			#endif
//			if(flag_backup == 1)
//			{
//				LCD_DisplayStringLine( 30, "BACKUP OUTPUT ERR  ");
//				flag_backup = 0;
//			}
//		}

    if(Image_Buf_bak)  {free(Image_Buf_bak);Image_Buf=0;Image_Buf_bak=0;}
		return 1;
}



int BackupDeviceData_7(void)
{
	FRESULT Tp_res;
	DIR tdir;
	static FATFS Tp_fatfs[2];
	static char filename_new[60]={0};
	NAND_ADDRESS_STR WriteReadAddr;
	static uint32_t Tp_i = 0;
	uint16_t  bytesToWrite;
	FRESULT res;
  static uint16_t Tp_Delay;
	uint8_t Tp_data[9]={0};
	
	
	//??BACKUPDATA 
//	#ifdef PROMPT_ENABLE 
//	LCD_DisplayStringLine( 30, "DOWNLOAD BACKUPDATA");
//	#endif
//	
	if(Image_Buf==0)
	{
	Image_Buf_bak = (uint8_t *)(((uint32_t )malloc(2048+64)));
	Image_Buf = Image_Buf_bak;//(uint8_t *)(((uint32_t )malloc(2048)+32)|0x80000000);
	Image_Buf = (uint8_t *)(shift_pointer((uint32_t)Image_Buf,32)+32);
	Image_Buf = (uint8_t *)((uint32_t)Image_Buf|0x80000000);
	}
	if(Image_Buf_bak==0)
	{
		return 1;
	}
	
	f_mount(0, "3:", 1);
	res = f_mount(&usb_fatfs, "3:", 1); 
	Tp_res =	f_opendir(&tdir,"3:/BELMONT_BACKUP");
	if(Tp_res == FR_NO_PATH)//?????? ??????
	{
		f_mkdir("3:/BELMONT_BACKUP");
		f_opendir(&tdir,"3:/BELMONT_BACKUP");  //?????????
	}
	
	f_mount(&Tp_fatfs[0],"",0);
	Creste_new_backup_file_name_7((uint8_t *)filename_new);
	 if( f_open(&file, (const char *)filename_new ,FA_CREATE_ALWAYS | FA_WRITE)!= FR_OK)
	//if(f_open(&file, "0:BASEDATA.CSV",FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
	  {
		      //SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
			    
			    systerm_error_status.bits.usb_canot_write_error=1;
     			return 0;
						//???????USB???
	  }
		Save_Logo();//20171019 ???SDRAM????????
		if(write_data_to_stm7buffer()==0) return 0;
		f_lseek(&file,0);
		 Printf_One_Line(".....BACKUPDATA.....",(uint8_t *)logodata_sdrambuffer_addr_arry[0],0,1);
		
		for(Tp_i = image_file_nandflash_start;Tp_i<image_tab__nandflash_start_stm7;Tp_i++)
		{
			
			WriteReadAddr.Page = Tp_i;
			for(Tp_Delay=0;Tp_Delay<540;Tp_Delay++);//????????
			//memset(Image_Buf,0X00,2048);
			NAND_ReadPage( WriteReadAddr.Page ,0,Image_Buf,2048);
			res = f_write(&file,Image_Buf,2048,(void *)&bytesToWrite);
		}
		
		
		for(Tp_i=0;Tp_i<65536;Tp_i++)
		{
			*(uint16_t *)(Image_Buf)=*(uint16_t *)(bmp_tab_BUFFER+Tp_i*8);
			*(uint16_t *)(Image_Buf+2)=*(uint16_t *)(bmp_tab_BUFFER+Tp_i*8+2);
			*(uint16_t *)(Image_Buf+4)=*(uint32_t *)(bmp_tab_BUFFER+Tp_i*8+4);
			res = f_write(&file,Image_Buf,6,(void *)&bytesToWrite);
		}
		
		for(Tp_i = baseB_data__nandflash_start_stm7;Tp_i<backupdata_nandflash_start_stm7;Tp_i++)
		{
			 //IWDG_Reset();//?? ?????30S
			if((Tp_i>=baseB_data__nandflash_start_stm7)&&(Tp_i<(baseB_data__nandflash_start_stm7+64)))
			{
				
				res = f_write(&file,(void *)(logodata_basedata_stm7_BUFFER+(Tp_i-baseB_data__nandflash_start_stm7)*2048),2048,(void *)&bytesToWrite);
				
			}
			else
			{
			WriteReadAddr.Zone = 0x00;
      WriteReadAddr.Block = 0x00;
	    WriteReadAddr.Page = logo_data__nandflash_start+(Tp_i-(baseB_data__nandflash_start_stm7+64));
			
			for(Tp_Delay=0;Tp_Delay<540;Tp_Delay++);//????????
			
			NAND_ReadPage( WriteReadAddr.Page ,0,Image_Buf,2048);
			res = f_write(&file,Image_Buf,2048,(void *)&bytesToWrite);
			
		 }
			if(res||(bytesToWrite != 2048))//????
	    {
			//	SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
				#ifdef  SYSUARTPRINTF 
				sysprintf("X command f_write=%X,bytesToWrite=%X\r\n",res,bytesToWrite);
				#endif
				systerm_error_status.bits.usb_canot_write_error=1;
				break;
				//??????
			}
		 
		 
		}
		if( f_close(&file)!=FR_OK)
		{
				// SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
				systerm_error_status.bits.usb_canot_write_error = 1;
			  return 0;
		}
		
		sprintf(filename_new,"3:/BELMONT_BACKUP/0.fil");
		
		if( f_open(&file, (const char *)filename_new ,FA_CREATE_ALWAYS | FA_WRITE)!= FR_OK)
	  {
		      
			    systerm_error_status.bits.usb_canot_write_error=1;
     			return 0;
		}
		
		f_lseek(&file,0);
		
			memset(Image_Buf,0,2048);
			memcpy(Tp_data,BaseData_ARR,8);
		sprintf((char *)Image_Buf,"%03d:%02d/%02d/%02d,%02d:%02d:%02d%c%c",
			       0,HEXTODEC(Tp_data[0]),HEXTODEC(Tp_data[1]),HEXTODEC(Tp_data[2]),HEXTODEC(Tp_data[3]),
		         HEXTODEC(Tp_data[4]),HEXTODEC(Tp_data[5]),13,10);
			res = f_write(&file,Image_Buf,strlen((char *)Image_Buf),(void *)&bytesToWrite);
		
		for(Tp_i=1;Tp_i<164;Tp_i++)
		{
			memset(Image_Buf,0,2048);
			memcpy(Tp_data,BaseData_ARR+Tp_i*9,8);
			sprintf((char *)Image_Buf,"%03d:%8s%c%c",Tp_i,Tp_data,13,10);
			res = f_write(&file,Image_Buf,strlen((char *)Image_Buf),(void *)&bytesToWrite);
		}
		
		if( f_close(&file)!=FR_OK)
		{
				// SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
				systerm_error_status.bits.usb_canot_write_error = 1;
			  return 0;
		}
		
		
//	f_lseek(&file,0);//????????????
//	  f_close(&file);
//	  f_mount(NULL,"",0);
//		
//	UINT bytesWritten;
//	 f_mount(&Tp_fatfs[1],"",0);
//	f_lseek(&file,0);
//	f_open(&file,"CYW.~TXT",FA_CREATE_ALWAYS | FA_WRITE);
//	 f_write (&file, writeTextBuff, 1, (void *)&bytesWritten);
//	 f_close(&file);
//	f_unlink("CYW.~TXT");
//	 f_mount(NULL,"",0);
//		
//		delay_us(500000);//1?0kb???????
//		if(Tp_i == backupdata_nandflash_start)	
//		{
//			#ifdef PROMPT_ENABLE 
//		  LCD_DisplayStringLine( 30, "BACKUP FINISH      ");
//			#endif
//			if(flag_backup == 1)
//			{
//				LCD_DisplayStringLine( 30, "BACKUP FINISH      ");
//				flag_backup = 0;
//			}
//		}
//		else
//		{
//			#ifdef PROMPT_ENABLE 
//			LCD_DisplayStringLine( 30, "BACKUP OUTPUT ERR  ");
//			#endif
//			if(flag_backup == 1)
//			{
//				LCD_DisplayStringLine( 30, "BACKUP OUTPUT ERR  ");
//				flag_backup = 0;
//			}
//		}

    if(Image_Buf_bak)  {free(Image_Buf_bak);Image_Buf=0;Image_Buf_bak=0;}
		if(logodata_basedata_stm7_BUFFER_bak==NULL) return 0;
	  else 
	  {
		  free((void *)logodata_basedata_stm7_BUFFER_bak);
		  logodata_basedata_stm7_BUFFER=0;
		  logodata_basedata_stm7_BUFFER_bak=0;
	  }
		return 1;
}

uint8_t stm7_format_handle(void)
{
	uint8_t Tp_i,Tp_j;
	uint8_t data_8byte[20]={0};
	FRESULT res;
	uint32_t numOfReadBytes_backup = 0,Tp_data;
	
	
	//f_lseek(file,0x5fe0016);
	res =f_lseek(&file,0);
	#ifdef  SYSUARTPRINTF 
	sysprintf("res=%d",res);
	#endif
	
	res =f_read(&file, (void *)data_8byte, 20, (void *)&numOfReadBytes_backup);
	
	#ifdef  SYSUARTPRINTF 
		sysprintf("res=%d",res);
		sysprintf("data_8byte=%X,%X,%X,%X,%X,%X,%X,%X\r\n",data_8byte[0],data_8byte[1],
		data_8byte[2],data_8byte[3],data_8byte[4],data_8byte[5],data_8byte[6],data_8byte[7]);
		#endif
	
	if((res!=0)||
		(memcmp((void *)data_8byte,".....BACKUPDATA.....",20)!=0))
		{
			systerm_error_status.bits.usb_cannot_find_hexortxt = 1;
			return 0;
		}
		
		res =f_lseek(&file,0x5fe0016);
	
	
	for(Tp_i=0;Tp_i<32;Tp_i++)
	{
		res = f_read(&file, (void *)data_8byte, 8, (void *)&numOfReadBytes_backup);
		#ifdef  SYSUARTPRINTF 
		sysprintf("res=%d",res);
		sysprintf("data_8byte=%X,%X,%X,%X,%X,%X,%X,%X\r\n",data_8byte[0],data_8byte[1],
		data_8byte[2],data_8byte[3],data_8byte[4],data_8byte[5],data_8byte[6],data_8byte[7]);
		#endif
		memcpy((void *)(BaseData_ARR+Tp_i*9),data_8byte,8);
		*(uint8_t *)(BaseData_ARR+Tp_i*9 + 8)=0;
		for(Tp_j=0;Tp_j<8;Tp_j++)
		{
			if(*(uint8_t *)(data_8byte + Tp_j)!=0)
			{
				*(uint8_t *)(BaseData_ARR+Tp_i*9 + 8)=*(uint8_t *)(BaseData_ARR+Tp_i*9 + 8)+1;
			}
		}
		
	}
	
	for(Tp_i=32;Tp_i<96;Tp_i++)
	{
		res = f_read(&file, (void *)data_8byte, 4, (void *)&numOfReadBytes_backup);
		#ifdef  SYSUARTPRINTF 
		sysprintf("res=%d",res);
		sysprintf("data_8byte=%X,%X,%X,%X\r\n",data_8byte[0],data_8byte[1],
		data_8byte[2],data_8byte[3]);
		#endif
		Tp_data = data_8byte[0] + (data_8byte[1]<<8)+(data_8byte[2]<<16)+(data_8byte[3]<<24);
		#ifdef  SYSUARTPRINTF 
		sysprintf("Tp_data=%08X",Tp_data);
		#endif
		switch((Tp_i-32)/16)
		{
			case 0:
				sprintf((void *)(BaseData_ARR+(Tp_i-32)*4+32),"%08X",Tp_data);
				break;
			case 1:
				sprintf((void *)(BaseData_ARR+(Tp_i-32)*4+32+1),"%08X",Tp_data);
				break;
			case 2:
				sprintf((void *)(BaseData_ARR+(Tp_i-32)*4+32+3),"%08X",Tp_data);
				break;
			case 3:
				sprintf((void *)(BaseData_ARR+(Tp_i-32)*4+32+2),"%08X",Tp_data);
				break;
			default:
				break;
		}
	}
	
	
	res =f_close(&file);
	#ifdef  SYSUARTPRINTF 
	sysprintf("res=%d",res);
	#endif
}


int RestoreDeviceData(void)
{
	FRESULT res;
static char filename_new[60]={0};
NAND_ADDRESS_STR WriteReadAddr;
static uint32_t Tp_i = 0;
static uint32_t numOfReadBytes_backup = 0;
DIR tdir;
	FILINFO fno;
uint8_t file_format=2;//0-new style, 1=old stm file, 2-fail

Creste_new_backup_file_name_7((uint8_t *)filename_new);

//static uint8_t Tp_no3[9]={0},Tp_no4[9]={0};
f_mount(0, "3:", 1);
	res = f_mount(&usb_fatfs, "3:", 1); 
	res =	f_opendir(&tdir,"3:/BELMONT_BACKUP");
	if(res == FR_NO_PATH)//?????? ??????
	{
		//f_mkdir("3:/BELMONT_BACKUP");
		//f_opendir(&tdir,"3:/BELMONT_BACKUP");  //?????????
	  systerm_error_status.bits.usb_cannot_find_hexortxt = 1;
		#ifdef  SYSUARTPRINTF 
		sysprintf("f_opendir error\r\n");
		#endif
		return 0;
	}
	
	//f_mount(&Tp_fatfs[0],"",0);
	
	for (;;) 
			 {
				  
				   if(((_ohci->HcRhPortStatus[0]&0x01)==0)&&((_ohci->HcRhPortStatus[1]&0x01)==0))
			     {
				      #ifdef  SYSUARTPRINTF 
		          sysprintf("USB disconnect\r\n");
		           #endif
						 break;
			     }
				   res = f_readdir(&tdir, &fno);
           if (res != FR_OK || fno.fname[0] == 0) 
					 {
						    #ifdef  SYSUARTPRINTF 
		          sysprintf("dir end\r\n");
		           #endif
						 break;
					 }
           if (fno.fname[0] == '.') continue;
				   if (fno.fattrib & AM_DIR) 
          {
             #ifdef  SYSUARTPRINTF 
		          sysprintf("dir find\r\n");
		           #endif
						continue;
          } 
          else 
          {
						if(memcmp((char *)fno.fname,"BACKUP",6)==0)//×Ö·ûÆ¥Åä
						{
							if((strstr((char *)fno.fname, ".txt")) || (strstr((char *)fno.fname, ".TXT")))//×Ö·û´®Æ¥Åä
              {
								 #ifdef  SYSUARTPRINTF 
		             sysprintf("strlen(fno.fname)=%d\r\n",strlen(fno.fname));
		              #endif
								memset(filename_new+18,0,sizeof(filename_new)-18);//3:/BELMONT_BACKUP/
								memcpy(filename_new+18,fno.fname,strlen(fno.fname));
							   res = f_open(&file, filename_new, FA_OPEN_EXISTING | FA_READ);
								 #ifdef  SYSUARTPRINTF 
		             sysprintf("res=%d\r\n",res);
								 sysprintf("filename_new=%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\r\n",
								filename_new[0],filename_new[1],filename_new[2],filename_new[3],filename_new[4],filename_new[5],
								filename_new[6],filename_new[7],filename_new[8],filename_new[9],filename_new[10],
								filename_new[11],filename_new[12],filename_new[13],filename_new[14],filename_new[15],
								filename_new[16],filename_new[17],filename_new[18],filename_new[19],filename_new[20],
								filename_new[21],filename_new[22],filename_new[23],filename_new[24],filename_new[25],
								filename_new[26],filename_new[27],filename_new[28],filename_new[29],filename_new[30],
								filename_new[31],filename_new[32],filename_new[33],filename_new[34],filename_new[35],
								filename_new[36],filename_new[37],filename_new[38],filename_new[39],filename_new[40],
								filename_new[41],filename_new[42],filename_new[43],filename_new[44],filename_new[45],
								filename_new[46],filename_new[47],filename_new[48],filename_new[49],filename_new[50],
								filename_new[51],filename_new[52],filename_new[53],filename_new[54],filename_new[55],
								filename_new[56],filename_new[57],filename_new[58],filename_new[59]);
		              #endif
								if(res == FR_OK)
									{
                     #ifdef  SYSUARTPRINTF 
		             sysprintf("strlen(fno.fname)=%d\r\n",strlen(fno.fname));
		              #endif
										if((strlen(fno.fname)>=24)&&(strlen(fno.fname)<=40)) 
										 {
										     file_format = 1;
											  sysprintf("file_format=1\r\n");
										 }
										 else if(strlen(fno.fname)==10)
										 {
											 //systerm_error_status.bits.usb_cannot_find_hexortxt = 1;
											 file_format = 0;
											 //f_close(&file);
											 //return 0;
										 }
								  }


                  
									break;
								 
							}
						}
					}
				   
			 }
	

  if(file_format==1)
	{
		//memcpy(Tp_no3,BaseData_ARR[3],9);
		//memcpy(Tp_no4,BaseData_ARR[4],9);
		if(logodata_basedata_stm7_BUFFER_bak==NULL)
		{
		logodata_basedata_stm7_BUFFER = (uint32_t )malloc((0x20000)+64);
	logodata_basedata_stm7_BUFFER_bak = logodata_basedata_stm7_BUFFER;
		}
	
	if(logodata_basedata_stm7_BUFFER_bak==NULL) 
	{
		f_close(&file);
		return 0;
	}
	//logodata_sdrambuffer_addr_arry_bak[0]=logodata_basedata_BUFFER;
	logodata_basedata_stm7_BUFFER = 32+shift_pointer(logodata_basedata_BUFFER,32);
	logodata_basedata_stm7_BUFFER = logodata_basedata_BUFFER|0X80000000;
		
	res = 	f_lseek(&file,0x5fe0016);
	if(res)
	{
		return 0;
	}
	
	for(Tp_i = baseB_data__nandflash_start_stm7;Tp_i<backupdata_nandflash_start_stm7;Tp_i++)
	{
		
		WriteReadAddr.Block =0;
		WriteReadAddr.Zone  =0; 
		WriteReadAddr.Page  =baseB_data__nandflash_start+(Tp_i-baseB_data__nandflash_start_stm7);
		
		if(WriteReadAddr.Page%64==0)
		{
		   //NAND_EraseBlock(WriteReadAddr.Page/64);
			if(Tp_i==baseB_data__nandflash_start_stm7)
			{
				
				sysprintf("f_read1\r\n");
			res = f_read(&file, (void *)logodata_basedata_stm7_BUFFER, 2048*64, (void *)&numOfReadBytes_backup);	
			sysprintf("f_read2\r\n");
				read_data_from_stm7buffer();	
			}
			else
			{
			res = f_read(&file, (void *)logodata_basedata_BUFFER, 2048*64, (void *)&numOfReadBytes_backup);
				
			}
			
		  if((numOfReadBytes_backup != 2048*64) || (res != FR_OK))//???????
		  {
			    break;
		  }
			SDRAM_TO_NANDFLASH(logodata_basedata_BUFFER ,WriteReadAddr.Page,1);
			
			 
		}

	 //delay_ms(1);
	 //  NAND_WritePage(WriteReadAddr.Page,0,Image_Buf, 2048);
		
	}
	
	
	if(logodata_basedata_stm7_BUFFER_bak==NULL) 
	{
		f_close(&file);
		return 0;
	}
	else 
	{
		free((void *)logodata_basedata_stm7_BUFFER_bak);
		logodata_basedata_stm7_BUFFER=0;
		logodata_basedata_stm7_BUFFER_bak=0;
	}
	
	if( f_close(&file)!=FR_OK)
	{
			// SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
			systerm_error_status.bits.usb_canot_write_error = 1;
		  return 0;
	}
	
	if(Tp_i == backupdata_nandflash_start_stm7) 
	{
		//memcpy(BaseData_ARR[3],Tp_no3,9);
		//memcpy(BaseData_ARR[4],Tp_no4,9);
		SDRAM_DATA_INIT();
    LOGO_DATA_OUT_FLAG =0;
		return 1;
	}
	else
	{
		return 0;
	}
	
	
	
  }
	
	
	if(file_format==0)//5 inch
	{
	res = f_lseek(&file,0);
	
	if(res)
	{
		return 0;
	}
		
	for(Tp_i = baseB_data__nandflash_start;Tp_i<backupdata_nandflash_start;Tp_i++)
	{
		
		WriteReadAddr.Block =0;
		WriteReadAddr.Zone  =0; 
		WriteReadAddr.Page  =Tp_i;
		
		if(WriteReadAddr.Page%64==0)
		{
		   //NAND_EraseBlock(WriteReadAddr.Page/64);
			
			res = f_read(&file, (void *)logodata_basedata_BUFFER, 2048*64, (void *)&numOfReadBytes_backup);
		  if((numOfReadBytes_backup != 2048*64) || (res != FR_OK))//???????
		  {
			    break;
		  }
			SDRAM_TO_NANDFLASH(logodata_basedata_BUFFER ,WriteReadAddr.Page,1);
			
			 
		}

	 //delay_ms(1);
	 //  NAND_WritePage(WriteReadAddr.Page,0,Image_Buf, 2048);
		
	}
	
	if( f_close(&file)!=FR_OK)
	{
			// SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
			systerm_error_status.bits.usb_canot_write_error = 1;
		  return 0;
	}
	
	if(Tp_i == backupdata_nandflash_start) 
	{
		//memcpy(BaseData_ARR[3],Tp_no3,9);
		//memcpy(BaseData_ARR[4],Tp_no4,9);
		SDRAM_DATA_INIT();
    LOGO_DATA_OUT_FLAG =0;
		return 1;
	}
	else
	{
		return 0;
	}
	
	}
	
	
	 systerm_error_status.bits.usb_cannot_find_hexortxt = 1;
	 #ifdef  SYSUARTPRINTF 
		    sysprintf("find file error\r\n");
		#endif
			
   return 0;
}
