
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "sys.h"
#include "gpio.h"
#include "BSP_init.h"
#include "Command_all.h"
#include "Aprotocol.h"
#include "rtc.h"

  uint32_t  led_log_on_cnt=0;
	uint8_t    led_log_on_flag = 0;
uint8_t FLAG_IIC_DATA_CHANGE = 0;
volatile uint16_t  TX_COUNT_IN_IIC = 0;
volatile uint16_t  TX_COUNT_OUT_IIC = 0;
volatile uint8_t  UART2_RX_BUFF_IIC[RX_BUFF_MAX_IIC];
volatile uint16_t  RX_COUNT_IN_IIC = 0;
volatile uint16_t  RX_COUNT_OUT_IIC = 0;
volatile uint8_t  UART2_TX_BUFF_NEW_IIC[TX_BUFF_MAX_UART_IIC]={0};
extern uint8_t * BaseData_ARR;
Dataclass4567_Info Fieldx_Info[16];
extern AreaConfig gs_AreaInfo[16],Tp_gs_AreaInfo[16];
extern uint32_t logodata_basedata_BUFFER,logodata_field1_BUFFER,logodata_field2_BUFFER,logodata_field3_BUFFER,logodata_field4_BUFFER,\
          logodata_field5_BUFFER,logodata_field6_BUFFER,logodata_field7_BUFFER,logodata_field8_BUFFER,logodata_field9_BUFFER,\
					logodata_field10_BUFFER,logodata_field11_BUFFER,logodata_field12_BUFFER,logodata_field13_BUFFER,logodata_field14_BUFFER,\
					logodata_field15_BUFFER;
extern TIME_TYPE timeandday_now_app;
extern UINT32 volatile time1ms_count;
extern uint8_t Field_unit;
extern uint32_t logodata_sdrambuffer_addr_arry[16];

uint32_t CharToHex(char *x_char)
{
	uint8_t Tp_j;
	uint8_t Tp_value[8];
	uint32_t Tp_data;
	for(Tp_j=0;Tp_j<8;Tp_j++)
		{
			if(*x_char>= 'A')
			{
			   Tp_value[Tp_j] = *x_char - 'A'  +  10;
			}
			else if(*x_char >= '0')
			{
			   Tp_value[Tp_j] = *x_char - '0';
			}
			else
			{
				Tp_value[Tp_j] = 0;
			}
			x_char++;
		}
		Tp_data = (Tp_value[0]<<28) +  (Tp_value[1]<<24) + (Tp_value[2]<<20) +  (Tp_value[3]<<16)
		         + (Tp_value[4]<<12)    +  (Tp_value[5]<<8)     + (Tp_value[6]<<4)     +  (Tp_value[7]);
		
		return Tp_data;
}

void led_log_on(uint16_t time_ms) //
{	
	LED_LOGO_ON( );
	led_log_on_cnt=time1ms_count;
	led_log_on_flag=1;
}

void code_protocol_ack_IIC(uint8_t  x_xor,uint8_t len,uint8_t *buff,uint8_t type)  //SEND_ACKnHEX   type =0 buff Ϊhex�������ת��ASCII ;  type=1 ����ת
{
	
	uint8_t data_len = 0;
	uint16_t n = 0;  
  uint8_t Tp_i = 0;	
	uint8_t    tx_buff_uart2_temp[TX_BUFF_UART2_MAX_LEN]={0};     //���ջ���,���USART_REC_LEN���ֽ�.
  uint16_t        tx_buff_uart2_num_temp=0;
	
	tx_buff_uart2_temp[0]='@';

	if(type==0)
	{
		data_len = len*2+3;
	  convet_hex_to_hex_asccii(data_len,tx_buff_uart2_temp+1);	   
		for(n =0;n<len;n++)
		{
		  convet_hex_to_hex_asccii(buff[n],tx_buff_uart2_temp+3+n*2);
		}
		convet_hex_to_hex_asccii(x_xor,tx_buff_uart2_temp+3+len*2);									
		tx_buff_uart2_temp[5+len*2]='*';
				
	   tx_buff_uart2_num_temp=6+len*2;
	}
	else
	{
		data_len = len+3;
	  convet_hex_to_hex_asccii(data_len,tx_buff_uart2_temp+1);	   	
		memcpy(&tx_buff_uart2_temp[3],buff,len);	
		convet_hex_to_hex_asccii(x_xor,tx_buff_uart2_temp+3+len);									
		tx_buff_uart2_temp[5+len]='*';
				
	  tx_buff_uart2_num_temp=6+len;

	}	
	
	  
		//stop_tim6();
	 FLAG_IIC_DATA_CHANGE = 1;
	 TX_COUNT_IN_IIC = 0;
	 TX_COUNT_OUT_IIC = 0;
	  for(Tp_i =0 ;Tp_i <tx_buff_uart2_num_temp; Tp_i++)
	  {
			 if(((TX_COUNT_IN_IIC+1) % TX_BUFF_MAX_UART_IIC)!=TX_COUNT_OUT_IIC)
			 {
			   UART2_TX_BUFF_NEW_IIC[TX_COUNT_IN_IIC] = tx_buff_uart2_temp[Tp_i];
			   TX_COUNT_IN_IIC = (TX_COUNT_IN_IIC+1) % TX_BUFF_MAX_UART_IIC;
			 }
		}
		FLAG_IIC_DATA_CHANGE = 0;
	  //start_tim6(50000);
		//SEND_data();
}

void field_info_init(void)
{
	uint32_t Tp_i;
	uint32_t Tp_data;
	
	for(Tp_i= 0;Tp_i<48;Tp_i++)
	{
		Tp_data = CharToHex((char *)(BaseData_ARR+(112+Tp_i)*9));
		
		 
		switch(Tp_i%3)
		{
		  case 0:
			Fieldx_Info[Tp_i/3].cycle = Tp_data;	
			break;
			case 1:				
			Fieldx_Info[Tp_i/3].num = Tp_data;
			break;
			case 2:				
			Fieldx_Info[Tp_i/3].loop = Tp_data;
			break;
			default:
			break;
		}
	}
}


void WriteAreaConfig_new(void)
{
	 uint16_t Tp_i;
	int  Tp_data;
	
	for(Tp_i=0;Tp_i<64;Tp_i++)
	{
		switch(Tp_i%4)
		{
		  case 0:				
			Tp_data = gs_AreaInfo[Tp_i/4].addr;
			break;
			case 1:				
			Tp_data = gs_AreaInfo[Tp_i/4].space;
			break;
			case 2:				
			Tp_data = gs_AreaInfo[Tp_i/4].type;
			break;
			case 3:				
			Tp_data = gs_AreaInfo[Tp_i/4].size;
			break;
			default:
			break;
		}
		
		sprintf((char *)(BaseData_ARR+(32+Tp_i)*9),"%08X",Tp_data);
		 
	}
}


//����BASEDATA�����No0����
void Updata_Basedata_No0_cyw(void)
{
	RTC_TIME_DATA_T *pTime;
	void *addr;
	//BaseAreaAll *pAreaBase;
	
	//addr = (void *)(logodata_field0_BUFFER );
	//pAreaBase = (BaseAreaAll *)addr;
	
	//pTime = (RTC_TIME_DATA_T *)BaseData_ARR[0];
	
////////////////////////////	RTC_Read(RTC_CURRENT_TIME,&timeandday_now);
	
	*(uint8_t *)BaseData_ARR= ((timeandday_now_app.year)&0xff);
	*(uint8_t *)(BaseData_ARR+1) = (timeandday_now_app.month&0xff);
	*(uint8_t *)(BaseData_ARR+2) = (timeandday_now_app.day&0xff);
	*(uint8_t *)(BaseData_ARR+3) = (timeandday_now_app.hour&0xff);
	*(uint8_t *)(BaseData_ARR+4) = (timeandday_now_app.minute&0xff);
	*(uint8_t *)(BaseData_ARR+5) = (timeandday_now_app.second&0xff);
	
}

void basedata_ram_to_sdram(void)
{
	  
	uint32_t Tp_checksum2;
	uint16_t Tp_i;
	char *Tp_char = (char *)BaseData_ARR;
	
	Tp_checksum2 = 0;
          for(Tp_i=0;Tp_i<163*9;Tp_i++)
          {
	               Tp_checksum2+=*(char*)(Tp_char);
						      Tp_char++;
          }
					sprintf((char *)(BaseData_ARR+BASE_data_checksum*9),"%08X",Tp_checksum2);
				sysprintf("----444----Tp_checksum2 = %08X",Tp_checksum2);	
				 memcpy((uint8_t*)logodata_basedata_BUFFER,BaseData_ARR, logodata_Basedata_SIZE);
				 memcpy((uint8_t*)(logodata_basedata_BUFFER +Basedata_copy0),BaseData_ARR,logodata_Basedata_SIZE);
		memcpy((uint8_t*)(logodata_basedata_BUFFER +Basedata_copy1),BaseData_ARR,logodata_Basedata_SIZE);
		memcpy((uint8_t*)(logodata_basedata_BUFFER +Basedata_copy2),BaseData_ARR,logodata_Basedata_SIZE);
}


void unit_ram_to_sdram(void)
{
	uint16_t Tp_i;
	//uint8_t Tp_checksum=0;
	memcpy((uint8_t*)(logodata_sdrambuffer_addr_arry[Field_unit]+UINT_START2_OFFSET),(uint8_t*)(logodata_sdrambuffer_addr_arry[Field_unit]),MAX_LOGO_UINT_NUM+2);
	memcpy((uint8_t*)(logodata_sdrambuffer_addr_arry[Field_unit]+UINT_START3_OFFSET),(uint8_t*)(logodata_sdrambuffer_addr_arry[Field_unit]),MAX_LOGO_UINT_NUM+2);
	
	
}
