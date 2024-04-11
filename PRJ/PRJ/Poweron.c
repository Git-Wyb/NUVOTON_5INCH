
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
#include "poweroff.h"
#include "display.h"
#include "lcd.h"
#include "Command_E.h"
#include "W25Q128.h"

static uint8_t vbat_ad_cnt=0;
static uint16_t vbat_ad_sum=0;
static uint8_t  vbat_average_ad=0;
uint8_t vbat_pwr_on_ad_finish=0;
extern uint8_t LOGO_DATA_OUT_FLAG;
uint8_t  BATTERY_ADCODE ;
extern SYSTEMERRORSTATUS_TYPE  systerm_error_status;
extern RTC_TIME_DATA_T pwr_on_time_ground,timeandday_now_ground;
extern TIME_TYPE       pwr_on_time_app,timeandday_now_app;
extern AreaConfig gs_AreaInfo[16];
extern PARA_TYPE para;
extern uint32_t logodata_sdrambuffer_addr_arry[16];
extern Dataclass4567_Info Fieldx_Info[16];
extern struct POWER_ON_RECORD_TYPE POWER_ON_RECORD_Str;
void AD_init_vbat(void);
void AD_init_8V(void);
extern uint32_t logodata_basedata_BUFFER;
extern uint8_t* BaseData_ARR;
extern volatile uint32_t  time1ms_count_forrun ;
extern uint32_t  time1ms_count_forrun_his ;
extern uint16_t System_Run_Cycle;
uint8_t RTC_read_timer;
extern  volatile UINT32 time1ms_count;
extern uint32_t Temp_logodata_BUFFER;
extern uint32_t logodata_basedata_BUFFER,logodata_field1_BUFFER,logodata_field2_BUFFER,logodata_field3_BUFFER,logodata_field4_BUFFER,\
          logodata_field5_BUFFER,logodata_field6_BUFFER,logodata_field7_BUFFER,logodata_field8_BUFFER,logodata_field9_BUFFER,\
					logodata_field10_BUFFER,logodata_field11_BUFFER,logodata_field12_BUFFER,logodata_field13_BUFFER,logodata_field14_BUFFER,\
					logodata_field15_BUFFER;
extern 	uint32_t System_Runtime_Lasttime;
extern uint8_t *bmpBuf_kkk,*bmpBuf_kkk_bak;
extern uint8_t *Image_Buf,*Image_Buf_bak;
extern uint32_t logodata_sdrambuffer_addr_arry_bak[16];
//extern uint8_t *Tp_Image_Buf;

char SOFT_VERSION[] = "TBNUVA04";
 uint8_t flag_AB=0;

uint8_t LOGO_ERR = 0;



uint8_t DECTOHEX(uint8_t x_data);

void RTC_ground_to_app(RTC_TIME_DATA_T *ground,TIME_TYPE *app)
{
	if(systerm_error_status.bits.lse_error==0)
	{
	
	app->year = DECTOHEX(ground->u32Year - RTC_YEAR2000);
	app->month = DECTOHEX(ground->u32cMonth);
	app->day  = DECTOHEX(ground->u32cDay);
	app->hour = DECTOHEX(ground->u32cHour);
	app->minute = DECTOHEX(ground->u32cMinute);
	app->second = DECTOHEX(ground->u32cSecond);
  if((app->year>0x99)||(app->month>0x12)||(app->day>0x31)||(app->hour>0x23)||(app->minute>0x59)||(app->second>0x59))
	{
		app->year = 0;
		app->month = 0;
		app->day = 0;
		app->hour = 0x99;
		app->minute = 0x99;
		app->second = 0x99;
		systerm_error_status.bits.lse_error=1;
	}
		
	}
	else
	{
		app->year = 0;
		app->month = 0;
		app->day = 0;
		app->hour = 0x99;
		app->minute = 0x99;
		app->second = 0x99;
	}
}

uint8_t get_vbat_ad_value(void)
{
   uint32_t Tp_data_vbat=0;
	static uint8_t  Tp_value ;
  uint8_t Tp_count=0;
		
		       //  delay_ms(2000);
      //while((inpw(REG_ADC_ISR)&ADC_ISR_VBF)==0);
//		//{
	    REG_OPERATE(REG_ADC_CTL,ADC_CTL_MST,set);
	        while((inpw(REG_ADC_ISR)&ADC_ISR_MF)==0);
						outpw(REG_ADC_ISR,ADC_ISR_MF);
	
		       if((inpw(REG_ADC_ISR)&ADC_ISR_VBF))
		      {
			        Tp_data_vbat =inpw(REG_ADC_VBADATA)  ;
						 //Tp_data_vbat = Tp_data_vbat/2;
			      //REG_OPERATE(REG_ADC_ISR,ADC_ISR_VBF,set);
			        outpw(REG_ADC_ISR,inpw(REG_ADC_ISR));
						
						#ifdef  SYSUARTPRINTF  
			         sysprintf("Tp_data_vbat=%x\n\r",Tp_data_vbat);
						#endif
				 
				   // while((inpw(REG_ADC_ISR)&ADC_ISR_MF)==0);
		       //REG_OPERATE(REG_ADC_ISR,ADC_ISR_MF,set);
		          
		       }
	
	      for(Tp_count=0;Tp_count<100;Tp_count++)
	     {
	        REG_OPERATE(REG_ADC_CTL,ADC_CTL_MST,set);
	        while((inpw(REG_ADC_ISR)&ADC_ISR_MF)==0);
						outpw(REG_ADC_ISR,ADC_ISR_MF);
	
		       if((inpw(REG_ADC_ISR)&ADC_ISR_VBF))
		      {
			        Tp_data_vbat =inpw(REG_ADC_VBADATA) + Tp_data_vbat ;
						 Tp_data_vbat = Tp_data_vbat/2;
			      //REG_OPERATE(REG_ADC_ISR,ADC_ISR_VBF,set);
			        outpw(REG_ADC_ISR,inpw(REG_ADC_ISR));
						
						#ifdef  SYSUARTPRINTF  
			         sysprintf("Tp_data_vbat=%x\n\r",Tp_data_vbat);
						#endif
				 
				   // while((inpw(REG_ADC_ISR)&ADC_ISR_MF)==0);
		       //REG_OPERATE(REG_ADC_ISR,ADC_ISR_MF,set);
		          
		       }
				 }
			 
	if(Tp_data_vbat<500)  //约1.7V
	{
		systerm_error_status.bits.lse_error=1;//402  不装电池要回402 403
		systerm_error_status.bits.vbat_error=1;
		systerm_error_status.bits.rtc_no_bat_after_no_set=1;
		Tp_data_vbat = 0;
	}
	else
	{
		//if(systerm_error_status.bits.lse_error==1) rtc_time_deinit();
		systerm_error_status.bits.vbat_error=0;  //lse_error 402 403 上电只检测一次，后面通信启动AD也不改变；
	}

					 
	Tp_value = Tp_data_vbat>>4;//STM ad[1]/16  2^4
					 
    Tp_value = (Tp_value*3)/4;//STM VREF-3.3V,XINTANG VREF - 2.5V  2.5/3.3=0.75 
					 
	return 	Tp_value;			 
}

void ReadAreaConfig_new(void)
{
	  uint16_t Tp_i;
	  int  Tp_data;
	  //uint8_t Tp_j;
//	  static  uint8_t Tp_value[8];
	
	for(Tp_i=0;Tp_i<64;Tp_i++)
	{
		//sprintf(BaseData_Str->areaData[32+Tp_i].data,"%08X",*(int *)(gs_AreaInfo+Tp_i*4));
		// printf((int *)(gs_AreaInfo+Tp_i*4),"0x%8S",BaseData_Str->areaData[32+Tp_i].data);
		Tp_data = CharToHex((char *)(BaseData_ARR+(32+Tp_i)*9));
		
		 
		switch(Tp_i%4)
		{
		  case 0:				
			gs_AreaInfo[Tp_i/4].addr = Tp_data;
			break;
			case 1:				
			gs_AreaInfo[Tp_i/4].space = Tp_data;
			break;
			case 2:				
			gs_AreaInfo[Tp_i/4].type = Tp_data;
			break;
			case 3:				
			gs_AreaInfo[Tp_i/4].size = Tp_data;
			break;
			default:
			break;
		}
	}
}
void para_default(void)
{
   
     	memset((char *)&para,0,sizeof(para));
    	para.touch_sensibility=1;
	para.lcd_back_light.brightness=128;
	para.lcd_back_light.state=0; 
	para.rtc_coarse_value=0x00;
	para.no_vbat_pwr_on_cnt=0x00;
	
	para.flag=0xA5;
    //write_para();
	
	sprintf((char *)(BaseData_ARR+99*9),"%08X", para.touch_sensibility);
	sprintf((char *)(BaseData_ARR+98*9),"%08X",para.lcd_back_light.brightness);
	sprintf((char *)(BaseData_ARR+103*9),"%08X",para.lcd_back_light.state);
	sprintf((char *)(BaseData_ARR+102*9),"%08X",para.rtc_coarse_value);
	sprintf((char *)(BaseData_ARR+BASE_data_Dataclass12RTCPowerOffTimer*9),"%08X",para.no_vbat_pwr_on_cnt );
	
	sprintf((char *)(BaseData_ARR+BASE_data_ParaFlag*9),"%08X",para.flag);
    
}


void read_para_new(void)
{
	  para.touch_sensibility = CharToHex((char *)(BaseData_ARR+99*9));
    para.lcd_back_light.brightness = CharToHex((char *)(BaseData_ARR+98*9));
	  para.lcd_back_light.state     = CharToHex((char *)(BaseData_ARR+103*9));
	  para.rtc_coarse_value     =  CharToHex((char *)(BaseData_ARR+102*9));
	  para.no_vbat_pwr_on_cnt  = CharToHex((char *)(BaseData_ARR+BASE_data_Dataclass12RTCPowerOffTimer*9));
	  para.dataclass_1_2_action_count = CharToHex((char *)(BaseData_ARR+BASE_data_actioncount*9));
	  para.flag                = CharToHex((char *)(BaseData_ARR+BASE_data_ParaFlag*9));
	  para.last_poweroff_year = CharToHex((char *)(BaseData_ARR+BASE_data_PowerOffYear*9));
	  para.last_poweroff_month  = CharToHex((char *)(BaseData_ARR+BASE_data_PowerOffMonth*9));
	 	
	
	  
}

void para_init(void)
{
//   uint8_t xor;
   read_para_new( );  
   //xor=CheckSum((sizeof(para)-1),(uint8_t *)&para);

   if(0xA5 != para.flag)               // 校验不等就读备份                                                                        
    {     
		para_default( );
    } 

		
}
void BASEDATA_RAM_INIT(void)
{
//  static  uint8_t Tp_cmp = 0;
  
//	void *addr;
	uint16_t Tp_i=0;
	uint32_t Tp_checksum1,Tp_checksum2;
	//BaseAreaAll *pAreaBase;
	start_basedata:	
		//Tp_cmp = memcmp((uint8_t*)(logodata_basedata_BUFFER  +Basedata_copy2),(uint8_t*)(logodata_basedata_BUFFER ),logodata_Basedata_SIZE);
	Tp_checksum1 = CharToHex((char*)(logodata_basedata_BUFFER+163*9));
	Tp_checksum2 = 0;
  for(Tp_i=0;Tp_i<163*9;Tp_i++)
  {
	  Tp_checksum2+=*(char*)(logodata_basedata_BUFFER+Tp_i);
  }
	if(Tp_checksum1 ==Tp_checksum2 )
	{
		memcpy(BaseData_ARR,(void *)(logodata_basedata_BUFFER),logodata_Basedata_SIZE);
		goto jump;
	}
	Tp_checksum1 = CharToHex((char*)(logodata_basedata_BUFFER+Basedata_copy0+163*9));
	Tp_checksum2 = 0;
  for(Tp_i=0;Tp_i<163*9;Tp_i++)
  {
	  Tp_checksum2+=*(char*)(logodata_basedata_BUFFER+Basedata_copy0+Tp_i);
  }
	if(Tp_checksum1 ==Tp_checksum2 )
	{
		memcpy(BaseData_ARR,(void *)(logodata_basedata_BUFFER+Basedata_copy0),logodata_Basedata_SIZE);
		goto jump;
	}
	Tp_checksum1 = CharToHex((char*)(logodata_basedata_BUFFER+Basedata_copy1+163*9));
	Tp_checksum2 = 0;
  for(Tp_i=0;Tp_i<163*9;Tp_i++)
  {
	  Tp_checksum2+=*(char*)(logodata_basedata_BUFFER+Basedata_copy1+Tp_i);
  }
	if(Tp_checksum1 ==Tp_checksum2 )
	{
		memcpy(BaseData_ARR,(void *)(logodata_basedata_BUFFER+Basedata_copy1),logodata_Basedata_SIZE);
		goto jump;
	}
	Tp_checksum1 = CharToHex((char*)(logodata_basedata_BUFFER+Basedata_copy2+163*9));
	Tp_checksum2 = 0;
  for(Tp_i=0;Tp_i<163*9;Tp_i++)
  {
	  Tp_checksum2+=*(char*)(logodata_basedata_BUFFER+Basedata_copy2+Tp_i);
  }
	if(Tp_checksum1 ==Tp_checksum2 )
	{
		memcpy(BaseData_ARR,(void *)(logodata_basedata_BUFFER+Basedata_copy2),logodata_Basedata_SIZE);
		goto jump;
	}
	
	systerm_error_status.bits.nandflash_error = 1;
	if(flag_AB==1)  LOGO_ERR = 1;
jump:
	  
	   ReadAreaConfig_new();
	
		if((((gs_AreaInfo[0].addr+logodata_2gbit_change_addr)/2048) != baseB_data__nandflash_start)||(gs_AreaInfo[0].space!=0x20000))
	{
//	   //memset((uint8_t *)FIELD_str,0,256);	
//		 Clear_sdram(0x100);
//		gs_AreaInfo[0].addr = 0x6000000;//base_data__nandflash_start*2048;
//		gs_AreaInfo[0].space = 0x20000;
//		
//		//memcpy((uint8_t *)(logodata_field0_BUFFER+Basedata_fieldinformation),(uint8_t*)FIELD_str,256);
//	   UPDATA_BASEDATA_MODIFYTIME();
		systerm_error_status.bits.nandflash_error = 1;
		if(flag_AB==1)  LOGO_ERR = 1;
	}
	
	if((systerm_error_status.bits.nandflash_error ==1)&&(flag_AB==0))
	{
		flag_AB=1;
		NANDFLASH_TO_SDRAM(logodata_basedata_BUFFER ,baseA_data__nandflash_start,1);
		goto start_basedata;
	}
	
	if(LOGO_ERR == 1)
	{
		for (Tp_i = 0; Tp_i < 16;Tp_i++)
	  {
		    gs_AreaInfo[Tp_i].type = 0xff;
			  gs_AreaInfo[Tp_i].addr=0;
			  gs_AreaInfo[Tp_i].space=0;
			  gs_AreaInfo[Tp_i].size=0;
			  Fieldx_Info[Tp_i].cycle = 0;
			  Fieldx_Info[Tp_i].loop = 0;
			  Fieldx_Info[Tp_i].num = 0;
			  
	  }
		
		gs_AreaInfo[0].addr = 0x6000000;//base_data__nandflash_start*2048;
		gs_AreaInfo[0].space = 0x20000;
		gs_AreaInfo[0].type = 0;
		
		W25Q128_Read();
	}
	
	ReadAreaConfig_new();
		  para_init();
	    field_info_init();
	    *(uint8_t *)(BaseData_ARR+LCD_Ver_index*9+8) = 8;
	    memcpy(BaseData_ARR+LCD_Ver_index*9, SOFT_VERSION,*(uint8_t *)(BaseData_ARR+LCD_Ver_index*9+8));
	    *(uint8_t *)(BaseData_ARR+BASE_data_ErrBlock*9+8) = 8;
			sprintf((char *)(BaseData_ARR+BASE_data_ErrBlock*9),"%08X",get_bad_block());
	
	
	if(systerm_error_status.bits.nandflash_error ==0)
	{
		
		
	
	   basedata_ram_to_sdram();
		SDRAM_TO_NANDFLASH(logodata_basedata_BUFFER ,baseA_data__nandflash_start,1);
	}
	else
	{
	
	   systerm_error_status.bits.nandflash_error = 0;//error from F9 COMMAND
	}
	
    	
	   // memcpy((void *)(&pwr_on_time),(void *)(&timeandday_now),sizeof(pwr_on_time));
			
			
}

void NANDFLASH_TO_SDRAM_LOGOBASEDATA(void)
{
	//static uint8_t Tp_data[logodata_field0_SIZE];//
	//NAND_ADDRESS WriteReadAddr;
//	uint16_t  Tp_badmanage=0; 
	uint16_t Tp_i =0;
	uint8_t  Tp_cs=0;//cs??
//	uint32_t Tp_count = 0x95000; 
	uint8_t Tp_field=0;
//	struct REALTIME_TYPE Tp_TimeAndData[1];//????????
	uint32_t Tp_addr;
	uint16_t Tp_timehis,Tp_timenow;
	uint32_t Tp_Type;//
	uint8_t Tp_rtcerr=0;//???????????????i??  ???????????????????????????????h??
	uint8_t Tp_rtcrecover = 0;//??????RTC????????
	uint8_t Tp_newmonth=0;//???h????
	uint8_t Tp_rtcpowerlow=0;
//  uint16_t Tp_data1;
	static  uint8_t Tp_rtcpower;
uint16_t Tp_month_losebat=0;

	uint32_t Tp_System_poweron_time;
	//???????CS??
	//???????????
//	NANDFLASH_TO_SDRAM(logodata_field0_BUFFER,base_data__nandflash_start,1);
//	
//	//???????????????
//	Tp_cmp = memcmp((uint8_t*)(logodata_field0_BUFFER +Basedata_copy2),(uint8_t*)(logodata_field0_BUFFER),logodata_Basedata_SIZE);
//	if(Tp_cmp !=0)
//	{
//		 Tp_cmp = memcmp((uint8_t*)(logodata_field0_BUFFER +Basedata_copy0),(uint8_t*)(logodata_field0_BUFFER+Basedata_copy1),logodata_Basedata_SIZE);
//		if(Tp_cmp == 0)
//		{
//			memcpy((uint8_t*)(logodata_field0_BUFFER),(uint8_t*)(logodata_field0_BUFFER +Basedata_copy0),logodata_Basedata_SIZE);
//		}
//		else
//		{
//			 Tp_cmp = memcmp((uint8_t*)(logodata_field0_BUFFER +Basedata_copy2),(uint8_t*)(logodata_field0_BUFFER+Basedata_copy1),logodata_Basedata_SIZE);
//		   if(Tp_cmp == 0 )
//			 {
//				 memcpy((uint8_t*)(logodata_field0_BUFFER),(uint8_t*)(logodata_field0_BUFFER +Basedata_copy1),logodata_Basedata_SIZE);
//			 }
//		}
//	}
//	
//	//??NANDFLASH????????????RAM
//	memcpy((uint8_t *)FIELD_str,(uint8_t*)(logodata_field0_BUFFER+Basedata_fieldinformation),256);
	
	//?????????????????0??'???????????L?????RAM??????????

	

	//???????????????

	//???????????????????
	//?????0???????????????????CS?????NANDFLASH ???????????
	
	//first debug basedata
	
	for(Tp_field = 1; Tp_field<16;Tp_field++)
	{
		Tp_Type = gs_AreaInfo[Tp_field].type; 
		//Tp_month_losebat=0;
		if((Tp_Type!=0xff)&&(Tp_Type!=0))
		{
			
			Temp_logodata_BUFFER = (uint32_t )malloc((0x20000)+64);
			 logodata_sdrambuffer_addr_arry_bak[Tp_field]=Temp_logodata_BUFFER;
       Temp_logodata_BUFFER = 32+shift_pointer(Temp_logodata_BUFFER,32);
	     Temp_logodata_BUFFER = Temp_logodata_BUFFER|0X80000000;
			 if(Temp_logodata_BUFFER) memset((void *)Temp_logodata_BUFFER,0,(0x20000));
			 	switch(Tp_field)
				{
					case 1:  logodata_field1_BUFFER = Temp_logodata_BUFFER;break;
					case 2:  logodata_field2_BUFFER = Temp_logodata_BUFFER;break;
					case 3:  logodata_field3_BUFFER =  Temp_logodata_BUFFER;break;
					case 4:  logodata_field4_BUFFER =  Temp_logodata_BUFFER;break;
					case 5:  logodata_field5_BUFFER =  Temp_logodata_BUFFER;break;
					case 6:  logodata_field6_BUFFER =  Temp_logodata_BUFFER;break;
					case 7:  logodata_field7_BUFFER =  Temp_logodata_BUFFER;break;
					case 8:  logodata_field8_BUFFER =  Temp_logodata_BUFFER;break;
					case 9:  logodata_field9_BUFFER =  Temp_logodata_BUFFER;break;
					case 10:  logodata_field10_BUFFER =  Temp_logodata_BUFFER;break;
					case 11:  logodata_field11_BUFFER =   Temp_logodata_BUFFER;break;
					case 12:  logodata_field12_BUFFER =   Temp_logodata_BUFFER;break;
					case 13:  logodata_field13_BUFFER =   Temp_logodata_BUFFER;break;
					case 14:  logodata_field14_BUFFER =   Temp_logodata_BUFFER;break;
					case 15:  logodata_field15_BUFFER =   Temp_logodata_BUFFER;break;
					default:break;
				}
						logodata_sdrambuffer_addr_arry[Tp_field]= Temp_logodata_BUFFER;
				 #ifdef  SYSUARTPRINTF  
				sysprintf("logodata_sdrambuffer_addr_arry[Tp_field]=%x",logodata_sdrambuffer_addr_arry[Tp_field]);
			#endif
		}
		
		
//		//这里是测试程序
//		sysprintf("logodata_sdrambuffer_addr_arry[1]=%X\r\n",logodata_sdrambuffer_addr_arry[1]);
//		memset((void *)logodata_sdrambuffer_addr_arry[1],0,(0x20000)+32);
//		*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[1])=0xab;
//		sysprintf("*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[1])=%X\r\n",*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[1]));
//		memset((void *)logodata_sdrambuffer_addr_arry[1],0,(0x20000)+32);
//			*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[1]+1)=0xab;
//		sysprintf("*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[1]+1)=%X\r\n",*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[1]+1));
//		memset((void *)logodata_sdrambuffer_addr_arry[1],0,(0x20000)+32);
//			*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[1]+2)=0xab;
//		sysprintf("*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[1]+2)=%X\r\n",*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[1]+2));
//		memset((void *)logodata_sdrambuffer_addr_arry[1],0,(0x20000)+32);
//			*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[1]+3)=0xab;
//		sysprintf("*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[1]+3)=%X\r\n",*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[1]+3));
//		memset((void *)logodata_sdrambuffer_addr_arry[1],0,(0x20000)+32);
//			*(__IO uint16_t*)(logodata_sdrambuffer_addr_arry[1])=0x1234;
//		sysprintf("*(__IO uint16_t*)(logodata_sdrambuffer_addr_arry[1])=%X\r\n",*(__IO uint16_t*)(logodata_sdrambuffer_addr_arry[1]));
//		memset((void *)logodata_sdrambuffer_addr_arry[1],0,(0x20000)+32);
//			*(__IO uint16_t*)(logodata_sdrambuffer_addr_arry[1]+1)=0x1234;
//		sysprintf("*(__IO uint16_t*)(logodata_sdrambuffer_addr_arry[1]+1)=%X\r\n",*(__IO uint16_t*)(logodata_sdrambuffer_addr_arry[1]+1));
//		memset((void *)logodata_sdrambuffer_addr_arry[1],0,(0x20000)+32);
//		*(__IO uint16_t*)(logodata_sdrambuffer_addr_arry[1]+2)=0x1234;
//		sysprintf("*(__IO uint16_t*)(logodata_sdrambuffer_addr_arry[1]+2)=%X\r\n",*(__IO uint16_t*)(logodata_sdrambuffer_addr_arry[1]+2));
//		memset((void *)logodata_sdrambuffer_addr_arry[1],0,(0x20000)+32);
//		*(__IO uint16_t*)(logodata_sdrambuffer_addr_arry[1]+3)=0x1234;
//		sysprintf("*(__IO uint16_t*)(logodata_sdrambuffer_addr_arry[1]+3)=%X\r\n",*(__IO uint16_t*)(logodata_sdrambuffer_addr_arry[1]+3));
//		memset((void *)logodata_sdrambuffer_addr_arry[1],0,(0x20000)+32);
//					*(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[1])=0x12345678;
//		sysprintf("*(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[1])=%X\r\n",*(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[1]));
//		memset((void *)logodata_sdrambuffer_addr_arry[1],0,(0x20000)+32);
//			*(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[1]+1)=0x12345678;
//		sysprintf("*(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[1]+1)=%X\r\n",*(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[1]+1));
//		memset((void *)logodata_sdrambuffer_addr_arry[1],0,(0x20000)+32);
//		*(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[1]+2)=0x12345678;
//		sysprintf("*(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[1]+2)=%X\r\n",*(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[1]+2));
//		memset((void *)logodata_sdrambuffer_addr_arry[1],0,(0x20000)+32);
//		*(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[1]+3)=0x12345678;
//		sysprintf("*(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[1]+3)=%X\r\n",*(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[1]+3));
//		memset((void *)logodata_sdrambuffer_addr_arry[1],0,(0x20000)+32);
//		while(1);
		
		switch(Tp_Type)
		{
			case ActionNumber_DATACLASS://????????????
			case ActionTimers_DATACLASS://???????????
				if(para.dataclass_1_2_action_count >= MAX_LOGO_ACTION_MONTH)//?????????????????????????
							 continue;
				//??????????????NANDFLASH?????
       
				
				Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + ((para.dataclass_1_2_action_count*4)/64)*64;		
			 NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
					
					#ifdef  SYSUARTPRINTF 
				sysprintf("logodata_sdrambuffer_addr_arry[Tp_field]=%x,%x,%x,%x",*(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]),
				*(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+1),*(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+2),
				*(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+3));
				#endif
				
				//RTC_GetTAndD((uint8_t *)Tp_TimeAndData);
			  Tp_timehis =HEXTODEC(para.last_poweroff_month)+12*HEXTODEC(para.last_poweroff_year);
			  Tp_timenow  = HEXTODEC(timeandday_now_app.year)*12  + HEXTODEC(timeandday_now_app.month);
			  if(systerm_error_status.bits.lse_error==1)//??????RTC?????????
				{
				   
           				 
					 if((para.no_vbat_pwr_on_cnt%ONE_RTC_LOSE_CYCLE)==0)//???h????????
					 {
						 //sdram_powerfailtimes =0;
						 if(Tp_rtcerr==0)
						 {
						//	 Tp_month_losebat = HEXTODEC(*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field] +(para.dataclass_1_2_action_count%16)*4*2048));
						// Tp_month_losebat = Tp_month_losebat + (*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field] +(para.dataclass_1_2_action_count%16)*4*2048+1))*100;
					Tp_month_losebat =  CharToHex((char *)(BaseData_ARR+BASE_data_Vbatlow_monthcount*9));
							 if(Tp_month_losebat>240) Tp_month_losebat = 0;
							 Tp_month_losebat = Tp_month_losebat + 1;
							 
							 sprintf((char *)(BaseData_ARR+BASE_data_Vbatlow_monthcount*9),"%08X",Tp_month_losebat);
							 
							 para.dataclass_1_2_action_count = para.dataclass_1_2_action_count+1;
						if(para.dataclass_1_2_action_count >= MAX_LOGO_ACTION_MONTH)
							 continue;
						  Tp_rtcerr = 1;
					   }
						Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + ((para.dataclass_1_2_action_count*4)/64)*64;		
			      NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
						memset((uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(para.dataclass_1_2_action_count%16)*4*2048),0xff,4*2048);//??????SDRAM???????????'?
					  //Clear_sdram(1<<(Tp_field+8));
						//Tp_month_losebat = ((para.no_vbat_pwr_on_cnt+ONE_RTC_LOSE_CYCLE)/ONE_RTC_LOSE_CYCLE);
						  
						*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field] +(para.dataclass_1_2_action_count%16)*4*2048)= (Tp_month_losebat%10)+((Tp_month_losebat%100)/10)*16;
				    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field] +(para.dataclass_1_2_action_count%16)*4*2048+1)= Tp_month_losebat/100;//???????
				    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field] +(para.dataclass_1_2_action_count%16)*4*2048+2)= 0xAA;
            *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field] +(para.dataclass_1_2_action_count%16)*4*2048+3)= 0xAA;
             #ifdef  SYSUARTPRINTF 
						 	sysprintf(
						 "AAAA=0x%x,0x%x\r\n",*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field] +(para.dataclass_1_2_action_count%16)*4*2048),
						 *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field] +(para.dataclass_1_2_action_count%16)*4*2048+1)
						 );
             #endif						 
					 }
					  Tp_rtcpowerlow = 1;
					  
				}
			  else if(Tp_timenow == Tp_timehis)//RTC?????? ???????????
				{
			    if(para.no_vbat_pwr_on_cnt!=0)//???????h????RTC?????? ????HIS??NOW?????????
					{
					   if(Tp_rtcrecover ==0)
						 {
						 
						 para.no_vbat_pwr_on_cnt = 0; 
						 para.dataclass_1_2_action_count = para.dataclass_1_2_action_count+1;
					   if(para.dataclass_1_2_action_count >= MAX_LOGO_ACTION_MONTH)
							 continue;
						 Tp_rtcrecover = 1;
					   }
						 Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + ((para.dataclass_1_2_action_count*4)/64)*64;		
			       NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
						 memset((uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(para.dataclass_1_2_action_count%16)*4*2048),0xff,4*2048);//??????SDRAM???????????'?
					  //Clear_sdram(1<<(Tp_field+8));
						*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field] +(para.dataclass_1_2_action_count%16)*4*2048)= timeandday_now_app.month;//DECTOHEX(timeandday_now.month);
				    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field] +(para.dataclass_1_2_action_count%16)*4*2048+1)=timeandday_now_app.year;//DECTOHEX(timeandday_now.year) ;//???????
				    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field] +(para.dataclass_1_2_action_count%16)*4*2048+2)= 0x00;
            *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field] +(para.dataclass_1_2_action_count%16)*4*2048+3)= 0x00;	
					}
				}
			  else //???h???????
				{
					if(Tp_newmonth == 0)
					{
					para.no_vbat_pwr_on_cnt = 0;
					para.dataclass_1_2_action_count = para.dataclass_1_2_action_count+1;
				  if(para.dataclass_1_2_action_count >= MAX_LOGO_ACTION_MONTH)
						  continue;
				  Tp_newmonth = 1;
					}
				  Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + (( para.dataclass_1_2_action_count*4)/64)*64;		
			    NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
					memset((uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+( para.dataclass_1_2_action_count%16)*4*2048),0xff,4*2048);//??????SDRAM???????????'?
					 // Clear_sdram(1<<(Tp_field+8));
					  *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field] +( para.dataclass_1_2_action_count%16)*4*2048) = timeandday_now_app.month;//DECTOHEX(timeandday_now.month);
				    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field] +( para.dataclass_1_2_action_count%16)*4*2048+1)= timeandday_now_app.year;//DECTOHEX(timeandday_now.year);//???????
				    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field] +( para.dataclass_1_2_action_count%16)*4*2048+2)= 0x00;
            *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field] +( para.dataclass_1_2_action_count%16)*4*2048+3)= 0x00;	
					
					
				}
				
				if(Tp_Type == ActionNumber_DATACLASS)//????????????????U????????-1
				{
			     //TP_view = *(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(para.dataclass_1_2_action_count%16)*4*2048+4) ;
			     //*(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(para.dataclass_1_2_action_count%16)*4*2048+4) = *(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(para.dataclass_1_2_action_count%16)*4*2048+4) - 1;//????????? NO1?????			
			   memcpy((void *)&Tp_System_poweron_time,(void *)(logodata_sdrambuffer_addr_arry[Tp_field]+(para.dataclass_1_2_action_count%16)*4*2048+4),4);
					Tp_System_poweron_time--;
					memcpy((void *)(logodata_sdrambuffer_addr_arry[Tp_field]+(para.dataclass_1_2_action_count%16)*4*2048+4),(void *)&Tp_System_poweron_time,4);
				}
				if(Tp_Type == ActionTimers_DATACLASS)//????h????e???????????? ????????????????????????????
				{
					//System_Runtime_Lasttime = *(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(para.dataclass_1_2_action_count%16)*4*2048+4);
					memcpy((void *)&System_Runtime_Lasttime,(void *)(logodata_sdrambuffer_addr_arry[Tp_field]+(para.dataclass_1_2_action_count%16)*4*2048+4),4);
				}
					#ifdef  SYSUARTPRINTF 
				sysprintf("logodata_sdrambuffer_addr_arry[Tp_field]=%x,%x,%x,%x",*(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]),
				*(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+1),*(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+2),
				*(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+3));
				#endif
			
				break;
			case UnitPara_DATACLASS:
				NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],(gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048,1);
			 for(Tp_i = 0;Tp_i <=MAX_LOGO_UINT_NUM; Tp_i++)
			 {
				 Tp_cs = Tp_cs + *(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+UINT_START1_OFFSET+Tp_i);
			 }
			 if(*(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field] + UINT_CS1_OFFSET) != Tp_cs)
			 //if(1)
			 {
				  Tp_cs = 0; 
				  for(Tp_i = 0;Tp_i <=MAX_LOGO_UINT_NUM; Tp_i++)
			    {
				     Tp_cs = Tp_cs + *(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field] + UINT_START2_OFFSET +Tp_i);
			    }
					if(*(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field] +UINT_CS2_OFFSET) != Tp_cs)
					{
						  Tp_cs = 0; 
						  for(Tp_i = 0;Tp_i <=MAX_LOGO_UINT_NUM; Tp_i++)
			        {
				         Tp_cs = Tp_cs + *(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field] + UINT_START3_OFFSET +Tp_i);
			        }
							if(*(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field] + UINT_CS3_OFFSET) != Tp_cs)
							{
								
							}
							else
							{
								 memcpy((uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]),(uint8_t *)logodata_sdrambuffer_addr_arry[Tp_field]+UINT_START3_OFFSET,MAX_LOGO_UINT_NUM+2);
							}
					}
					else
					{
						 memcpy((uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]),(uint8_t *)logodata_sdrambuffer_addr_arry[Tp_field]+UINT_START2_OFFSET,MAX_LOGO_UINT_NUM+2);
					}
			 }
			 
			  break;
			case TestFinalData_DATACLASS:
			  Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Fieldx_Info[Tp_field].cycle*64;		
			//??????????
			  NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
			  //???????????
			  
			if(systerm_error_status.bits.lse_error==1)	//??j?RTC????
			{
				  *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_FINALTEST_ONEPACKET)%0x20000)=0;
			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_FINALTEST_ONEPACKET)%0x20000+1)=0;
			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_FINALTEST_ONEPACKET)%0x20000+2)=0;
			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_FINALTEST_ONEPACKET)%0x20000+3)=0x99; 
			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_FINALTEST_ONEPACKET)%0x20000+4)=0x99;
				  *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_FINALTEST_ONEPACKET)%0x20000+5)=0x99;
    
			}
			else
			{
			  	*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_FINALTEST_ONEPACKET)%0x20000)=pwr_on_time_app.year;//DECTOHEX(timeandday_now.year);
			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_FINALTEST_ONEPACKET)%0x20000+1)=pwr_on_time_app.month;//DECTOHEX(timeandday_now.month);
			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_FINALTEST_ONEPACKET)%0x20000+2)=pwr_on_time_app.day;//DECTOHEX(timeandday_now.day);
			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_FINALTEST_ONEPACKET)%0x20000+3)=pwr_on_time_app.hour;//DECTOHEX(timeandday_now.hour); 
			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_FINALTEST_ONEPACKET)%0x20000+4)=pwr_on_time_app.minute;//DECTOHEX(timeandday_now.minute);
          *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_FINALTEST_ONEPACKET)%0x20000+5)=pwr_on_time_app.second;
			}          
			//	Tp_data1 = GET_Battery_AD_Code();//???1??
				//			TP_view = 		 Tp_data1;
					//?????????????????1/4
					Tp_rtcpower =BATTERY_ADCODE;//12bit - >8bit
					if(Tp_rtcpower <0xf) Tp_rtcpower=0;//??????	
           *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_FINALTEST_ONEPACKET)%0x20000+6)	=		Tp_rtcpower;
			     for(Tp_i =0;Tp_i<(MAX_LOGO_FINALTEST_ONEPACKET-7);Tp_i++)
			        *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_FINALTEST_ONEPACKET)%0x20000+Tp_i+7) = 0;
			    
			    
				break;
			 case TestAllData_DATACLASS:
			 case AnomalyRecord_DATACLASS:
			 
				 Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Fieldx_Info[Tp_field].cycle*64;
			   NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
          			 
				break; 
			 case ActionRecord_DATACLASS://????????
				  Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Fieldx_Info[Tp_field].cycle*64;
			    NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
			     
			    if(systerm_error_status.bits.lse_error==1)	//??j?RTC????
						 {
					*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000)=0xE0;
					*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+1)=0;
			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+2)=0;
			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+3)=0;
			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+4)=0x99; 
			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+5)=0x99;
          *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+6)=0x99;
				 
						 }
						 else
						 {
				  *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000) = 0xE0;
					*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+1)=pwr_on_time_app.year;//DECTOHEX(timeandday_now.year);
			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+2)=pwr_on_time_app.month;//DECTOHEX(timeandday_now.month);
			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+3)=pwr_on_time_app.day;//DECTOHEX(timeandday_now.day);
			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+4)=pwr_on_time_app.hour;//DECTOHEX(timeandday_now.hour); 
			    *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+5)=pwr_on_time_app.minute;//DECTOHEX(timeandday_now.minute);
          *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(Fieldx_Info[Tp_field].num*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))%0x20000+6)=pwr_on_time_app.second;//DECTOHEX(timeandday_now.second);
						 }
						 
					 //?????????  ???????
						 Fieldx_Info[Tp_field].num= Fieldx_Info[Tp_field].num + 1;
					 if(((Fieldx_Info[Tp_field].num+1)*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))>0x20000)//???????? +1?????j??? ???????h????????
					 {
						 //????????????
						 Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Fieldx_Info[Tp_field].cycle*64;		
			       SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
						 Fieldx_Info[Tp_field].num =0;
						 Fieldx_Info[Tp_field].cycle = Fieldx_Info[Tp_field].cycle +1;
						 if((Fieldx_Info[Tp_field].cycle * 0x20000)>=gs_AreaInfo[Tp_field].space)
						 {
							 Fieldx_Info[Tp_field].cycle = 0;
							 Fieldx_Info[Tp_field].loop = Fieldx_Info[Tp_field].loop  +1;
						 }
						// Clear_sdram(1<<(Tp_field+8));//AREANO
						   Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Fieldx_Info[Tp_field].cycle*64;		
			         NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
					 }	 
				 break;
			default:
				break;
		}
		//FIELD_INITED = FIELD_INITED|(1<<(Tp_field-1));
	}
	
	
	if(Tp_rtcpowerlow == 1)
	{
		 para.no_vbat_pwr_on_cnt=(para.no_vbat_pwr_on_cnt+1);
     if(para.no_vbat_pwr_on_cnt>60)	 	para.no_vbat_pwr_on_cnt=1;
	}
	sprintf((char *)(BaseData_ARR+BASE_data_Dataclass12RTCPowerOffTimer*9),"%08X",para.no_vbat_pwr_on_cnt);	
	sprintf((char *)(BaseData_ARR+BASE_data_actioncount*9),"%08X",para.dataclass_1_2_action_count );
	//??????????
	if(systerm_error_status.bits.lse_error==1)	//??j?RTC????
	{
		
		POWER_ON_RECORD_Str.Poweron_Year = 0;
		POWER_ON_RECORD_Str.Poweron_Month = 0;
		POWER_ON_RECORD_Str.Poweron_Day = 0;
		POWER_ON_RECORD_Str.Poweron_Hour=0x99;
		POWER_ON_RECORD_Str.Poweron_Minute=0x99;
		POWER_ON_RECORD_Str.Poweron_Second=0x99;
		POWER_ON_RECORD_Str.Battery_Voltage = 0;
		
	}
	else
	{
		POWER_ON_RECORD_Str.Poweron_Year = pwr_on_time_app.year;//DECTOHEX(timeandday_now.year);
		POWER_ON_RECORD_Str.Poweron_Month = pwr_on_time_app.month;//DECTOHEX(timeandday_now.month);
		POWER_ON_RECORD_Str.Poweron_Day = pwr_on_time_app.day;//DECTOHEX(timeandday_now.day);
		POWER_ON_RECORD_Str.Poweron_Hour=pwr_on_time_app.hour;//DECTOHEX(timeandday_now.hour);
		POWER_ON_RECORD_Str.Poweron_Minute=pwr_on_time_app.minute;//DECTOHEX(timeandday_now.minute);
		POWER_ON_RECORD_Str.Poweron_Second=pwr_on_time_app.second;//DECTOHEX(timeandday_now.second);
		POWER_ON_RECORD_Str.Battery_Voltage = BATTERY_ADCODE;
	}
		
	//20180815
	if(systerm_error_status.bits.lse_error==0)
	{
	para.last_poweroff_month = pwr_on_time_app.month;//DECTOHEX(timeandday_now.month);
	para.last_poweroff_year = pwr_on_time_app.year;//DECTOHEX(timeandday_now.year); //??h?e??????  I???17??1???'???
	}
	else
	{
		para.last_poweroff_month = 0x99;
	para.last_poweroff_year = 0x99; //??h?e??????  I???17??1???'???
	}
	
}

void record_pwr_on_msg(void)
{
  if(systerm_error_status.bits.lse_error==0)
  {		
	   RTC_Read(RTC_CURRENT_TIME,&pwr_on_time_ground);
		 RTC_ground_to_app(&pwr_on_time_ground,&pwr_on_time_app);
		 
	}
	else
	{
		pwr_on_time_app.year=0;
		pwr_on_time_app.month = 0;
		pwr_on_time_app.day = 0;
		pwr_on_time_app.hour = 0x99;
		pwr_on_time_app.minute = 0x99;
		pwr_on_time_app.second = 0x99;
		
	}
	
	RTC_ground_to_app(&pwr_on_time_ground,&timeandday_now_app);
}

void power_on_logo(void)
{
	     if(vbat_pwr_on_ad_finish==0)
      {
			
			vbat_ad_sum+=get_vbat_ad_value( );
			vbat_ad_cnt++;		
			if((vbat_ad_cnt>=3)&&(LOGO_DATA_OUT_FLAG==1))
				{
					
					vbat_pwr_on_ad_finish=1;
				//////	if(systerm_error_status.bits.vbat_error==1) systerm_error_status.bits.lse_error=1;
					vbat_average_ad=vbat_ad_sum/vbat_ad_cnt;
					BATTERY_ADCODE = vbat_average_ad;
					vbat_ad_sum-=vbat_average_ad;
					
//					SDRAM_DATA_INIT();
          record_pwr_on_msg( );
					
					LOGO_DATA_OUT_FLAG ++;//begin change
					
					
					NANDFLASH_TO_SDRAM_LOGOBASEDATA();
//                                  //为了减少电流，如果不关要vbat 24.５ua  

//					
//					
//					judge_dataclass_1_and_2_change_month( );  //只执行一次
//					add_action_number_data_class_pwr_on( );//开机次数
//					DataClass7PowerONRecord( );//最终数据DATCLASS 7
//					
//					add_test_final_data_class_pwr_on( );//最终数据DATCLASS 4
//					
					LOGO_DATA_OUT_FLAG ++;//over change
//////////					AD_init_8V();		
				//	LVD_disable();
				  #ifdef SYSUARTPRINTF
					sysprintf("\r\n--------------8V AD on------------------\r\n");
           #endif
				}
      }
}

void MEM_CLEAR(void)
{
	if(bmpBuf_kkk_bak!=0) {free(bmpBuf_kkk_bak);bmpBuf_kkk=0;bmpBuf_kkk_bak=0;}
	 if(Image_Buf_bak!=0)  {free(Image_Buf_bak);Image_Buf=0;Image_Buf_bak=0;}
	//if(Tp_Image_Buf) free(Tp_Image_Buf);
}


void LOGO_handle(void)
{
	 //static  uint32_t Tp_time=0;
	 //static  uint16_t  Tp_month=0xffff;
	 //static  uint8_t   Tp_rtcpower =1;
//	static  uint16_t   Tp_last_status= 0xffff;
//	static  uint16_t   Tp_New_status = 0xffff;
	
	
	if(get_timego(RTC_read_timer)>=1000)
	{
	
		RTC_Read(RTC_CURRENT_TIME,&timeandday_now_ground);
		RTC_read_timer = time1ms_count;
		RTC_ground_to_app(&timeandday_now_ground,&timeandday_now_app);
	//	sysprintf("f_mkdir=%X\r\n",timeandday_now);
	}
	
	

	
//	first debug basedata
//	
	//time1ms_count_forrun = time1ms_count;
  if(time1ms_count_forrun_his != time1ms_count_forrun)//???????????
	{
	  if(time1ms_count_forrun_his > time1ms_count_forrun)
		 {
			 System_Run_Cycle++;//???h???????  
		 } 
		time1ms_count_forrun_his = time1ms_count_forrun;
		 
	}
//	
//	
//	
    LOGO_CLEAR();//???LOGO???? ????????????????????
    MEM_CLEAR();
	
}










