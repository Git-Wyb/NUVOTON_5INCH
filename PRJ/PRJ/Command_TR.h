
#ifndef COMMAND_TR_H
#define COMMAND_TR_H

#include <stdint.h>

typedef struct 
{
	short state;
	//short cnt;
	short x;
	short y;
	int  sendCnt;
	uint8_t T_flag_new;
}tpInfo;

typedef struct 
{
	short x;
	short y;
	int state;
}tpSendData;

typedef enum
{
    int_enable         =   0,   /*!< Sunday    */
    int_disable         =   1,   /*!< Monday    */
    int_wait        =   2,   /*!< Tuesday   */
}E_INT_STATUS;


void touch_dev_poll(void);
void check_tp_send(void);
void ETMR2_IRQHandler(void);
void FT5x06_RD_Reg(uint8_t reg,uint8_t *buf,uint8_t len);
#endif





