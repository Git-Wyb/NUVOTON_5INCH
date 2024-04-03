#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <stdint.h>

uint32_t get_timego(uint32_t x_data_his);
void lcd_printf(const char *fmt,...);

void lcd_printf_new(const char *fmt,...);
void SetZuobiao(uint16_t x_x,uint16_t x_y);
#endif
