#ifndef POWEROFF_H
#define POWEROFF_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define VOLT_SLEEP  (0x99F-(TYPE_PRODUCT?0:1)*0x1CD)//(0xC31-(READ_PRODUCTTYPE?0:1)*0x200)//6.6V  B7C-6.2V//8V
#define VOLT_WORK   (0x99f-(TYPE_PRODUCT?0:1)*0x1CD)//A39-8.5V 99F-8V 0xAD3-9V

void main_pwr_process(void);
void LVD_init_scan(void);
void LVD_init_irq(void);
uint16_t get_main_pwr_ad_value(void);
void LVD_disable(void);
vu32 GPIOHCallback(UINT32 status, UINT32 userData);
#endif

