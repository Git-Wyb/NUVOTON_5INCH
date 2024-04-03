#ifndef DIP_SW_H
#define DIP_SW_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define SW_ON  0
#define SW_OFF 1

void POWER_5V_SETTING_initstart(void);
void POWER_5V_SETTING_initover(void);
void check_sw5(void);
void check_sw234(void);
void Fresh_lcd_dis(void);

 typedef struct 
{
	uint8_t ush_user_app_bit:1;  //usb可以用户用使用
	uint8_t usb_user_app_task_bit:1;  //usb 是copy data
	uint8_t sw1_4:1;  // 1 on; 0 off

	uint8_t sw1_3:1;  // 1 on; 0 off
	uint8_t sw1_2:1;  // 1 on; 0 off
	uint8_t sw1_1:1;  // 1 on; 0 off
  uint8_t sw1_6:1;  // 1 on; 0 off	
	//uint8_t renesas_power_on:1; // 1 on 0ff
}STATE_TYPE;   //

 extern  STATE_TYPE state; 

typedef struct
{
 uint8_t  task; // 1 表示有升级任务，0无 
 uint32_t  hex_len; //升级文件的长度
 //buffer start addr DOWN_LOAD_DATA_BUFFER_START_ADD
 //char filename_new_hex[60];
 uint32_t bps; //通信波特率
 uint8_t   start; // 1  可以升级DOWNLOAD_START_FLAG   0 清掉   download.start
 uint8_t   use_uart2; //=1 表示升级在用uart2  0 表示不用
}DOWNLOAD_TYPE;

#endif

