#ifndef COMMAND_G_H
#define COMMAND_G_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define LCD_G0	0
#define LCD_G1	1
#define LCD_G2	2
#define LCD_G3	3
#define LCD_G4	4
#define LCD_G5	5
#define LCD_G6	6
#define LCD_G7	7
#define LCD_FF  0xff

#define LAY_MAX_BMP_CNT		128
#define TEMP_BMP_FILE_INFO_MAX_NUM   128//(21*2)  //最大数

typedef struct 
{
	int idx;//图像ID
	int x; //x y 显示的坐标;
	int y;
	int width;// width height 图像的长和高　layer在哪层显示
	int height;
	void *pBuf;
}layBmpInfo;

typedef struct 
{
	layBmpInfo bmp[LAY_MAX_BMP_CNT];
	int bmpCnt;
}layInfo;

typedef struct 
{
	uint16_t x;
	uint16_t y;
	uint8_t  layer;
	uint16_t name;
	uint16_t width;
	uint16_t height;
}BmpFIleInfo;

typedef struct
{
     BmpFIleInfo bmp[TEMP_BMP_FILE_INFO_MAX_NUM];
     uint16_t num;
}BMP_RPOTOCOL_TYPE;

void DisPlayLayer(char cmd, int layer1, int layer2);		//G0 ~ G5

#endif
