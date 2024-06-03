
#ifndef COMMAND_I_H
#define COMMAND_I_H

#define TX_BUFF_MAX_UART_IIC 2048
#define RX_BUFF_MAX_IIC 8192//2048//500

#define Basedata_copy0             0x05000//????????i?????0
#define Basedata_copy1             0x10000//????????i?????1
#define Basedata_copy2             0x15000//????????i?????2

#define logodata_Basedata_SIZE 1476//0x5c4//784//64*2048//20170511根据规格这个值变更0x310->0x320
#define logodata_ActionNumTim_SIZE 0x1000//1032//0x1000//64*2048
#define logodata_ActionTimers_SIZE 64*2048//2056//64*2048
#define logodata_UnitPara_SIZE 64*2048//1024//64*2048
#define logodata_TestFinalData_SIZE 64*2048//108//64*2048
#define logodata_TestAllData_SIZE 0x20000
#define logodata_AnomalyRecord_SIZE 0x20000
#define logodata_ActionRecord_SIZE 0x20000

typedef struct
{
	uint32_t  cycle;
	uint32_t  num;
	uint32_t  loop;
}Dataclass4567_Info;


void led_log_on(uint16_t time_ms);
void code_protocol_ack_IIC(uint8_t  x_xor,uint8_t len,uint8_t *buff,uint8_t type);  //SEND_ACKnHEX   type =0 buff 为hex　里面会转成ASCII ;  type=1 不用转
void field_info_init(void);
void WriteAreaConfig_new(void);
//void RTC_Read(void);
void Updata_Basedata_No0_cyw(void);
void basedata_ram_to_sdram(void);
uint32_t CharToHex(char *x_char);
void unit_ram_to_sdram(void);
#endif 


