
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
#include "display.h"

extern uint32_t logodata_sdrambuffer_addr_arry[16];
extern  BADMANAGE_TAB_TYPE_U badmanage_str[1];
extern AreaConfig gs_AreaInfo[16],Tp_gs_AreaInfo[16];
extern uint8_t* BaseData_ARR;
extern FieldArea3All_cyw *pArea3;
static uint32_t CHECK_SUM_NAND_1;
uint32_t CHECK_SUM_HIGH = 0;
static uint32_t BMP_COUNT = 0;
extern uint32_t CHECK_SUM_NAND;
uint8_t checksum_flag=0;
extern uint8_t *bmpBuf_kkk,*bmpBuf_kkk_bak;
uint32_t shift_pointer(uint32_t ptr, uint32_t align);


void Display_checksum(void)
{
		char Tp_version[9]={0};
	     if(checksum_flag == 1)
			 {
					SetZuobiao(10, 360);
	        lcd_printf_new("CHECK_SUM_HIGH = 0x%08X         ",CHECK_SUM_HIGH);
				  SetZuobiao(10, 380);
		      lcd_printf_new("BMP_COUNT = 0x%08X         ",BMP_COUNT);
				  SetZuobiao(10, 400 + 60);
		      lcd_printf_new("NAND Chechsum = 0x%08X         ",CHECK_SUM_NAND_1);
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
	BMP_COUNT = 0;
	CHECK_SUM_HIGH = 0;
		
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
	
	if(bmpBuf_kkk_bak==0)
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
		 {
		   BMP_COUNT++;
			 check_sum_nand(Tp_addr*2048,Tp_bmp_TAB[0],Tp_bmp_TAB[1]);
		 }
	}
	
	CHECK_SUM_NAND_1 = CHECK_SUM_NAND;
	
	//sprintf(display_checksum,"NAND Chechsum = 0x%08X",CHECK_SUM_NAND);
  //LCD_DisplayStringLine(30,display_checksum);
	
	if(bmpBuf_kkk_bak) 
  {
		#ifdef SYSUARTPRINTF_p
	 sysprintf("bmpBuf_kkk=%08X,bmpBuf_kkk_bak=%08X, free\r\n",bmpBuf_kkk,bmpBuf_kkk_bak);
	 #endif	
	free(bmpBuf_kkk_bak);
		bmpBuf_kkk_bak=0;
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
			#ifdef SYSUARTPRINTF
			sysprintf("BaseData_ARR[idx] = %s\r\n",(char *)(BaseData_ARR+idx*9));
			#endif
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