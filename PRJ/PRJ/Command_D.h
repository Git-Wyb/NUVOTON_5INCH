#ifndef COMMAND_D_H
#define COMMAND_D_H


#include <stdint.h>
#include "Command_G.h"

#define LCD_D0	0
#define LCD_D1	1
#define LCD_D8	2
#define LCD_D9	3

uint8_t BMP_judge(int cnt , BmpFIleInfo *info);
void DisCmdDisplay(char cmd, BmpFIleInfo *info1, uint16_t cnt1,BmpFIleInfo *info2, uint16_t cnt2,BmpFIleInfo *info3, uint16_t cnt3);
uint8_t NANDFLASH_TO_SDRAM_RANDOM(uint32_t x_sdram_start,uint32_t x_nandflash_start,uint32_t x_sdram_size,uint32_t x_nandflash_size);
int GetSdramImgInfoByIdx(int idx, uint16_t *width, uint16_t *height, void **pBuf) ;
int NandToSdramBmp(uint16_t x_order, uint16_t *x, uint16_t *y, void *pBuf);
void DisplayBmpToFrame(int x, int y, int width, int height, void *pBuf, int frame);
uint32_t nandflash_to_sdram_Q0(uint32_t x_nandflashstartaddr,uint32_t addr,uint32_t *x_bmp,uint32_t addrmax);
uint32_t CHECK_Same(uint16_t x,uint16_t y ,uint16_t width,uint16_t height,uint8_t layer);
int GetBmpFIleSize_SDRAMfrist(uint16_t file_name, uint16_t *width, uint16_t *height);
#endif

