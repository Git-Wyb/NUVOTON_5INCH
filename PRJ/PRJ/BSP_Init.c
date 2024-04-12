

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "sys.h"
#include "uart.h"
#include "BSP_Init.h"
#include "ff.h"
#include "usbh_lib.h"
#include "nand.h"
#include "gpio.h"
#include "lcd.h"
#include "2d.h"
#include "nand_nand.h"
#include "Command_all.h"
#include "RTC.h"
#include "adc.h"
#include "etimer.h"
#include "poweron.h"
#include "poweroff.h"
#include "wwdt.h"
#include "Aprotocol.h"
#include "DIP_SW.h"


//usb
#define BUFF_SIZE       (64*1024)
BYTE Buff_Pool[BUFF_SIZE] __attribute__((aligned(32))); 
FRESULT RES;
UART_T param;
BYTE  *Buff;
FATFS usb_fatfs;
/*__attribute__((aligned(32)))*/ LAYER_SDRAM_STR display_layer_sdram={0};

//uart
#define uartprintf      sysprintf
//////static UINT8 TX_Test[] = "��� ��ͣ��� \r\n";
//nandflash
#define CONFIG_SYS_MAX_NAND_DEVICE  1

nand_info_t nand_info[CONFIG_SYS_MAX_NAND_DEVICE];
struct nand_chip nand_chip[CONFIG_SYS_MAX_NAND_DEVICE];

static char dev_name[CONFIG_SYS_MAX_NAND_DEVICE][8];

static unsigned long total_nand_size;

extern const unsigned char image_bmp1[];
extern const unsigned char image_bmp4[];
extern const unsigned char image_bmp3[];
extern const unsigned char image_bmp2[];
extern const unsigned char image_bmp6[];

extern uint32_t NANDFLASH_CUSTOMER_INX;
extern uint32_t NANDFLASH_USER_INX; 

extern  BADMANAGE_TAB_TYPE_U badmanage_str[1];
extern RTC_TIME_DATA_T pwr_on_time_ground;


int nand_curr_device = -1;

//uint8_t NAND_WRITE_BUF[2112];

uint32_t logodata_basedata_BUFFER=0,logodata_field1_BUFFER=0,logodata_field2_BUFFER=0,logodata_field3_BUFFER=0,logodata_field4_BUFFER=0,\
          logodata_field5_BUFFER=0,logodata_field6_BUFFER=0,logodata_field7_BUFFER=0,logodata_field8_BUFFER=0,logodata_field9_BUFFER=0,\
					logodata_field10_BUFFER=0,logodata_field11_BUFFER=0,logodata_field12_BUFFER=0,logodata_field13_BUFFER=0,logodata_field14_BUFFER=0,\
					logodata_field15_BUFFER=0;

extern uint32_t logodata_sdrambuffer_addr_arry[16];
//const uint32_t   logodata_field0_SIZE = 0x20000,logodata_field1_SIZE = 0x20000,logodata_field2_SIZE = 0x20000,logodata_field3_SIZE = 0x20000,\
//	               logodata_field4_SIZE = 0x20000,logodata_field5_SIZE = 0x20000,logodata_field6_SIZE = 0x20000,logodata_field7_SIZE = 0x20000,;

void delay_ms(uint32_t i);
extern  uint8_t *BaseData_ARR;
extern  char* writeTextBuff;
extern uint8_t FLAG_WDT;
extern  UINT8 *RX_Test/*[500]*/;
extern UINT8 *TX_Test;
extern uint8_t *UART2_RX_BUFF;
extern  uint8_t  *UART2_TX_BUFF_NEW;
 
extern uint8_t *RxBuffer ;//[NAND_PAGE_SIZE];
extern uint8_t *TxBuffer ;//[NAND_PAGE_SIZE];
uint8_t *RxBuffer_noshift;
 uint8_t flag_usb_init=0;
 extern int gs_usb_mount_flag;
 extern uint8_t TYPE_PRODUCT;
 
void  REG_OPERATE(uint32_t x_reg,uint32_t x_data,ENUM_REG x_type)
{
	uint32_t Tp_data;
	if(x_type==clear)
	{
		Tp_data = inpw(x_reg);
		Tp_data = Tp_data & (~x_data);
		outpw(x_reg,Tp_data);
	}
	else
	{
		Tp_data = inpw(x_reg);
		Tp_data = Tp_data | x_data;
		outpw(x_reg,Tp_data);
	}
}

void uart2_init_download(uint32_t bound)
{
		 int  retval;//,len;
	
	  REG_OPERATE(REG_SYS_APBIPRST0,1<<17,set);
	  REG_OPERATE(REG_CLK_PCLKEN0,1<<17,clear);
	
	  /* enable UART1 clock */
    //outpw(REG_CLK_PCLKEN0, inpw(REG_CLK_PCLKEN0) | (1<<17));
	   REG_OPERATE(REG_SYS_APBIPRST0,1<<17,clear);
	  REG_OPERATE(REG_CLK_PCLKEN0,1<<17,set);
	
	
	REG_OPERATE(REG_SYS_GPE_MFPL,0X0000FF00,clear);
    REG_OPERATE(REG_SYS_GPE_MFPL,0X00009900,set);
	
	//uartClose(&param);
	param.uFreq = 12000000;
    param.uBaudRate = bound;
    param.ucUartNo = UART1;
    param.ucDataBits = DATA_BITS_8;
    param.ucStopBits = STOP_BITS_1;
    param.ucParity = PARITY_NONE;
    param.ucRxTriggerLevel = UART_FCR_RFITL_1BYTE;
	
    retval = uartOpen(&param);

	 // #ifdef DEBUG
	  if(retval != 0) {
        uartprintf("���ڴ�ʧ��115200!\n");
    }

    retval = uartIoctl(param.ucUartNo, UART_IOC_SETTXMODE, UARTINTMODE, 0);
    if (retval != 0) {
        uartprintf("���÷����ж�ģʽʧ��115200!\n");
    }

    retval = uartIoctl(param.ucUartNo, UART_IOC_SETRXMODE, UARTINTMODE, 0);
    if (retval != 0) {
        uartprintf("���ý����ж�ģʽʧ��115200!\n");
    }
		
		#ifdef SYSUARTPRINTF
		uartprintf("uartset 115200\n");
		#endif
//		 len = strlen((PINT8) TX_Test);
//		 uartWrite(param.ucUartNo, TX_Test, len);
		
}

void  protocol_uart_init(void)
{
	  int  retval;
	  
	  REG_OPERATE(REG_SYS_APBIPRST0,1<<17,set);
	  REG_OPERATE(REG_CLK_PCLKEN0,1<<17,clear);
	
	  /* enable UART1 clock */
    //outpw(REG_CLK_PCLKEN0, inpw(REG_CLK_PCLKEN0) | (1<<17));
	   REG_OPERATE(REG_SYS_APBIPRST0,1<<17,clear);
	  REG_OPERATE(REG_CLK_PCLKEN0,1<<17,set);

    /* GPB2, GPB3 */
    //outpw(REG_SYS_GPB_MFPL, (inpw(REG_SYS_GPE_MFPL) & 0xffff00ff) | (0x99<< 8));  // UART0 multi-function
    //PE2 PE3
	  REG_OPERATE(REG_SYS_GPE_MFPL,0X0000FF00,clear);
    REG_OPERATE(REG_SYS_GPE_MFPL,0X00009900,set);
	
		 param.uFreq = 12000000;
    param.uBaudRate = 38400;
    param.ucUartNo = UART1;
    param.ucDataBits = DATA_BITS_8;
    param.ucStopBits = STOP_BITS_1;
    param.ucParity = PARITY_EVEN;
    param.ucRxTriggerLevel = UART_FCR_RFITL_1BYTE;
	
    
		
	  #ifdef DEBUG

retval = uartOpen(&param);

 	  if(retval != 0) {
        uartprintf("���ڴ�ʧ��!\n");
    }
		
		
    retval = uartIoctl(param.ucUartNo, UART_IOC_SETRXMODE, UARTINTMODE, 0);
    if (retval != 0) {
        uartprintf("���ý����ж�ģʽʧ��!\n");
    }
		   retval = uartIoctl(param.ucUartNo, UART_IOC_SETTXMODE, UARTPOLLMODE, 0);
    if (retval != 0) {
        uartprintf("���÷����ж�ģʽʧ��!\n");
    }

		
		
		//REG_OPERATE(REG_UART1_IER,0x09,clear);
		#ifdef SYSUARTPRINTF
		uartprintf("uartset 38400\n");
		#endif
		  //���ڷ������� ����� ��ͣ�����
		// len = strlen((PINT8) TX_Test);
		// uartWrite(param.ucUartNo, TX_Test, len);
		#endif
		
		

		
}


void Timer_1ms_Init(void)
{
			 /*--- init timer ---*/
	   #ifdef SYSUARTPRINTF
	   sysprintf("Init Timer0 1ms");
	   #endif
    sysSetTimerReferenceClock (TIMER0, 12000000);
    sysStartTimer(TIMER0, 1000, PERIODIC_MODE);
}

void usb_deinit(void)
{
	// if(flag_usb_init==1)
	 {
	outpw(REG_SYS_GPE_MFPH, (inpw(REG_SYS_GPE_MFPH) & (~0xff000000)) );
		 flag_usb_init = 0;
	 }
}

void usb_reinit(void)
{
	uint32_t Tp_count;
  if(flag_usb_init==0)
	{
   flag_usb_init = 1;
	//TCHAR       usb_path[] = { '3', ':', 0 };    /* USB drive started from 3 */
  REG_OPERATE(REG_CLK_HCLKEN,0x40000,clear);
	
	outpw(REG_CLK_HCLKEN, inpw(REG_CLK_HCLKEN) | 0x40000);//USBH EN
   // outpw(REG_CLK_PCLKEN0, inpw(REG_CLK_PCLKEN0) | 0x10000);//UART0 EN

    // set PE.14 & PE.15 for USBH_PPWR0 & USBH_PPWR1
    outpw(REG_SYS_GPE_MFPH, (inpw(REG_SYS_GPE_MFPH) & ~0xff000000) | 0x77000000);



    Buff = (BYTE *)((UINT32)&Buff_Pool[0] | 0x80000000);   /* use non-cache buffer */
 	#ifdef  SYSUARTPRINTF
	sysprintf("LCD_FRAME1_BUFFER=%X,%X,%X,%X,%X\r\n",
	*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER),*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+1), 
	*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+2), *(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+3),
	*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+4));
	#endif
    usbh_core_init();
	#ifdef  SYSUARTPRINTF
		sysprintf("LCD_FRAME1_BUFFER=%X,%X,%X,%X,%X\r\n",
	*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER),*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+1), 
	*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+2), *(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+3),
	*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+4));
#endif
    usbh_umas_init();
	#ifdef  SYSUARTPRINTF	
			sysprintf("LCD_FRAME1_BUFFER=%X,%X,%X,%X,%X\r\n",
	*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER),*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+1), 
	*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+2), *(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+3),
	*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+4));
#endif

    for(Tp_count=0;Tp_count<3000;Tp_count++)
		{
      			usbh_pooling_hubs();
			     if(gs_usb_mount_flag==1)
					 {
						 break;
					 }
		}
		if(Tp_count>=3000)
		{
			usb_deinit();
			//flag_usb_init = 0;
		}
		
		#ifdef  SYSUARTPRINTF
			sysprintf("LCD_FRAME1_BUFFER=%X,%X,%X,%X,%X\r\n",
	*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER),*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+1), 
	*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+2), *(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+3),
	*(uint8_t *)(display_layer_sdram.LCD_FRAME1_BUFFER+4));
	#endif
	
	//	RES = f_chdrive("3:");
	//	sysprintf("%d-----",RES);
  
	 
	 
	 
 }
}
void usb_init(void)
{
	uint8_t Tp_count;
	
	for(Tp_count=0;Tp_count<5;Tp_count++)
	{
	  
		if(gs_usb_mount_flag==1) break;
	    usb_reinit();
	}
	
	 #ifdef  SYSUARTPRINTF
		sysprintf("usb_reinit=%d\r\n",Tp_count);
		#endif
}




int nand_register(int devnum)
{
	struct mtd_info *mtd;

//	if (devnum >= CONFIG_SYS_MAX_NAND_DEVICE)
//		return -EINVAL;

	mtd = &nand_info[devnum];

	sprintf(dev_name[devnum], "nand%d", devnum);
	mtd->name = dev_name[devnum];

	/*
	 * Add MTD device so that we can reference it later
	 * via the mtdcore infrastructure (e.g. ubi).
	 */
//////	add_mtd_device(mtd);

	total_nand_size += mtd->size / 1024;

	if (nand_curr_device == -1)
		nand_curr_device = devnum;

    sysprintf("%s\n", mtd->name);

	return 0;
}
int board_nand_init(struct nand_chip *nand)
{
    struct mtd_info *mtd;

   

    /* initial NAND controller */
    outpw(REG_CLK_HCLKEN, (inpw(REG_CLK_HCLKEN) | 0x300000));

    /* select NAND function pins */
    //if (inpw(REG_SYS_PWRON) & 0x08000000)
    //{
        /* Set GPI1~15 for NAND */
    //    outpw(REG_SYS_GPI_MFPL, 0x55555550);
    //    outpw(REG_SYS_GPI_MFPH, 0x55555555);
    //}
    ///else
    //{
        /* Set GPC0~14 for NAND */
        outpw(REG_SYS_GPC_MFPL, 0x55555555);
        outpw(REG_SYS_GPC_MFPH, 0x05055555);
				
				 GPIO_OpenBit(GPIOC, BIT13, DIR_INPUT, NO_PULL_UP);


////////////    //??PF11-PF14??????????
////////////    GPIO_EnableTriggerType(GPIOC,BIT13,FALLING);
////////////		//??????
////////////    sysSetLocalInterrupt(ENABLE_IRQ);

////////////    //??GPIO??
////////////    GPIO_EnableInt(GPIOC, (GPIO_CALLBACK)GPIOCCallback, 0);
				
				//REG_OPERATE(REG_GPIOC_DIR,1<<13,clear);//pc13 busy in
				//REG_OPERATE(REG_GPIOC_IFEN,1<<13,set);//PC13 FAIL INT
				//GPIO_EnableInt(GPIOC, (GPIO_CALLBACK)GPIOCCallback, 0);
				// sysSetLocalInterrupt(ENABLE_IRQ);
				//REG_OPERATE(REG_GPIO_ISR,1<<2,set);//PC trigger GPIO INT
				//sysInstallISR();
    //}

    // Enable SM_EN
    REG_OPERATE(REG_FMI_CTL, 1<<3,set);
    outpw(REG_NANDTMCTL, 0x20305);
	 // outpw(REG_NANDTMCTL, 0x30406);

    // Enable SM_CS0
    outpw(REG_NANDCTL, (inpw(REG_NANDCTL)&(~0x06000000))|0x04000000);
    outpw(REG_NANDECTL, 0x1); /* un-lock write protect */

    // NAND Reset
    outpw(REG_NANDCTL, inpw(REG_NANDCTL) | 0x1);    // software reset
    while (inpw(REG_NANDCTL) & 0x1);
    
		
		//ECC ENABLE
		
		// Redundant area size
 ////   outpw(REG_NANDRACTL, 0X40);
	////	REG_OPERATE(REG_NANDCTL,(1<<17)|(1<<18)|(1<<20)|(1<<21)|(1<<22),clear);
	////	REG_OPERATE(REG_NANDCTL,(1<<4)|(1<<7)|(1<<8)|(1<<16)|(1<<19)|(1<<23),set);
		
		
    /* Detect NAND chips */
    /* first scan to find the device and get the page size */
//////////////    if (nand_scan_ident(&(nuc970_nand->mtd), 1, NULL)) {
//////////////        sysprintf("NAND Flash not found !\n");
//////////////        return -1;
//////////////    }

//////////////    //Set PSize bits of SMCSR register to select NAND card page size
//////////////     sysprintf("NUC970 NAND PAGE SIZE(%d)\n", mtd->writesize );
//////////////		switch (mtd->writesize) {
//////////////        case 2048:
//////////////            outpw(REG_NANDCTL, (inpw(REG_NANDCTL)&(~0x30000)) + 0x10000);
//////////////            nuc970_nand->eBCHAlgo = 0; /* T4 */
//////////////            nuc970_layout_oob_table ( &nuc970_nand_oob, mtd->oobsize, g_i32ParityNum[1][nuc970_nand->eBCHAlgo] );
//////////////            break;

//////////////        case 4096:
//////////////            outpw(REG_NANDCTL, (inpw(REG_NANDCTL)&(~0x30000)) + 0x20000);
//////////////            nuc970_nand->eBCHAlgo = 1; /* T8 */
//////////////            nuc970_layout_oob_table ( &nuc970_nand_oob, mtd->oobsize, g_i32ParityNum[2][nuc970_nand->eBCHAlgo] );
//////////////            break;

//////////////        case 8192:
//////////////            outpw(REG_NANDCTL, (inpw(REG_NANDCTL)&(~0x30000)) + 0x30000);
//////////////            nuc970_nand->eBCHAlgo = 2; /* T12 */
//////////////            nuc970_layout_oob_table ( &nuc970_nand_oob, mtd->oobsize, g_i32ParityNum[3][nuc970_nand->eBCHAlgo] );
//////////////            break;

//////////////        /* Not support now. */
//////////////        case 512:

//////////////        default:
//////////////            sysprintf("NUC970 NAND CONTROLLER IS NOT SUPPORT THE PAGE SIZE. (%d, %d)\n", mtd->writesize, mtd->oobsize );
//////////////    }

//////////////    nuc970_nand->m_i32SMRASize  = mtd->oobsize;
//////////////    nand->ecc.bytes = nuc970_nand_oob.eccbytes;
//////////////    nand->ecc.size  = mtd->writesize;

//////////////    nand->options = 0;

//////////////    // Redundant area size
//////////////    outpw(REG_NANDRACTL, nuc970_nand->m_i32SMRASize);

//////////////    // Protect redundant 3 bytes
//////////////    // because we need to implement write_oob function to partial data to oob available area.
//////////////    // Please note we skip 4 bytes
//////////////    outpw(REG_NANDCTL, inpw(REG_NANDCTL) | 0x100);

//////////////    // To read/write the ECC parity codes automatically from/to NAND Flash after data area field written.
//////////////    outpw(REG_NANDCTL, inpw(REG_NANDCTL) | 0x10);
//////////////    // Set BCH algorithm
//////////////    outpw(REG_NANDCTL, (inpw(REG_NANDCTL) & (~0x007C0000)) | g_i32BCHAlgoIdx[nuc970_nand->eBCHAlgo]);
//////////////    // Enable H/W ECC, ECC parity check enable bit during read page
//////////////    outpw(REG_NANDCTL, inpw(REG_NANDCTL) | 0x00800080);

    return 0;
}

void nandflash_init(void)
{
	struct mtd_info *mtd = &nand_info[0];
	struct nand_chip *nand = &nand_chip[0];
	int maxchips = CONFIG_SYS_NAND_MAX_CHIPS;
	uint8_t Tp_buf[54]={0x55};
	size_t  Tp_byte=54;
	uint32_t Tp_type;
	
	
	if (maxchips < 1)
		maxchips = 1;
	
	/* enable FMI NAND */
    outpw(REG_CLK_HCLKEN, (inpw(REG_CLK_HCLKEN) | 0x300000));
	
	 /* Set GPC0~14 for NAND */
     outpw(REG_SYS_GPC_MFPL, 0x55555555);
     outpw(REG_SYS_GPC_MFPH, 0x05555555);
	
	
	//REG_OPERATE(REG_FMI_CTL,0x8,set);
	//REG_OPERATE(REG_NANDECTL,0x1,set);
	
	mtd->priv = nand;
	nand->IO_ADDR_R = nand->IO_ADDR_W = (void  __iomem *)0xB000D8B8;//NAND Flash Data Port Register

	if (board_nand_init(nand))
		return;

	//NAND_ReadID();
//	NAND_ReadID();
	//NAND_EARSE();
	//NAND_WRITE();
	
//if (nand_scan(mtd, maxchips))
//		return;

//	TEST_NANDFLASH();
	
//   nand_register(0);
 NAND_ReadID();
	
// NAND_EraseBlock(0);
//	NAND_WRITE_BUF[0]=0x55;
// NAND_WritePage(0,0,NAND_WRITE_BUF,2048);
//	NAND_WRITE_BUF[0]=0xff;
// NAND_ReadPage(0,0,NAND_WRITE_BUF,2048);
//	sysprintf("DATA = %x\n", NAND_WRITE_BUF[0]);
//	
//	TEST_NANDFLASH();
  ////////nand->erase_cmd(mtd,0);
  ///////nand->write_page(mtd,nand,Tp_buf,0,0,0);
//	
//////	Tp_buf[0]=0;
//	
//	nand->cmdfunc(mtd,NAND_CMD_READID,0,0);
//	NAND_ReadID();
	
	
//	nand_erase(nand_info,0,1);
//	nand_write(nand_info,0,&Tp_byte,Tp_buf);
//	Tp_buf[0]=0;
//	nand_read(nand_info,0,&Tp_byte,Tp_buf);
	
	 sysprintf("%x\n",Tp_type);

}


void SDRAM_DATA_INIT(void)
{
	
	uint16_t width,height;
//		union 
//	{
//		uint32_t DATA_U32;
//		uint8_t  DATA_U8[4];
//	}Tp_CHECK;
	
	
	display_layer_sdram.IMAGE_TAB_BUFFER = (uint32_t )malloc((4*64*2048)+64);
	display_layer_sdram.IMAGE_TAB_BUFFER = 32+shift_pointer((uint32_t)display_layer_sdram.IMAGE_TAB_BUFFER, 32);
	display_layer_sdram.IMAGE_TAB_BUFFER = display_layer_sdram.IMAGE_TAB_BUFFER|0X80000000;
	#ifdef  SYSUARTPRINTF  
	sysprintf("IMAGE_TAB_BUFFER=0x%8x\r\n",display_layer_sdram.IMAGE_TAB_BUFFER);
  #endif	
	
	display_layer_sdram.IMAGE_Q_BUFFER = (uint32_t )malloc((65536*8)+64);
	display_layer_sdram.IMAGE_Q_BUFFER  = 32+shift_pointer((uint32_t)display_layer_sdram.IMAGE_Q_BUFFER, 32);
	display_layer_sdram.IMAGE_Q_BUFFER=display_layer_sdram.IMAGE_Q_BUFFER|0X80000000;
	if(SDRAM_Q_TAB) memset((void *)SDRAM_Q_TAB,0,65536*8);
	#ifdef  SYSUARTPRINTF
	sysprintf("SDRAM_Q_TAB=0x%8x\r\n",display_layer_sdram.IMAGE_Q_BUFFER);
	sysprintf("SDRAM_Q_TAB=0x%8x\r\n",*(uint32_t *)(SDRAM_Q_TAB+9*8+4));
	GetBmpFIleSize_SDRAMfrist(0x0009,&width,&height);
	#endif	
	
	
	display_layer_sdram.IMAGE_Q_SHIFT_BUFFER = (uint32_t )malloc((65536*4)+64);
	display_layer_sdram.IMAGE_Q_SHIFT_BUFFER =  32+shift_pointer((uint32_t)display_layer_sdram.IMAGE_Q_SHIFT_BUFFER, 32);
	display_layer_sdram.IMAGE_Q_SHIFT_BUFFER = display_layer_sdram.IMAGE_Q_SHIFT_BUFFER|0X80000000;
	if(SDRAM_Q_SHIFT_TAB) memset((void *)SDRAM_Q_SHIFT_TAB,0,65536*4);
	
  display_layer_sdram.NAND_TEMP_BUFFER =  (uint32_t )malloc((800*480*2)+64);
	display_layer_sdram.NAND_TEMP_BUFFER = 32+shift_pointer(display_layer_sdram.NAND_TEMP_BUFFER,32);
	display_layer_sdram.NAND_TEMP_BUFFER= display_layer_sdram.NAND_TEMP_BUFFER|0X80000000;
	
	 RxBuffer = (uint8_t  *)(uint32_t )(malloc((NAND_PAGE_SIZE)+64));
	 //RxBuffer_noshift =RxBuffer;
	 RxBuffer = (uint8_t  *)(uint32_t )((32+shift_pointer((uint32_t)RxBuffer,32))|0x80000000);
	  memset(RxBuffer,0,NAND_PAGE_SIZE);
	 	#ifdef  SYSUARTPRINTF  
		sysprintf("RxBuffer mallco=0x%x\r\n",RxBuffer);
		#endif
	 //free(RxBuffer_noshift);
	 
//	 TxBuffer = (uint8_t  *)(uint32_t )(malloc((NAND_PAGE_SIZE)+64));
//	 TxBuffer = (uint8_t  *)(uint32_t )((32+shift_pointer((uint32_t)TxBuffer,32))|0x80000000);
//	  memset(TxBuffer,0,NAND_PAGE_SIZE);
//	 	#ifdef  SYSUARTPRINTF  
//		sysprintf("TxBuffer mallco=0x%x\r\n",TxBuffer);
//		#endif
	
	
	BaseData_ARR =  (uint8_t *)((uint32_t)malloc((164*9)+64));
	BaseData_ARR = (uint8_t *)(shift_pointer((uint32_t)BaseData_ARR,32)+32);
	BaseData_ARR = (uint8_t *)((uint32_t)BaseData_ARR|0x80000000);
	 memset(BaseData_ARR,0,164*9);
	#ifdef  SYSUARTPRINTF  
		sysprintf("BaseData_ARR mallco=0x%x\r\n",BaseData_ARR);
		#endif
	
	//sysprintf("BaseData_ARR=%X\r\n",BaseData_ARR);
	writeTextBuff= (char *)((uint32_t )malloc((0x2000+1)+64));
	writeTextBuff= (char *)(shift_pointer((uint32_t)writeTextBuff,32)+32);
	writeTextBuff= (char *)((uint32_t)writeTextBuff|0x80000000);
	memset(writeTextBuff,0,0x2000);
		#ifdef  SYSUARTPRINTF  
		sysprintf("writeTextBuff mallco=0x%x\r\n",writeTextBuff);
		#endif
	
	logodata_basedata_BUFFER = (uint32_t )malloc((0x20000)+64);
	//logodata_sdrambuffer_addr_arry_bak[0]=logodata_basedata_BUFFER;
	logodata_basedata_BUFFER = 32+shift_pointer(logodata_basedata_BUFFER,32);
	logodata_basedata_BUFFER = logodata_basedata_BUFFER|0X80000000;
	logodata_sdrambuffer_addr_arry[0]=logodata_basedata_BUFFER;
	
	RX_Test=(uint8_t  *)(uint32_t )(malloc((2048)+64));
	 RX_Test = (uint8_t  *)(uint32_t )((32+shift_pointer((uint32_t)RX_Test,32))|0x80000000);
	 memset(RX_Test,0,2048);
	 	#ifdef  SYSUARTPRINTF  
		sysprintf("RX_Test mallco=0x%x\r\n",RX_Test);
		#endif
	 
	 TX_Test=(uint8_t  *)(uint32_t )(malloc((1024)+64));
	 TX_Test = (uint8_t  *)(uint32_t )((32+shift_pointer((uint32_t)TX_Test,32))|0x80000000);
	 memset(TX_Test,0,1024);
	 	#ifdef  SYSUARTPRINTF  
		sysprintf("TX_Test mallco=0x%x\r\n",TX_Test);
		#endif
	 
	 UART2_RX_BUFF = (uint8_t  *)(uint32_t )(malloc((RX_BUFF_MAX)+64));
	 UART2_RX_BUFF = (uint8_t  *)(uint32_t )((32+shift_pointer((uint32_t)UART2_RX_BUFF,32))|0x80000000);
	 	memset(UART2_RX_BUFF,0,RX_BUFF_MAX);
	 #ifdef  SYSUARTPRINTF  
		sysprintf("UART2_RX_BUFF mallco=0x%x\r\n",UART2_RX_BUFF);
		#endif
	 
	 UART2_TX_BUFF_NEW = (uint8_t  *)(uint32_t )(malloc((TX_BUFF_MAX_UART)+64));
	 UART2_TX_BUFF_NEW = (uint8_t  *)(uint32_t )((32+shift_pointer((uint32_t)UART2_TX_BUFF_NEW,32))|0x80000000);
	 	memset(UART2_TX_BUFF_NEW,0,TX_BUFF_MAX_UART);
	 	#ifdef  SYSUARTPRINTF  
		sysprintf("UART2_TX_BUFF_NEW mallco=0x%x\r\n",UART2_TX_BUFF_NEW);
		#endif
	
	
	NAND_ReadPage(backup_tab_nandflash_start,0,(uint8_t *)badmanage_str->BAD_MANAGE_arr,sizeof(badmanage_str->BAD_MANAGE_arr));
//	Tp_CHECK.DATA_U32 = 0;
//	for(Tp_i=0;Tp_i<(sizeof(badmanage_str->BAD_MANAGE_arr)-4);Tp_i++)
//	{
//		Tp_CHECK.DATA_U32 =Tp_CHECK.DATA_U32 + badmanage_str->BAD_MANAGE_arr[Tp_i];
//	}
	
	//sprintf("badmanage_str->BAD_MANAGE_str.NANDFLASH_USER_INX=%x,read\n\r",badmanage_str->BAD_MANAGE_str.NANDFLASH_USER_INX);
	//NANDFLASH_BADMANAGE_INIT();
	#ifdef  SYSUARTPRINTF 
	sysprintf("READ_PIN_SW1_6=0x%x,badmanage_str->BAD_MANAGE_str.flag=0x%08x\r\n",READ_PIN_SW1_6,badmanage_str->BAD_MANAGE_str.flag);
	#endif
	if((READ_PIN_SW1_6!=SW_ON)&&
		 ((badmanage_str->BAD_MANAGE_str.flag!=BAD_BLOCK_LOCK))&&
	   (READ_WORKMODE==WORK_FUNCTION)) 
	{		
	power_checkreset();
	//HAL_NVIC_SystemReset( );
	while(1);
	
	}
//	if((READ_PIN_SW1_6==SW_ON)&&
//		 ((Tp_CHECK.DATA_U32!=badmanage_str->BAD_MANAGE_str.backup_checksum))&&
//	   (READ_WORKMODE==WORK_FUNCTION))
//	{
//		badmanage_str->BAD_MANAGE_str.ERR_NUMBER=0;
//	}
	
	
	NANDFLASH_TO_SDRAM(bmp_tab_BUFFER,image_tab__nandflash_start,4);
//	NAND_EraseBlock(baseB_data__nandflash_start/64);
//	NAND_EraseBlock(baseA_data__nandflash_start/64);
	NANDFLASH_TO_SDRAM(logodata_basedata_BUFFER ,baseB_data__nandflash_start,1);
   BASEDATA_RAM_INIT();
		#ifdef  SYSUARTPRINTF  
		sysprintf("logodata_basedata_BUFFER mallco=0x%x\r\n",logodata_basedata_BUFFER);
		#endif
	
		
	if(TYPE_PRODUCT==PORDUCT_7INCH)
	{
		
	uint32_t bmp_TAB[3];
	uint16_t Tp_addr;	
		
	Tp_addr = READ_TAB_FROMSDRAM(0,1,bmp_TAB);
	if(Tp_addr==0xffff)
	{
		//û������ͼ
		return;
	}
	if((bmp_TAB[0]!=800)||(bmp_TAB[1]!=480))//��֧�ֵĸ�ʽ
	{
		return;
	}		
	
	nandflash_to_sdram_Q0(Tp_addr,bmp_LCD_BUFFER,bmp_TAB,bmp_layer1_BUFFER);
   }

}
void display_init(void)
{
	 uint8_t *u8FrameBufPtr;
	
	
	
		 // �������Ÿ��õ�LCM
		//GPG6 (CLK), GPG7 (HSYNC)
		outpw(REG_SYS_GPG_MFPL, (inpw(REG_SYS_GPG_MFPL)& ~0xFF000000) | 0x22000000);
		//GPG8 (VSYNC), GPG9 (DEN)
		outpw(REG_SYS_GPG_MFPH, (inpw(REG_SYS_GPG_MFPH)& ~0xFF) | 0x22);
		//GPA0 ~ GPA7 (DATA0~7)
		outpw(REG_SYS_GPA_MFPL, 0x22222222);
		//GPA8 ~ GPA15 (DATA8~15)	
		outpw(REG_SYS_GPA_MFPH, 0x22222222);
		//GPD8~D15 (DATA16~23)
		outpw(REG_SYS_GPD_MFPH, 0x22222222);
			
	  //ʹ�� LCD clock
		//outpw(REG_CLK_DIVCTL1, (inpw(REG_CLK_DIVCTL1) & ~0xff1f) | 0x718);//37.5M	
	  //outpw(REG_CLK_DIVCTL1, (inpw(REG_CLK_DIVCTL1) & ~0xff1f) | 0x818);//33.3M	
	  outpw(REG_CLK_DIVCTL1, (inpw(REG_CLK_DIVCTL1) & ~0xff1f) | 0x918);//30M	
	 // outpw(REG_CLK_DIVCTL1, (inpw(REG_CLK_DIVCTL1) & ~0xff1f) | 0xA18);//27.27M
		//outpw(REG_CLK_DIVCTL1, (inpw(REG_CLK_DIVCTL1) & ~0xff1f) | 0xB18);//25M
		//outpw(REG_CLK_DIVCTL1, (inpw(REG_CLK_DIVCTL1) & ~0xff1f) | 0xC18);//23.07M
	//	outpw(REG_CLK_DIVCTL1, (inpw(REG_CLK_DIVCTL1) & ~0xff1f) | 0xD18);//21.42M
	  //outpw(REG_CLK_DIVCTL1, (inpw(REG_CLK_DIVCTL1) & ~0xff1f) | 0xe18);	//20M	
		//20M UPLLFOUT=300 
		//UCLKOUT=300/1=300  UCLKOut = UPLLFout /(LCD_SDIV + 1).
		//UCLK��ѡΪҺ����ʱ��LCD_srcCLK
		//ECLKlcd = LCD_SrcCLK / (LCD_N + 1).
		//ECLKlcd = 300/15=20M;
		
	 //LCD��ʼ��
	  vpostLCMInit(0);
	 //�������ű���1:1
	  vpostVAScalingCtrl(1, 0, 1, 0, VA_SCALE_INTERPOLATION);
	//���� ���ظ�ʽRGB565
    vpostSetVASrc(VA_SRC_RGB565);
		
		// ��ȡ�Դ��ַָ��
	u8FrameBufPtr = vpostGetFrameBuffer();
	
	 #ifdef DEBUG
  if(u8FrameBufPtr == NULL)
	{
		#ifdef SYSUARTPRINTF
		sysprintf("��ȡ�Դ��ַָ��ʧ��!!\n");
		#endif
	}
	else
	{
		#ifdef SYSUARTPRINTF
		sysprintf("%X\r\n", (void *)u8FrameBufPtr);
		sysprintf("%X\r\n", *(uint32_t *)u8FrameBufPtr);
		#endif
		 //*((uint32_t  *)0x3D7A40) = 0x55555555;
		//memset((void *)u8FrameBufPtr,0x55,800*480*2);
		//return 0;
	}
	#endif
	
	 display_layer_sdram.LCD_CACHE_BUFFER=(uint32_t )u8FrameBufPtr;

	
	display_layer_sdram.LCD_FRAME_BUFFER=(uint32_t )malloc((800 * 480 * 2)+64);
	display_layer_sdram.LCD_FRAME_BUFFER=32+shift_pointer(display_layer_sdram.LCD_FRAME_BUFFER,32);
	
	display_layer_sdram.LCD_FRAME1_BUFFER=(uint32_t )malloc((800 * 480 * 2)+64);
	display_layer_sdram.LCD_FRAME1_BUFFER=32+shift_pointer(display_layer_sdram.LCD_FRAME1_BUFFER,32);
	
	display_layer_sdram.LCD_FRAME2_BUFFER=(uint32_t )malloc((800 * 480 * 2)+64);
	display_layer_sdram.LCD_FRAME2_BUFFER=32+shift_pointer(display_layer_sdram.LCD_FRAME2_BUFFER,32);
	
	display_layer_sdram.LCD_FRAME3_BUFFER=(uint32_t )malloc((800 * 480 * 2)+64);
	display_layer_sdram.LCD_FRAME3_BUFFER=32+shift_pointer(display_layer_sdram.LCD_FRAME3_BUFFER,32);
	
	display_layer_sdram.LCD_CACHE_BUFFER = display_layer_sdram.LCD_CACHE_BUFFER|0x80000000;
	display_layer_sdram.LCD_FRAME_BUFFER = display_layer_sdram.LCD_FRAME_BUFFER|0x80000000;
	display_layer_sdram.LCD_FRAME1_BUFFER = display_layer_sdram.LCD_FRAME1_BUFFER|0x80000000;
	display_layer_sdram.LCD_FRAME2_BUFFER = display_layer_sdram.LCD_FRAME2_BUFFER|0x80000000;
	display_layer_sdram.LCD_FRAME3_BUFFER = display_layer_sdram.LCD_FRAME3_BUFFER|0x80000000;
	#ifdef SYSUARTPRINTF
	sysprintf("LCD_CACHE_BUFFER=%X LCD_FRAME_BUFFER=%X LCD_FRAME1_BUFFER=%X LCD_FRAME2_BUFFER=%X LCD_FRAME3_BUFFER=%X \r\n",
	
	
	display_layer_sdram.LCD_CACHE_BUFFER, display_layer_sdram.LCD_FRAME_BUFFER, 
	display_layer_sdram.LCD_FRAME1_BUFFER, display_layer_sdram.LCD_FRAME2_BUFFER,display_layer_sdram.LCD_FRAME3_BUFFER);
	#endif
	
	memset((void *)display_layer_sdram.LCD_FRAME_BUFFER,0x00,800*480*2);
	memset((void *)display_layer_sdram.LCD_FRAME1_BUFFER,0x00,800*480*2);
	memset((void *)display_layer_sdram.LCD_FRAME2_BUFFER,0x00,800*480*2);
	memset((void *)display_layer_sdram.LCD_FRAME3_BUFFER,0x00,800*480*2);

//	NANDFLASH_TO_SDRAM_RANDOM(display_layer_sdram.LCD_FRAME3_BUFFER,NANDFLASH_USER_INX/2048,0Xbb800,image_tab__nandflash_start-NANDFLASH_USER_INX/2048);
	//memcpy((void *)display_layer_sdram.LCD_FRAME3_BUFFER,(void *)image_bmp4,800*480*2);
	//memcpy((void *)display_layer_sdram.LCD_FRAME1_BUFFER,(void *)image_bmp1,800*480*2);
	//memcpy((void *)display_layer_sdram.LCD_FRAME2_BUFFER,(void *)image_bmp6,800*480*2);
	
	vpostVAStartTrigger();
	
	/////ge2dInit(16, 800, 480, (void *)display_layer_sdram.LCD_CACHE_BUFFER);
	
	//ge2dClearScreen(0x000000);
  ge2dSpriteBlt_Screen(0,0,800,480,(void *)display_layer_sdram.LCD_FRAME_BUFFER,(void *)bmp_LCD_BUFFER);
	//BLACK TO LCD FOREGROUND
	
		//ge2dFill_Solid(0, 0, 800, 480, 0xffff00);
	//memset((void *)display_layer_sdram.LCD_CACHE_BUFFER,0x11,800*480*2);

	
}

INT32 BattreyConvCallback(UINT32 status, UINT32 userData)
{
	uint32_t Tp_data,Tp_data1;
	Tp_data =inpw(REG_ADC_VBADATA) ;
	Tp_data1=inpw(REG_ADC_DATA);
	sysprintf("VBAT_Callback,Tp_data=%X,%X\n\r",Tp_data,Tp_data1);
	
	return 0;
}

void AD_init(void)
{
	uint32_t Tp_data;
	uint8_t complete;
	//adcOpen();
//	REG_OPERATE(REG_CLK_PCLKEN1, (1<<24),set);
//	REG_OPERATE(REG_ADC_CTL,ADC_CTL_ADEN|ADC_CTL_VBGEN,set);
//	REG_OPERATE(REG_ADC_CONF,ADC_CONF_VBATEN,set);
//	REG_OPERATE(REG_ADC_CONF,ADC_CONF_NACEN,set);
//	REG_OPERATE(REG_ADC_CONF,(1<<3)|(1<<4)|(1<<5),clear);
//	REG_OPERATE(REG_ADC_CONF,(1<<6)|(1<<7),set);
//	REG_OPERATE(REG_ADC_CONF,ADC_CONF_HSPEED,set);
//	REG_OPERATE(REG_ADC_ISR,ADC_ISR_MF|ADC_ISR_NACF,set);
//	REG_OPERATE(REG_ADC_IER,ADC_IER_MIEN,set);
//	
//	do{
//complete = 0;
//REG_OPERATE(REG_ADC_CTL,ADC_CTL_MST,set);
//sysprintf("Waiting for Normal mode Interrupt\r\n");
//while((inpw(REG_ADC_ISR) & ADC_ISR_MF)==0);
////rREG_ISR = ADC_ISR_MF; //Clear MF flag
//		REG_OPERATE(REG_ADC_ISR,ADC_ISR_MF,set);
////while((inpw(REG_ADC_ISR) & ADC_ISR_NACF)==0);
////if(inpw(REG_ADC_ISR) & ADC_ISR_NACF)
//{

//	Tp_data=inpw(REG_ADC_DATA);
//	REG_OPERATE(REG_ADC_ISR,ADC_ISR_NACF,set);
////n=(33*data*100)>>12;
////d1=n/1000;
////d2=n%1000;
//sysprintf("DATA=0x%08x\r\n",Tp_data);
//}
////else
////sysprintf("interrupt error\r\n");
//}while(1);
//adcOpen();
	//REG_OPERATE(REG_CLK_PCLKEN1, (1<<24),set);
	//adcIoctl(VBPOWER_ON,0,0);
	
	//adcIoctl(VBAT_ON,(UINT32)BattreyConvCallback,0);
	//REG_OPERATE(REG_ADC_CONF,(1<<6)|(1<<7),set);
	//adcChangeChannel(AIN0);
	//adcIoctl(START_MST,0,0);
	
	adcOpen();
	
	REG_OPERATE(REG_ADC_CONF,ADC_CONF_VBATEN,set);
//	REG_OPERATE(REG_ADC_CONF,ADC_CONF_HSPEED,set);
	REG_OPERATE(REG_ADC_CONF,(1<<6)|(1<<7),clear);//vref
	REG_OPERATE(REG_ADC_CONF,(1<<3)|(1<<4)|(1<<5),clear);//000-bat
	REG_OPERATE(REG_ADC_CONF,(1<<16)|(1<<17)|(1<<18)|(1<<19),set);
	REG_OPERATE(REG_ADC_CTL,ADC_CTL_VBGEN,set);
	
	////////////////////REG_OPERATE(REG_ADC_CONF,ADC_CONF_SELFTEN,set);
	/////////REG_OPERATE(REG_ADC_CONF,ADC_CONF_NACEN,set);
	REG_OPERATE(REG_ADC_ISR,ADC_ISR_VBF/*ADC_ISR_MF|ADC_ISR_VBF|ADC_ISR_SELFTF|ADC_ISR_NACF*/,set);
	
	/* Enable ADC Power */
    outpw(REG_ADC_CTL, ADC_CTL_ADEN);
	
		
	//outpw(REG_ADC_CONF,ADC_CONF_VBATEN);
	///////////////REG_OPERATE(REG_ADC_IER,ADC_IER_MIEN,set);
#ifdef SYSUARTPRINTF
sysprintf("AD INIT END\n\r");
#endif
	do
	{
		REG_OPERATE(REG_ADC_CTL,ADC_CTL_MST,set);
				while((inpw(REG_ADC_ISR)&ADC_ISR_MF)==0);
		//REG_OPERATE(REG_ADC_ISR,ADC_ISR_MF,set);
		outpw(REG_ADC_ISR,ADC_ISR_MF);
		delay_ms(1000);
      //while((inpw(REG_ADC_ISR)&ADC_ISR_VBF)==0);
//		//{
		if(inpw(REG_ADC_ISR)&ADC_ISR_VBF)
		{
			Tp_data =inpw(REG_ADC_VBADATA) ;
			//REG_OPERATE(REG_ADC_ISR,ADC_ISR_VBF,set);
			outpw(REG_ADC_ISR,ADC_ISR_VBF);
			sysprintf("Tp_data=%x\n\r",Tp_data);
				
		}
		if(inpw(REG_ADC_ISR)&ADC_ISR_SELFTF)
		{
			Tp_data =inpw(REG_ADC_SELFDATA);
			outpw(REG_ADC_ISR,ADC_ISR_SELFTF);
			sysprintf("Tp_data_self=%x\n\r",Tp_data);
		}
		if(inpw(REG_ADC_ISR)&ADC_ISR_NACF)
		{
			Tp_data =inpw(REG_ADC_DATA);
			outpw(REG_ADC_ISR,ADC_ISR_NACF);
			sysprintf("Tp_data_normal=%x\n\r",Tp_data);
		}
	}
	while(1);
}

void rtc_time_deinit(void)
{
	    pwr_on_time_ground.u32Year = RTC_YEAR2000;
	    pwr_on_time_ground.u32cMonth = 1;
	    pwr_on_time_ground.u32cDay=1;
			pwr_on_time_ground.u32cHour = 0;
			pwr_on_time_ground.u32cMinute = 0;
			pwr_on_time_ground.u32cSecond = 0;
	    RTC_Write(RTC_CURRENT_TIME,&pwr_on_time_ground);
	
	    RTC_Read(RTC_CURRENT_TIME,&pwr_on_time_ground);
}

void rtc_init(void)
{
	//ʹ��RECʱ��
	
	
    RTC_EnableClock(TRUE);
    //RTC ��ʼ��
    RTC_Init();
	
	  RTC_Read(RTC_CURRENT_TIME,&pwr_on_time_ground);
			
			#ifdef  SYSUARTPRINTF  
				sysprintf("SYS_TIME=%d,%d,%d,%d,%d,%d\r\n",pwr_on_time_ground.u32Year,pwr_on_time_ground.u32cMonth,pwr_on_time_ground.u32cDay,pwr_on_time_ground.u32cHour
			,pwr_on_time_ground.u32cMinute,pwr_on_time_ground.u32cSecond);
			#endif
			
		///RTC_ground_to_app(&pwr_on_time_ground,&pwr_on_time_app);
	  if((pwr_on_time_ground.u32Year==0x7d5)&&(pwr_on_time_ground.u32cMonth==1)&&(pwr_on_time_ground.u32cDay==1)&&
			(pwr_on_time_ground.u32cHour == 0)&&(pwr_on_time_ground.u32cMinute==0)&&(pwr_on_time_ground.u32cSecond==0))
		{
			rtc_time_deinit();
		}
		
}




void init_gpio(void)
{
	 //LED INIT pi5 pi6 pi7 pi8
	//
	   
	  REG_OPERATE(REG_SYS_GPI_MFPH,0XF,clear);
	  REG_OPERATE(REG_SYS_GPI_MFPL,0XFFF00000,clear);
    GPIO_OpenBit(GPIOI,BIT5|BIT6|BIT7|BIT8, DIR_OUTPUT, PULL_UP);
	  if(inpw(REG_SYS_RSTSTS)&0x20)
		{
	  LED_POWER_OFF();
		LED_FLASHERR_OFF();
	  LED_POWERLOW_ON();
	  LED_LOGO_OFF( );
	  }
		else
		{
	  LED_POWER_ON();
		LED_FLASHERR_OFF();
	  LED_POWERLOW_OFF();
	  LED_LOGO_OFF( );//COMMUNICATION_LED_ON;
		}
	  //SW 1-6
    //pe10 11 12 13
    //ph 2 3
    REG_OPERATE(REG_SYS_GPE_MFPH,0X00FFFF00,clear);
	  GPIO_OpenBit(GPIOE,BIT10|BIT11|BIT12|BIT13, DIR_INPUT, PULL_UP);
	  REG_OPERATE(REG_SYS_GPH_MFPL,0X0000FF00,clear);
	  GPIO_OpenBit(GPIOH,BIT2|BIT3, DIR_INPUT, PULL_UP);
	   
	  //POWER 5V
		//PF12
		REG_OPERATE(REG_SYS_GPF_MFPH,0X000F0000,clear);
		GPIO_OpenBit(GPIOF,BIT12, DIR_OUTPUT, PULL_UP);
	
	  
	   //POWERLCD  -PB4
		 REG_OPERATE(REG_SYS_GPB_MFPL,0X000F0000,clear);
		 GPIO_OpenBit(GPIOB,BIT4, DIR_OUTPUT, PULL_UP);
		 //GPIO_Set(GPIOB, BIT4);
		 LCD_PWR_ON();
		 
		 
		 //PB0 PB1 lcd reverse
		 REG_OPERATE(REG_SYS_GPB_MFPL,0X000000FF,clear);
		 GPIO_OpenBit(GPIOB,BIT0|BIT1, DIR_OUTPUT, PULL_UP);
		 
		 
		 //PI13 AMP power
		  REG_OPERATE(REG_SYS_GPI_MFPH,0XF00000,clear);
			GPIO_OpenBit(GPIOI,BIT13,DIR_OUTPUT,NO_PULL_UP);
			AMP_POWER_ON;
			
			//pi11  AMP DATA
			REG_OPERATE(REG_SYS_GPI_MFPH,0XF000,clear);
			GPIO_OpenBit(GPIOI,BIT11,DIR_OUTPUT,PULL_UP);
			 AMP_DATA_0;
	 
	     AMP_DATA_1;
			 
			 
			//PD7 1-INCH5 0-INCH7
			REG_OPERATE(REG_SYS_GPD_MFPL,0XF0000000,clear);
			GPIO_OpenBit(GPIOD,BIT7,DIR_INPUT,PULL_UP);
			#ifdef  SYSUARTPRINTF 
			if(READ_PRODUCTTYPE==PORDUCT_7INCH)
			{
				sysprintf("product is inch7\r\n");
			}
			else if(READ_PRODUCTTYPE==PORDUCT_5INCH)
			{
				sysprintf("product is inch5\r\n");
			}
			#endif
			
			//PB12 1-WORK 0-TEST
			REG_OPERATE(REG_SYS_GPB_MFPH,0X000F0000,clear);
			GPIO_OpenBit(GPIOB,BIT12,DIR_INPUT,PULL_UP);
			
			
			
		 //pi14 AMP SD  new sound ic delete
		 REG_OPERATE(REG_SYS_GPI_MFPH,0XF000000,clear);
			GPIO_OpenBit(GPIOI,BIT14,DIR_OUTPUT,NO_PULL_UP);
			AUDIO_AMPLIFIER_SHUT_DOWN	;
			
			//pi12 AMP BUSY
			 REG_OPERATE(REG_SYS_GPI_MFPH,0XF0000,clear);
			GPIO_OpenBit(GPIOI,BIT12,DIR_INPUT,NO_PULL_UP);
			
			GPIO_EnableTriggerType(GPIOI, BIT12,BOTH_EDGE);
	    GPIO_EnableInt(GPIOI, (GPIO_CALLBACK)GPIOICallback, 0);
		 
		 
		 //PF13 PF14 tool reset
		 download_gpio_set_input( );
		 
		  #ifdef DEBUG_PG3 
		 REG_OPERATE(REG_SYS_GPG_MFPL,0XF000,clear);
			GPIO_OpenBit(GPIOG,BIT3,DIR_OUTPUT,PULL_UP);
		 GPIO_Set(GPIOG,BIT3);
		 #endif
		 
//    GPIO_OpenBit(GPIOB,BIT5, DIR_OUTPUT, PULL_UP);
//    //����LED0��LED1
//	  GPIO_Clr(GPIOB, BIT4|BIT5);
//	  sysprintf("����LED0��LED1....\r\n");
//	  delay_ms(2000);
//	  //�ر�LED0��LED1
//	  GPIO_Set(GPIOB,BIT4|BIT5);
//		sysprintf("�ر�LED0��LED1....\r\n");
//	  delay_ms(2000);
//		
//		//LED0��LED1������˸
//		sysprintf("LED0��LED1������˸....CYW\r\n");
}

void Touch_tim_init(void)
{
	REG_OPERATE(REG_CLK_PCLKEN0,1<<6,set);//TIM2 CLK ENABLE
	ETIMER_Open(2,ETIMER_PERIODIC_MODE,20);//50ms
	
	ETIMER_EnableInt(2);
	sysInstallISR(LOW_LEVEL_SENSITIVE | IRQ_LEVEL_2, ETMR2_IRQn, (PVOID)ETMR2_IRQHandler);
  sysSetLocalInterrupt(ENABLE_IRQ);
  sysEnableInterrupt(ETMR2_IRQn);
	
	
 //  ETIMER_Start(2);
    
}
void Touch_tim_start(void)
{
	ETIMER_Start(2);
}
void Touch_tim_stop(void)
{
	 ETIMER_Stop(2);
}

void voice_tim_start(uint32_t x_timer)
{
	//???????'
    //ETIMER_Start(2);
//	float Tp_data=0.0;
//	
//	sysprintf("ETIMER_Start-3\r\n");
	ETIMER_Stop(3);
	//////////REG_OPERATE(REG_CLK_PCLKEN0,1<<7,clear);
//	
//	Tp_data = x_timer/1000000.0;
//	Tp_data = 1.0/Tp_data;
//	sysprintf("Etimer3 enable Tp_data=%d\n\r",(UINT)Tp_data);
//	
	///////////	REG_OPERATE(REG_CLK_PCLKEN0,1<<7,set);//TIM2 CLK ENABLE
	ETIMER_Open(3,ETIMER_ONESHOT_MODE,15);//5ms
	//outpw(REG_ETMR2_PRECNT,200);  //??
	outpw(REG_ETMR3_CMPR, x_timer*12);
//	
//
	///////ETIMER_EnableInt(3);
	///////sysInstallISR(HIGH_LEVEL_SENSITIVE | IRQ_LEVEL_1, ETMR3_IRQn, (PVOID)ETMR3_IRQHandler);
  ///////sysSetLocalInterrupt(ENABLE_IRQ);
  /////////sysEnableInterrupt(ETMR3_IRQn);
//	
//	
   ETIMER_Start(3);
   
//  sysEnableInterrupt(ETMR3_IRQn);
}

void voice_tim_stop(void)
{
	  // sysprintf("ETIMER_Stop-3\r\n");
	ETIMER_Stop(3);
	//sysDisableInterrupt(ETMR3_IRQn);
	
}

void voice_tim_init(void)
{
		REG_OPERATE(REG_CLK_PCLKEN0,1<<7,set);//TIM2 CLK ENABLE
	ETIMER_Open(3,ETIMER_ONESHOT_MODE,15);//5ms
	/////////////////////////////////////////outpw(REG_ETMR2_PRECNT,200);//?????????
	 
	ETIMER_EnableInt(3);
	sysInstallISR(FIQ_LEVEL_0, ETMR3_IRQn, (PVOID)ETMR3_IRQHandler);
 
	sysSetLocalInterrupt(ENABLE_FIQ_IRQ);
  sysEnableInterrupt(ETMR3_IRQn);
	ETIMER_ClearIntFlag(3);
	
  // ETIMER_Start(3);
}


void power_int_init(void)
{

	GPIO_EnableTriggerType(GPIOH, BIT5,FALLING);
	GPIO_EnableInt(GPIOH, (GPIO_CALLBACK)GPIOHCallback, 0);
}
void power_int_disable(void)
{
	GPIO_DisableTriggerType(GPIOH, BIT5);
}

	
void power_checkon(void)
{
		REG_OPERATE(REG_SYS_GPH_MFPL,0X00F00000,clear);
	 GPIO_OpenBit(GPIOH,BIT5, DIR_INPUT, PULL_UP);
	while(GPIO_ReadBit(GPIOH,BIT5)==0);
	
}

void power_checkoff(void)
{
		REG_OPERATE(REG_SYS_GPH_MFPL,0X00F00000,clear);
	 GPIO_OpenBit(GPIOH,BIT5, DIR_INPUT, PULL_UP);
	while(GPIO_ReadBit(GPIOH,BIT5)==1);
	
}

void power_checkreset(void)
{
	// REG_OPERATE(REG_SYS_APBIPRST0,1<<3,set);
	//REG_OPERATE(REG_SYS_APBIPRST0,1<<3,clear);
		//REG_OPERATE(REG_SYS_GPH_MFPL,0X00F00000,clear);
	 //GPIO_OpenBit(GPIOH,BIT5, DIR_INPUT, PULL_UP);
//	  LED_POWERLOW_OFF();
//	LVD_disable();
////////////////	if(GPIO_ReadBit(GPIOH,BIT5)==0)
////////////////	{
////////////////	//	init_gpio();
////////////////	}
////////////////	while(1)
////////////////	{
////////////////	if(GPIO_ReadBit(GPIOH,BIT5)==1)
////////////////	{
////////////////	  	REG_OPERATE(REG_CLK_PCLKEN0,1<<1,set);//ENABLE WWDT
////////////////			WWDT_Open(WWDT_PRESCALER_2048,0xf,TRUE);
////////////////		//asm("call Reset_Go");
////////////////		 while(1);
////////////////	}
////////////////  }
	//LVD_disable();
////////////////////////	AD_init_8V();
////////////////////////	
////////////////////////	while(1)
////////////////////////	{
////////////////////////		if((GPIO_ReadBit(GPIOH,BIT5)==1)&&(get_main_pwr_ad_value()>VOLT_WORK))
////////////////////////		{
////////////////////////		if(FLAG_WDT==0)
////////////////////////		{
////////////////////////			#ifdef  SYSUARTPRINTF 
////////////////////////			sysprintf("wdt on\n\r");
////////////////////////			#endif
////////////////////////		FLAG_WDT = 1;	
////////////////////////		REG_OPERATE(REG_CLK_PCLKEN0,1<<1,set);//ENABLE WWDT
////////////////////////		WWDT_Open(WWDT_PRESCALER_2048,0xf,TRUE);
////////////////////////		}
////////////////////////		}
////////////////////////		else
////////////////////////		{
////////////////////////			
////////////////////////		}
////////////////////////		
////////////////////////	}
    REG_OPERATE(REG_CLK_PCLKEN0,1<<1,set);//ENABLE WWDT
    WWDT_Open(WWDT_PRESCALER_2048,0x1f,TRUE);//Լ1.35S��λ
		//outpw(REG_WWDT_RLDCNT,0x5AA5);
}