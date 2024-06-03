#ifndef COMMAND_Y_H
#define COMMAND_Y_H


#define LCD_Y0		0
#define LCD_Y1		1

#define LCD_Y5		5
#define LCD_Y6		6

 void field_info_save(void);
 int AreaDataToUsbCmd(char cmd, int idx);
 uint8_t GET_LOGODATA_Y5Y6_20170919(uint8_t *x_data,uint8_t x_FSC,uint8_t x_datasource);
 int GetUsbMountFlag(void);
 void DIVIDED_DATA_HEX2BYTE(uint8_t x_FSC,uint8_t *x_BUFF);
 void Printf_One_Line(const char  *x_name,uint8_t  *x_sou,uint8_t x_long,uint32_t x_num);
#endif



