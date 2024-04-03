

#ifndef COMMAND_M_H
#define COMMAND_M_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

typedef enum
{
	PPT_IDLE,//����
	PPT_START,//��ʼPPT
	//SEARCH_FILE,//����ص��ļ�
	FADE_OUT,//����
	FADE_IN, //����
	DISPLAY_HOLD,//��ʾά��״̬
	PPT_OVER,//PPT����
}PPT_enum;	


typedef struct
{
  uint16_t keep_time;  // 1��255���룩��1����Ƭ��ʾ��ʱ��
  //uint32_t keep_time_cnt;  // ms
  //uint8_t   task;  //=1 ��ʾ��PPT���񡡡�0��ʾ����
  PPT_enum  step;
}PPT_TYPE;



void Backlinght_Control_Init_HARDV4(uint8_t x_level);
void start_ppt(void);
void ppt_find_file_and_Q(char cmd);
void end_ppt(void);
void ppt_process(void);
void DisPlayPptBmp(int fileName);
#endif



