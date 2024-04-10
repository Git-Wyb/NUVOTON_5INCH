#ifndef BSP_INIT_H
#define  BSP_INIT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct 
{
	uint32_t LCD_CACHE_BUFFER;
	uint32_t LCD_FRAME_BUFFER;
	uint32_t LCD_FRAME1_BUFFER;
	uint32_t LCD_FRAME2_BUFFER;
	uint32_t LCD_FRAME3_BUFFER;
	uint32_t IMAGE_TAB_BUFFER;
	uint32_t IMAGE_Q_BUFFER;
	uint32_t NAND_TEMP_BUFFER;
	uint32_t IMAGE_Q_SHIFT_BUFFER;
	
}LAYER_SDRAM_STR;

typedef enum
{clear,
 set} ENUM_REG;

 //LED
 #define LED_POWER_ON()  GPIO_Clr(GPIOI, BIT5)
 #define LED_POWER_OFF() GPIO_Set(GPIOI, BIT5)
 #define LED_LOGO_ON()   GPIO_Clr(GPIOI, BIT6)
 #define LED_LOGO_OFF()  GPIO_Set(GPIOI, BIT6)
 #define LED_POWERLOW_ON()  GPIO_Clr(GPIOI, BIT7)
 #define LED_POWERLOW_OFF() GPIO_Set(GPIOI, BIT7)
 #define LED_FLASHERR_ON() GPIO_Clr(GPIOI, BIT8)
 #define LED_FLASHERR_OFF() GPIO_Set(GPIOI, BIT8)
 
 //SW
#define   READ_PIN_SW1_2   					GPIO_ReadBit(GPIOE, BIT11)
#define   READ_PIN_SW1_1   					GPIO_ReadBit(GPIOE, BIT10)
#define   READ_PIN_SW1_3   					GPIO_ReadBit(GPIOE, BIT12)
#define   READ_PIN_SW1_6            GPIO_ReadBit(GPIOH, BIT3)
#define   READ_PIN_SW5              GPIO_ReadBit(GPIOH, BIT2)
#define   READ_PIN_SW1_4            GPIO_ReadBit(GPIOE, BIT13)
 
 
 //POWER 5V
#define  POWER_5V_ON()   GPIO_Set(GPIOF, BIT12)
#define   POWER_5V_OFF()   GPIO_Clr(GPIOF, BIT12)

//lcd reverse
#define    SCREEN_CONTROL133_0        		GPIO_Clr(GPIOB,BIT0)	
#define    SCREEN_CONTROL133_1         		GPIO_Set(GPIOB,BIT0)

#define    SCREEN_CONTROL134_0        		GPIO_Clr(GPIOB,BIT1)
#define    SCREEN_CONTROL134_1         		GPIO_Set(GPIOB,BIT1)

//TOUCH IC
#define   TOUCH_IC_RESET()           GPIO_Clr(GPIOB,BIT11)
#define   TOUCH_IC_SET()           GPIO_Set(GPIOB,BIT11)

//AMP POWER
#define    AMP_POWER_ON          				GPIO_Clr(GPIOI,BIT13)//GPIO_Set(GPIOI,BIT13)
#define    AMP_POWER_OFF        					GPIO_Set(GPIOI,BIT13)//GPIO_Clr(GPIOI,BIT13)

//AMP DATA
#define    AMP_DATA_1          						GPIO_Set(GPIOI,BIT11)
#define    AMP_DATA_0          						GPIO_Clr(GPIOI,BIT11)

//PD7
#define    READ_PRODUCTTYPE             GPIO_ReadBit(GPIOD,BIT7)
#define    PORDUCT_5INCH                1
#define    PORDUCT_7INCH                0

//PB12
#define    READ_WORKMODE                GPIO_ReadBit(GPIOB,BIT12)
#define    WORK_FUNCTION                1
#define    WORK_TEST                    0


//LCD 
#define   LCD_PWR_ON()           GPIO_Clr(GPIOB, BIT4)
#define   LCD_PWR_OFF()          GPIO_Set(GPIOB, BIT4)

//tool reset
#define DOWNLOAD_RESET_SET()    GPIO_Set(GPIOF, BIT13)
#define DOWNLOAD_RESET_CLR()    GPIO_Clr(GPIOF, BIT13)
#define DOWNLOAD_TOOL_CLR()  GPIO_Clr(GPIOF, BIT14)
#define DOWNLOAD_TOOL_SET()  GPIO_Set(GPIOF, BIT14) 


//AMP SD
#define     AUDIO_AMPLIFIER_WORK					//GPIO_Clr(GPIOI,BIT14)
#define    AUDIO_AMPLIFIER_SHUT_DOWN				       //GPIO_Set(GPIOI,BIT14)

#define logodata_field0_SIZE 0x20000
#define logodata_field1_SIZE 0x20000
#define logodata_field2_SIZE 0x20000
#define logodata_field3_SIZE 0x20000
#define logodata_field4_SIZE 0x20000
#define logodata_field5_SIZE 0x20000
#define logodata_field6_SIZE 0x20000
#define logodata_field7_SIZE 0x20000
#define logodata_field8_SIZE 0x20000
#define logodata_field9_SIZE 0x20000
#define logodata_field10_SIZE 0x20000
#define logodata_field11_SIZE 0x20000
#define logodata_field12_SIZE 0x20000
#define logodata_field13_SIZE 0x20000
#define logodata_field14_SIZE 0x20000
#define logodata_field15_SIZE 0x20000
void AD_init_8V(void);
void  protocol_uart_init(void);
void Timer_1ms_Init(void);
void usb_init(void);
void usb_deinit(void);
void nandflash_init(void);
void SDRAM_DATA_INIT(void);
void display_init(void);
void  REG_OPERATE(uint32_t x_reg,uint32_t x_data,ENUM_REG x_type);
void rtc_init(void);
void AD_init(void);
void init_gpio(void); 
void delay_ms(uint32_t i);
void FT5x06_Init(void);
void Touch_tim_start(void);
void Touch_tim_stop(void);
void Touch_tim_init(void);
void Touch_int_enable(void);
void Touch_int_disable(void);
void voice_tim_start(uint32_t x_timer);
void voice_tim_stop(void);
void voice_tim_init(void);
void uart2_init_download(uint32_t bound);
void power_int_init(void);
void LVD_init_irq(void);
void power_int_disable(void);
void power_checkon(void);
void power_checkoff(void);
void power_checkreset(void);
void tim1ms_int_disable(void);
void tim1ms_int_enable(void);
void rtc_time_deinit(void);
#endif

