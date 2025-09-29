#ifndef COMMAND_F_H
#define COMMAND_F_H

#define LCD_F7		7
#define LCD_F8		8

#include "rtc.h"

uint8_t get_bad_block(void);
int AreaReadCmdF(char cmd, int filed, int idx, char *buf, int *len);
//void RTC_CLKOUT(RTC_TICK x_tick);
void RTC_CLKOUT(void);
void RTC_CLKSTOP(void);
void Display_checksum(void);
void uart0_tx_checksum(void);
void rtc_clkout_1Hz(void);
#endif

