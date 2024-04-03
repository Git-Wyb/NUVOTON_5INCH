

#ifndef COMMAND_M_H
#define COMMAND_M_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

typedef enum
{
	PPT_IDLE,//空闲
	PPT_START,//开始PPT
	//SEARCH_FILE,//找相关的文件
	FADE_OUT,//渐暗
	FADE_IN, //渐亮
	DISPLAY_HOLD,//显示维持状态
	PPT_OVER,//PPT结束
}PPT_enum;	


typedef struct
{
  uint16_t keep_time;  // 1～255（秒）　1张照片显示的时间
  //uint32_t keep_time_cnt;  // ms
  //uint8_t   task;  //=1 表示有PPT任务　　0表示结束
  PPT_enum  step;
}PPT_TYPE;



void Backlinght_Control_Init_HARDV4(uint8_t x_level);
void start_ppt(void);
void ppt_find_file_and_Q(char cmd);
void end_ppt(void);
void ppt_process(void);
void DisPlayPptBmp(int fileName);
#endif



