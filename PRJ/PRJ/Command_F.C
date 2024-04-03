
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "sys.h"
#include "Command_all.h"
#include "Aprotocol.h"
#include "BSP_init.h"
#include "rtc.h"
#include "etimer.h"

extern uint32_t logodata_sdrambuffer_addr_arry[16];
extern  BADMANAGE_TAB_TYPE_U badmanage_str[1];
extern AreaConfig gs_AreaInfo[16],Tp_gs_AreaInfo[16];
extern uint8_t* BaseData_ARR;
extern FieldArea3All_cyw *pArea3;


//ERR OUTPUT 1HZ
//void RTC_CLKOUT(RTC_TICK x_tick)
//{
//	REG_OPERATE(REG_SYS_GPH_MFPL,0x000f0000,clear);
//	REG_OPERATE(REG_SYS_GPH_MFPL,0x000d0000,set);
//	if(x_tick>RTC_TICK_1_128_SEC) x_tick = RTC_TICK_1_128_SEC;
//	REG_OPERATE(REG_RTC_TICK,0x07,clear);
//	REG_OPERATE(REG_RTC_TICK,x_tick,set);
//	
//}

//p107 PB2
void RTC_CLKOUT(void)
{
	REG_OPERATE(REG_SYS_GPB_MFPL,0x00000F00,set);
	REG_OPERATE(REG_CLK_PCLKEN0,1<<4,set);//TIM0 CLK ENABLE
	REG_OPERATE(REG_CLK_DIVCTL8,3<<16,set);//32768 AS CLK
	//ETIMER_Open(0,ETIMER_TOGGLE_MODE,20);
	//Frequency = TMRx_CLK / ((PRESCALE + 1) * TCMP)
	//32.768k/2=16.384k toggle

	outpw(REG_ETMR0_CMPR, 2);
  outpw(REG_ETMR0_PRECNT, 0);
  outpw(REG_ETMR0_CTL, 1 | (1<<5));
	
	ETIMER_Start(0);
}


void RTC_CLKSTOP(void)
{
	//REG_OPERATE(REG_SYS_GPH_MFPL,0x000f0000,clear);
	//REG_OPERATE(REG_SYS_GPH_MFPL,0x000d0000,set);
	REG_OPERATE(REG_SYS_GPB_MFPL,0x00000F00,clear);
}

uint8_t get_bad_block(void)  //qdhai add
{
    //return (nand_dev.block_totalnum-nand_dev.good_blocknum);
	return (uint8_t)badmanage_str->BAD_MANAGE_str.ERR_NUMBER;
}

int AreaReadCmdF(char cmd, int filed, int idx, char *buf, int *len)
{

	void *addr;
	//BaseAreaAll *pAreaBase;
//	FieldArea3All *pArea3;
	uint8_t Tp_i;
	static uint16_t Tp_idx;
	static uint32_t Tp_data;

	if (filed >= 16) return 0;
	//addr = (void *)(AREA_FILED_START + 0x2000 * filed);
	addr = (void *)(logodata_sdrambuffer_addr_arry[filed]);
	switch (gs_AreaInfo[filed].type)
	{
	case 0:
		if (cmd == LCD_F7)
		{
			sysprintf("BaseData_ARR[idx] = %s\r\n",(char *)(BaseData_ARR+idx*9));
			if (idx < BASE_data_4byte_end)
			{
				//pAreaBase = (BaseAreaAll *)addr;
				//*len = pAreaBase->areaData[idx].size;
				
				if(idx == 0)
				{
					 for(Tp_i = 0;Tp_i<6;Tp_i++)
					 {
						 DIVIDED_DATA_HEX2BYTE(*(uint8_t *)(BaseData_ARR+idx*9+Tp_i),(uint8_t *)(buf+Tp_i*2));
					 }
					 *len  = 12; 
					 return 1;
				}
				else if((idx>0)&&(idx<BASE_data_8byte_end))
				{
				    *len  = 8;
				    sprintf(buf, "%8.*s,",*(uint8_t *)(BaseData_ARR+idx*9+8),BaseData_ARR+idx*9);
				    //memcpy(buf, pAreaBase->areaData[idx].data, *len);
				    for(Tp_i=0;Tp_i<8;Tp_i++)
				    {
					     if(buf[Tp_i]==' ') buf[Tp_i]='0';
				    }
				     return 1;
				}
				else if((idx>=BASE_data_8byte_end)&&(idx<BASE_data_fieldarea_end))
				{
					   Tp_idx = idx - BASE_data_8byte_end;
					   if(Tp_idx%4==0)
						 {
							 Tp_data = gs_AreaInfo[Tp_idx/4].addr;
							 sprintf(buf, "%08X",Tp_data);
						 }
						 if(Tp_idx%4==1)
						 {
							 Tp_data = gs_AreaInfo[Tp_idx/4].space;
							 sprintf(buf, "%08X",Tp_data);
						 }
						 if(Tp_idx%4==2)
						 {
							 Tp_data = gs_AreaInfo[Tp_idx/4].type;
							 sprintf(buf, "%08d",Tp_data);
						 }
						 if(Tp_idx%4==3)
						 {
							 Tp_data = gs_AreaInfo[Tp_idx/4].size;
							 sprintf(buf, "%08d",Tp_data);
						 }
						 *len  = 8;
						 
						 return 1;
				}
				else if((idx>=BASE_data_fieldarea_end)&&(idx<BASE_data_4byte_end))
				{
					  field_info_save();
					  *len  = 8;
				    sprintf(buf, "%8.*s,",8,(uint8_t *)(BaseData_ARR+idx*9));
				    //memcpy(buf, pAreaBase->areaData[idx].data, *len);
				    for(Tp_i=0;Tp_i<8;Tp_i++)
				    {
					     if(buf[Tp_i]==' ') buf[Tp_i]='0';
				    }
					return 1;
				}

			}
			else
			{
				return 0;
			}
		}
		break;
	case 3:
		if (cmd == LCD_F8)
		{
			if (idx < 1000)
			{
				pArea3 = (FieldArea3All_cyw *)addr;
				*len = 1;  //qdhai change  1->3
				*buf = pArea3->areaData[idx].data;
				//sprintf(buf, "%03d", pArea3->areaData[idx].data);  //qdhai login
				return 1;
			}
			else
			{
				return 0;
			}
		}
		break;
	default:
		break;
	}

	return 0;
}