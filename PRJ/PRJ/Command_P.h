#ifndef COMMAND_P_H
#define COMMAND_P_H

#include <stdint.h>

#define BASE_data_fresh_time 0
#define LCD_Ver_index  2 
#define Master_Ver_index 3
#define BMP_Ver_index  4
#define IDNumber_High_index 6
#define IDNumber_Low_index 7
#define PCB_Checked_5inch  12
#define VBAT_FLAG_5inch  13
#define BASE_data_8byte_end 32
#define BASE_data_PowerOffYear 96
#define BASE_data_fieldarea_end 96
#define BASE_data_actioncount  97
#define BASE_data_lcdbackligt   98
#define BASE_data_touchsensibility  99
#define BASE_data_PowerOffMonth 100
#define BASE_data_Vbatlow_monthcount 101
#define BASE_data_RTCcoarse   102
#define BASE_data_PowerSave   103
#define BASE_data_ParaFlag    104
#define BASE_data_SW3_6_TIMES  105
#define BASE_data_fieldinfo_count 112
#define BASE_data_ErrBlock    160
#define BASE_data_Dataclass12RTCPowerOffTimer 161 //range :1-60
#define BASE_data_checksum 163
#define BASE_data_4byte_end 164


#define U2N_P0			0
#define U2N_P1			1
#define U2N_P2			2
#define U2N_P3			3


#define U2N_PA			10
#define U2N_PB			11
#define U2N_PC			12
#define U2N_PD			13
#define U2N_PE			14

#define ERR_CREAT_FAILED		1
#define ERR_WRITE_FAILED		2
#define ERR_OPEN_FAILED			3
#define ERR_READ_FAILED     4//2011017
#define ERR_LOGIC_FAILED    5

#define NAND_PAGE_SIZE             ((uint16_t)0x0800)
#define IMAGE_BUFFER_SIZE    800*3
#define BAD_BLOCK_LOCK 0x36373839
#define BAD_BLOCK_TOTAL 80

#define backup_tab_nandflash_start      0//0         //  0x0000_0000??0x0001_FFFF
#define image_file_nandflash_start      (1*64)//(1*64)//64        
#define image_user_nandflash_start      (1425*64)//(669*64)//(1425*64)//42816
#define image_tab__nandflash_start      (1529*64)//(765*64)//(1529*64)//48960
#define baseB_data__nandflash_start      (1533*64)//(769*64)//(1533*64)//49152   //SAVE
#define logo_data__nandflash_start      (1534*64)//(770*64)//(1534*64)//49216
#define unit_data__B_nandflash_start      (1966*64)
#define baseA_data__nandflash_start      (1967*64)//POWERDOWN DON'T SAVE
#define backupdata_nandflash_start      (1968*64)//(1004*64)//(2008*64)//64256
#define nandflash______________end      (2048*64)//(1024*64)//(2048*64)//65536     // 

#define NAND_CHECKSUM_IN_SPIFLASH     6


#define bmp_LCD_BUFFER 				display_layer_sdram.LCD_CACHE_BUFFER
#define bmp_mixlayer_BUFFER  	display_layer_sdram.LCD_FRAME_BUFFER
#define bmp_layer1_BUFFER 		display_layer_sdram.LCD_FRAME1_BUFFER
#define bmp_layer2_BUFFER 		display_layer_sdram.LCD_FRAME2_BUFFER
#define bmp_layer3_BUFFER 		display_layer_sdram.LCD_FRAME3_BUFFER
#define bmp_tab_BUFFER    		display_layer_sdram.IMAGE_TAB_BUFFER
#define SDRAM_Q_TAB       		display_layer_sdram.IMAGE_Q_BUFFER
#define SDRAM_Q_SHIFT_TAB     display_layer_sdram.IMAGE_Q_SHIFT_BUFFER
#define SDRAM_NANDFLASH_TEMP 	display_layer_sdram.NAND_TEMP_BUFFER 


#define __IO  volatile

typedef struct 
{
	int total;
	int okCnt;
	int failedCnt;
}CopyFileInfo;


//STRUCT DEFINE 
typedef struct 
{
  uint32_t Zone;
  uint32_t Block;
  uint32_t Page;
} 
NAND_ADDRESS_STR;


typedef union 
{
   uint32_t val;
   struct 
	 {
		 uint32_t  ctpiic_error:1;//0  u1   101
		 uint32_t  usb_error:1;//1
		 uint32_t  logoiic_error:1;//2       //601
		 uint32_t  ctpversion_error:1;//3
		 uint32_t  nandflash_error:1;//4  //702
		 uint32_t  lsi_error:1;//5
		 uint32_t  lse_error:1;//6      402
		 uint32_t  refresh_error:1;//7
		 uint32_t  imagemore_error:1;//8
		 uint32_t  voice_error:1;//9   901
		 uint32_t  uart_recived:1;//10
		 uint32_t  logo_recived:1;//11
		 uint32_t  mainpower_error:1;//12//主电源开机ad采集low 可能无法进入logo保存
     uint32_t  vbat_error:1;//13//实时时钟电池电压低于 1.7v		   403
		 uint32_t  logodatacheck_error:1;//14//logo数据读出校验错误
		 uint32_t  usb_unable_connect:1;//15// u盘无法连接  501
		 uint32_t  usb_cannot_find_image:1;//16// u盘找不到图像文件  206
		 uint32_t  usb_cannot_find_hexortxt:1;//17// u盘找不到hex文件  503
		 uint32_t  comm_handle_finish:1;//18 // 用于二次送信的处理完成的标志  
		 uint32_t  draw_filenotfound_error:1;//19 //对应版协议的描画的文件不存在 u1801  801
		 uint32_t  draw_coordinatebeyond_error:1;//20 //对应图像坐标越界的错误 u1    802
		 uint32_t  image_outofrange_error:1;//21 图像超出分配的范围    201
		 uint32_t  image_bmp800480_large_error:1;//22 图像超出800*480大小 202
		 uint32_t  image_samefilename_error:1;//23 追加烧写时文件名重复  204
		 uint32_t  image_coversize_error:1;//24 覆盖大小错误  205
		 uint32_t  image_filenobmp_error:1;//25 不是bmp文件   203
		 uint32_t  image_movenantosdram_error:1;//26 图像从nand导入到sdram错误  301		 	
		 uint32_t	 nand_canot_find_Q_bmp_error:1;// 27 在NANDFLASH中找不到Q要的图片  302
		 uint32_t	 usb_canot_write_error:1;// 28 //USB访问错误不能写入到USB  502

		 uint32_t rtc_no_bat_after_no_set:1; //29 qdhai add 时钟数据读取指令，没有数据设定的情况下、输出　拔掉RTC电池后，再装上电池，
		 uint32_t	 layer_img_num_over_max:1;//30 层的图像数超了最大数
		 uint32_t  nandflash_Write_error:1;//31  //701
	 }bits;		 
}SYSTEMERRORSTATUS_TYPE;		

typedef struct BADMANAGE_TAB
{
	uint32_t NANDFLASH_CUSTOMER_INX;
	uint32_t NANDFLASH_USER_INX;
	uint32_t flag;//被检测后置位为一个定值0x36373839
	uint32_t  ERR_NUMBER;//坏块个数只在flag被置位为定值时才有效
	uint16_t ERR_BLOCK[BAD_BLOCK_TOTAL];//检查出有问题的块
	uint16_t BACKUP_BLOCK[BAD_BLOCK_TOTAL];//对应替换的块
  uint32_t backup_checksum;
}BADMANAGE_TAB_TYPE;

typedef union BADMANGE_TAB_U
{
	BADMANAGE_TAB_TYPE BAD_MANAGE_str;
	uint8_t  BAD_MANAGE_arr[BAD_BLOCK_TOTAL*4+20];
}BADMANAGE_TAB_TYPE_U;
	
typedef union Checksum_SPI_U
{
	uint32_t U32_ARRY[NAND_CHECKSUM_IN_SPIFLASH];
	uint8_t U8_ARRY[NAND_CHECKSUM_IN_SPIFLASH*4];
}TAB_Checksum_SPI_U;

void UsbWriteNandFlash(char cmd, unsigned short *iFile, int cnt);			// P0~P4 PA~PD
uint8_t SDRAM_TO_NANDFLASH(uint32_t x_sdram_start,uint32_t x_nandflash_start,uint16_t x_block_num);
uint32_t READ_TAB_FROMSDRAM(uint16_t x_order,uint8_t x_layer,uint32_t *x_bmp);
uint8_t NANDFLASH_TO_SDRAM(uint32_t x_sdram_start,uint32_t x_nandflash_start,uint16_t x_block_num);
void NANDFLASH_BADMANAGE_INIT(void);
int check_u_disk_img(void);
void nandflash_format(void);
uint8_t Clear_sdram(uint32_t x_Flag);
uint8_t get_command_xor(void);
uint16_t BAD_BLOCK_CHANGE(uint16_t x_block);
void check_sum_nand(uint32_t x_addr,uint16_t x_width,uint16_t x_height);
void NANDFLASH_backup_checksum(void);
void NANDFLASH_P3PD_INX_SAVE(void);
void poweroff_wait(void);
#endif 
