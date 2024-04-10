

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "sys.h"
#include "uart.h"
#include "Aprotocol.h"
#include "display.h"
#include "Command_all.h"
#include "rtc.h"
#include "BSP_Init.h"
#include "lcd.h"
#include "nand_nand.h"
#include "Poweron.h"
#include "BSP_init.h"
#include "DIP_SW.h"
#include "gpio.h"

extern UART_T param;
uint8_t *UART2_RX_BUFF;//[RX_BUFF_MAX];
volatile uint16_t  RX_COUNT_IN = 0;
volatile uint16_t  RX_COUNT_OUT = 0;
uint8_t  *UART2_TX_BUFF_NEW=0;//[TX_BUFF_MAX_UART]={0};
uint16_t  TX_COUNT_IN = 0;
uint16_t  TX_COUNT_OUT = 0;
UINT8 *RX_Test=0,*TX_Test=0;//[500];
static uint8_t COMMAND_AP,COMMAND_IN ;
uint8_t  command_xor=0x00;  //ÓÃÓÚUSBÊý¾ÝCOPYÍêÍ¼Æ¬Ó¦´ð
char  USB_IMAGE_TYPE = ' ';
uint8_t  USB_HAS_USABLE_IMG=0;
uint8_t IMAGE_SEARCHED = 0;
PARA_TYPE para;
extern uint8_t* BaseData_ARR;
extern PPT_TYPE ppt;
extern TTS_TYPE  tts;
extern volatile uint8_t STOP_FLAG_CYW;
AreaConfig gs_AreaInfo[16]={0},Tp_gs_AreaInfo[16]={0};

uint32_t Temp_logodata_BUFFER;
extern uint32_t logodata_basedata_BUFFER,logodata_field1_BUFFER,logodata_field2_BUFFER,logodata_field3_BUFFER,logodata_field4_BUFFER,\
          logodata_field5_BUFFER,logodata_field6_BUFFER,logodata_field7_BUFFER,logodata_field8_BUFFER,logodata_field9_BUFFER,\
					logodata_field10_BUFFER,logodata_field11_BUFFER,logodata_field12_BUFFER,logodata_field13_BUFFER,logodata_field14_BUFFER,\
					logodata_field15_BUFFER;

uint32_t logodata_sdrambuffer_addr_arry[16]={0};
uint32_t logodata_sdrambuffer_addr_arry_bak[16]={0};
uint8_t Tp_layerover=0;
extern  uint8_t flag_AB;

extern uint8_t LOGO_ERR;
extern uint8_t COMMAND_N;

const uint32_t logodata_sdrambuffer_size_arry[]=
                                {
																	logodata_field0_SIZE,logodata_field1_SIZE,logodata_field2_SIZE,logodata_field3_SIZE,
	                                 logodata_field4_SIZE,logodata_field5_SIZE,logodata_field6_SIZE,logodata_field7_SIZE,
	                                 logodata_field8_SIZE,logodata_field9_SIZE,logodata_field10_SIZE,logodata_field11_SIZE,
	                                 logodata_field12_SIZE,logodata_field13_SIZE,logodata_field14_SIZE,logodata_field15_SIZE
																};	
extern uint8_t   rx_finish_state_uart2;
extern uint16_t  rrx_buff_uart2_cnt;																
//extern uint8_t    rrx_buff_uart2[RX_BUFF_UART2_MAX_LEN]; 
uint8_t rx_buff_uart2_delay=0;
extern uint8_t err_code_buff[9];																
uint16_t flag_logo_change=0;	
volatile  uint32_t UART_TX_REAL=0,UART_TX_WANT=0;																
//extern uint8_t *TX_Test;

 
																
typedef enum COMM_HANDLE_STATUS
{
	COMM_IDLE,
	WAIT_LEN,//µÈ´ý³¤¶È
	LEN_JUDGE,//³¤¶È¼ÆËã
	WAIT_ALL,//µÈ´ýËùÓÐµÄÊý¾Ý
	COMM_HANDLE,//´®¿Ú´¦Àí
	
}COMM_HANDLE_TYPE;
//Í¨Ñ¶´¦Àí
COMM_HANDLE_TYPE COMM_STEP=COMM_IDLE;
uint32_t WAIT_LEN_TIMER;
extern uint32_t volatile time1ms_count;
 uint8_t uart2_send_finish_state=0;
volatile uint16_t command_D8_D9_time=0;
volatile uint8_t command_D8_D9_flag=0;
BMP_RPOTOCOL_TYPE  bmp_protocol[3]={0};
 BMP_RPOTOCOL_TYPE  bmp_protocol_bak[3]={0};
extern  SYSTEMERRORSTATUS_TYPE  systerm_error_status;
extern uint8_t LOGO_DATA_OUT_FLAG;
RTC_TIME_DATA_T pwr_on_time_ground={0},timeandday_now_ground={0};
TIME_TYPE       pwr_on_time_app={0},timeandday_now_app={0};
uint32_t System_Runtime_Lasttime = 0;
volatile uint32_t  time1ms_count_forrun = 0;
uint32_t  time1ms_count_forrun_his = 0;
struct POWER_ON_RECORD_TYPE POWER_ON_RECORD_Str;
extern Dataclass4567_Info Fieldx_Info[16];

struct LOGO_Y5Y6_TYPE LOGO_Y5Y6_str;

uint16_t FIELD_ERASE_STEP = 0;//????????
uint16_t FIELD_ERASE_FLAG = 0;//??????? bit0 basedata bit1 field1
extern DOWNLOAD_TYPE  download;


void COMM_SOFT_DEINIT(void)
{
	RX_COUNT_IN = 0;
	RX_COUNT_OUT = 0;
	COMM_STEP = COMM_IDLE;
}


AreaConfig* get_gs_AreaInfo(uint8_t field_num) //qdhai add
{
	return &gs_AreaInfo[field_num];
}

void wait_send_over(void)
{
	uint32_t Tp_count=0x20000;
	while(Tp_count--)
	{
		if(uart2_send_finish_state == 0)
		{
			break;
		}
		
	}
}

uint8_t DECTOHEX(uint8_t x_data)
{
	return (x_data/10)*16 + x_data%10;
	
}																
																
uint8_t HEXTODEC(uint8_t x_data)
{
	return (x_data/16)*10 + x_data%16; 
}	


uint16_t CHANGE_HEXDATA_2BYTE(uint8_t  *x_data)
{
	static uint16_t Tp_long=0xffff;
	Tp_long=0xffff;
	if(((x_data[0]>=0x30)&&(x_data[0]<=0x39))||((x_data[0]>=0x41)&&(x_data[0]<=0x46)))
	{
		if(((x_data[1]>=0x30)&&(x_data[1]<=0x39))||((x_data[1]>=0x41)&&(x_data[1]<=0x46)))
	  {
		  if(x_data[0]>=0x41)
			{
				Tp_long = (x_data[0]-55)*16;
			}
			else
			{
				Tp_long = (x_data[0]-48)*16;
			}
			if(x_data[1]>=0x41)
			{
				Tp_long = Tp_long + (x_data[1]-55);
			}
			else
			{
				Tp_long = Tp_long + (x_data[1]-48);
			}
	  }
	}
	return Tp_long;
}

uint16_t GET_READ_RXNUM(uint16_t x_rx_in,uint16_t x_rx_out,uint16_t x_buff_max)
{
	uint16_t Tp_number=0;
	if(x_rx_out > x_rx_in)     
        {					
	     		 Tp_number = x_buff_max - x_rx_out + x_rx_in;
				}					
				else
				{
					Tp_number = x_rx_in - x_rx_out;
				}
	return  Tp_number;
}

uint8_t XORCHECK(uint8_t  *x_data,uint8_t  x_long)
{
	uint8_t Tp_result=0,Tp_i;
	for(Tp_i=0;Tp_i<x_long;Tp_i++)
	{
	   Tp_result = Tp_result ^ x_data[Tp_i];
	}
	return Tp_result;
}

void convet_hex_to_hex_asccii(uint8_t x_FSC,uint8_t *x_BUFF)  //0x A1  ×ªÎª'A' '1' DIVIDED_DATA_HEX2BYTE(uint8_t x_FSC,uint8_t *x_BUFF)
{
	if((x_FSC/16)>9)
	{
		x_BUFF[0]='A'+(x_FSC/16-10);
	}
	else
	{
		x_BUFF[0]='0'+x_FSC/16;
	}
	if((x_FSC%16)>9)
	{
		x_BUFF[1]='A'+(x_FSC%16-10);
	}
	else
	{
		x_BUFF[1]='0'+(x_FSC%16);
	}
}

int8_t convert_1_hex_to_hex_ascii(int8_t a)  //ÊÊºÏÒ»Î»µÄ£¬¼´Ð¡ÓÚ0x10  Èç 0x0A ±äÎª'A'    0x01±äÎª'1'
{
	if(a>=10)
	{
	   return ('A'+(a-10));
	}
	if((a>=0)&&(a<10))
	{
	  return ('0'+a);
	}
	else
	{
		return 0xff;
	}
}

uint16_t convet_1_hex_ASCII_to_hex(uint8_t *buff)   //'F'  0x0f  '1' 1  CHANGE_HEXDATA_1BYTE
{
	 uint16_t Tp_long=0xffff;


	if(((buff[0]>=0x30)&&(buff[0]<=0x39))||((buff[0]>=0x41)&&(buff[0]<=0x46)))
	{
		if(buff[0]>=0x41)
			{
				Tp_long = (buff[0]-55);
			}
			else
			{
				Tp_long = (buff[0]-48);
			}
	}		
	
	return Tp_long;
}

uint16_t convert_1_dec_ascii_to_hex(uint8_t  *x_data)//µÃµ½³¤¶È CHANGE_DECDATA_1BYTE
{
	static uint16_t Tp_long=0xffff;
	Tp_long=0xffff;
	if((x_data[0]>=48)&&(x_data[0]<=57))
	{
		Tp_long = x_data[0]-48;
	}
	
	return Tp_long;
}

uint16_t convet_2_hex_ASCII_to_hex(uint8_t *buff)   //CHANGE_HEXDATA_2BYTE(uint8_t  *x_data)
{
	 uint16_t Tp_long=0xffff;
	Tp_long=0xffff;
	if(((buff[0]>=0x30)&&(buff[0]<=0x39))||((buff[0]>=0x41)&&(buff[0]<=0x46)))
	{
		if(((buff[1]>=0x30)&&(buff[1]<=0x39))||((buff[1]>=0x41)&&(buff[1]<=0x46)))
	  {
		  if(buff[0]>=0x41)
			{
				Tp_long = (buff[0]-55)*16;
			}
			else
			{
				Tp_long = (buff[0]-48)*16;
			}
			if(buff[1]>=0x41)
			{
				Tp_long = Tp_long + (buff[1]-55);
			}
			else
			{
				Tp_long = Tp_long + (buff[1]-48);
			}
	  }
	}
	return Tp_long;
}

uint16_t convet_3_dec_asccii_to_hex(uint8_t  *x_data)//µÃµ½³¤¶È CHANGE_DECDATA_3BYTE
{
	static uint16_t Tp_long=0xffff;
	Tp_long=0xffff;
	if((x_data[0]>=48)&&(x_data[0]<=57)&&(x_data[1]>=48)&&(x_data[1]<=57)&&(x_data[2]>=48)&&(x_data[2]<=57))
	{
		Tp_long = (x_data[0]-48)*100+ (x_data[1]-48)*10 + (x_data[2]-48);
	}
	
	return Tp_long;
}


uint32_t conver_4_hex_asccii_to_hex(uint8_t  *x_data)   //CHANGE_HEXDATA_4BYTE
{
	static uint32_t Tp_long=0xfffff;
	Tp_long=0xfffff;
	if(((x_data[3]>=0x30)&&(x_data[3]<=0x39))||((x_data[3]>=0x41)&&(x_data[3]<=0x46)))
	{
	if(((x_data[2]>=0x30)&&(x_data[2]<=0x39))||((x_data[2]>=0x41)&&(x_data[2]<=0x46)))
	{
	if(((x_data[0]>=0x30)&&(x_data[0]<=0x39))||((x_data[0]>=0x41)&&(x_data[0]<=0x46)))
	{
		if(((x_data[1]>=0x30)&&(x_data[1]<=0x39))||((x_data[1]>=0x41)&&(x_data[1]<=0x46)))
	  {
		  if(x_data[0]>=0x41)
			{
				Tp_long = (x_data[0]-55)*4096;
			}
			else
			{
				Tp_long = (x_data[0]-48)*4096;
			}
			if(x_data[1]>=0x41)
			{
				Tp_long = Tp_long + (x_data[1]-55)*256;
			}
			else
			{
				Tp_long = Tp_long + (x_data[1]-48)*256;
			}
			if(x_data[2]>=0x41)
			{
				Tp_long = Tp_long + (x_data[2]-55)*16;
			}
			else
			{
				Tp_long = Tp_long + (x_data[2]-48)*16;
			}
			if(x_data[3]>=0x41)
			{
				Tp_long = Tp_long + (x_data[3]-55);
			}
			else
			{
				Tp_long = Tp_long + (x_data[3]-48);
			}
	  }
	}
 }
}
	return Tp_long;
}

uint32_t conver_x_hex_asccii_to_hex(uint8_t  *x_data,uint8_t x_long)  //CHANGE_HEXDATA_xBYTE
{
	  uint8_t Tp_i =0;
	  uint32_t Tp_long=0;
	
	  for(Tp_i=0;Tp_i<x_long;Tp_i++)
	  {
			 Tp_long = (Tp_long << 4);
			if(((x_data[Tp_i]>=0x30)&&(x_data[Tp_i]<=0x39))||((x_data[Tp_i]>=0x41)&&(x_data[Tp_i]<=0x46)))
	    {
		     if(x_data[Tp_i]>=0x41)
			  {
			   	Tp_long |= (x_data[Tp_i]-55);
		  	}
		  	else
		  	{
				  Tp_long |= (x_data[Tp_i]-48);
			  }
	    }
			else
			{
				return 0xffffffff;
			}
			
	  }
		
			return Tp_long;
}

uint16_t convert_2_dec_ascii_to_hex(uint8_t  *x_data)//µÃµ½³¤¶È  CHANGE_DECDATA_2BYTE
{
	static uint16_t Tp_long=0xffff;
	Tp_long=0xffff;
	if((x_data[0]>=48)&&(x_data[0]<=57)&&(x_data[1]>=48)&&(x_data[1]<=57))
	{
		Tp_long = (x_data[0]-48)*10+ (x_data[1]-48);
	}
	
	return Tp_long;
}


uint8_t xor_check(uint8_t  *x_data,uint8_t  x_long)
{
	uint8_t Tp_result=0,Tp_i;
	for(Tp_i=0;Tp_i<x_long;Tp_i++)
	{
	   Tp_result = Tp_result ^ x_data[Tp_i];
	}
	return Tp_result;
}

void code_protocol_error(int8_t type,uint8_t Value_1,uint16_t Value_2,uint8_t cnt)  //code_protocol_error(ERROR_TYPE_U1,(U1A01_ERROR>>8),(int16_t)U1A01_ERROR,ERROR_TYPE_SEND_3_TIMES);  ·¢ËÍµÄ´ÎÊýÖÁÉÙÒ»´Î
{
      uint8_t i, data_len = 0;
	//uint16_t n = 0;      
	uint8_t    tx_buff_uart2_temp[TX_BUFF_UART2_MAX_LEN]={0};     //½ÓÊÕ»º³å,×î´óUSART_REC_LEN¸ö×Ö½Ú.
  uint16_t        tx_buff_uart2_num_temp=0;
	//static uint8_t   * tx_buff_uart2_temp,* tx_buff_uart2_temp_noshift;
	
	//tx_buff_uart2_temp_noshift = malloc(TX_BUFF_UART2_MAX_LEN+64);
	//tx_buff_uart2_temp = (uint8_t *)((32+(uint8_t   *)shift_pointer((uint32_t)tx_buff_uart2_temp_noshift,32)));
	//tx_buff_uart2_temp = (uint8_t *)((uint32_t)tx_buff_uart2_temp|0x80000000);
	//if(tx_buff_uart2_temp==0) return;
	
	tx_buff_uart2_temp[0]='@';
	data_len =6;
  convet_hex_to_hex_asccii(data_len,tx_buff_uart2_temp+1);

	tx_buff_uart2_temp[3]='U';
	tx_buff_uart2_temp[4]=type;

	tx_buff_uart2_temp[5]=convert_1_hex_to_hex_ascii(Value_1);

	tx_buff_uart2_temp[6]=convert_1_hex_to_hex_ascii(Value_2>>8);
  tx_buff_uart2_temp[7]=convert_1_hex_to_hex_ascii(Value_2);
	tx_buff_uart2_temp[8]='*';
     
      
      
	for(i=0;i<(cnt-1);i++) //
	{      	  
	   memcpy((tx_buff_uart2_temp+9*(1+i)),tx_buff_uart2_temp,9);
	}
	tx_buff_uart2_num_temp=9*cnt;
	
	
	//stop_tim6();
	//Touch_tim_stop();
	Touch_int_disable();
	for(i=0;i<tx_buff_uart2_num_temp;i++)
	{
     if(((TX_COUNT_IN+1)%TX_BUFF_MAX_UART)!=TX_COUNT_OUT)
		 {
		   UART2_TX_BUFF_NEW[TX_COUNT_IN] =tx_buff_uart2_temp[i];
			 TX_COUNT_IN = (TX_COUNT_IN+1)%TX_BUFF_MAX_UART;
		 }
	}
	Touch_int_enable();
	//Touch_tim_start();
	 //start_tim6(50000);  //test
	
	//if(tx_buff_uart2_temp_noshift) free(tx_buff_uart2_temp_noshift);
}


void send_code_protocol_error(void)
{
  uint8_t i;
	//tx_buff_uart2_num=9;
	//for(i=0;i<9;i++) //
	//{      	  
	//   memcpy(tx_buff_uart2,err_code_buff,9);
	//}
	
	////////stop_tim6();
	Touch_int_disable();
	for(i=0;i<9;i++)
	{
		UART2_TX_BUFF_NEW[TX_COUNT_IN] = err_code_buff[i];
		TX_COUNT_IN++;
	  TX_COUNT_IN = TX_COUNT_IN % TX_BUFF_MAX_UART;
	}
	Touch_int_enable();
	////////////start_tim6(50000);
	SEND_data();
	//uart2_send_buff(err_code_buff,9);	
}

void code_protocol_ack(uint8_t  x_xor,uint8_t len,uint8_t *buff,uint8_t type)  //SEND_ACKnHEX   type =0 buff Îªhex¡¡ÀïÃæ»á×ª³ÉASCII ;  type=1 ²»ÓÃ×ª
{
	
	uint8_t data_len = 0;
	uint16_t n = 0;  
  uint8_t Tp_i = 0;	
	//static uint8_t    tx_buff_uart2_temp[TX_BUFF_UART2_MAX_LEN]={0};     //½ÓÊÕ»º³å,×î´óUSART_REC_LEN¸ö×Ö½Ú.
  static uint8_t   * tx_buff_uart2_temp,* tx_buff_uart2_temp_noshift;
	uint16_t        tx_buff_uart2_num_temp=0;
	
	
	tx_buff_uart2_temp_noshift = malloc(TX_BUFF_UART2_MAX_LEN+64);
	tx_buff_uart2_temp = (uint8_t *)((32+(uint8_t   *)shift_pointer((uint32_t)tx_buff_uart2_temp_noshift,32)));
	tx_buff_uart2_temp = (uint8_t *)((uint32_t)tx_buff_uart2_temp|0x80000000);
	if(tx_buff_uart2_temp_noshift==0) return;
	
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
	// Touch_tim_stop();
	Touch_int_disable();
	  for(Tp_i =0 ;Tp_i <tx_buff_uart2_num_temp; Tp_i++)
	  {
			 if(((TX_COUNT_IN+1) % TX_BUFF_MAX_UART)!=TX_COUNT_OUT)
			 {
			  UART2_TX_BUFF_NEW[TX_COUNT_IN] = tx_buff_uart2_temp[Tp_i];
			  TX_COUNT_IN = (TX_COUNT_IN+1) % TX_BUFF_MAX_UART;
			 }
		}
		Touch_int_enable();
//		Touch_tim_start();
	  //start_tim6(50000);
		
		

	
	
	SEND_data();
	
	if(tx_buff_uart2_temp_noshift) free(tx_buff_uart2_temp_noshift);
}

void protocol_D8_D9_command_time_dealwith(void)  //1// 1ms run one
{
	if(COMM_STEP!=COMM_IDLE)//20181108
	{
		return;
	}
	
	if(command_D8_D9_time)
	{
		command_D8_D9_time--;
		if(command_D8_D9_time==0)
			{
				
				if((bmp_protocol[0].num)||(bmp_protocol[1].num)||(bmp_protocol[2].num))
				{
					 command_D8_D9_flag=1;
					//code_protocol_error(ERROR_TYPE_U1,(U1A01_ERROR>>16),(int16_t)U1A01_ERROR,ERROR_TYPE_SEND_3_TIMES); //
				}
				// bmp_protocol.num=0; //ÃüÁîÈ¡Ïû
			}
	}
}



uint8_t calculate_monandday(uint8_t x_year,uint8_t x_mon)
{
  //?? ?????
  uint8_t Tp_day = 0;
  uint16_t Tp_year=0;
  Tp_year = 2000+ x_year;
	
  switch(x_mon)
  {    
     case 1:    
     case 3:    
     case 5:   
     case 7:    
     case 8:    
     case 10:    
     case 12:Tp_day=31;break;    
     case 4:    
     case 6:    
     case 9:    
     case 11:Tp_day=30;break;    
     case 2:Tp_day=28;
		        if(((Tp_year%4==0)&&(Tp_year%100!=0))||(Tp_year%400==0))
						{
							Tp_day=29;
						}							
     default :break;    
  }
  return  Tp_day;
}	





void COMM_check(void)
{
	
	static uint8_t Temp_long_arry[2];
	static uint16_t Tp_long = 0;
	static uint8_t Tp_rx[256]={0};
	uint8_t Tp_FSC = 0;
	static uint16_t number=0;
	
	//if((SOUND_str.Soung_SPI_BUSY == SPI_IDLE)&&(SYSTEM_ERR_STATUS->bits.COMM_Handle_FINISH==0))//ÓïÒô¿ÕÏÐÊ±²ÅÄÜ½øÈë
	//{
	   
		 switch(COMM_STEP)
		 {
			 case COMM_IDLE:
				 if(RX_COUNT_IN!=RX_COUNT_OUT)
				 {
					 COMM_STEP = WAIT_LEN;
					 WAIT_LEN_TIMER = time1ms_count;
				 }
			 break;
			 case WAIT_LEN:
				 if(UART2_RX_BUFF[RX_COUNT_OUT]!='@')//µÚÒ»¸ö×Ö·û²»¶Ô
				 {
					#ifdef  SYSUARTPRINTF
		      sysprintf("\r\nUART_receive_err1,UART2_RX_BUFF[RX_COUNT_OUT]=%d\r\n",UART2_RX_BUFF[RX_COUNT_OUT]);
					 sysprintf("\r\nUART2_RX_BUFF addr=0x%x\r\n",UART2_RX_BUFF);
					 #endif
					 RX_COUNT_OUT = (RX_COUNT_OUT+1)%RX_BUFF_MAX;
					 COMM_STEP = COMM_IDLE;
					// code_protocol_error(ERROR_TYPE_U1,(U1A01_ERROR>>16),(int16_t)U1A01_ERROR,ERROR_TYPE_SEND_3_TIMES); // ·¢ËÍµÄ´ÎÊýÖÁÉÙ				
					 return;
				 }
				 number = GET_READ_RXNUM(RX_COUNT_IN,RX_COUNT_OUT,RX_BUFF_MAX);
			   if(number>=3)//±íÊ¾³¤¶ÈÒÑ¾­À´ÁË
				 {
					 COMM_STEP = LEN_JUDGE;
					 sysprintf("COMM_STEP = LEN_JUDGE\r\n");
					 return;
				 }
				 if(get_timego(WAIT_LEN_TIMER)>3)//¼ÆËãÖµ38400bps 3¸öB ÀíÂÛÖµ625us
				 {
					 RX_COUNT_OUT = RX_COUNT_IN;//³Ù³Ù²»À´
					 COMM_STEP = COMM_IDLE;
					 code_protocol_error(ERROR_TYPE_U1,(U1A01_ERROR>>16),(int16_t)U1A01_ERROR,ERROR_TYPE_SEND_3_TIMES); // ·¢ËÍµÄ´ÎÊýÖÁÉÙ				
					 //SEND_DATA_COPY((uint8_t *)ERR_ILLEGAL,27);
					 return;
				 }
				 break;
			 case LEN_JUDGE:
				 Temp_long_arry[0] =*(uint8_t  *)(UART2_RX_BUFF+(RX_COUNT_OUT+1)%RX_BUFF_MAX) ;
  			 Temp_long_arry[1] =*(uint8_t  *)(UART2_RX_BUFF+(RX_COUNT_OUT+2)%RX_BUFF_MAX) ;
 				 Tp_long = CHANGE_HEXDATA_2BYTE((uint8_t  *)(Temp_long_arry));
			   if(Tp_long == 0xffff)//³¤¶È²»ºÏ·¨
				 {
					 RX_COUNT_OUT = (RX_COUNT_OUT+1)%RX_BUFF_MAX;
					 COMM_STEP = COMM_IDLE;
					 code_protocol_error(ERROR_TYPE_U1,(U1A01_ERROR>>16),(int16_t)U1A01_ERROR,ERROR_TYPE_SEND_3_TIMES); // ·¢ËÍµÄ´ÎÊýÖÁÉÙ				
					 return;
				 }
				 else//µÈ´ýËùÓÐµÄÊý¾Ý
				 {
					 WAIT_LEN_TIMER = time1ms_count;
					 COMM_STEP =WAIT_ALL;
					 return;
				}
				 break;
			 case WAIT_ALL:
				 number = GET_READ_RXNUM(RX_COUNT_IN,RX_COUNT_OUT,RX_BUFF_MAX);
			   if(number>=(Tp_long+3))
				 {
					  COMM_STEP =COMM_HANDLE;
					  return;
				 }
				 if(get_timego(WAIT_LEN_TIMER)>100)//¼ÆËãÖµ38400bps 256¸öB ÀíÂÛÖµ53ms
				 {
					 RX_COUNT_OUT = (RX_COUNT_OUT+3)%RX_BUFF_MAX;//³Ù³Ù²»À´
					 COMM_STEP = COMM_IDLE;
					 code_protocol_error(ERROR_TYPE_U1,(U1A01_ERROR>>16),(int16_t)U1A01_ERROR,ERROR_TYPE_SEND_3_TIMES); // ·¢ËÍµÄ´ÎÊýÖÁÉÙ				
					// SEND_DATA_COPY((uint8_t *)ERR_ILLEGAL,27);
					 return;
				 }
				 break;
			 case COMM_HANDLE:
				 if(UART2_RX_BUFF[(RX_COUNT_OUT+Tp_long+2)%RX_BUFF_MAX]!='*')
				 {
					// RX_COUNT_OUT = (RX_COUNT_OUT+Tp_long+3)%RX_BUFF_MAX;
					 RX_COUNT_OUT = (RX_COUNT_OUT+1)%RX_BUFF_MAX;
					 COMM_STEP = COMM_IDLE;
					 code_protocol_error(ERROR_TYPE_U1,(U1A01_ERROR>>16),(int16_t)U1A01_ERROR,ERROR_TYPE_SEND_3_TIMES); // ·¢ËÍµÄ´ÎÊýÖÁÉÙ				
					 //SEND_DATA_COPY((uint8_t *)ERR_ILLEGAL,27);
					 return;
				 }
				 if((RX_COUNT_OUT + Tp_long+3)>RX_BUFF_MAX)
					{
						memcpy(Tp_rx,(uint8_t *)(UART2_RX_BUFF+RX_COUNT_OUT),RX_BUFF_MAX-RX_COUNT_OUT);
						memcpy(Tp_rx+RX_BUFF_MAX-RX_COUNT_OUT,(uint8_t *)UART2_RX_BUFF,Tp_long+3+RX_COUNT_OUT-RX_BUFF_MAX);
					}
					else
					{
						memcpy(Tp_rx,(uint8_t *)(UART2_RX_BUFF+RX_COUNT_OUT),Tp_long+3);
					}
					Tp_FSC    = XORCHECK((uint8_t  *)(Tp_rx),Tp_long+3);
					if(decode_protocol((uint8_t  *)(Tp_rx),Tp_long+3,COMM_DATA_UARST)==0)//´Ó±¨ÎÄµÄµÚÈý¸ö×Ö½Ú¿ªÊ¼´¦Àí
					{
					   
						  RX_COUNT_OUT = (RX_COUNT_OUT+1)%RX_BUFF_MAX;
						  COMM_STEP = COMM_IDLE;
						  //SEND_DATA_COPY((uint8_t *)ERR_ILLEGAL,27);
						   code_protocol_error(ERROR_TYPE_U1,(U1A01_ERROR>>16),(int16_t)U1A01_ERROR,ERROR_TYPE_SEND_3_TIMES); // ·¢ËÍµÄ´ÎÊýÖÁÉÙ				
					    // command_D8_D9_time = 0;
					}
					else//ÊÇÕýÈ·´¦ÀíµÄ
					{
						  RX_COUNT_OUT = (RX_COUNT_OUT+Tp_long+3)%RX_BUFF_MAX;
						  COMM_STEP = COMM_IDLE;
						  
					}
					
					sysFlushCache(I_D_CACHE);
				 break;
			 default:break;
					
		 }
	//}//ÓïÒô¿ÕÏÐÊ±²ÅÄÜ½øÈë
	//LED_LOGO_CONTROL_OFF();//LOGO LED¹Ø±ÕµÄÂß¼­ÅÐ¶Ï
}

//·¢ËÍ´®¿ÚÊý¾Ý
void SEND_data(void)
{
	 uint16_t retval,Tp_i;
	
	
	
	
	Touch_int_disable();
	
	if((UART_TX_REAL<UART_TX_WANT)&&(UART_TX_WANT!=0))
	{
		if((UART_TX_WANT-UART_TX_REAL)>1000)//1000->UARTTXBUFSIZE[1] 
		{
/////////////////		REG_OPERATE(REG_UART1_FCR,0x06,set);
		TX_COUNT_OUT=TX_COUNT_IN;
		UART_TX_REAL = 0;
		UART_TX_WANT = 0;
		}
		return;
	}
	
	if((UART_TX_REAL>=UART_TX_WANT)&&(UART_TX_WANT>1000))
	{
///////////////		REG_OPERATE(REG_UART1_FCR,0x06,set);
	//delay_us1(1);
		UART_TX_REAL = 0;
		UART_TX_WANT = 0;
	}
	
	
	if(TX_COUNT_IN!=TX_COUNT_OUT)//ÓÐÊý¾ÝÒª·¢
	 {
		  //if(uart2_send_finish_state ==0)
			{
			uart2_send_finish_state = 1;	
		  retval = GET_READ_RXNUM(TX_COUNT_IN,TX_COUNT_OUT,TX_BUFF_MAX_UART);
		#ifdef  SYSUARTPRINTF  
		sysprintf("retval=0x%X\r\n",retval);
		#endif
				
		  for(Tp_i=0;Tp_i<retval;Tp_i++)
		  {
				#ifdef  SYSUARTPRINTF  
//				sysprintf("Tp_i=0x%X\r\n",Tp_i);
//				sysprintf("TX_COUNT_OUT=0x%X\r\n",TX_COUNT_OUT);
//				sysprintf("TX_Test+Tp_i=0x%X\r\n",TX_Test+Tp_i);
//				sysprintf("UART2_TX_BUFF_NEW+TX_COUNT_OUT=0x%X\r\n",UART2_TX_BUFF_NEW+TX_COUNT_OUT);
				#endif
				
				*(TX_Test+Tp_i)=*(UART2_TX_BUFF_NEW+TX_COUNT_OUT);
				TX_COUNT_OUT = (TX_COUNT_OUT+1)%TX_BUFF_MAX_UART;
				
				#ifdef  SYSUARTPRINTF  
//				sysprintf("*(TX_Test+Tp_i)=%c\r\n",*(TX_Test+Tp_i));
				#endif
			}
			
				
			
			#ifdef  SYSUARTPRINTF  
		sysprintf("param.ucUartNo=0x%X\r\n",param.ucUartNo);
		#endif
			
			UART_TX_WANT+=retval;
			uartWrite(param.ucUartNo, TX_Test, retval);
			
			//REG_OPERATE(REG_UART1_FCR,0X04,set);
			
				#ifdef  SYSUARTPRINTF  
		sysprintf("uartWrite\r\n");
		#endif
		  }
			
	}	 
		Touch_int_enable();	
}

void judge_uart2_receive_end(void)  // run 1ms
{
	if(rx_buff_uart2_delay)
	{			
		rx_buff_uart2_delay--;
		if(rx_buff_uart2_delay==0)
		{					
        if(rrx_buff_uart2_cnt>4)  //°´Ð­ÒéÖÁÉÙÈý¸ö×Ö½Ú
				{
					rx_finish_state_uart2=1;
				}
    }
	}	

}


void comm_handle(void)
{
  static uint16_t retval,Tp_i;
	memset(RX_Test,0,2048);
	retval = uartRead(param.ucUartNo, RX_Test, 2048);
	
	for(Tp_i=0;Tp_i<retval;Tp_i++)
	{
		#ifdef  SYSUARTPRINTF
		 sysprintf("\r\nUART_receive\r\n");
		// sysprintf("\r\nUART2_RX_BUFF=%d\r\n",UART2_RX_BUFF[1]);
		#endif
		if(download.start == 0)
		{
		if(((RX_COUNT_IN+1)%RX_BUFF_MAX)!=RX_COUNT_OUT)
		{
		UART2_RX_BUFF[RX_COUNT_IN++]=RX_Test[Tp_i];
		RX_COUNT_IN = RX_COUNT_IN%RX_BUFF_MAX;
		}
	  }
		else if(download.start == 1)
		{
		
	  }
	}
	
	
	
	if(READ_PIN_SW1_2==SW_ON)//20180928
	{
		return;
	}
	//if((READ_PIN_SW1_3==SW_ON)&&((FLAG_SW_FINISH&0x04)==0))
	if(READ_PIN_SW1_3==SW_ON)//20180928
	{
		return;
	}
	if(READ_PIN_SW1_4==SW_ON)//20180928
	//if((READ_PIN_SW1_4==SW_ON)&&((FLAG_SW_FINISH&0x08)==0))
	{
		return;
	}
	if(READ_PIN_SW1_6==SW_ON)//20181130
	{
		return;
	}	
	
	if(rx_finish_state_uart2)  //uart2_send_finish_state
		{
			rx_finish_state_uart2=0;
				//LED_POWER_TOGGLE();
			#if 0
			if(decode_protocol(bak_rx_buff_uart2,bak_rx_buff_uart2_cnt,COMM_DATA_UARST)==0)
		  #endif
	 }
	
	COMM_check();
	SEND_data();
	
}

uint8_t  decode_protocol(uint8_t *buff,uint16_t len,uint8_t type)
{
  
     //TIME_TYPE  time;
	static RTC_TIME_DATA_T time_T;
	 uint8_t data_len;//
	uint8_t n=0;
	uint16_t Tp_i = 0,Tp_count_field,Tp_ii;
	static uint16_t Tp_bmp_filename[PE_FILE_MAX];
	
	
	uint8_t Tp_xor=0;   //Tp_FSC
	static uint8_t  ack_buf[128];
	int ack_buf_len,tp_area_no;
 // BaseAreaAll *pAreaBase;
	
	uint8_t flag1=0,flag=0,i_return_flag=0; //,tp_data_class,tp_number;		 
	//uint32_t tp_start_addr,tp_size;
	//FIELD_TYPE  tp_field[16];
	AreaConfig  tp_field;	
	AreaConfig  *p_gs_AreaInfo=NULL;
       uint16_t Tp_data1;
	uint32_t Tp_data2;
	uint32_t Tp_data3;
      int filed_num;
	void *addr;
	uint16_t Tp_bmp_num[3];
  
	uint16_t Q0_i;
	uint8_t Tp_field;
	
  uint32_t Q0_addr;
	
	uint32_t Tp_bmp_filename_new,Tp_field_addr;
	uint32_t Tp_checksum2;
//	uint16_t Tp_iiii;
	uint8_t Tp_layer;
  Tp_xor=xor_check(buff,len);  //XORCHECK
	data_len =convet_2_hex_ASCII_to_hex(buff + 1);  // CHANGE_HEXDATA_2BYTE(buff + 1);
	uint32_t Tp_data_uint32;
	 static BmpFIleInfo Tp_info={0};
	 //static  int width, height;
//	  static  char dirPath[24]={0};
	static	uint32_t Tp_index;
	
	
	 		//ÔÚ½ÓÊÕD8 D9 ÓÐºóÐøÃüÁî£¬ÊÕµ½ÆäËüÃüÁî³ÉÈ¡Ïû
	 if(command_D8_D9_time)
	{
         if(buff[3]!='D')//IICÀ´µÄ²»¹Ü 20181108
         	{
                    //
                    //bmp_protocol.num=0; //ÃüÁîÈ¡Ïû
                    code_protocol_error(ERROR_TYPE_U1,(U1A02_ERROR>>16),(int16_t)U1A02_ERROR,ERROR_TYPE_SEND_3_TIMES); // ·¢ËÍµÄ´ÎÊýÖÁÉÙ
				            command_D8_D9_time = 0;//20220401
                    return 1;
         	}

	}
	 
	 
	COMMAND_AP = buff[3];
	COMMAND_IN = TX_COUNT_IN;
	
	switch(buff[3])
	{
		case 'D'://???????
			    if(buff[16]!=','&&buff[16]!='*') return 0;//????l????1??????	
			    if ((buff[4] == '0') || (buff[4] == '1'))
				{
					command_D8_D9_time = 0;
				}
			   switch(buff[4])
				 {
					 case '0':
					 case '1':
           case '8'://20170713 ???D2??????????????  ??????j????LCD?????????		
           case '9':	
						     Tp_i=0;	
					       Tp_bmp_num[0] =  bmp_protocol[0].num;
					       Tp_bmp_num[1] =  bmp_protocol[1].num;
					       Tp_bmp_num[2] =  bmp_protocol[2].num;
					       memcpy(bmp_protocol_bak,bmp_protocol,sizeof(bmp_protocol));
					 
					       do
							 {
								
								
								
								if((buff[11+Tp_i*12]>'3')||(buff[11+Tp_i*12]<'1')) return 0 ;
								
								Tp_layer=convert_1_dec_ascii_to_hex(buff+11+Tp_i*12);

								if(Tp_layer==0xff)
									{
										return 0;
									}
									
									
									Tp_layer = Tp_layer - 1;
									
								if(Tp_layerover&(1<<Tp_layer))	
								{
									Tp_bmp_num[Tp_layer]=0;
									 bmp_protocol_bak[Tp_layer].num = 0;
									Tp_layerover&= ~(1<<Tp_layer);
								}
									
                
								if( Tp_bmp_num[Tp_layer]>=TEMP_BMP_FILE_INFO_MAX_NUM)//128????????
                {
									//bmp_protocol.num=(TEMP_BMP_FILE_INFO_MAX_NUM-1);
								  //Tp_bmp_num[Tp_layer]=0;
									return 0;	
										
                }	
								
								Tp_info.layer =  Tp_layer +1;
								Tp_info.x     =   convet_3_dec_asccii_to_hex(buff+5+Tp_i*12);
								Tp_info.y     =   convet_3_dec_asccii_to_hex(buff+8+Tp_i*12);
							  //bmp_protocol[Tp_layer].bmp[Tp_bmp_num[Tp_layer]].layer = Tp_layer +1;
								//bmp_protocol[Tp_layer].bmp[Tp_bmp_num[Tp_layer]].x=convet_3_dec_asccii_to_hex(buff+5+Tp_i*12);
								//bmp_protocol[Tp_layer].bmp[Tp_bmp_num[Tp_layer]].y=convet_3_dec_asccii_to_hex(buff+8+Tp_i*12);
								if(Tp_info.x==0xffff)
									{
										return 0;
									}
								if(Tp_info.y==0xffff)
									{
										return 0;
									}
								
                if(conver_4_hex_asccii_to_hex(buff+12+Tp_i*12)==0xfffff) return 0;
								Tp_info.name=conver_4_hex_asccii_to_hex(buff+12+Tp_i*12);
								if((Tp_info.layer<1)||(Tp_info.layer>3)) return 0;
								
								 if (Tp_info.x > 800 || Tp_info.y > 480)
	               {
		                systerm_error_status.bits.draw_coordinatebeyond_error=1; 
				            //return 0;
	                }
                  
									if(GetBmpFIleSize_SDRAMfrist(Tp_info.name,&Tp_info.width,&Tp_info.height)==0)
			           {
				              systerm_error_status.bits.draw_filenotfound_error=1;
				 //return 0;
			           }
		              if(((Tp_info.x+Tp_info.width)>800)||((Tp_info.y+Tp_info.height)>480))
			          {
				                systerm_error_status.bits.draw_coordinatebeyond_error=1; 
  				            //return 0;
			          }
								
								
								
								if(systerm_error_status.bits.draw_coordinatebeyond_error)
								{
									  systerm_error_status.bits.draw_coordinatebeyond_error = 0;
										 code_protocol_error(ERROR_TYPE_U1,(U1802_ERROR>>16),(int16_t)U1802_ERROR,ERROR_TYPE_SEND_3_TIMES); 
									   return 1;//???????+802 cyw
							  }
								if(systerm_error_status.bits.draw_filenotfound_error)
							 {
									  systerm_error_status.bits.draw_filenotfound_error = 0;
									  code_protocol_error(ERROR_TYPE_U1,(U1801_ERROR>>16),(int16_t)U1801_ERROR,ERROR_TYPE_SEND_3_TIMES); 
										return 1;//?????????

							}
					//	if(systerm_error_status.bits.layer_img_num_over_max)
					//	{
					//				   systerm_error_status.bits.layer_img_num_over_max=0;
					//				   return 0;	//???????
					//		}
									 // return 0;
								//bmp_protocol_bak[Tp_layer].num=Tp_bmp_num[Tp_layer];
							Tp_index = CHECK_Same(Tp_info.x,Tp_info.y,Tp_info.width,Tp_info.height,Tp_layer);
									if(Tp_index!=0xffffffff)
									{
										
										//bmp_protocol_bak[Tp_layer].num=Tp_bmp_num[Tp_layer];
										bmp_protocol_bak[Tp_layer].bmp[Tp_index].x=Tp_info.x;
										bmp_protocol_bak[Tp_layer].bmp[Tp_index].y=Tp_info.y;
										bmp_protocol_bak[Tp_layer].bmp[Tp_index].name=Tp_info.name;
										bmp_protocol_bak[Tp_layer].bmp[Tp_index].layer=Tp_info.layer;
										bmp_protocol_bak[Tp_layer].bmp[Tp_index].height=Tp_info.height;
										bmp_protocol_bak[Tp_layer].bmp[Tp_index].width=Tp_info.width;
									}
									else
									{
										
										bmp_protocol_bak[Tp_layer].num=Tp_bmp_num[Tp_layer];
										bmp_protocol_bak[Tp_layer].bmp[Tp_bmp_num[Tp_layer]].x=Tp_info.x;
										bmp_protocol_bak[Tp_layer].bmp[Tp_bmp_num[Tp_layer]].y=Tp_info.y;
										bmp_protocol_bak[Tp_layer].bmp[Tp_bmp_num[Tp_layer]].name=Tp_info.name;
										bmp_protocol_bak[Tp_layer].bmp[Tp_bmp_num[Tp_layer]].layer=Tp_info.layer;
										bmp_protocol_bak[Tp_layer].bmp[Tp_bmp_num[Tp_layer]].height=Tp_info.height;
										bmp_protocol_bak[Tp_layer].bmp[Tp_bmp_num[Tp_layer]].width =Tp_info.width;
										Tp_bmp_num[Tp_layer]++;
									}
									if(Tp_bmp_num[Tp_layer] >= LAY_MAX_BMP_CNT)
									{
										return 0;
									}
									
								bmp_protocol_bak[Tp_layer].num=Tp_bmp_num[Tp_layer];
									
								Tp_i++;
							//	Tp_layerover=Tp_layerover|(1<<Tp_layer);	
							//	Tp_bmp_num[Tp_layer]++;
                 
								
							}
							while((buff[16+(Tp_i-1)*12]==',')&&Tp_i<256);
							
							//bmp_protocol[0].num = Tp_bmp_num[0];
							//bmp_protocol[1].num = Tp_bmp_num[1];
							//bmp_protocol[2].num = Tp_bmp_num[2];
							memcpy(bmp_protocol,bmp_protocol_bak,sizeof(bmp_protocol));
							
							
							if ((buff[4] == '0') || (buff[4] == '1'))
							{	
								//systerm_error_status.bits.layer_img_num_over_max=0; //
                #ifdef  SYSUARTPRINTF
	              sysprintf("bmp_protocol[0].num=0x%X\r\n",bmp_protocol[0].num);
								sysprintf("bmp_protocol[1].num=0x%X\r\n",bmp_protocol[1].num);
								sysprintf("bmp_protocol[2].num=0x%X\r\n",bmp_protocol[2].num);
	              #endif
								if(bmp_protocol[0].num) Tp_layerover=Tp_layerover|(1<<0);
								if(bmp_protocol[1].num) Tp_layerover=Tp_layerover|(1<<1);
								if(bmp_protocol[2].num) Tp_layerover=Tp_layerover|(1<<2);
								DisCmdDisplay((buff[4] - '0'),bmp_protocol[0].bmp,bmp_protocol[0].num,bmp_protocol[1].bmp,bmp_protocol[1].num,bmp_protocol[2].bmp,bmp_protocol[2].num);                                                         
						
								systerm_error_status.bits.nand_canot_find_Q_bmp_error=0; //DisCmdDisplay ?l???1?????????????????


								

							}
							else
							{
								Tp_layerover = 0;
							}
							

 
							if((buff[4]=='0')||(buff[4]=='8'))
						   	{
								code_protocol_ack(Tp_xor,0,NULL,0);
						   	}
						
						 break;
					 default:
						 break;										 
				 }

			   //
				    
			    	if( (buff[4]=='8')||(buff[4]=='9')  )
	          {
	             command_D8_D9_time=TIM_100MS;
			    	}				
						if((buff[4]=='1')||(buff[4]=='9'))
						{
							 return 1;
						}					
			break;			 
		case 'G':
			 switch(buff[4])
			 {
			    case '0':	
					if((buff[5]>'3')||(buff[5]<'1')) return 0 ;
					if(((buff[6]>'3')||(buff[6]<'1'))&&(buff[6]!='F')) return 0 ;

					DisPlayLayer((buff[4] - '0'), (buff[5] - '0'), (buff[6] - '0'));		//G0 ~ G5
					if(type == COMM_DATA_UARST)//Êý¾ÝÀ´ÖÁUARST
					{
					  code_protocol_ack(Tp_xor,0,NULL,0);

					}
					break;
				case '1':  
					if((buff[5]>'3')||(buff[5]<'1')) return 0 ;
					if(((buff[6]>'3')||(buff[6]<'1'))&&(buff[6]!='F')) return 0 ;

					DisPlayLayer((buff[4] - '0'), (buff[5] - '0'), (buff[6] - '0'));		//G0 ~ G5
					if(type == COMM_DATA_UARST)//Êý¾ÝÀ´ÖÁUARST
					{
					  code_protocol_ack(Tp_xor,0,NULL,0);

					}
					
				break; 

				case '2':
					if(buff[5]!='F') return 0;
					if(buff[6]!='F') return 0;

					DisPlayLayer((buff[4] - '0'), (buff[5] - '0'), (buff[6] - '0'));		//G0 ~ G5
					if(type == COMM_DATA_UARST)//Êý¾ÝÀ´ÖÁUARST
					{
					  code_protocol_ack(Tp_xor,0,NULL,0);

					}
					
				break;
				case '3':
					if(buff[5]!='F') return 0;
					if(buff[6]!='F') return 0;

					DisPlayLayer((buff[4] - '0'), (buff[5] - '0'), (buff[6] - '0'));		//G0 ~ G5
					if(type == COMM_DATA_UARST)//Êý¾ÝÀ´ÖÁUARST
					{
					  code_protocol_ack(Tp_xor,0,NULL,0);

					}	
				
				break;
				case '4':
					if((buff[5]>'3')||(buff[5]<'1')) return 0 ;
					if(((buff[6]>'3')||(buff[6]<'1'))&&(buff[6]!='F')) return 0 ;


					DisPlayLayer((buff[4] - '0'), (buff[5] - '0'), (buff[6] - '0'));		//G0 ~ G5
					if(type == COMM_DATA_UARST)//Êý¾ÝÀ´ÖÁUARST
					{
					  code_protocol_ack(Tp_xor,0,NULL,0);

					}
					
				break;
				case '5':
					if(buff[5]!='F') return 0;
					if(buff[6]!='F') return 0;

					DisPlayLayer((buff[4] - '0'), (buff[5] - '0'), (buff[6] - '0'));		//G0 ~ G5
					if(type == COMM_DATA_UARST)//Êý¾ÝÀ´ÖÁUARST
					{
					  code_protocol_ack(Tp_xor,0,NULL,0);

					}
					
					
				break;
					
				case '6':
					if((buff[5]>'3')||(buff[5]<'1')) return 0 ;
					if(((buff[6]>'3')||(buff[6]<'1'))&&(buff[6]!='F')) return 0 ;


					DisPlayLayer((buff[4] - '0'), (buff[5] - '0'), (buff[6] - '0'));		//G0 ~ G5
					if(type == COMM_DATA_UARST)//Êý¾ÝÀ´ÖÁUARST
					{
					  code_protocol_ack(Tp_xor,0,NULL,0);

					}					
				break;
				case '7':
					if(buff[5]!='F') return 0;
					if(buff[6]!='F') return 0;

					DisPlayLayer((buff[4] - '0'), (buff[5] - '0'), (buff[6] - '0'));		//G0 ~ G5
					if(type == COMM_DATA_UARST)//Êý¾ÝÀ´ÖÁUARST
					{
					  code_protocol_ack(Tp_xor,0,NULL,0);

					}															
				break;	
				default:
				break;
				}
			    break;
		 case 'P':
					 
		       command_xor=Tp_xor;
		       USB_IMAGE_TYPE = buff[4];
		       USB_HAS_USABLE_IMG = 0;
		       IMAGE_SEARCHED =0;
		       if(buff[4]=='E')
					 {
							Tp_i=0;
							n=0;
				     do
				     {
						/*
						if(conver_4_hex_asccii_to_hex(buff+5+Tp_i*5) == 0xFFFFF) return 0;
						File_name_PE[File_count_PE] = conver_4_hex_asccii_to_hex(buff+5+Tp_i*5);	
						if(File_name_PE[File_count_PE] < 0xff00)  return 0;
						File_count_PE ++;
                                 */
							if(n>=PE_FILE_MAX)
							{
								n=PE_FILE_MAX-1;
							}
							Tp_bmp_filename[n]= conver_4_hex_asccii_to_hex(buff+5+Tp_i*5);	
							if(Tp_bmp_filename[n]== 0xFFFF) return 0;						
							if(Tp_bmp_filename[n] < 0xff00)  return 0;
					
							Tp_i++;
							n++;
						}
							while((buff[9+(Tp_i-1)*5]==',')&&Tp_i<256);


							ack_buf[0]=0X16;
							if(type == COMM_DATA_UARST)
							{ 
									wait_send_over();
									code_protocol_ack(Tp_xor,1,ack_buf,0);	
							}
			        
							usb_init();
							
							UsbWriteNandFlash(0x0e,Tp_bmp_filename, n);
					 }
					 else
					 {
						 if(buff[9]!='*') return 0;//·ÀÖ¹ÎÄ¼þÃû¶à1Î»Ò²»ØÎÄ
				    if(conver_4_hex_asccii_to_hex(buff+5) != 0xFFFF) return 0;
				    if((buff[4]=='0')||(buff[4]=='1')||(buff[4]=='2')||(buff[4]=='3')
				    ||(buff[4]=='A')||(buff[4]=='B')||(buff[4]=='C')||(buff[4]=='D'))//Í¼ÏñÊý¾ÝÐ´Èë  //20170713×·¼ÓÍ¼Æ¬¸üÐÂ·½Ê½ @03P1*
				    {
					
					       ack_buf[0]=0X16;
					       if (buff[4] == '0' || buff[4] == 'A')
					      {
			
						    if (type == COMM_DATA_UARST)
						   {
							    wait_send_over();
							    code_protocol_ack(Tp_xor, 1, ack_buf,0);
						   }
							 
							 usb_init();
						
						   if(buff[4] == '0')
						   {
							    //nandflash_format_disk0();
							    UsbWriteNandFlash(buff[4] - '0', Tp_bmp_filename, 0);
						   }
						
						   if(buff[4] == 'A')
						  {
							
							  UsbWriteNandFlash(buff[4] - 'A' + 0xa, Tp_bmp_filename, 0);
						   }
					     }
					     else
					    {
						//SetUsbCmd(1);
						if (type == COMM_DATA_UARST)
						{
							 wait_send_over();
							code_protocol_ack(Tp_xor, 1, ack_buf,0);
						}
						//UsbWriteNandFlash((buff[4] & 0x0f), &Tp_bmp_filename, 0);
						if (buff[4] >= '0' && buff[4] <= '9')
						{
							UsbWriteNandFlash(buff[4] - '0', Tp_bmp_filename, 0);
						}
						if (buff[4] >= 'A' && buff[4] <= 'F')
						{
							UsbWriteNandFlash(buff[4] - 'A' + 0xa, Tp_bmp_filename, 0);
						}
					  }

					
				}
				else
					return 0;
					 }
		     break;
		  case 'Q':
				if(buff[9]!=','&&buff[9]!='*') return 0;//·ÀÖ¹ÎÄ¼þÃû¶à1Î»Ò²»ØÎÄ
			  	switch(buff[4])
				{
				case '0':
					
					CpyNandToSdramCMd((buff[4]&0x0f),NULL, 0);	//Çåµô
					//¼Ó¶ÔÓ¦µÄÎÄ¼þ
					Tp_i=0;
					do
					{

						Tp_data2=conver_4_hex_asccii_to_hex(buff+5+Tp_i*5);
						if(Tp_data2!=0xfffff)
						{
							//Tp_bmp_setting[Tp_bmp_setting_layer][2]=Tp_bmp_filename;
							Tp_bmp_filename[0] = Tp_data2;
						}
						else
						{
							return 0;
						}						
						if(CpyNandToSdramCMd(1,Tp_bmp_filename, 1)==0)
            {
							break;
						}							
						Tp_i++;
					}
					while((buff[9+(Tp_i-1)*5]==',')&&Tp_i<256);


					if((systerm_error_status.bits.nand_canot_find_Q_bmp_error)||(systerm_error_status.bits.image_movenantosdram_error))
					{
						return 1;
					}
					
					if(type == COMM_DATA_UARST)
					{
					   code_protocol_ack(Tp_xor,0,NULL,0);
					}
					break;
										
				case '1':
					
					Tp_i=0;
					do
					{

						Tp_data2=conver_4_hex_asccii_to_hex(buff+5+Tp_i*5);
						if(Tp_data2!=0xfffff)
						{
							//Tp_bmp_setting[Tp_bmp_setting_layer][2]=Tp_bmp_filename;
							Tp_bmp_filename[0] = Tp_data2;
						}
						else
						{
							return 0;
						}

						
						if(CpyNandToSdramCMd((buff[4]&0x0f),Tp_bmp_filename, 1)==0)
						{
							break;
						}
						
						Tp_i++;
					}
					while((buff[9+(Tp_i-1)*5]==',')&&Tp_i<256);

					if((systerm_error_status.bits.nand_canot_find_Q_bmp_error)||(systerm_error_status.bits.image_movenantosdram_error))
					{
						return 1;
					}			 

				
					if(type == COMM_DATA_UARST)
					{
					   code_protocol_ack(Tp_xor,0,NULL,0);
					}
				break;
					default:break;
				}
			  break;
			case 'C'://ÊµÊ±Ê±ÖÓ¶ÁÈ¡ ºÍÉèÖÃ
			switch(buff[4])
			{
				case '0':
					
				  if(buff[17]!='*') return 0;//·ÀÖ¹ÎÄ¼þÃû¶à1Î»Ò²»ØÎÄ
          get_vbat_ad_value(); 
				  
				  if(systerm_error_status.bits.lse_error==1)	//µ±Ç°ÎªRTCÃ»ÓÐµç
					{
						#ifdef  SYSUARTPRINTF  
						sysprintf("vat err\r\n");
            #endif						
						return 0;
					}
					
					time_T.u8cClockDisplay = RTC_CLOCK_24;
					time_T.u32Year = convert_2_dec_ascii_to_hex(buff+5);
					 time_T.u32cMonth=convert_2_dec_ascii_to_hex(buff+7);
					 time_T.u32cDay=convert_2_dec_ascii_to_hex(buff+9);
					 time_T.u32cHour=convert_2_dec_ascii_to_hex(buff+11);
					 time_T.u32cMinute=convert_2_dec_ascii_to_hex(buff+13);
					  time_T.u32cSecond=convert_2_dec_ascii_to_hex(buff+15);
					  
					if((time_T.u32Year ==0xFF)||(time_T.u32Year>99)||(time_T.u32Year<15))
					{
							#ifdef  SYSUARTPRINTF  
						sysprintf("u32Year err\r\n");
            #endif
						return 0;//³ö½çÁË »ØÃ°
					}
				  time_T.u32Year+=RTC_YEAR2000;
					if((time_T.u32cMonth==0xFF)||(time_T.u32cMonth==0)||(time_T.u32cMonth>12))
					{
							#ifdef  SYSUARTPRINTF  
						sysprintf("u32cMonth err\r\n");
            #endif
						return 0;//³ö½çÁË »ØÃ°
					}
					if((time_T.u32cDay==0xFF)||(time_T.u32cDay==0)||(time_T.u32cDay>calculate_monandday(time_T.u32Year-RTC_YEAR2000,time_T.u32cMonth)))
					{
							#ifdef  SYSUARTPRINTF  
						sysprintf("u32cDay err\r\n");
            #endif
						return 0;//³ö½çÁË »ØÃ°
					}
					if((time_T.u32cHour==0xFF)||(time_T.u32cHour>23))
					{
								#ifdef  SYSUARTPRINTF  
						sysprintf("u32cHour err\r\n");
            #endif
						return 0;//³ö½çÁË »ØÃ°
					}
					if((time_T.u32cMinute==0xFF)||(time_T.u32cMinute>59))
					{
								#ifdef  SYSUARTPRINTF  
						sysprintf("u32cMinute err\r\n");
            #endif
						return 0;//³ö½çÁË »ØÃ°
					}
					if((time_T.u32cSecond==0xFF)||(time_T.u32cSecond>59))
					{
								#ifdef  SYSUARTPRINTF  
						sysprintf("u32cSecond err\r\n");
            #endif
						return 0;//³ö½çÁË »ØÃ°
					}
					
					

					RTC_Write( RTC_CURRENT_TIME,&time_T);
					  systerm_error_status.bits.rtc_no_bat_after_no_set=0;
				
					if(type == COMM_DATA_UARST)  //if(type == COMM_DATA_UARST)
					    code_protocol_ack(Tp_xor,0,NULL,0);
					break;
				case '1':
					if(buff[5]!='*') return 0;//·ÀÖ¹ÎÄ¼þÃû¶à1Î»Ò²»ØÎÄ
					get_vbat_ad_value(); 
				 	if((systerm_error_status.bits.lse_error==1) || (systerm_error_status.bits.rtc_no_bat_after_no_set)) //if(0)  //if(SYSTEM_ERR_STATUS->bits.VBAT_error==1)	//µ±Ç°ÎªRTCÃ»ÓÐµç
					{
						ack_buf[0] = 0;
						ack_buf[1] = 0;
						ack_buf[2] = 0;
						ack_buf[3] = 0x99;
						ack_buf[4] = 0x99;
						ack_buf[5] = 0x99;
					}
					else//RTCÓÐµç
					{
						//RTC_GetTAndD(ack_buf);
						//read_rtc((TIME_TYPE*)ack_buf);
						RTC_Read(RTC_CURRENT_TIME,&time_T);
						ack_buf[0] = DECTOHEX(time_T.u32Year-RTC_YEAR2000); //0x17
						ack_buf[1] = DECTOHEX(time_T.u32cMonth);
						ack_buf[2] = DECTOHEX(time_T.u32cDay);
						ack_buf[3] = DECTOHEX(time_T.u32cHour);
						ack_buf[4] = DECTOHEX(time_T.u32cMinute);
						ack_buf[5] = DECTOHEX(time_T.u32cSecond);
					}
				  if(type == COMM_DATA_UARST)
				  	{
				      		 code_protocol_ack(Tp_xor,6,ack_buf,0); //SEND_ACKnDEC(Tp_FSC,6,ack_buf);  code_protocol_send_time(6,ack_buf); //
				  	}
					break;
				default:
					break;
			}
			 break;	 	
			case 'M':
				if(buff[8]!='*') return 0;//·ÀÖ¹ÎÄ¼þÃû¶à1Î»Ò²»ØÎÄ
			switch(buff[4])
			{
				case '2'://µ÷¹â
					if(convet_3_dec_asccii_to_hex(buff+5)==0XFFFF) return 0;
					if(convet_3_dec_asccii_to_hex(buff+5)>255) return 0;
					//Backlinght_Control_Init_HARDV3(CHANGE_DECDATA_3BYTE(buff+5));
					para.lcd_back_light.brightness=convet_3_dec_asccii_to_hex(buff+5); //  sdram_lcdbacklight = convet_3_dec_asccii_to_hex(buff+5);
					//sdram_lcdbacklight_verfi = ~(sdram_lcdbacklight);
					//write_para();
				   
				  sprintf((char *)(BaseData_ARR+BASE_data_lcdbackligt*9),"%08X",para.lcd_back_light.brightness);
					if(para.lcd_back_light.state== 0) //if(LCD_BACKLIGHT_ONOFF == 0)
					{
						Backlinght_Control_Init_HARDV4(convet_3_dec_asccii_to_hex(buff+5));
					}
					if(type == COMM_DATA_UARST)
					{
					   code_protocol_ack(Tp_xor,0,NULL,0);
					}
				break;
				case '1':// »Ø¸´Õý³£Ä£Ê½  
					if((buff[5]!='F')||(buff[6]!='F')||(buff[7]!='F')) return 0;
					para.lcd_back_light.state=0; //LCD_BACKLIGHT_ONOFF =0;
					//write_para( );
				  sprintf((char*)(BaseData_ARR+BASE_data_PowerSave*9),"%08X",para.lcd_back_light.state);
					 Backlinght_Control_Init_HARDV4(para.lcd_back_light.brightness); //LCD_CONTROL_BACKLIGHT_ON();
					if(type == COMM_DATA_UARST)
					{
					   code_protocol_ack(Tp_xor,0,NULL,0);
					}
				break;
				case '0'://  ½øÈëÊ¡µçÄ£Ê½ºó
					if((buff[5]!='F')||(buff[6]!='F')||(buff[7]!='F')) return 0;
					para.lcd_back_light.state=1; //LCD_BACKLIGHT_ONOFF =1;
					//write_para();
				  sprintf((char*)(BaseData_ARR+BASE_data_PowerSave*9),"%08X",para.lcd_back_light.state);
					 Backlinght_Control_Init_HARDV4(255); //LCD_CONTROL_BACKLIGHT_OFF();
					if(type == COMM_DATA_UARST)
					{
					   code_protocol_ack(Tp_xor,0,NULL,0);
					}
				break;
				
				case '4'://¸Ð¶È
					if(convet_3_dec_asccii_to_hex(buff+5)==0XFFFF) return 0;
					if(convet_3_dec_asccii_to_hex(buff+5)>3) return 0;
					if(convet_3_dec_asccii_to_hex(buff+5)==0) return 0; //20170906
					//TOUCH_DELAY = convet_3_dec_asccii_to_hex(buff+5);
					//TOUCH_DELAY = TOUCH_DELAY*10;
					//TOUCH_COUNT = 0;//20170907
					para.touch_sensibility = convet_3_dec_asccii_to_hex(buff+5);
					//write_para();
				  sprintf((char *)(BaseData_ARR+BASE_data_touchsensibility*9),"%08X",para.touch_sensibility);
				//	touch_sensibility_cnt=0;
					if(type == COMM_DATA_UARST)
					{
					   code_protocol_ack(Tp_xor,0,NULL,0);
					}
				break;
				#if 1
				case '5'://¿ªÊ¼»ÃµÆÆ¬ÁË
					if(convet_3_dec_asccii_to_hex(buff+5)==0XFFFF) return 0;
					if(convet_3_dec_asccii_to_hex(buff+5)>255) return 0;
				  if(convet_3_dec_asccii_to_hex(buff+5)==0) return 0;
					ppt.keep_time= convet_3_dec_asccii_to_hex(buff+5); //PPT_KEEP_TIME = convet_3_dec_asccii_to_hex(buff+5);
					if(ppt.keep_time == 0) return 0;
					//ppt.task=1;  //PPT_STEP = PPT_START;
				  ppt_find_file_and_Q(N2S_Q1);
					start_ppt();
				  if(systerm_error_status.bits.image_movenantosdram_error == 1)
					{
						return 1;
					}
					if(type == COMM_DATA_UARST)
					{
					   code_protocol_ack(Tp_xor,0,NULL,0);
					}
				break;

				case '6'://Í£Ö¹»ÃµÆÆ¬ÁË
					if((buff[5]!='F')||(buff[6]!='F')||(buff[7]!='F')) return 0;
					end_ppt();//ppt.task=0;  //PPT_STEP = PPT_OVER;
					if(type == COMM_DATA_UARST)
					{
					   code_protocol_ack(Tp_xor,0,NULL,0);
					}
				break;
				#endif
				default:
					
				break;
			}				 
			 break;
      case 'B'://ÉùÒô¿ØÖÆ
			if(buff[9]!='*') return 0;//·ÀÖ¹ÎÄ¼þÃû¶à1Î»Ò²»ØÎÄ
     
			if(convet_1_hex_ASCII_to_hex(buff+4)==0xFFFF) return 0; //COM
			if(convet_1_hex_ASCII_to_hex(buff+5)==0xFFFF) return 0;  // TIMES
			if(convet_1_hex_ASCII_to_hex(buff+6)==0xFFFF) return 0; //volume
			if(convet_2_hex_ASCII_to_hex(buff+7)==0xFFFF) return 0;//filename
			if((convet_1_hex_ASCII_to_hex(buff+4)>0XA)&&(convet_1_hex_ASCII_to_hex(buff+4)!=0XF)) return 0;
			if((convet_1_hex_ASCII_to_hex(buff+4)>=1)&&(convet_1_hex_ASCII_to_hex(buff+4)<=0x0A))//·´¸´Êä³ö
			{
				if((convet_1_hex_ASCII_to_hex(buff+5)<2)||(convet_1_hex_ASCII_to_hex(buff+5)>0X0F)) return 0;
			}
			if((convet_1_hex_ASCII_to_hex(buff+6)>9)||(convet_1_hex_ASCII_to_hex(buff+6)==0)) return 0;
			
			
			//if(tts.ack_flag!=0) return 0;
			
			
			
			if(buff[4]>='F')  //stop play
			{
					tts.play_num=0;
					//
					stop_tts_play( );
					while(STOP_FLAG_CYW);
						
					
					if(type == COMM_DATA_UARST)
			   code_protocol_ack(Tp_xor,0,NULL,0);//SEND_ACKnHEX(Tp_FSC,0,ack_buf);
					
				
					return 1 ;
			}
                   
			
 
      
			
			
 			//Ö®Ç°ÓÐÉùÒô£¬ÔÚ²¥·Å£¬ÔòÖ±½Ó·¢Í£Ö¹
                      //if(get_wt588h_voice_state()==0)//cyw 20180425
			               // if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_2)==GPIO_PIN_RESET)
                     // 	{
					//stop_tts_play( );
                    //  	}
			
			tts.file=convet_2_hex_ASCII_to_hex(&buff[7]);
			tts.play_num=convet_1_hex_ASCII_to_hex(&buff[5]);
			
			
			tts.voice=(((buff[6]-'1')*2)&0x0f);
			if(buff[6]=='9') tts.voice=0xf;
			//ÔÚÓïÒô²¥·Å»òÕß´ý»ú×´Ì¬·¢´ËÃüÁî¿ÉÒÔµ÷½Ú 8 ¼¶ÒôÁ¿£¬FFE0H ×îÐ¡£¬FFE7H ÒôÁ¿×î´ó¡£
			//¡ùVolume:Êý×ÖÔ½´óÒôÁ¿Ô½´ó¡£¡¸1¡¹¡«¡¸9¡¹¿ÉÒÔÉè¶¨¡¢¡¸5¡¹ÊÇ±ê×¼¡£
                 
//			if(tts.voice<3)  // 1~9¶ÔÓ¦ 1 1 1 2 3 4 5 6 7 
//				{
//					tts.voice=1;
//			      }
//			if(tts.voice>2)
//				{
//					tts.voice-=2;
//				}
			
			 
			  if((buff[4]>='1')&&(buff[4]<='9'))
			  	{
				   tts.interval=100*(buff[4]&0x0f);                                
			  	}
			 if(buff[4]>='A')
			  	{
					tts.interval=1000;					
			  	}
			  if(buff[4]=='0')
			  	{
					tts.play_num=1;
			  	}
					
			   tts.interval_cnt=0; 
      			
			
		  if(type == COMM_DATA_UARST)	
			{
			   tts.ack_flag = 1;
         tts.fcs_code = 	Tp_xor;		
				code_protocol_ack(tts.fcs_code ,0,NULL,0); 
				//code_protocol_ack(Tp_xor,0,NULL,0);
			//	sysprintf("B COMMAND RECEIVE\r\n");
          return 1;				
      }				
			///if(type == COMM_DATA_UARST)
			// code_protocol_ack(Tp_xor,0,NULL,0);//SEND_ACKnHEX(Tp_FSC,0,ack_buf);
			break;
     case 'A'://@02A*
			  if(buff[4]!='*') return 0;
			 // nandflash_format();
		   // badmanage_str->flag=0;
		   // NANDFLASH_BABMANAGE_INIT();
	//	SDRAM_TO_NANDFLASH(badmanage_tab_BUFFER,backup_tab_nandflash_start,1);
		    nandflash_format();
		    code_protocol_ack(Tp_xor,0,ack_buf,0);	
			break;
       case 'I'://ÁìÓòÊý¾Ý·ÖÅä
      sysprintf("Icommand start\r\n");
	    if(LOGO_DATA_OUT_FLAG!=3 )    return 0;   
			if((buff[23]!=',')&&(buff[23]!='*')) return 0;//·ÀÖ¹ÎÄ¼þÃû¶à1Î»Ò²»ØÎÄ
      
		 	if((buff[4]=='0')||(buff[4]=='1'))//????????
				{
					
					if(buff[4]=='0')//?¹??????????j??????
					{
						memset((uint8_t *)Tp_gs_AreaInfo,0,sizeof(Tp_gs_AreaInfo));
						for (Tp_ii = 0; Tp_ii < 16;Tp_ii++)
	         {
		          Tp_gs_AreaInfo[Tp_ii].type = 0xff;
						  Tp_gs_AreaInfo[Tp_ii].addr=0;
			        Tp_gs_AreaInfo[Tp_ii].space=0;
			        Tp_gs_AreaInfo[Tp_ii].size=0;
	         }
					  Tp_gs_AreaInfo[0].type=0x00;   //qdhai add 
	          Tp_gs_AreaInfo[0].space=0x20000; //qdhai add
	          Tp_gs_AreaInfo[0].addr = 0x6000000;
					}
					else
					{
						memcpy((uint8_t *)Tp_gs_AreaInfo,(uint8_t *)gs_AreaInfo,sizeof(gs_AreaInfo));//?????????COPY?????????
				
					}
					
				   Q0_i = 0;
						do
          {
					//FIELD_str[0].Field_Addr = 0x4000000;//????0???????????????I??????'??? ?????????
						
					tp_area_no = convet_1_hex_ASCII_to_hex(buff+5+Tp_i*19);//AreaNo
					if(tp_area_no==0XFFFF) return 0;//AreaNo ????????????L?
					if(tp_area_no == 0)  return 0;//AreaNo ??????????????
					tp_field.addr = conver_x_hex_asccii_to_hex(buff+6+Tp_i*19,7);//Address
					if(tp_field.addr==0xFFFFFFFF) return 0;// ????????????L?
					tp_field.space = conver_x_hex_asccii_to_hex(buff+13+Tp_i*19,7);//Size
					if(tp_field.space==0xFFFFFFFF) return 0;// ????????????L?
				  if(Tp_gs_AreaInfo[tp_area_no].type!=0xff) return 0;
					for(Tp_count_field=0;Tp_count_field<16;Tp_count_field++)//????????h??NAND???????????I?
					{
						if(Tp_count_field == tp_area_no) continue;
						if((Tp_gs_AreaInfo[Tp_count_field].addr+Tp_gs_AreaInfo[Tp_count_field].space)!=0)
						{
						 // Tp_field_used = Tp_FIELD_str[Tp_i].Field_Addr+Tp_FIELD_str[Tp_i].Field_Size;
							if((tp_field.addr>=Tp_gs_AreaInfo[Tp_count_field].addr)&&(tp_field.addr<(Tp_gs_AreaInfo[Tp_count_field].addr+Tp_gs_AreaInfo[Tp_count_field].space)))
							   return 0;
						  if(((tp_field.addr+tp_field.space)>Tp_gs_AreaInfo[Tp_count_field].addr)&&((tp_field.addr+tp_field.space)<(Tp_gs_AreaInfo[Tp_count_field].addr+Tp_gs_AreaInfo[Tp_count_field].space)))
							   return 0; 
						}
					}
					
						
					//if((Tp_field_addr + Tp_field_size)>0x8000000) return 0;	
					//if((tp_field.addr + tp_field.space)>=0x9DC0000) return 0;//´óÐ¡³¬¹ýNANDFLASHµÄ×Ü´óÐ¡ÁË ³¬³öÉÏÏÞÁË ÒòÎªÊÇ2GbitµÄÉè±¸ËùÒÔ±ä´óÁË
					if((tp_field.addr + tp_field.space)>=0x9B40000) return 0;
					if(tp_field.addr < 0x6000000 )  return 0; //¿ªÊ¼µØÖ·²»ÔÚLOGOÊý¾ÝÓ¦¸ÃÔÚµÄ·¶Î§ÄÚ 20171016
					
					if((tp_field.addr %0x20000)!=0)  return 0;	
					if((tp_field.space%0x20000)!=0)  return 0;
					
					tp_field.type = convet_1_hex_ASCII_to_hex(buff+20+Tp_i*19);//dataclass
					if(tp_field.type==0XFFFF) return 0;
					if((tp_field.type==Base_DATACLASS)||(tp_field.type>ActionRecord_DATACLASS)) return 0;//??????????????
					//if((Tp_data2==ActionNumber_DATACLASS)||(Tp_data2==ActionTimers_DATACLASS)||(Tp_data2==UnitPara_DATACLASS)||(Tp_data2==TestFinalData_DATACLASS)||(Tp_data2==AnomalyRecord_DATACLASS))//????????????????????DATACLASS
					//if((Tp_data2==ActionNumber_DATACLASS)||(Tp_data2==ActionTimers_DATACLASS)||(Tp_data2==AnomalyRecord_DATACLASS)||(Tp_data2==UnitPara_DATACLASS))//20170823 uint????????????????????
					//20171023 ??DATACLASS 1 2 3 ???????????
					if((tp_field.type==ActionNumber_DATACLASS)||(tp_field.type==ActionTimers_DATACLASS)||(tp_field.type==UnitPara_DATACLASS))
					{
						for(Tp_count_field=0;Tp_count_field<16;Tp_count_field++)
						{
							if(Tp_count_field!=tp_area_no)//????????AREANO?j???????????DATACLASS ????????????
							{
							if(Tp_gs_AreaInfo[Tp_count_field].type == tp_field.type)//???????????????DATACLASS
							{
								return 0;
							}
							}
						}
					}
					tp_field.size = convert_2_dec_ascii_to_hex(buff+21+Tp_i*19);//Number 1????????????Ÿ???
					
					if(tp_field.size==0XFFFF) return 0;
					if(tp_field.size==0) return 0;
					
					if((tp_field.type==ActionNumber_DATACLASS)&&(tp_field.size!=3)) return 0;//???????????????????3 ??
					if((tp_field.type==ActionTimers_DATACLASS)&&(tp_field.size!=11)) return 0;//?????????????????11 ??
					if((tp_field.type==ActionNumber_DATACLASS)&&(tp_field.space!=0x200000/*(MAX_LOGO_ACTION_MONTH*0x2000)*/)) return 0;//????????????????????
					if((tp_field.type==ActionTimers_DATACLASS)&&(tp_field.space!=0x200000/*(MAX_LOGO_ACTION_MONTH*0x2000)*/)) return 0;//???????????????????
					//20171023 ??????????????0x20000????????
					if((tp_field.type==UnitPara_DATACLASS)&&(tp_field.space!=0x20000)) return 0;//?????????????????
					if((tp_field.type==UnitPara_DATACLASS)&&(tp_field.size!=5)) return 0;//???????
					if((tp_field.type==TestFinalData_DATACLASS)&&(tp_field.size!=4)) return 0;//??????????
					if((tp_field.type==TestAllData_DATACLASS)&&(tp_field.size==0))  return 0;//?????????? ???1-99
			  //  if((Tp_data2==TestAllData_DATACLASS)&&(Tp_data3%2==1))  return 0;//?????????? ???1-99 ?????????
          if((tp_field.type==AnomalyRecord_DATACLASS)&&(tp_field.size==0))  return 0;//?????????? ???1-99
				//  if((Tp_data2==AnomalyRecord_DATACLASS)&&(Tp_data3%2==1))  return 0;//?????????? ???1-99 ?????????			
					if((tp_field.type==ActionRecord_DATACLASS)&&(tp_field.size==0))  return 0;//?????????? ???1-99
				//  if((Tp_data2==ActionRecord_DATACLASS)&&(Tp_data3%2==1))  return 0;//?????????? ???1-99 ?????????			
          					
					
					Tp_gs_AreaInfo[tp_area_no].addr=tp_field.addr;
					Tp_gs_AreaInfo[tp_area_no].space=tp_field.space;
					Tp_gs_AreaInfo[tp_area_no].size=tp_field.size;
					Tp_gs_AreaInfo[tp_area_no].type=tp_field.type;
					Tp_i++;
					
					Q0_i = Q0_i|(1<<tp_area_no);//??????AREA NO
					
					
				  }//do ???
					while((buff[23+(Tp_i-1)*19]==',')&&(Tp_i<256));
					
					led_log_on(300);
					
					ack_buf[0] = 0x13;
			      if(type == COMM_DATA_UARST)
						{
						   code_protocol_ack(Tp_xor,1,ack_buf,0); //SEND_ACKnHEX(Tp_FSC,1,ack_buf);
						}
						if(type == COMM_DATA_IIC)
						{
							code_protocol_ack_IIC(Tp_xor,1,ack_buf,0);
						}
					
					//addr = (void *)(logodata_field0_BUFFER + 0x2000 );
						
					if(buff[4]=='0')
					{
							for(Tp_field = 1;Tp_field <16; Tp_field++)
						  {
							if(logodata_sdrambuffer_addr_arry[Tp_field]!=0) 
						{
							#ifdef  SYSUARTPRINTF 
							sysprintf("free start=%x \r\n",logodata_sdrambuffer_addr_arry[Tp_field]);
							#endif
							free((void *)logodata_sdrambuffer_addr_arry_bak[Tp_field]);
							logodata_sdrambuffer_addr_arry[Tp_field]=0;
							logodata_sdrambuffer_addr_arry_bak[Tp_field]=0;
							#ifdef  SYSUARTPRINTF 
							sysprintf("free end =%x\r\n",logodata_sdrambuffer_addr_arry[Tp_field]);
							#endif
						}
							}
					}
						
					for(Tp_field = 1;Tp_field <16; Tp_field++)
					{
//					if(((Q0_i&(1<<Tp_field))==0)&&(buff[4]=='0'))
//					{
//						if(logodata_sdrambuffer_addr_arry[Tp_field]!=0) 
//						{
//							sysprintf("free start=%x \r\n",logodata_sdrambuffer_addr_arry[Tp_field]);
//							free((void *)logodata_sdrambuffer_addr_arry_bak[Tp_field]);
//							logodata_sdrambuffer_addr_arry[Tp_field]=0;
//							logodata_sdrambuffer_addr_arry_bak[Tp_field]=0;
//							sysprintf("free end =%x\r\n",logodata_sdrambuffer_addr_arry[Tp_field]);
//						}
//					}
					if((Q0_i&(1<<Tp_field))!=0)
					{
						if(logodata_sdrambuffer_addr_arry[Tp_field]==0)
						{
						Temp_logodata_BUFFER = (uint32_t )malloc((0x20000)+64);
						logodata_sdrambuffer_addr_arry_bak[Tp_field]=Temp_logodata_BUFFER;
          	Temp_logodata_BUFFER = 32+shift_pointer(Temp_logodata_BUFFER,32);
	          Temp_logodata_BUFFER = Temp_logodata_BUFFER|0X80000000;
						 if(Temp_logodata_BUFFER) memset((void *)Temp_logodata_BUFFER,0,(0x20000));
						}
						else
						{
							Temp_logodata_BUFFER=logodata_sdrambuffer_addr_arry[Tp_field];
							 memset((void *)Temp_logodata_BUFFER,0,(0x20000));
						}
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
						sysprintf("Icommand:Tp_field=%X,Temp_logodata_BUFFER=%X\r\n",Tp_field,Temp_logodata_BUFFER);
						//???????
					switch(Tp_gs_AreaInfo[Tp_field].type)
					{
						case ActionNumber_DATACLASS://???????????õ?
						case ActionTimers_DATACLASS://???????????
							   // Tp_field =Tp_data1;
			           
						para.dataclass_1_2_action_count=0;
						sprintf((char *)(BaseData_ARR+BASE_data_actioncount*9),"%08X",para.dataclass_1_2_action_count);   
						 sprintf((char *)(BaseData_ARR+BASE_data_Vbatlow_monthcount*9),"%08X",0);
            para.no_vbat_pwr_on_cnt=0;		
			      sprintf((char *)(BaseData_ARR+BASE_data_Dataclass12RTCPowerOffTimer*9),"%08X",para.no_vbat_pwr_on_cnt);					
                 if(systerm_error_status.bits.lse_error==1)	//??j?RTCû???
                 {
									 Clear_sdram(1<<(Tp_field+8));//AREANO
						       sdram_field_offset0(Tp_field) = 1;
				           sdram_field_offset1(Tp_field) = 0;//??¼????
				           sdram_field_offset2(Tp_field) = 0xAA;
                   sdram_field_offset3(Tp_field) = 0xAA;
									 para.last_poweroff_year = 99;//20170924
									 para.last_poweroff_month = 99;//20170924
									 if(Tp_gs_AreaInfo[Tp_field].type==ActionNumber_DATACLASS)
                     *(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+4) = 0xfffffffe;						 
									  para.no_vbat_pwr_on_cnt=1;
                     sprintf((char *)BaseData_ARR+BASE_data_Vbatlow_monthcount*9,"%08X",1);										 
			              sprintf((char *)(BaseData_ARR+BASE_data_Dataclass12RTCPowerOffTimer*9),"%08X",para.no_vbat_pwr_on_cnt);
                    sprintf((char *)(BaseData_ARR+BASE_data_PowerOffYear*9),"%08X",para.last_poweroff_year);
                    sprintf((char *)(BaseData_ARR+BASE_data_PowerOffMonth*9),"%08X",para.last_poweroff_month);		 
								 }
                 else
                 {
									 Clear_sdram(1<<(Tp_field+8));//AREANO
						       sdram_field_offset0(Tp_field) = pwr_on_time_app.month;//DECTOHEX(timeandday_now->MONTH);
				           sdram_field_offset1(Tp_field) = pwr_on_time_app.year;//DECTOHEX(timeandday_now->YEAR);//??¼????
				           sdram_field_offset2(Tp_field) = 0x00;
                   sdram_field_offset3(Tp_field) = 0x00;
									 //????????????????·???????????
									 para.last_poweroff_year = timeandday_now_app.year;//20170924
						       para.last_poweroff_month = timeandday_now_app.month;//20170924
									 sprintf((char *)(BaseData_ARR+BASE_data_PowerOffYear*9),"%08X",para.last_poweroff_year);
                    sprintf((char *)(BaseData_ARR+BASE_data_PowerOffMonth*9),"%08X",para.last_poweroff_month);	
                   if(Tp_gs_AreaInfo[Tp_field].type==ActionNumber_DATACLASS)									 
									   *(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+4) = 0xfffffffe;		
								 }
                 //?????????
                for(Q0_addr=0;Q0_addr<(Tp_gs_AreaInfo[Tp_field].space/0x20000);Q0_addr++)
								 {
									 NAND_EraseBlock((Tp_gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/0x20000+Q0_addr);
								 }
 								 if(Tp_gs_AreaInfo[Tp_field].type ==ActionTimers_DATACLASS )
								 {
									 System_Runtime_Lasttime = 0xffffffff;
									 time1ms_count_forrun = 0;
									 time1ms_count_forrun_his=0;
								 }
     						break;
						case UnitPara_DATACLASS:
							  //?????????
						     Clear_sdram(1<<(Tp_field+8));//AREANO
                for(Q0_addr=0;Q0_addr<(Tp_gs_AreaInfo[Tp_field].space/0x20000);Q0_addr++)
								 {
									 NAND_EraseBlock((Tp_gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/0x20000+Q0_addr);
								 }
								 *(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field] + UINT_CS1_OFFSET) = 0X18;
								 *(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field] + UINT_CS2_OFFSET) = 0X18;
								 *(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field] + UINT_CS3_OFFSET) = 0X18;
								  SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],(Tp_gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048,1);
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
						*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+5) = POWER_ON_RECORD_Str.Poweron_Second;
						*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+6) = POWER_ON_RECORD_Str.Battery_Voltage;
						
						   
			        for(Tp_ii =0;Tp_ii<(MAX_LOGO_FINALTEST_ONEPACKET-7);Tp_ii++)
			            *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+Tp_ii+7) = 0;
								 //???????
                for(Q0_addr=0;Q0_addr<(Tp_gs_AreaInfo[Tp_field].space/0x20000);Q0_addr++)
								 {
									 NAND_EraseBlock((Tp_gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/0x20000+Q0_addr);
								 }
								 field_info_init();
                break;
            case 	TestAllData_DATACLASS:
						case  AnomalyRecord_DATACLASS:
						
							  //???????
						    Clear_sdram(1<<(Tp_field+8));//AREANO
						    //Tp_field =Tp_data1;
						   sprintf((char *)(BaseData_ARR+(BASE_data_fieldinfo_count+Tp_field*3)*9),"%08X",0);	
						 sprintf((char *)(BaseData_ARR+(BASE_data_fieldinfo_count+Tp_field*3+1)*9),"%08X",0);	
						 sprintf((char *)(BaseData_ARR+(BASE_data_fieldinfo_count+Tp_field*3+2)*9),"%08X",0);	
						
                for(Q0_addr=0;Q0_addr<(Tp_gs_AreaInfo[Tp_field].space/0x20000);Q0_addr++)
								 {
									 NAND_EraseBlock((Tp_gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/0x20000+Q0_addr);
								 }
								 field_info_init();
						    break;
						case ActionRecord_DATACLASS:
							  //???????
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
						 sprintf((char *)(BaseData_ARR+(BASE_data_fieldinfo_count+Tp_field*3+1)*9),"%08X",1);	
						 sprintf((char *)(BaseData_ARR+(BASE_data_fieldinfo_count+Tp_field*3+2)*9),"%08X",0);	
						
                for(Q0_addr=0;Q0_addr<(Tp_gs_AreaInfo[Tp_field].space/0x20000);Q0_addr++)
								 {
									 NAND_EraseBlock((Tp_gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/0x20000+Q0_addr);
								 }
								 
								 SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],(Tp_gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048,1);
								 
								 field_info_init();
                break;							
						default:break;	
					}//??????
				  }//IF
				  }//FOR
					
					memcpy((uint8_t *)gs_AreaInfo,(uint8_t *)Tp_gs_AreaInfo,sizeof(gs_AreaInfo));
				//	memcpy((uint8_t*)(logodata_field0_BUFFER+Basedata_fieldinformation),(uint8_t *)FIELD_str,256);
					
					WriteAreaConfig_new();
					
					Updata_Basedata_No0_cyw();
						//	 Base_DATACLASEE
		      //memcpy((uint8_t*)(logodata_field0_BUFFER +Basedata_copy0),(uint8_t*)(logodata_field0_BUFFER),logodata_Basedata_SIZE);//20170428
		      //memcpy((uint8_t*)(logodata_field0_BUFFER +Basedata_copy1),(uint8_t*)(logodata_field0_BUFFER),logodata_Basedata_SIZE);//20170428
		      //memcpy((uint8_t*)(logodata_field0_BUFFER +Basedata_copy2),(uint8_t*)(logodata_field0_BUFFER),logodata_Basedata_SIZE);//20170428
					field_info_init();
					
					basedata_ram_to_sdram();
					
					SDRAM_TO_NANDFLASH(logodata_basedata_BUFFER ,baseA_data__nandflash_start,1);//??????
				  SDRAM_TO_NANDFLASH(logodata_basedata_BUFFER ,baseB_data__nandflash_start,1);
					//	memset((void *)logodata_field0_BUFFER,0,logodata_field0_SIZE);
					
			//	 ReadAreaConfig_new();
		//  para_init();
	    //field_info_init();
					
					//LED_LOGO_CONTROL_ON();
					if(systerm_error_status.bits.nandflash_Write_error == 0)
					{
					ack_buf[0] = 0x1C;
					}
					else
					{
					ack_buf[0] = 0xEF;
					}
			      if(type == COMM_DATA_UARST)
						{
						   code_protocol_ack(Tp_xor,1,ack_buf,0); //SEND_ACKnHEX(Tp_FSC,1,ack_buf);
						}
						if(type == COMM_DATA_IIC)
						{
							code_protocol_ack_IIC(Tp_xor,1,ack_buf,0);
						}
             					
				}
				else
				{
					return 0;//20170929
				}
        break;
      case 'W'://logoÐ´
			
		//	if(pwr_on_cnt) return 0;  //if(LOGO_HANDLE_flag == 0) return 0;//ÔÚÁìÓòÊý¾Ý³õÊ¼»¯Ç°µÄÐ´¶¼²»´¦Àí
			if(LOGO_DATA_OUT_FLAG != 3) return 0;
		//	if(pwr_on_cnt) return 0;
			filed_num = convet_1_hex_ASCII_to_hex(buff + 4);//Tp_data1 = AreaNo 
			if(filed_num == 0xffff) return 0;
				p_gs_AreaInfo=get_gs_AreaInfo(filed_num);
			if((p_gs_AreaInfo->type==0)&&(filed_num !=0))  return 0;//Õâ¸öAreaNo¿ÉÄÜ»¹Ã»ÓÐ±»Ñ§Ö¸¶¨
			Tp_data2 = convet_2_hex_ASCII_to_hex(buff + 1);//Õâ¸ö¶ÔÓ¦µÄÊÇLEN
			if(Tp_data2 == 0xffff) return 0;
			Tp_data2 = Tp_data2 - 3; //Õâ¸öÊÇ±¨ÎÄVALUEÇøÓòµÄ³¤¶È
			if((p_gs_AreaInfo->size!=Tp_data2)&&(filed_num !=0)) return 0;//Õâ¸öËµÃ÷·¢ËÍµÄÊý¾Ý¸öÊý²»¶Ô   field[filed_num].number
			if((Tp_data2>11)&&(filed_num ==0)) return 0;



		     p_gs_AreaInfo=get_gs_AreaInfo(filed_num);
			led_log_on(300);// ÁÁ300ms
		  
                  //È·¶¨ÏîÄ¿ºÅ
			switch(p_gs_AreaInfo->type)  //field[filed_num].data_class)
			{
				case BASE_DATA_CLASS://»ù±¾Êý¾Ý
					Tp_data3 = convet_3_dec_asccii_to_hex(buff + 5);
					//if(Tp_data3 >= Basedata_4BYTE_Type_Start/8) return 0;
					if((Tp_data3 == BASE_data_fresh_time) ||(Tp_data3 == LCD_Ver_index) || (Tp_data3 == BMP_Ver_index))  return 0;//20180906
					for(n = 0;n<(data_len-6);n ++)
					{
						if((buff[8+n]<'A')||(buff[8+n]>'Z'))//²»ÊÇ´óÐ´×ÖÄ¸
						{
							if((buff[8+n]<'0')||(buff[8+n]>'9'))//²»ÊÇÊý×Ö
							{
								return 0;
							}
						}
					}
					
					
					
					if(AreaWriteCmd(BASE_DATA_CLASS,filed_num,Tp_data3,&buff[8] ,(data_len-1-2-3))==0)//p_gs_AreaInfo->size);-1-2-3
					{
						
						if(systerm_error_status.bits.nandflash_Write_error == 1)
						{
							 return 1;//²»ÐèÒªÔÙACK
						}
						return 0;
					
					}
					
					/*
					memset((uint32_t*)(logodata_field0_BUFFER+Tp_data3*8),0xff,8);
					memcpy((uint32_t*)(logodata_field0_BUFFER+Tp_data3*8),buff+8,Tp_len-6);
					*/
				break;
				case ACTION_NUMBER_DATA_CLASS://À´µÄÊý¾ÝÊÇ¶¯×÷»ØÊýÊý¾Ý
					
				 Tp_field = filed_num;
					    Tp_data3 =convet_3_dec_asccii_to_hex(buff + 5);//?????????
					     if(Tp_data3 == 0xffff) return 0;//??????
					     if((Tp_data3 == 0)||(Tp_data3 == 1)) return 0;//????1-999
					     //Tp_data3 = Tp_data3 +1;
					    if(para.dataclass_1_2_action_count >= MAX_LOGO_ACTION_MONTH) return 0 ;//?????????????//20170510
              // *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_data1]) = DECTOHEX(timeandday_now->MONTH);
				     //  *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_data1]+1) = DECTOHEX(timeandday_now->YEAR);//????
				     //  *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_data1]+2) = 0x00;
             //  *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_data1]+3) = 0x00;				
					     Tp_bmp_filename_new = logodata_sdrambuffer_addr_arry[filed_num]+(para.dataclass_1_2_action_count%16)*4*2048+Tp_data3*4;
				       memcpy((void *)(&Tp_data_uint32),(void *)Tp_bmp_filename_new,4);
				       Tp_data_uint32--;
				        memcpy((void *)Tp_bmp_filename_new,(void *)(&Tp_data_uint32),4);
				//*(__IO uint32_t*)(Tp_bmp_filename_new) = *(__IO uint32_t*)(Tp_bmp_filename_new) - 1;//?????? NO Tp_data3???			
					
//					//Tp_field = filed_num;
//					Tp_data3 =convet_3_dec_asccii_to_hex(buff + 5);//Õâ¸öÊÇ¶¯×÷»ØÊýÊý¾Ý
//					if(Tp_data3 == 0xffff) return 0;//Êý¾Ý²»ºÏ¹æ·¶
//					if((Tp_data3 == 0)||(Tp_data3 == 1)) return 0;//È¡Öµ·¶Î§1-999
//					
//					//if(sdram_actionindex >= MAX_LOGO_ACTION_MONTH) return 0 ;//¶¯×÷´ÎÊýÊý¾ÝÒÑÂú²»±ØÔÙ¶¯×÷//20170510

//					AreaWriteCmd(ACTION_NUMBER_DATA_CLASS,filed_num,Tp_data3,(void*)Tp_data3, 1); //(data_len-1-2-3)); //p_gs_AreaInfo->size);
//					/*
//					Tp_bmp_filename = logodata_sdrambuffer_addr_arry[filed_num]+(sdram_actionindex%16)*4*2048+Tp_data3*4;
//					*(__IO uint32_t*)(Tp_bmp_filename) = *(__IO uint32_t*)(Tp_bmp_filename) - 1;//¼ÇÂ¼ÉÏµç´ÎÊý NO Tp_data3µÄÏîÄ¿			
//					*/

				break;
				case ACTION_TIMERS_DATA_CLASS://À´µÄÊý¾ÝÊÇ¶¯×÷Ê±¼äÊý¾Ý
         	//  Tp_field = filed_num;
					 if(para.dataclass_1_2_action_count >= MAX_LOGO_ACTION_MONTH) return 0 ;//?????????????//20170510
						 Tp_data3 =convet_3_dec_asccii_to_hex(buff + 5);//?????????
					   if(Tp_data3 == 0xffff) return 0;//??????
					   if((Tp_data3 == 0)||(Tp_data3 == 1)) return 0;//????1-999
					   //Tp_data3 = Tp_data3 +1;
					   Tp_field_addr = conver_x_hex_asccii_to_hex(buff + 8,8);
					   if(Tp_field_addr == 0xffffffff) return 0;//????????8???? ,??????????8???0-0xfffffffe
					   Tp_bmp_filename_new = *(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[filed_num]+(para.dataclass_1_2_action_count%16)*4*2048+Tp_data3*4);
					   Tp_bmp_filename_new = Tp_bmp_filename_new - Tp_field_addr;//????  ???  ?????????
					   //*(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[filed_num]+(para.dataclass_1_2_action_count%16)*4*2048+Tp_data3*4) = Tp_bmp_filename_new ;
					   memcpy((void *)(logodata_sdrambuffer_addr_arry[filed_num]+(para.dataclass_1_2_action_count%16)*4*2048+Tp_data3*4),(void *)&Tp_bmp_filename_new,4);
			
//					//Tp_field = filed_num;
//					//if(sdram_actionindex >= MAX_LOGO_ACTION_MONTH) return 0 ;//¶¯×÷Ê±¼äÊý¾ÝÒÑÂú²»±ØÔÙ¶¯×÷//20170510
//					Tp_data3 =convet_3_dec_asccii_to_hex(buff + 5);//Õâ¸öÊÇ¶¯×÷Ê±¼äÊý¾Ý
//					if(Tp_data3 == 0xffff) return 0;//Êý¾Ý²»ºÏ¹æ·¶
//					if((Tp_data3 == 0)||(Tp_data3 == 1)) return 0;//È¡Öµ·¶Î§1-999
//					//Tp_data3 = Tp_data3 +1;
//					Tp_data2 = conver_x_hex_asccii_to_hex(buff + 8,8);
//					if(Tp_data2 == 0xffffffff) return 0;//¶¯×÷Ê±¼äÊý¾ÝµÄºó8Î»²»ºÏ·¨ £¬ËùÒÔ¶¯×÷Ê±¼äÊý¾ÝµÄºó8Î»·¶Î§0-0xfffffffe

//					AreaWriteCmd(ACTION_TIMERS_DATA_CLASS,filed_num,Tp_data3,(void*)Tp_data2,(data_len-1-2-3)); //p_gs_AreaInfo->size);
//				   
//					/*
//					Tp_bmp_filename = *(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[filed_num]+(sdram_actionindex%16)*4*2048+Tp_data3*4);
//					Tp_bmp_filename = Tp_bmp_filename - Tp_field_addr;//Ê±¼äÀÛ¼Æ  ÓÃ¼õµÄ  Òª²»Òª×ö´óÐ¡ÅÐ¶¨£¿
//					*(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[filed_num]+(sdram_actionindex%16)*4*2048+Tp_data3*4) = Tp_bmp_filename ;
//					*/
				break;
				case UNIT_PARA_DATA_CLASS://À´µÄÊý¾ÝÊÇµ¥ÔªÊý¾Ý
						 //Tp_field = Tp_data1;
						 Tp_data3 =convet_3_dec_asccii_to_hex(buff + 5);//???????
					   if(Tp_data3 == 0xffff) return 0;//??????
					  // if(Tp_data3 == 0) return 0;//??????//20170628
					   if(Tp_data3>MAX_LOGO_UINT_NUM) return 0;//????1-MAX_LOGO_UINT_NUM
					   Tp_field_addr = convet_2_hex_ASCII_to_hex(buff + 8);
					   if(Tp_field_addr == 0xffff) return 0;
					   if(Tp_field_addr > 0xff)    return 0;
					   
					   
					   *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num] + UINT_CS1_OFFSET) = 
					   (uint8_t)(*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num] + UINT_CS1_OFFSET) -
     					*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num]+UINT_START1_OFFSET + Tp_data3) + Tp_field_addr) ;
					   *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num]+UINT_START1_OFFSET + Tp_data3) = Tp_field_addr ;
					 
					 
					 *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num] + UINT_CS2_OFFSET) = 
					   (uint8_t)(*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num] + UINT_CS2_OFFSET) -
 					  *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num] + Tp_data3 +  UINT_START2_OFFSET )  + Tp_field_addr) ;
					 *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num] + Tp_data3 +  UINT_START2_OFFSET ) = Tp_field_addr ;
					   
					     
					   *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num] + UINT_CS3_OFFSET) = 
					   (uint8_t)(*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num] + UINT_CS3_OFFSET)-
						 *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num] + Tp_data3 +  UINT_START3_OFFSET) + Tp_field_addr) ;
						 *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num] + Tp_data3 +  UINT_START3_OFFSET) = Tp_field_addr ;
					 
					 SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[filed_num],(gs_AreaInfo[filed_num].addr+logodata_2gbit_change_addr)/2048,1);
				
//					//Tp_field = filed_num;
//					Tp_data3 =convet_3_dec_asccii_to_hex(buff + 5);//Õâ¸öÊÇµ¥Ôª²ÎÊý
//					if(Tp_data3 == 0xffff) return 0;//Êý¾Ý²»ºÏ¹æ·¶
//					// if(Tp_data3 == 0) return 0;//Êý¾Ý²»ºÏ¹æ·¶//20170628
//					if(Tp_data3>MAX_LOGO_UINT_NUM) return 0;//È¡Öµ·¶Î§1-MAX_LOGO_UINT_NUM
//					Tp_data2 = convet_2_hex_ASCII_to_hex(buff + 8);
//					if(Tp_data2 == 0xffff) return 0;
//					if(Tp_data2 > 0xff)    return 0;
//          
//				  
//				
//					if(AreaWriteCmd(UNIT_PARA_DATA_CLASS,filed_num,Tp_data3,(void*)Tp_data2, 1) == 0) //(data_len-1-2-3)/2); //p_gs_AreaInfo->size);
//				  {
//						if(systerm_error_status.bits.nandflash_Write_error == 1)
//						{
//							 return 1;//²»ÐèÒªÔÙACK
//						}
//						return 0;
//					}
					//WriteAreaData(filed_num, pArea3, sizeof(FieldArea3All_cyw), 1);
					//int AreaWriteCmd(char cmd, int filed, int idx,void *data,int len)		//ÃüÁî FILED  ·¬ºÅ Êý¾Ý ³¤¶È
					/*
					*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num] + Tp_data3) = Tp_field_addr ;
					*/
				break;
				case TEST_FINAL_DATA_DATA_CLASS://À´µÄÊÇ×îÖÕÊý¾Ý
        // 	Tp_field = Tp_data1; 
					  Tp_data3 =convert_2_dec_ascii_to_hex(buff + 5);
					   if(Tp_data3 == 0xffff) return 0;//??????
             if((Tp_data3 == 0)||(Tp_data3>(MAX_LOGO_FINALTEST_ONEPACKET - 7))) return 0; 		
             Tp_field_addr = convet_2_hex_ASCII_to_hex(buff + 7);
					   if(Tp_field_addr == 0xffff) return 0;
					   if(Tp_field_addr > 0xff)    return 0;	
             *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num] + (Fieldx_Info[filed_num].num*MAX_LOGO_FINALTEST_ONEPACKET)%0x20000 + 6 + Tp_data3) = Tp_field_addr ;					 
		
//					//Tp_field = filed_num; 
//					Tp_data3 =convert_2_dec_ascii_to_hex(buff + 5);
//					if(Tp_data3 == 0xffff) return 0;//Êý¾Ý²»ºÏ¹æ·¶
//					if((Tp_data3 == 0)||(Tp_data3>(MAX_LOGO_FINALTEST_ONEPACKET - 6))) return 0; 		
//					Tp_data2 = convet_2_hex_ASCII_to_hex(buff + 7);
//					if(Tp_data2 == 0xffff) return 0;
//					if(Tp_data2 > 0xff)    return 0;	

//					AreaWriteCmd(TEST_FINAL_DATA_DATA_CLASS,filed_num,Tp_data3,(void*)Tp_data2, 1); //(data_len-1-2-2)); //p_gs_AreaInfo->size);
//					/*
//					*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num] + (sdram_TestFinalNumx*MAX_LOGO_FINALTEST_ONEPACKET)%0x20000 + 5 + Tp_data3) = Tp_field_addr ;					 
//					//×îÖÕÊý¾Ý¼ÇÂ¼µÄÊÇÖ÷µçÔ´µÄÍ¶ÈëÊ±¼ä
//					*/
				break;
				case TEST_ALL_DATA_DATA_CLASS://À´µÄÊÇÈ«²¿²â¶¨Êý¾Ý Õâ¸öÊý¾ÝÖÖÀàÊÇ¿ÉÒÔÖØ¸´·ÖÅäµÄ
				case ERROR_RECORD_DATACLASS:
				case ACTION_RECORD_DATACLASS:
					// Tp_field = Tp_data1;
					    for(Tp_i =0; Tp_i<(gs_AreaInfo[filed_num].size/2 + gs_AreaInfo[filed_num].size%2) ;Tp_i++)
				 	   {
					     if((gs_AreaInfo[filed_num].size%2)&&(Tp_i==gs_AreaInfo[filed_num].size/2))
							 {
								 if(convet_1_hex_ASCII_to_hex(buff + 5 + Tp_i*2) == 0xffff) return 0;//??????
							   ack_buf[Tp_i] = convet_1_hex_ASCII_to_hex(buff + 5 + Tp_i*2)<<4;
							 }
							 else
							 {
							   if(convet_2_hex_ASCII_to_hex(buff + 5 + Tp_i*2) == 0xffff) return 0;//??????
							   ack_buf[Tp_i] = convet_2_hex_ASCII_to_hex(buff + 5 + Tp_i*2);
							 }
							
						 }
						 if(systerm_error_status.bits.lse_error == 1)	//???RTC???
						 {
							*(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num]+(Fieldx_Info[filed_num].num*MAX_LOGO_ALLTEST_ONEPACKET(filed_num))%0x20000)=0;
			        *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num]+(Fieldx_Info[filed_num].num*MAX_LOGO_ALLTEST_ONEPACKET(filed_num))%0x20000+1)=0;
			        *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num]+(Fieldx_Info[filed_num].num*MAX_LOGO_ALLTEST_ONEPACKET(filed_num))%0x20000+2)=0;
			        *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num]+(Fieldx_Info[filed_num].num*MAX_LOGO_ALLTEST_ONEPACKET(filed_num))%0x20000+3)=0x99; 
			        *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num]+(Fieldx_Info[filed_num].num*MAX_LOGO_ALLTEST_ONEPACKET(filed_num))%0x20000+4)=0x99;
              *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num]+(Fieldx_Info[filed_num].num*MAX_LOGO_ALLTEST_ONEPACKET(filed_num))%0x20000+5)=0x99;
				 
						 }
						 else
						 {
				      *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num]+(Fieldx_Info[filed_num].num*MAX_LOGO_ALLTEST_ONEPACKET(filed_num))%0x20000)=(timeandday_now_app.year);
			        *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num]+(Fieldx_Info[filed_num].num*MAX_LOGO_ALLTEST_ONEPACKET(filed_num))%0x20000+1)=(timeandday_now_app.month);
			        *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num]+(Fieldx_Info[filed_num].num*MAX_LOGO_ALLTEST_ONEPACKET(filed_num))%0x20000+2)=(timeandday_now_app.day);
			        *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num]+(Fieldx_Info[filed_num].num*MAX_LOGO_ALLTEST_ONEPACKET(filed_num))%0x20000+3)=(timeandday_now_app.hour); 
			        *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num]+(Fieldx_Info[filed_num].num*MAX_LOGO_ALLTEST_ONEPACKET(filed_num))%0x20000+4)=(timeandday_now_app.minute);
              *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num]+(Fieldx_Info[filed_num].num*MAX_LOGO_ALLTEST_ONEPACKET(filed_num))%0x20000+5)=(timeandday_now_app.second);
						 }
							 for(Tp_i =0; Tp_i<(gs_AreaInfo[filed_num].size/2 + gs_AreaInfo[filed_num].size%2) ;Tp_i++)
				 	   {
							 *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[filed_num]+(Fieldx_Info[filed_num].num*MAX_LOGO_ALLTEST_ONEPACKET(filed_num))%0x20000+Tp_i+6) = ack_buf[Tp_i];
						 }
						 //?????  ?????
						 Fieldx_Info[filed_num].num = Fieldx_Info[filed_num].num + 1;
					 if(((Fieldx_Info[filed_num].num+1)*MAX_LOGO_ALLTEST_ONEPACKET(filed_num))>0x20000)//????? +1?????? ?????????
					 {
						 //???????
						 Tp_field_addr = (gs_AreaInfo[filed_num].addr+logodata_2gbit_change_addr)/2048 + Fieldx_Info[filed_num].cycle*64;		
			       SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[filed_num],Tp_field_addr,1);
						 Fieldx_Info[filed_num].num  =0;
						 Fieldx_Info[filed_num].cycle = Fieldx_Info[filed_num].cycle +1;
						 if((Fieldx_Info[filed_num].cycle * 0x20000)>=gs_AreaInfo[filed_num].space)
						 {
							 Fieldx_Info[filed_num].cycle = 0;
							 Fieldx_Info[filed_num].loop = Fieldx_Info[filed_num].loop  +1;
						 }
						// Clear_sdram(1<<(Tp_field+8));//AREANO
						   Tp_field_addr = (gs_AreaInfo[filed_num].addr+logodata_2gbit_change_addr)/2048 + Fieldx_Info[filed_num].cycle*64;		
			         NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[filed_num],Tp_field_addr,1);
					 }
//          buff[5+p_gs_AreaInfo->size]=0x00;//ÀïÃæº¯ÊýÓÐËü×Ö¸¶´®²Ù×÷£¬ÒªÒÔ0x00½áÎ²
//					if(AreaWriteCmd(0,filed_num,0,&buff[5], p_gs_AreaInfo->size)==0)  //µÚÈý¸ö²ÎÊýÃ»ÓÃµ½
//					{
//						if(systerm_error_status.bits.nandflash_Write_error == 1)
//						{
//							 return 1;//²»ÐèÒªÔÙACK
//						}
//						return 0;
//					}
				break; 
				default:
					return 0;
				//break;
			}
			//ÊéÐ´»ØÎÄ @ LEN FCS *

			 if(p_gs_AreaInfo->type<=TEST_FINAL_DATA_DATA_CLASS)
		   	{
		    		
		   	}
                   
			//Updata_Basedata_No0_cyw();//20181106 ÔÚÐ´µÄÊ±ºò¸üÐÂÊ±¼ä
			//LED_LOGO_CONTROL_ON();
				Updata_Basedata_No0_cyw();
				flag_logo_change|=(1<<filed_num);
				
			if(type == COMM_DATA_UARST)
			{
			code_protocol_ack(Tp_xor,0,NULL,0);  //SEND_ACKnHEX(Tp_FSC,0,ack_buf);
			}
			if(type == COMM_DATA_IIC)//IIC
			{
			code_protocol_ack_IIC(Tp_xor,0,NULL,0); 
      }
			break;
			case 'Y'://´ÓUSB¶Á³öLOGOÊý¾Ý
			//if(flag_usb!=4)   return 0;
			//if(USB_OVER_FLAG == 0) return 0;
			//if(pwr_on_cnt) return 0;//if(LOGO_HANDLE_flag == 0) return 0;//ÔÚÁìÓòÊý¾Ý³õÊ¼»¯Ç°³é³ö²»´¦Àí
      if(LOGO_DATA_OUT_FLAG != 3) return 0;
			led_log_on(300);// ÁÁ300ms


			switch(buff[4])
			{
				case '0':
					//if(pwr_on_cnt) return 0;//if(LOGO_HANDLE_flag == 0) return 0;
					if(buff[6]!='*') return 0;//·ÀÖ¹ÎÄ¼þÃû¶à1Î»Ò²»ØÎÄ
					filed_num = convet_1_hex_ASCII_to_hex(buff+5);//area
					if(filed_num == 0xffff) return 0;
					if((buff[4]=='0')&&(buff[5]=='F'))
					{

						ack_buf[0] = 0X16;//20170721 ITEM2 Ö¸ÁîÊÜÐÅÍêÁË
						command_xor=Tp_xor;////IMAGE_FCS = Tp_FSC;//Á½´ÎËÍÐÅ
						if(type == COMM_DATA_UARST)
						{
					  wait_send_over();
						code_protocol_ack(Tp_xor,1,ack_buf,0); //SEND_ACKnHEX(Tp_FSC,1,ack_buf);
						}
						if(type == COMM_DATA_IIC)//IIC
						{
						//SEND_ACKnHEX_IIC(Tp_FSC,1,ack_buf);
						code_protocol_ack_IIC(Tp_xor,1,ack_buf,0);	
						}
//						Tp_i = 16;
//						do
//				    {
//			          Tp_i--;
//					      SaveAreaData(0, Tp_i);     //test Ê±¼ä 544ms
//				    }
//				    while(Tp_i);
						//SaveAreaData( 0);//Êý¾ÝÀàÐÍ0~3 ÔÚÌáÈ¡µ½UÅÌÊ±£¬Òª°ÑÄÚ´æºÍ¼ÇÂ¼Í³Ò»Ò»ÏÂ£¬ËùÒÔÌáÈ¡Ê±Òª±£´æÒ»ÏÂ
						usb_init();
						i_return_flag = AreaDataToUsbCmd(LCD_Y0,filed_num);
						
            if((systerm_error_status.bits.usb_unable_connect)||(systerm_error_status.bits.usb_canot_write_error))
            {
							return 1;
							
             }
						if(i_return_flag==0)  //DOWNLOAD_LOGO_INIT(0X0000FFFF);
            {
							return 0;
						}
						ack_buf[0] = 0X1c;
						if(type == COMM_DATA_UARST)
						{
						code_protocol_ack(get_command_xor(), 1, ack_buf,0);
						}
						if(type == COMM_DATA_IIC)
						{
						code_protocol_ack_IIC(get_command_xor(), 1, ack_buf,0);	
						}
							
					}
					else
					{
						return 0;
					}
					//LED_LOGO_CONTROL_ON();

					
					
					/*
					ack_buf[0] = 0X16;//20170721 ITEM2 Ö¸ÁîÊÜÐÅÍêÁË
					command_xor=xor;////IMAGE_FCS = Tp_FSC;//Á½´ÎËÍÐÅ
					if(type == COMM_DATA_UARST)
					code_protocol_ack(xor,1,ack_buf,0); //SEND_ACKnHEX(Tp_FSC,1,ack_buf);
					//if(type == COMM_DATA_IIC)//IIC
					//SEND_ACKnHEX_IIC(Tp_FSC,1,ack_buf);
					*/
				break;
				
				case '1':
					//if(pwr_on_cnt) return 0;//if(LOGO_HANDLE_flag == 0) return 0;
					if(buff[6]!='*') return 0;//·ÀÖ¹ÎÄ¼þÃû¶à1Î»Ò²»ØÎÄ
					filed_num = convet_1_hex_ASCII_to_hex(buff+5);//area
					if(filed_num == 0xffff) return 0;
					if(buff[4]=='1')
					{
						ack_buf[0] = 0X16;//20170721 ITEM2 Ö¸ÁîÊÜÐÅÍêÁË
						command_xor=Tp_xor;////IMAGE_FCS = Tp_FSC;
						//LED_LOGO_CONTROL_ON();
						if(type == COMM_DATA_UARST)
						{
					  wait_send_over();
						code_protocol_ack(Tp_xor,1,ack_buf,0);//SEND_ACKnHEX(Tp_FSC,1,ack_buf);
						}
						if(type == COMM_DATA_IIC)
						{
						code_protocol_ack_IIC(Tp_xor,1,ack_buf,0);	
						}
					//	SaveAreaData(0,filed_num );//Êý¾ÝÀàÐÍ0~3 ÔÚÌáÈ¡µ½UÅÌÊ±£¬Òª°ÑÄÚ´æºÍ¼ÇÂ¼Í³Ò»Ò»ÏÂ£¬ËùÒÔÌáÈ¡Ê±Òª±£´æÒ»ÏÂ
					//	SaveAreaData(0,0 );
						usb_init();
						i_return_flag =  AreaDataToUsbCmd(LCD_Y1,filed_num);
						
						if((systerm_error_status.bits.usb_unable_connect)||(systerm_error_status.bits.usb_canot_write_error))
           	{
							return 1;
							
           	}
						if(i_return_flag==0)
						{
							return 0;
						}
						ack_buf[0] = 0X1c;
						if(type == COMM_DATA_UARST)
						{
						code_protocol_ack(get_command_xor(), 1, ack_buf,0);
						}
						if(type == COMM_DATA_IIC)
						{
						code_protocol_ack_IIC(get_command_xor(), 1, ack_buf,0);
						}
					}

				break;
				case '5':
					//if(pwr_on_cnt) return 0;//if(LOGO_HANDLE_flag == 0) return 0;
					if(buff[8]!='*') return 0;//·ÀÖ¹ÎÄ¼þÃû¶à1Î»Ò²»ØÎÄ
				  
					filed_num = convet_1_hex_ASCII_to_hex(buff+5);//area
					if(filed_num == 0xffff) return 0;
					 p_gs_AreaInfo=get_gs_AreaInfo(filed_num);
				
				  LOGO_Y5Y6_str.AREA_NO = filed_num;
				  
				  LOGO_Y5Y6_str.ITEM_NOW = 0;
				  LOGO_Y5Y6_str.DATA_Type = p_gs_AreaInfo->type;
				
					

					if(p_gs_AreaInfo->type==TEST_FINAL_DATA_DATA_CLASS)//dataclass = 4
						{
							Tp_data2 = convert_2_dec_ascii_to_hex(buff+6);
							if(Tp_data2 == 0xffff/*||(Tp_data2 == 0)*/) return 0;
               LOGO_Y5Y6_str.DATACLASS4_ITEMNO = Tp_data2;
						}else
						{
							Tp_data2 = convet_2_hex_ASCII_to_hex(buff+6);
							if(Tp_data2 != 0xff) return 0;
						}
						if(GET_LOGODATA_Y5Y6_20170919(ack_buf,Tp_xor,type)==0) return 0;
//							//AreaReadCmd(LCD_Y5,filed_num,Tp_data2,(char*)ack_buf,&ack_buf_len);		//value+time
//							//addr = (void *)(AREA_FILED_START + 2048 * 64 * filed_num);
//							//pArea4 = (FieldArea4All_cyw *)addr;	
//							ack_buf[0] = pArea4->areaData[Tp_data2].data;
//							One_data_long[filed_num] = sizeof(FieldArea4All_cyw);
//							One_space_num[filed_num] = 0x2000/One_data_long[filed_num];
//							Y6_count[filed_num] = One_space_num[filed_num]*One_data_long[filed_num]*Fieldx_Info[filed_num].cycle + (Fieldx_Info[filed_num].num)*One_data_long[filed_num];
//							if(type == COMM_DATA_UARST)
//						  {
//							   code_protocol_ack(Tp_xor,1,ack_buf,0);
//							}
//							if(type == COMM_DATA_IIC)
//						  {
//							   code_protocol_ack_IIC(Tp_xor,1,ack_buf,0);
//							}
//						}
//					else if((p_gs_AreaInfo->type==5)||(p_gs_AreaInfo->type==6)||((p_gs_AreaInfo->type==7)))
//						{
//							Tp_data2 = convet_2_hex_ASCII_to_hex(buff+6);
//							if(Tp_data2 != 0xff) return 0;
//              
//							SaveAreaData(0 , filed_num);
//							SaveAreaData(0 , 0);
//							One_data_long[filed_num] = 6 + gs_AreaInfo[filed_num].size;
//							One_space_num[filed_num] = 0x2000/One_data_long[filed_num];
//							//AreaReadCmd(LCD_Y5,filed_num,Tp_data2,(char*)ack_buf,&ack_buf_len);	//time(12)+value	
//							//change_position_Y5_Y6(ack_buf,ack_buf_len);
//							Y6_count[filed_num] =  One_space_num[filed_num]*One_data_long[filed_num]*Fieldx_Info[filed_num].cycle + (Fieldx_Info[filed_num].num)*One_data_long[filed_num];
//							if(Y6_count[filed_num] >= One_data_long[filed_num])
//							{
//								Y6_count[filed_num] = Y6_count[filed_num] - One_data_long[filed_num];
//							}
//							else
//							{
//								return 0;
//							}
//							memset(ack_buf,0,sizeof(ack_buf));
//							GetAreaData(filed_num, Y6_count[filed_num] ,ack_buf,One_data_long[filed_num]);
//							
//							sprintf(ack_buf_new, "%s%02x%02x%02x%02x%02x%02x", (char *)(ack_buf+6), ack_buf[0], ack_buf[1], ack_buf[2], ack_buf[3], ack_buf[4], ack_buf[5]);
//							if(type == COMM_DATA_UARST)
//						  {
//							code_protocol_ack(Tp_xor,One_data_long[filed_num]+6,ack_buf_new,1);
//							}
//							if(type == COMM_DATA_IIC)
//							{
//							code_protocol_ack_IIC(Tp_xor,One_data_long[filed_num]+6,ack_buf_new,1);	
//							}
//						}											
//					
					
				break;
				case '6':
				//	if(pwr_on_cnt) return 0;//if(LOGO_HANDLE_flag == 0) return 0;
					if(buff[8]!='*') return 0;//·ÀÖ¹ÎÄ¼þÃû¶à1Î»Ò²»ØÎÄ
					filed_num = convet_1_hex_ASCII_to_hex(buff+5);//area
					if(filed_num == 0xffff) return 0;

				
					 p_gs_AreaInfo=get_gs_AreaInfo(filed_num);
				
				  //Tp_field = Tp_data1;
				  if(LOGO_Y5Y6_str.AREA_NO != filed_num) return 0;//????
					if(LOGO_Y5Y6_str.ITEM_NOW <=0xfffffffe)
				     LOGO_Y5Y6_str.ITEM_NOW++;
					LOGO_Y5Y6_str.DATA_Type=p_gs_AreaInfo->type;
				
					if(p_gs_AreaInfo->type==TEST_FINAL_DATA_DATA_CLASS)
						{
							Tp_data2 = convert_2_dec_ascii_to_hex(buff+6);
							if(Tp_data2 == 0xffff/*||(Tp_data2 == 0)*/) return 0;
							if(LOGO_Y5Y6_str.DATACLASS4_ITEMNO != Tp_data2) return 0;
							//LOGO_Y5Y6_str.DATACLASS4_ITEMNO = Tp_data2;
//							One_data_long[filed_num] = sizeof(FieldArea4All_cyw);
//							One_space_num[filed_num] = 0x2000/One_data_long[filed_num];
//							if(Y6_count[filed_num] >= One_data_long[filed_num])
//							{
//								Y6_count[filed_num] = Y6_count[filed_num] - One_data_long[filed_num];
//							}
//							else
//							{
//								return 0;
//							}
//							
//							addr = ack_buf;
//							len = GetAreaData(filed_num, Y6_count[filed_num] ,addr,sizeof(FieldArea4All_cyw));
//							//AreaReadCmd(LCD_Y6,filed_num,Tp_data2,(char*)ack_buf,&ack_buf_len);
//							//addr = (FieldArea4All_cyw *)addr;
//							Temp_area4 = (FieldArea4All_cyw *)addr;
//							ack_buf[0] = Temp_area4->areaData[Tp_data2].data;
//							if(type == COMM_DATA_UARST)
//						  {
//							code_protocol_ack(Tp_xor,1,ack_buf,0);
//							}
//							if(type == COMM_DATA_IIC)
//						  {
//							code_protocol_ack_IIC(Tp_xor,1,ack_buf,0);	
//							}
						}
					else if((p_gs_AreaInfo->type==5)||(p_gs_AreaInfo->type==6)||(p_gs_AreaInfo->type==7))
						{
							Tp_data2 = convet_2_hex_ASCII_to_hex(buff+6);
							if(Tp_data2 != 0xff) return 0;
//							SaveAreaData(0 , filed_num);
//							SaveAreaData(0 , 0);
//							if(Y6_count[filed_num] >= One_data_long[filed_num])
//							{
//								Y6_count[filed_num] = Y6_count[filed_num] - One_data_long[filed_num];
//							}
//							else
//							{
//								return 0;
//							}
//							memset(ack_buf,0,sizeof(ack_buf));
//							GetAreaData(filed_num, Y6_count[filed_num] ,ack_buf,One_data_long[filed_num]);
//							sprintf(ack_buf_new, "%s%02x%02x%02x%02x%02x%02x", (char *)(ack_buf+6) ,ack_buf[0], ack_buf[1], ack_buf[2], ack_buf[3], ack_buf[4], ack_buf[5]);
//						  if(type == COMM_DATA_UARST)
//						  {
//							code_protocol_ack(Tp_xor,One_data_long[filed_num]+6,ack_buf_new,1);
//							}
//							if(type == COMM_DATA_IIC)
//						  {
//							code_protocol_ack_IIC(Tp_xor,One_data_long[filed_num]+6,ack_buf_new,1);	
//							}
							
						}			
					
					 if(GET_LOGODATA_Y5Y6_20170919(ack_buf,Tp_xor,type)==0) return 0;
				break;


				default:
				return 0;
//				break;
			}
		break;
				case 'E':
			if(LOGO_DATA_OUT_FLAG!=3 )    return 0;      //20191101
			switch(buff[4])
			{
				case '0'://ÁìÓòÊýÈ«²¿ÏûÈ¥
				     led_log_on(300);// ÁÁ300ms
					if(buff[5]!='*') return 0;//·ÀÖ¹ÎÄ¼þÃû¶à1Î»Ò²»ØÎÄ
					/*
					FIELD_ERASE_STEP = 1;
					FIELD_ERASE_FLAG = 0xffff;
                                 */

					ack_buf[0]=0x16;
					command_xor=Tp_xor;////IMAGE_FCS = Tp_FSC;
					//LED_LOGO_CONTROL_ON();
					if(type == COMM_DATA_UARST)
					{
					 wait_send_over();
					code_protocol_ack(Tp_xor,1,ack_buf,0); //SEND_ACKnHEX(Tp_FSC,1,ack_buf);
					}
					if(type == COMM_DATA_IIC)
					{
					 
					code_protocol_ack_IIC(Tp_xor,1,ack_buf,0); //SEND_ACKnHEX(Tp_FSC,1,ack_buf);
					}
					//if(type == COMM_DATA_IIC)//IIC
					//SEND_ACKnHEX_IIC(Tp_FSC,1,ack_buf);

					
					//AreaEraseCmd(LCD_E0,0);
           FIELD_ERASE_STEP = 1;
				   FIELD_ERASE_FLAG = 0xffff;
          ack_buf[0] = 0;
					for(Q0_i = 1;Q0_i <16;Q0_i++)
					{
						if(gs_AreaInfo[Q0_i].type<=7)
						{
							ack_buf[0]=0x1C;
						}
					}
					if(ack_buf[0] !=0x1C)
					{
						ack_buf[0]=0xEF;
					}
					
				
					command_xor=Tp_xor;////IMAGE_FCS = Tp_FSC;
					//LED_LOGO_CONTROL_ON();
					if(type == COMM_DATA_UARST)
					{
					code_protocol_ack(Tp_xor,1,ack_buf,0); //SEND_ACKnHEX(Tp_FSC,1,ack_buf);
					}
					if(type == COMM_DATA_IIC)
					{
					code_protocol_ack_IIC(Tp_xor,1,ack_buf,0);	
					}
					
					//if(type == COMM_DATA_IIC)//IIC
					//SEND_ACKnHEX_IIC(Tp_FSC,1,ack_buf);
				break;
				case '1'://ÁìÓòÊý²¿·ÖÏûÈ¥
				     led_log_on(300);// ÁÁ300ms
					if(buff[6]!='*') return 0;//·ÀÖ¹ÎÄ¼þÃû¶à1Î»Ò²»ØÎÄ
					/*
					FIELD_ERASE_FLAG = CHANGE_HEXDATA_1BYTE(buff+5);
					if(FIELD_ERASE_FLAG == 0xffff) return 0;
					if((FIELD_ERASE_FLAG == 0) ||(FIELD_ERASE_FLAG > 0xf))  return 0;
					*/
					
					filed_num = convet_1_hex_ASCII_to_hex(buff+5);
					if(filed_num == 0xffff) return 0;
					if((filed_num == 0) ||(filed_num > 0xf))  return 0;
								 
					//LED_LOGO_CONTROL_ON();
					ack_buf[0]=0x16;
					command_xor=Tp_xor;////IMAGE_FCS = Tp_FSC;
					//FIELD_ERASE_STEP = 1;
					if(type == COMM_DATA_UARST)
					{
					wait_send_over();
					code_protocol_ack(Tp_xor,1,ack_buf,0); //SEND_ACKnHEX(Tp_FSC,1,ack_buf);
					}
					if(type == COMM_DATA_IIC)
					{
					code_protocol_ack_IIC(Tp_xor,1,ack_buf,0);	
					}
					//if(type == COMM_DATA_IIC)//IIC
					//SEND_ACKnHEX_IIC(Tp_FSC,1,ack_buf);

					
					//AreaEraseCmd(LCD_E1,filed_num);
           FIELD_ERASE_STEP = 1;
				   FIELD_ERASE_FLAG = filed_num;
					
					//LED_LOGO_CONTROL_ON();
					if(gs_AreaInfo[filed_num].type<=7)
					{
					ack_buf[0]=0x1C;
					}
					else 
					{
					ack_buf[0]=0xEF;
					}
					command_xor=Tp_xor;////IMAGE_FCS = Tp_FSC;
					//FIELD_ERASE_STEP = 1;
					if(type == COMM_DATA_UARST)
					{
					code_protocol_ack(Tp_xor,1,ack_buf,0); //SEND_ACKnHEX(Tp_FSC,1,ack_buf);
					}
					else
					{
					code_protocol_ack_IIC(Tp_xor,1,ack_buf,0); 	
					}
					//if(type == COMM_DATA_IIC)//IIC
					//SEND_ACKnHEX_IIC(Tp_FSC,1,ack_buf);
				break;			
				default:
				return 0;     
//				break;
			}
		break;
	
      case 'H'://USBÉý¼¶ÈðÈøµ¥Æ¬»úµÄ³ÌÐò
			  //if(flag_usb!=4)   return 0;
		    //if(USB_OVER_FLAG == 0) return 0;
			  if(buff[5]!='*') return 0;//·ÀÖ¹ÎÄ¼þÃû¶à1Î»Ò²»ØÎÄ
		    
			 
			  if(buff[4]=='0')//³ÌÐòÊý¾ÝÐ´Èë
				{			
				  ack_buf[0] = 0X16;//20170721 ITEM2 Ö¸ÁîÊÜÐÅÍêÁË
					  command_xor=Tp_xor;//IMAGE_FCS = Tp_FSC;
					  if(type == COMM_DATA_UARST)
					  {
					   wait_send_over(); 
						  code_protocol_ack(Tp_xor,1,ack_buf,0);		
					  }
						
					  usb_init();
					
					if (GetUsbMountFlag() == 0)
					{						
						
						systerm_error_status.bits.usb_unable_connect = 1;
						return 1;
					}	
					set_download_task(1); //DOWNLOAD_PROGRAM_INIT();
					
					
					
          				
				}
				 break;
	case 'X'://  BACKUP
		    //if(flag_usb!=4)   return 0;
		    //if(USB_OVER_FLAG == 0) return 0;
	      if(LOGO_DATA_OUT_FLAG!=3 )    return 0;   
	      
		    switch(buff[4])
				{
				 case '0':
					 // if(pwr_on_cnt) return 0;//if(LOGO_HANDLE_flag == 0) return 0;//ÔÚÁìÓòÊý¾Ý³õÊ¼»¯Ç°³é³ö²»´¦Àí
					  if(buff[5]!='*') return 0;//·ÀÖ¹ÎÄ¼þÃû¶à1Î»Ò²»ØÎÄ
				     //OUTPUT_BACKUP_FILE();//
             ack_buf[0] = 0X16;//20170721 ITEM2 Ö¸ÁîÊÜÐÅÍêÁË
		         command_xor=Tp_xor;//IMAGE_FCS = Tp_FSC;
				     if(type == COMM_DATA_UARST)
						 {
				     	 wait_send_over();
							 code_protocol_ack(Tp_xor,1,ack_buf,0);
						 }
						 
						 usb_init();
						 
				   if (GetUsbMountFlag() == 0)
					{						
						systerm_error_status.bits.usb_unable_connect = 1;
						return 1;
					}	
				   
				 

           i_return_flag =    BackupDeviceData( );
						  if(systerm_error_status.bits.usb_canot_write_error)
            {
							return 1;
							
             }
						if(i_return_flag==0)  //DOWNLOAD_LOGO_INIT(0X0000FFFF);
            {
							return 0;
						}

					 ack_buf[0] = 0X1C;//20170721 ITEM2 Ö¸ÁîÊÜÐÅÍêÁË
		         	     command_xor=Tp_xor;//IMAGE_FCS = Tp_FSC;
				     if(type == COMM_DATA_UARST)
				     		code_protocol_ack(Tp_xor,1,ack_buf,0);			
					 
					 break;
				 case '1':
					//if(pwr_on_cnt) return 0;   //if(LOGO_HANDLE_flag == 0) return 0;//ÔÚÁìÓòÊý¾Ý³õÊ¼»¯Ç°³é³ö²»´¦Àí
				      if(buff[5]!='*') return 0;//·ÀÖ¹ÎÄ¼þÃû¶à1Î»Ò²»ØÎÄ
				      //SYNCHRONOUS_BACKUP_FILE();//¿ªÊ¼´Ó±¸·ÝÊý¾Ýµ¼ÈëÈí¼þ×´Ì¬
		          ack_buf[0] = 0X16;//20170721 ITEM2 Ö¸ÁîÊÜÐÅÍêÁË
		          command_xor=Tp_xor;//IMAGE_FCS = Tp_FSC;
				      if(type == COMM_DATA_UARST)
							{
				     		 wait_send_over();
								code_protocol_ack(Tp_xor,1,ack_buf,0);
							}
							usb_init();
					 if (GetUsbMountFlag() == 0)
					{						
						systerm_error_status.bits.usb_unable_connect = 1;
						return 1;
					}	

			

               i_return_flag        =  RestoreDeviceData( ); // CheckFromDir("3:/BELMONT_BACKUP","0:");//RestoreDeviceData( );
							if(systerm_error_status.bits.usb_cannot_find_hexortxt)
							{
								return 1;
							}
							if(i_return_flag==0)  
              {
							   return 0;
						  }	  
							
					 ack_buf[0] = 0X1C;//20170721 ITEM2 Ö¸ÁîÊÜÐÅÍêÁË
		         	      command_xor=Tp_xor;//IMAGE_FCS = Tp_FSC;
				      if(type == COMM_DATA_UARST)
				     		code_protocol_ack(Tp_xor,1,ack_buf,0);	
					 
					 break;
				 default:
					 return 0;
					 //break;
				}
	  		break;
				 case 'F'://ÆäËûÖ¸Áî
		 if(LOGO_DATA_OUT_FLAG!=3 )    return 0;   
		if(buff[8]!='*') return 0;//·ÀÖ¹ÎÄ¼þÃû¶à1Î»Ò²»ØÎÄ
		switch(buff[4])
		{
			case '0':
				return 0;
//				if((buff[5]=='F')&&(buff[6]=='F')&&(buff[7]=='F'))
//				{
//					ack_buf[0] = 0x1C;
//					if(type == COMM_DATA_UARST)
//					code_protocol_ack(Tp_xor,1,ack_buf,0);
//				}
//				else
//				 {
//					  return 0;
//				 }
			break;
			case '1'://Æô¶¯Ò»´ÎÊµÊ±Ê±ÖÓ²ÉÑù
				if((buff[5]=='F')&&(buff[6]=='F')&&(buff[7]=='F'))
				{
					ack_buf[0] = 0x1C;
					ack_buf[1] =get_vbat_ad_value( ); //get_vbat_average_ad_value( );
					  //ÎªÁË¼õÉÙµçÁ÷£¬Èç¹û²»¹ØÒªvbat 24.£µua  
					//close_adc( );
					//app_ADC_Init( ); //ÎªÁËÖ÷µç¼ì²â
					
					
					if(type == COMM_DATA_UARST)
					code_protocol_ack(Tp_xor,2,ack_buf,0);
				}
				else
				{
					return 0;
				}
			break;
			case '2': //NANDFLASHµÄ´íÎóÇé¿ö¶Á³ö
				if((buff[5]=='F')&&(buff[6]=='F')&&(buff[7]=='F'))//
				{
					ack_buf[0] = get_bad_block( );  //systerm_error_status.bits.nandflash_error;
					if(type == COMM_DATA_UARST)
					code_protocol_ack(Tp_xor,1,ack_buf,0);
				}
				 else
				{
					return 0;
				}
			break;
			case '3':
				switch(buff[5])
				{
					case '0':
					if((buff[6]=='F')&&(buff[7]=='F'))
					{
					Save_Logo();
					code_protocol_ack(Tp_xor,0,ack_buf,0);
					}
					else
   				{
   					return 0;
   				}
					break;
					case '1':
					if((buff[6]=='F')&&(buff[7]=='F'))
					{
						ack_buf[0] = 0x1C;//
						if(type == COMM_DATA_UARST)
						code_protocol_ack(Tp_xor,1,ack_buf,0);
					}
					else
   				      {
   					   return 0;
   				      }
					break;
					default:
					    return 0;	
						//break;
				}
			break;
			case '4'://°æ±¾¶Á³ö
				switch(buff[5])
				{
					case '0'://LCD MCU °æ±¾
						return 0;

					break;
					case '1'://Í¼Ïñ°æ±¾
					  return 0;
					break;
					default:
					   return 0;
					break;

				}
			break;						
			case '5':
				switch(buff[5])
				{
					case '0':
						return 0;
//						if(buff[6]=='F'&&buff[7]=='F')
//						{
//							//////////////////////////////////rcc_mco_start( );
//							//HAL_RTCEx_SetCalibrationOutPut(&RTC_Handler,RTC_CALIBOUTPUT_1HZ);
//							//RTC_CalibOutputCmd(ENABLE);//Ê¹ÄÜ»ùÓÚÍâ²¿32.768KµÄ512HZ²¨ÐÎÊä³ö
//							//ack_buf[0] = (uint8_t)(RTC->CALIBR&0xff);
//							/////////////////////////////////ack_buf[0]=(uint8_t)(RTC->CALR&0xff);
//							if(type == COMM_DATA_UARST)
//							code_protocol_ack(Tp_xor,1,ack_buf,0);
//						}
//						else
//	   				      {
//	   					   return 0;
//	   				      }
					break;
					case '1':
						return 0;
						//if(buff[7]!='F')  return 0;
////						if(convet_2_hex_ASCII_to_hex(buff+6)==0xFFFF) return 0;
////						if(convet_2_hex_ASCII_to_hex(buff+6)&0x60)    return 0;//ÓÐÐ©Î»ÖÃ²»ÄÜÓÐÖµ
////						//RTC_CoarseCalibCmd(DISABLE);
////						//RTC_CoarseCalibConfig(convet_2_hex_ASCII_to_hex(buff+6),convet_2_hex_ASCII_to_hex(buff+6));//ÉèÖÃ´Öµ÷µÄÖµ
////						//RTC_CoarseCalibCmd(ENABLE);//Ê¹ÄÜ´ÖÐ£×¼
////                                         //
////						para.rtc_coarse_value=((convet_2_hex_ASCII_to_hex(buff+6))&0X80)|((convet_2_hex_ASCII_to_hex(buff+6))&0X1F);

						/////////////////////HAL_RTCEx_SetSmoothCalib(&RTC_Handler,RTC_SMOOTHCALIB_PERIOD_32SEC,RTC_SMOOTHCALIB_PLUSPULSES_RESET,para.rtc_coarse_value);
           ////////////////////// sprintf((char *)BaseData_ARR[BASE_data_RTCcoarse],"%08X",para.rtc_coarse_value);
						//write_para( );
////					  
////						if(type == COMM_DATA_UARST)
////						code_protocol_ack(Tp_xor,0,NULL,0);//SEND_ACKnHEX(Tp_FSC,0,ack_buf);
					break;
					case '2':
						return 0;
////						if(buff[6]=='F'&&buff[7]=='F')
////						{
////						/////////////////////	rcc_mco_stop( );
////							//RTC_CalibOutputCmd(DISABLE);
////							if(type == COMM_DATA_UARST)
////							code_protocol_ack(Tp_xor,0,NULL,0);//SEND_ACKnHEX(Tp_FSC,0,ack_buf);
////						}
////						else
////	   				      {
////	   					   return 0;
////	   				      }
					break;

					default:
						 return 0;
					break;
				}
			break;
			case '6':
				return 0;
//				if((buff[5]=='F')&&(buff[6]=='F')&&(buff[7]=='F'))//»ù°å¼ì²éÄ£Ê½
//				{
//					//CHECK_str.HardwareCheck_Flag = 1;
//					//CHECK_str.HardwareCheck_Step = CHECK_IDLE;


//				}
//				else
//			      {
//				   return 0;
//			      }
			break;	
			
			case '7':
				led_log_on(300);// ÁÁ300ms
			
				//if(pwr_on_cnt) return 0;//if(LOGO_HANDLE_flag == 0) return 0; 
				Tp_data1 =convet_3_dec_asccii_to_hex(buff+5);
				if(Tp_data1==0xffff) return 0;

				

				if(AreaReadCmdF(LCD_F7, 0,Tp_data1,(char*)ack_buf, &ack_buf_len))//ack_buf Òª¸ÄÊÇasccii dc
				{
				    if(type == COMM_DATA_UARST)
						{
					  code_protocol_ack(Tp_xor,ack_buf_len,ack_buf,1);
						}
						if(type == COMM_DATA_IIC)
						{
						code_protocol_ack_IIC(Tp_xor,ack_buf_len,ack_buf,1);	
						}
				}
				else
			  {
					return 0;

				}
				
			
			break;
			case '8':
				led_log_on(300);// ÁÁ300ms
			
				//if(pwr_on_cnt) return 0;//if(LOGO_HANDLE_flag == 0) return 0;
				Tp_data2 = convet_3_dec_asccii_to_hex(buff+5);
				if(Tp_data2 ==0xffff) return 0;

				

				for(n=1;n<16;n++)
				{
					p_gs_AreaInfo=get_gs_AreaInfo(n);
					if(p_gs_AreaInfo->type==UNIT_PARA_DATA_CLASS)  //    if(field[Tp_field] .data_class== UNIT_PARA_DATA_CLASS)
					{

						AreaReadCmdF(LCD_F8, n,Tp_data2, (char*)ack_buf, &ack_buf_len);
					  if(type == COMM_DATA_UARST)
						{
						code_protocol_ack(Tp_xor,ack_buf_len,ack_buf,0);
						return 1;	
						}
						else
						{
						code_protocol_ack_IIC(Tp_xor,ack_buf_len,ack_buf,0);
						return 1;
						}
						//LED_LOGO_CONTROL_ON();
						//ack_buf[0] = *(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field] + Tp_data2);
						//if(type == COMM_DATA_UARST)
						//{
						//code_protocol_ack(xor,1,ack_buf,0);
						//}
						//if(type == COMM_DATA_IIC)//IIC
						//{
						//SEND_ACKnHEX_IIC(Tp_FSC,1,ack_buf); 	
						//}
						break;
					}
				}
        return 0;
				break;//F8
				
      case '9':
					if((buff[5]=='F')&&(buff[6]=='F')&&(buff[7]=='F'))//»ù°å¼ì²éÄ£Ê½
				{
					//CHECK_str.HardwareCheck_Flag = 1;
					//CHECK_str.HardwareCheck_Step = CHECK_IDLE;
             if((flag_AB==1)||(LOGO_ERR==1))
						 {
							 ack_buf[0] = 0xEF;//
						   if(type == COMM_DATA_UARST)
						    code_protocol_ack(Tp_xor,1,ack_buf,0);
						 }
						 else
						 {
							 ack_buf[0] = 0x1C;//
						   if(type == COMM_DATA_UARST)
						    code_protocol_ack(Tp_xor,1,ack_buf,0);
						 }

				}
				else
			  {
				   return 0;
			  }
			
			  
				break;//F9
			
			
      				
			default:break;
			}//SWITCH F-COM
		   break;
	case 'N':
		  if(buff[4] == '0')
		  {
				COMMAND_N = 1;
				if(type == COMM_DATA_UARST)
							code_protocol_ack(Tp_xor,0,NULL,0);
			}
			else if(buff[4] == '1')
			{
				COMMAND_N = 0;
				if(type == COMM_DATA_UARST)
							code_protocol_ack(Tp_xor,0,NULL,0);
			}
			
			break;//STATUS = 'N'
		default:break;
	}//switch (STATUS)
	
	if(buff[3]!='Y')
	{
		LOGO_Y5Y6_str.AREA_NO = 0;
		LOGO_Y5Y6_str.ITEM_NOW = 0;
		LOGO_Y5Y6_str.DATA_Type = 0;
	}
	if(type == COMM_DATA_UARST)
	{
	   if(COMMAND_IN==TX_COUNT_IN) return 0;
	}
	
	
	return 1;
}
