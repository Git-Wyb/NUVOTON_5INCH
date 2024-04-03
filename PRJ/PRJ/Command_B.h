#ifndef COMMAND_B_H
#define COMMAND_B_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct
{
	int8_t play_num; //0 是停止 1~E  停放一次 F表示循环
	int16_t file; //即声音文件序号
	uint16_t interval;//间隔
	uint16_t interval_cnt;
	int8_t voice;//间隔
	uint8_t ack_flag;
	uint8_t fcs_code;
}TTS_TYPE;

void ETMR3_IRQHandler(void);
void send_wt588h_init(uint16_t data);
void stop_tts_play(void);
void tts_control(void);
void send_tts_command_control(void);
void tts_time(void) ;
//void set_wt588h_voice_state(uint8_t a);
vu32 GPIOICallback(UINT32 status, UINT32 userData);
void check_wt588h_exist(void) ;

#endif

