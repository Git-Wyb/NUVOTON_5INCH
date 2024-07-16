
#ifndef APROTOCO_H
#define APROTOCO_H


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define TX_BUFF_MAX_UART 2048
#define RX_BUFF_MAX 8192//4096//2048//500
#define TX_BUFF_UART2_MAX_LEN     		(256+128)  	//�����������ֽ��� 200

#define COMM_DATA_UARST    0
#define COMM_DATA_IIC      1

#define  U1A01_ERROR    0xA0001     //ָ�����Ŵ���	U1A01
#define  U1A02_ERROR    0xA0002  //�軭ָ��ͨ�Ŵ��� U1A02
#define  U1801_ERROR    0X80001
#define  U1802_ERROR    0X80002    

#define  ERROR_TYPE_U1  '1'
#define  ERROR_TYPE_U0   '0'
#define  ERROR_TYPE_SEND_3_TIMES  3  
#define  ERROR_TYPE_SEND_1_TIMES  1  

#define TOUCH_I2C_ERROR_101    						0x10001  //�봥��������IC��ͨ�Ŵ���   101
#define P_BMP_OVER_SIZE_ERROR_201    			0x20001  //ͼ��������д���� �����ڴ�����  201
#define P_BMP_OVER_800X400_ERROR_202    	0x20002  //ͼ��������д���󡡳���ͼ���С������800��480�� 202
#define P_BMP_NO_BMP_FILE_ERROR_203    		0x20003  //ͼ��������д�����ļ���ʽ�����ʣ�����bmp�ļ���203
#define P_BMP_RENAME_FILE_ERROR_204    		0x20004  //ͼ��������д�����ļ����ظ�����1�� 204
#define P_BMP_REWRITE_SIZE_ERROR_205    	0x20005  //ͼ��������д����     ���ǳߴ���󡡣���2��  205
#define P_BMP_NO_RIGHT_FILEERROR_206    	0x20006  //ͼ��������д����û�з��ϵ��ļ�������3��  206
#define SDRAM_OVER_SIZE_ERROR_301    			0x30001  //SDRAMͼ�����ݴ��ʹ���  �����ڴ�����  301
#define SDARM_NO_FILE_ERROR_302    				0x30002  //��SDRAM��ͼ�����ݴ��ʹ���NAND_FLASH�ﲻ����ָ���ļ�  302
#define RTC_CLOCK_ERROR_402    						0x40002  //Clock�쳣  RTC(32.786kHz��Clock�쳣  402
#define RTC_BAT_ERROR_403    							0x40003  //Clock�쳣  ��ص�ѹ���� 403
#define USB_COMM_ERROR_501    						0x50001  //USB���ʴ���  ͨ�Ŵ���USBδװ����6��   501
#define USB_W_ERROR_502    								0x50002  //USB���ʴ��� ����д�뵽USB      502
#define USB_W_ERROR1_503    								0x50003  //USB���ʴ��� ����д�뵽USB      502
#define LOG_I2C_ERROR_601    							0x60001  //IICͨ�ţ�LOG���ݣ�����    601
#define NANDFLASH_Q_ERROR_701    					0x70001  //NAND FLASH �ڴ������ȥ����   701
#define NANDFLASH_W_ERROR_702    					0x70002  //NAND FLASH �ڴ������д����    702
#define D_BMP_NO_FILE_ERROR_801   				0x80001  //ͼ���軭����ָ�����ļ������ڣ���4��  801
#define D_BMP_XY_ERROR_802  							0x80002  //ͼ���軭����  �軭������󣨡�5��   802
#define VOICE_ERROR_901    								0x90001  //����IC�쳣    901


#define PE_FILE_MAX  120

#define TIM_100MS    100 

#define MAX_LOGO_ACTION_MONTH 240//���������Ͷ���ʱ����ۼ�����


#define sdram_field_offset0(Tp_field)       *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field])
#define sdram_field_offset1(Tp_field)       *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+1)
#define sdram_field_offset2(Tp_field)       *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+2)
#define sdram_field_offset3(Tp_field)       *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+3)
#define sdram_field_offset4(Tp_field)       *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+4)
#define sdram_field_offset5(Tp_field)       *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+5)
#define sdram_field_offset6(Tp_field)       *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+6)
#define sdram_field_offset7(Tp_field)       *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+7)

#define logodata_2gbit_change_addr              0x5fa0000 //0x5fa0000

#define MAX_LOGO_ACTION_MONTH 240//????????????????????????
#define MAX_LOGO_UINT_NUM     999//???????????
#define UINT_START1_OFFSET    0   
#define UINT_START2_OFFSET    ((MAX_LOGO_UINT_NUM +2)*1)    
#define UINT_START3_OFFSET    ((MAX_LOGO_UINT_NUM + 2)*2)   
#define UINT_CS1_OFFSET       (MAX_LOGO_UINT_NUM + 1)
#define UINT_CS2_OFFSET       ((MAX_LOGO_UINT_NUM + 1)*2 +1)
#define UINT_CS3_OFFSET       ((MAX_LOGO_UINT_NUM + 1)*3 +2) 

#define MAX_LOGO_FINALTEST_ONEPACKET 106//105//105//һ�����ղ������ݵĳ���  106-�����̶���7 + �������99
#define MAX_LOGO_ALLTEST_ONEPACKET(Tp_field)  (gs_AreaInfo[Tp_field].size/2 + gs_AreaInfo[Tp_field].size%2+ 6)

typedef struct
{	
	uint8_t year; //	hex	�꣨[15]��[99]����
	uint8_t month;//	�£�[01]��[12]��
	uint8_t day;//	�գ�[01]��[31]��
	uint8_t hour;//�r��[00]��[23]��
	uint8_t minute;//	�֣�[00]��[59]��
	uint8_t second;//	�루[00]��[59]��                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
}TIME_TYPE;


typedef struct
{
	unsigned int addr;  //��ַ	
	int space;			    //�ռ�
	int type;			      //��������
	int size;			      //Ԫ�ش�С
}AreaConfig;


typedef struct
{
  
   uint8_t   state:1;  //0 �� 1 �ǹ�
   uint16_t  brightness;
}LCD_BACK_LIGHT_TYPE;

typedef struct
{
	uint8_t touch_sensibility;
	uint8_t rtc_coarse_value;
	uint8_t no_vbat_pwr_on_cnt; //û��ؿ������������ڼ������л�,No battery boot times, for the accumulation of the month switch
	uint8_t dataclass_1_2_action_count;
	uint8_t last_poweroff_year;
	uint8_t last_poweroff_month;
	LCD_BACK_LIGHT_TYPE  lcd_back_light;
	//uint32_t basedata_checksum;
	uint8_t flag; //0xa5 xor;
}PARA_TYPE;


typedef enum DATACLASS
{ 
	Base_DATACLASS,          //0 �������� //BASE_DATA_CLASS,          //
	ActionNumber_DATACLASS,   //1 ��������//ACTION_NUMBER_DATA_CLASS,  //
	ActionTimers_DATACLASS,   //2 ����ʱ��//ACTION_TIMERS_DATA_CLASS,  // //2 
	UnitPara_DATACLASS,       //3 ��Ԫ����//UNIT_PARA_DATA_CLASS,//
	TestFinalData_DATACLASS,  //4 �ⶨֵ��������//TEST_FINAL_DATA_DATA_CLASS,//
	TestAllData_DATACLASS,    //5 �ⶨֵȫ������//TEST_ALL_DATA_DATA_CLASS,    //5 
	AnomalyRecord_DATACLASS,  //6 ��������//ERROR_RECORD_DATACLASS,  //
	ActionRecord_DATACLASS,    //7 ��������//ACTION_RECORD_DATACLASS,    //7  
	//BLANK_DATACLASS
}DATACLASS_TYPE; 


typedef enum IO_TYPE
{
	TYPE_SELECT=0,
	MODE_SELECT=1,
}IO_TYPE_E;


typedef enum IO_TYPE_status
{
	LEVEL_0=0,
	LEVEL_1,
	STATUS_Err,
}IO_TYPE_status_E;


struct POWER_ON_RECORD_TYPE
{
	uint8_t Poweron_Year;
	uint8_t Poweron_Month;
	uint8_t Poweron_Day;
	uint8_t Poweron_Hour;
	uint8_t Poweron_Minute;
	uint8_t Poweron_Second;
	uint8_t Battery_Voltage;
};

struct LOGO_Y5Y6_TYPE
{
	DATACLASS_TYPE  DATA_Type;
  uint8_t   AREA_NO;  
	uint32_t  ITEM_NOW;//????????????
 // uint16_t  ITEM_ALL;//h??????????
	uint16_t  DATACLASS4_ITEMNO;//DATACLASS4???ITEMNO
};	

void comm_handle(void);
void SEND_data(void);
uint8_t  decode_protocol(uint8_t *buff,uint16_t len,uint8_t type);
void code_protocol_ack(uint8_t  x_xor,uint8_t len,uint8_t *buff,uint8_t type);
void convet_hex_to_hex_asccii(uint8_t x_FSC,uint8_t *x_BUFF);
uint8_t HEXTODEC(uint8_t x_data);
uint16_t convet_2_hex_ASCII_to_hex(uint8_t *buff);
void COMM_SOFT_DEINIT(void);
void judge_uart2_receive_end(void);
int8_t convert_1_hex_to_hex_ascii(int8_t a);
uint16_t CHANGE_HEXDATA_2BYTE(uint8_t  *x_data);
void send_code_protocol_error(void);
uint16_t GET_READ_RXNUM(uint16_t x_rx_in,uint16_t x_rx_out,uint16_t x_buff_max);
void code_protocol_error(int8_t type,uint8_t Value_1,uint16_t Value_2,uint8_t cnt) ;
void  NAND_BMP_Read_checksum(void);
#endif
