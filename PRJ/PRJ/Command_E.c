

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "sys.h"
#include "Aprotocol.h"
#include "Command_P.h"
#include "nand_nand.h"
#include "Command_all.h"

extern uint16_t FIELD_ERASE_STEP;//????????
extern uint16_t FIELD_ERASE_FLAG ;//??????? bit0 basedata bit1 field1
extern PARA_TYPE para;
extern uint8_t *BaseData_ARR;
extern SYSTEMERRORSTATUS_TYPE  systerm_error_status;
extern uint32_t logodata_sdrambuffer_addr_arry[16];
extern AreaConfig gs_AreaInfo[16];
extern TIME_TYPE       pwr_on_time_app,timeandday_now_app;
extern uint32_t System_Runtime_Lasttime;
extern uint32_t  time1ms_count_forrun ;
extern uint32_t  time1ms_count_forrun_his ;
extern struct POWER_ON_RECORD_TYPE POWER_ON_RECORD_Str;

void INIT_LOGO_DATA(DATACLASS_TYPE x_datatype,uint8_t x_field)
{
	static uint8_t Tp_field;
	static uint32_t Tp_addr;
	uint32_t Q0_addr;
	static uint8_t Tp_ii;
	uint32_t Temp_data;
	
	Tp_field = x_field;
	
	//???????
					   switch(x_datatype)
					   {
						      case ActionNumber_DATACLASS://?????????????
						      case ActionTimers_DATACLASS://???????????
							   // Tp_field =Tp_data1;
			               para.dataclass_1_2_action_count=0;
						sprintf((char *)BaseData_ARR+BASE_data_actioncount*9,"%08X",para.dataclass_1_2_action_count);   
            para.no_vbat_pwr_on_cnt=0;	
						sprintf((char *)(BaseData_ARR+BASE_data_Vbatlow_monthcount*9),"%08X",0);		
			      sprintf((char *)(BaseData_ARR+BASE_data_Dataclass12RTCPowerOffTimer*9),"%08X",para.no_vbat_pwr_on_cnt);					
                 if(systerm_error_status.bits.lse_error==1)	//??j?RTC????
                 {
									 Clear_sdram(1<<(Tp_field+8));//AREANO
						       sdram_field_offset0(Tp_field) = 1;
				           sdram_field_offset1(Tp_field) = 0;//???????
				           sdram_field_offset2(Tp_field) = 0xAA;
                   sdram_field_offset3(Tp_field) = 0xAA;
									 para.last_poweroff_year = 0X99;//20170924
									 para.last_poweroff_month = 0X99;//20170924
									 if(gs_AreaInfo[Tp_field].type==ActionNumber_DATACLASS)
									 {
                     Temp_data = 0xfffffffe;
										 //*(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+4) = 0xfffffffe;
                     memcpy((void *)(logodata_sdrambuffer_addr_arry[Tp_field]+4),(void *)&Temp_data,4);										 
									 }										 
									  para.no_vbat_pwr_on_cnt=1;
                    sprintf((char *)(BaseData_ARR+BASE_data_Vbatlow_monthcount*9),"%08X",1);										 
			              sprintf((char *)(BaseData_ARR+BASE_data_Dataclass12RTCPowerOffTimer*9),"%08X",para.no_vbat_pwr_on_cnt);
                    sprintf((char *)(BaseData_ARR+BASE_data_PowerOffYear*9),"%08X",para.last_poweroff_year);
                    sprintf((char *)(BaseData_ARR+BASE_data_PowerOffMonth*9),"%08X",para.last_poweroff_month);		 
								 }
                 else
                 {
									 Clear_sdram(1<<(Tp_field+8));//AREANO
						       sdram_field_offset0(Tp_field) = pwr_on_time_app.month;//DECTOHEX(timeandday_now->MONTH);
				           sdram_field_offset1(Tp_field) = pwr_on_time_app.year;//DECTOHEX(timeandday_now->YEAR);//???????
				           sdram_field_offset2(Tp_field) = 0x00;
                   sdram_field_offset3(Tp_field) = 0x00;
									 //????????????????????????????
									 para.last_poweroff_year = timeandday_now_app.year;//20170924
						       para.last_poweroff_month = timeandday_now_app.month;//20170924
									 sprintf((char *)(BaseData_ARR+BASE_data_PowerOffYear*9),"%08X",para.last_poweroff_year);
                    sprintf((char *)(BaseData_ARR+BASE_data_PowerOffMonth*9),"%08X",para.last_poweroff_month);	
                   if(gs_AreaInfo[Tp_field].type==ActionNumber_DATACLASS)
									 {										 
									   Temp_data = 0xfffffffe;
										 //*(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+4) = 0xfffffffe;		
										 memcpy((void *)(logodata_sdrambuffer_addr_arry[Tp_field]+4),(void *)&Temp_data,4);
									 }
								 }
                 //?????????
                for(Q0_addr=0;Q0_addr<(gs_AreaInfo[Tp_field].space/0x20000);Q0_addr++)
								 {
									 NAND_EraseBlock((gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/0x20000+Q0_addr);
								 }
 								 if(gs_AreaInfo[Tp_field].type ==ActionTimers_DATACLASS )
								 {
									 System_Runtime_Lasttime = 0xffffffff;
									 time1ms_count_forrun = 0;
									 time1ms_count_forrun_his=0;
								 }
     						       break;
						      case UnitPara_DATACLASS:
							        //?????????
						            Clear_sdram(1<<(Tp_field+8));//AREANO
                for(Q0_addr=0;Q0_addr<(gs_AreaInfo[Tp_field].space/0x20000);Q0_addr++)
								 {
									 NAND_EraseBlock((gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/0x20000+Q0_addr);
								 }
								 *(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field] + UINT_CS1_OFFSET) = 0X18;
								 *(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field] + UINT_CS2_OFFSET) = 0X18;
								 *(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field] + UINT_CS3_OFFSET) = 0X18;
								  SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],(gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048,1);
						      //SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],unit_data__B_nandflash_start,1);
						           break;
						      case 	TestFinalData_DATACLASS:
							           Clear_sdram(1<<(Tp_field+8));//AREANO
							 //  Tp_field =Tp_data1;
						     //sdram_TestFinalCyclex = 0;
						     //sdram_TestFinalNumx = 0;
						     //sdram_TestFinalLoopx =0;
						 sprintf((char *)(BaseData_ARR+(BASE_data_fieldinfo_count+Tp_field*3)*9),"%08X",0);	
						 sprintf((char *)(BaseData_ARR+(BASE_data_fieldinfo_count+Tp_field*3+ 1)*9),"%08X",0);	
						 sprintf((char *)(BaseData_ARR+(BASE_data_fieldinfo_count+Tp_field*3+ 2)*9),"%08X",0);	
						
						*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]) = POWER_ON_RECORD_Str.Poweron_Year;
						*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+1) = POWER_ON_RECORD_Str.Poweron_Month;
						*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+2) = POWER_ON_RECORD_Str.Poweron_Day;
						*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+3) = POWER_ON_RECORD_Str.Poweron_Hour;
						*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+4) = POWER_ON_RECORD_Str.Poweron_Minute;
						*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+5) = POWER_ON_RECORD_Str.Battery_Voltage;
						
						   
			        for(Tp_ii =0;Tp_ii<(MAX_LOGO_FINALTEST_ONEPACKET-6);Tp_ii++)
			            *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+Tp_ii+6) = 0;
								 //???????
                for(Q0_addr=0;Q0_addr<(gs_AreaInfo[Tp_field].space/0x20000);Q0_addr++)
								 {
									 NAND_EraseBlock((gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/0x20000+Q0_addr);
								 }
								 field_info_init();
                         break;
                case 	TestAllData_DATACLASS:
						    case  AnomalyRecord_DATACLASS:
						
                            
                            field_info_save();
							       //?????????
						          Clear_sdram(1<<(Tp_field+8));//AREANO
						    //Tp_field =Tp_data1;
						   sprintf((char *)(BaseData_ARR+(BASE_data_fieldinfo_count+Tp_field*3)*9),"%08X",0);	
						 sprintf((char *)(BaseData_ARR+(BASE_data_fieldinfo_count+Tp_field*3+ 1)*9),"%08X",0);	
						 sprintf((char *)(BaseData_ARR+(BASE_data_fieldinfo_count+Tp_field*3+ 2)*9),"%08X",0);	
						
                for(Q0_addr=0;Q0_addr<(gs_AreaInfo[Tp_field].space/0x20000);Q0_addr++)
								 {
									 NAND_EraseBlock((gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/0x20000+Q0_addr);
								 }
								 field_info_init();
						             break;
						    case ActionRecord_DATACLASS:
							  //?????????
                            field_info_save();
                            
						           Clear_sdram(1<<(Tp_field+8));//AREANO
						    //Tp_field =Tp_data1;
						     *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]) = 0xE0;
						     *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+1) = POWER_ON_RECORD_Str.Poweron_Year;
						     *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+2) = POWER_ON_RECORD_Str.Poweron_Month;
						     *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+3) = POWER_ON_RECORD_Str.Poweron_Day;
						     *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+4) = POWER_ON_RECORD_Str.Poweron_Hour;
						     *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+5) = POWER_ON_RECORD_Str.Poweron_Minute;
						     *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+6) = POWER_ON_RECORD_Str.Poweron_Second;
						     
						   sprintf((char *)(BaseData_ARR+(BASE_data_fieldinfo_count+Tp_field*3)*9),"%08X",0);	
						 sprintf((char *)(BaseData_ARR+(BASE_data_fieldinfo_count+Tp_field*3+ 1)*9),"%08X",1);	
						 sprintf((char *)(BaseData_ARR+(BASE_data_fieldinfo_count+Tp_field*3+ 2)*9),"%08X",0);	
						
                for(Q0_addr=0;Q0_addr<(gs_AreaInfo[Tp_field].space/0x20000);Q0_addr++)
								 {
									 NAND_EraseBlock((gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/0x20000+Q0_addr);
								 }
								 field_info_init();
                       break;							
						          default:break;	
					     }//?????????? SWITCH
}



 void LOGO_CLEAR(void)
{
	//NAND_ADDRESS WriteReadAddr;
	static uint8_t Tp_field;
	static uint32_t Tp_addr;

	
	switch(FIELD_ERASE_FLAG)
	{
		case 0xffff://??????
			//??????
		  if(FIELD_ERASE_STEP == 0) 
			{
				;
			}
			if((FIELD_ERASE_STEP>=1)&&(FIELD_ERASE_STEP<=0xf))
			{
				INIT_LOGO_DATA(gs_AreaInfo[FIELD_ERASE_STEP].type,FIELD_ERASE_STEP);
				FIELD_ERASE_STEP++;
			}
		  if(FIELD_ERASE_STEP > 0XF)
			{
				FIELD_ERASE_STEP = 0;
			//	SYSTEM_ERR_STATUS->bits.COMM_Handle_FINISH = 1;
			}				
		 //??????
		
			break;
			default://???????
				
				switch(FIELD_ERASE_STEP)
	     { 
					case 0:
						break;
					case 1:
						
				    for(Tp_field =1;Tp_field<16;Tp_field++)
		       {
				     if(FIELD_ERASE_FLAG == Tp_field)//????????
				    {
					    INIT_LOGO_DATA(gs_AreaInfo[Tp_field].type,Tp_field);
							FIELD_ERASE_STEP = 0;
						//	SYSTEM_ERR_STATUS->bits.COMM_Handle_FINISH = 1;
				    }//??????
			    }
			    break;
			 
		   }
		  break;//??????? E1
	}
	
	

}


