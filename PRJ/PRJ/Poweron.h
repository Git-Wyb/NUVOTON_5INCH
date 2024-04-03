#ifndef 	POWERON_H
#define   POWERON_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ONE_RTC_LOSE_CYCLE 60

void power_on_logo(void);
uint8_t get_vbat_ad_value(void);
void power_save(void);
void ReadAreaConfig_new(void);
void BASEDATA_RAM_INIT(void);
void LOGO_handle(void);
#endif








