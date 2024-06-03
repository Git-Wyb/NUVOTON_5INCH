#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "gpio.h"
#include "sys.h"
#include "W25Q128.h"
#include "BSP_Init.h"
#include "spi.h"
#include "Command_all.h"
#include "Aprotocol.h"



#define CS_LOW        0
#define CS_HIGH       1


// SPI Flash Commands
#define CMD_RD        0x0B                          // High-Speed Read
#define CMD_SE        0x20                          //  4-KByte Sector Erase
#define CMD_BE        0xC7                          // Chip Erase
#define CMD_WR        0x02                          // Program One Data Byte
#define CMD_RDSR      0x05                          // Read Status Register
#define CMD_EWSR      0x50                          // Enable Write Status Register
#define CMD_WRSR      0x01                          // Write Status Register
#define CMD_WREN      0x06                          // Write Enable
#define CMD_WRDI      0x04                          // Write Disable
#define CMD_RDID      0x9F                          // JEDEC ID read

extern BADMANAGE_TAB_TYPE_U badmanage_str[1];
extern  uint8_t *BaseData_ARR;
extern uint8_t Field_unit;
extern uint32_t logodata_sdrambuffer_addr_arry[16];
  TAB_Checksum_SPI_U Dataclass1_U;
extern  LAYER_SDRAM_STR display_layer_sdram;
 
 void DATACLASS1_Check_Write(void);
 void DATACLASS1_Check_Read(void);
 
uint16_t SpiFlash_ReadMidDid(void)
{
    uint8_t u8RxData[2];

    // /CS: 使能 CS
    spiIoctl(0, SPI_IOC_ENABLE_SS, SPI_SS_SS0, 0);

    // 发送命令: 0x90, 读取Manufacturer/Device ID
    spiWrite(0, 0, 0x90);
    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
    while(spiGetBusyStatus(0));
    
    // 发送 24-bit '0'
    spiWrite(0, 0, 0x00);
    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
    while(spiGetBusyStatus(0));
    
    spiWrite(0, 0, 0x00);
    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
    while(spiGetBusyStatus(0));
    
    spiWrite(0, 0, 0x00);
    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
    while(spiGetBusyStatus(0));
 
    // 接收16 位数据
    spiWrite(0, 0, 0x00);
    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
    while(spiGetBusyStatus(0));
    u8RxData[0] = spiRead(0, 0);
    
    spiWrite(0, 0, 0x00);
    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
    while(spiGetBusyStatus(0));
    u8RxData[1] = spiRead(0, 0);
        
    // /CS: 失能CS
    spiIoctl(0, SPI_IOC_DISABLE_SS, SPI_SS_SS0, 0);

    return ( (u8RxData[0]<<8) | u8RxData[1] );
}


void W25Q128_init(void)
{
	uint16_t u16ID;
	
	/* 配置相关引脚到 SPI0 */
		REG_OPERATE(REG_SYS_GPB_MFPL,0xff000000,clear);
	REG_OPERATE(REG_SYS_GPB_MFPL,0xbb000000,set);
//	
	REG_OPERATE(REG_SYS_GPB_MFPH,0x00000FFF,clear);
	REG_OPERATE(REG_SYS_GPB_MFPH,0x00000BBB,set);
	
		spiInit(0);
		spiOpen(0);
    // 设置SPI时钟为2MHz
    spiIoctl(0, SPI_IOC_SET_SPEED, 2000000, 0);
    // 设置传输长度为8位
    spiIoctl(0, SPI_IOC_SET_TX_BITLEN, 8, 0);
    // 设置传输模式0
    spiIoctl(0, SPI_IOC_SET_MODE, 0, 0);
	/***********FLASH操作***********/
    // 核对flash ID
////////////////    if((u16ID = SpiFlash_ReadMidDid()) != 0xEF17) {
////////////////        sysprintf("ID错误, 0x%x \r\n", u16ID);
////////////////        //return -1;
////////////////    } else
////////////////        sysprintf("发现SPI FLASH: W25Q128BV \r\n");
////////////////	
	
//	uint8_t Tp_id[4];
//	
//	REG_OPERATE(REG_SYS_GPB_MFPL,0xff000000,clear);
//	REG_OPERATE(REG_SYS_GPB_MFPL,0xbb000000,set);
//	
//	REG_OPERATE(REG_SYS_GPB_MFPH,0x00000FFF,clear);
//	REG_OPERATE(REG_SYS_GPB_MFPH,0x00000BBB,set);
//	
//	REG_OPERATE(REG_CLK_PCLKEN1,0X10,set);
//	
//	REG_OPERATE(REG_SYS_APBIPRST1,0X10,set);
//	REG_OPERATE(REG_SYS_APBIPRST1,0X10,clear);
//	
//	REG_OPERATE(REG_SPI1_DIVIDER,0XFFFF,clear);
//	REG_OPERATE(REG_SPI1_DIVIDER,75,set);//75M/75=1M
//	
//	outpw(REG_SPI1_CNTRL,(8UL       <<  3)|(1UL       <<  2)|(1UL       <<  1));
//	
//	REG_OPERATE(REG_SPI1_SSR,5,set);//CS is inactive (High)
//	
//	#ifdef SYSUARTPRINTF
//	SPI_FLASH_ReadID(Tp_id,4);
//	sysprintf("Tp_id[0]=0x%x\r\n",Tp_id[0]);
//	sysprintf("Tp_id[1]=0x%x\r\n",Tp_id[1]);
//	sysprintf("Tp_id[2]=0x%x\r\n",Tp_id[2]);
//	sysprintf("Tp_id[3]=0x%x\r\n",Tp_id[3]);
//	#endif
	spiIoctl(0, SPI_IOC_DISABLE_SS, SPI_SS_SS0, 0);
	 
}
/*
???:SpiFlash_WaitReady(void)
??:??flash????
??:	 				?
???:	u8Status:flash??
*/
uint8_t SpiFlash_ReadStatusReg(void)
{
    uint8_t u8Status;
    
    // /CS: ??
    spiIoctl(0, SPI_IOC_ENABLE_SS, SPI_SS_SS0, 0);

    // ????: 0x05, ???????
    spiWrite(0, 0, 0x05);
    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
    while(spiGetBusyStatus(0))
			;

    // ????
     spiWrite(0, 0, 0x00);
    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
    while(spiGetBusyStatus(0));
    u8Status = spiRead(0, 0);
    
    // /CS: ??
    spiIoctl(0, SPI_IOC_DISABLE_SS, SPI_SS_SS0, 0);
    
    return u8Status;
}
/*
???:SpiFlash_WaitReady(void)
??:??flash????
??:	 				?
???:				?
*/
void SpiFlash_WaitReady(void)
{
    uint8_t ReturnValue;

    do {
        ReturnValue = SpiFlash_ReadStatusReg();
        ReturnValue = ReturnValue & 1;
    } while(ReturnValue!=0); 
}



/*
???:SpiFlash_ChipErase(void)
??:??SpiFlash
??:					?
???:				?
*/
void SpiFlash_SectorErase(uint32_t StartAddress)
{
    // /CS: ??
    spiIoctl(0, SPI_IOC_ENABLE_SS, SPI_SS_SS0, 0);

    // ????: 0x06, ???
    spiWrite(0, 0, 0x06);
    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
    while(spiGetBusyStatus(0));
    // /CS:??
    spiIoctl(0, SPI_IOC_DISABLE_SS, SPI_SS_SS0, 0);
    // /CS: ??
    spiIoctl(0, SPI_IOC_ENABLE_SS, SPI_SS_SS0, 0);
    // ????: 0xC7,????
    // 发送命令: 0x20, sector擦除
    spiWrite(0, 0, 0x20);
    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
    while(spiGetBusyStatus(0));
	  
	  // 发送 24-bit 首地址
    spiWrite(0, 0, (StartAddress>>16) & 0xFF);
    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
    while(spiGetBusyStatus(0));
    
    spiWrite(0, 0, (StartAddress>>8) & 0xFF);
    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
    while(spiGetBusyStatus(0));
    
    spiWrite(0, 0, StartAddress & 0xFF);
    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
    while(spiGetBusyStatus(0));
    // /CS:??
    spiIoctl(0, SPI_IOC_DISABLE_SS, SPI_SS_SS0, 0);
		
		SpiFlash_WaitReady();
}


/*
函数名：SpiFlash_NormalPageProgram(uint32_t StartAddress, uint8_t *u8DataBuffer)
功能：写入SpiFlash数据
参数：	
			StartAddress：要写入数据的首地址 为256的倍数
			u8DataBuffer：写入数据存入指针
返回值：				无
*/


void SpiFlash_NormalPageProgram(uint32_t StartAddress, uint8_t *u8DataBuffer,uint16_t x_len)
{
    uint32_t i = 0;
    
    // /CS: 使能
    spiIoctl(0, SPI_IOC_ENABLE_SS, SPI_SS_SS0, 0);

    // 发送指令: 0x06, 写使能
    spiWrite(0, 0, 0x06);
    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
    while(spiGetBusyStatus(0));

    // /CS: 使能
    spiIoctl(0, SPI_IOC_DISABLE_SS, SPI_SS_SS0, 0);
    // /CS: 使能
    spiIoctl(0, SPI_IOC_ENABLE_SS, SPI_SS_SS0, 0);
    
//	// 发送命令: 0x52, block擦除
//    spiWrite(0, 0, 0x52);
//    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
//    while(spiGetBusyStatus(0));
//	  
//	  // 发送 24-bit 首地址
//    spiWrite(0, 0, (0xff0000>>16) & 0xFF);
//    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
//    while(spiGetBusyStatus(0));
//    
//    spiWrite(0, 0, (0xff0000>>8) & 0xFF);
//    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
//    while(spiGetBusyStatus(0));
//    
//    spiWrite(0, 0, 0xff0000 & 0xFF);
//    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
//    while(spiGetBusyStatus(0));
//	
	  
   
	
	
	  // 发送命令: 0x02, 页写入
    spiWrite(0, 0, 0x02);
    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
    while(spiGetBusyStatus(0));

    // 发送 24-bit 首地址
    spiWrite(0, 0, (StartAddress>>16) & 0xFF);
    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
    while(spiGetBusyStatus(0));
    
    spiWrite(0, 0, (StartAddress>>8) & 0xFF);
    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
    while(spiGetBusyStatus(0));
    
    spiWrite(0, 0, StartAddress & 0xFF);
    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
    while(spiGetBusyStatus(0));
    
    // 写数据
    for(i=0;i<x_len;i++)
    {
        spiWrite(0, 0, u8DataBuffer[i]);
        spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
        while(spiGetBusyStatus(0));
    }
    // /CS: 失能
    spiIoctl(0, SPI_IOC_DISABLE_SS, SPI_SS_SS0, 0);
		
		SpiFlash_WaitReady();
}

void SpiFlash_NormalPageProgram_16(uint32_t StartAddress, uint8_t *u8DataBuffer,uint16_t x_len)
{
	uint16_t Tp_i,Tp_long;
	if(StartAddress%0x1000!=0)  return;
	if(x_len>4096)             return;
	
	
	SpiFlash_SectorErase(StartAddress);
	
	 if(x_len>256) Tp_long=256;
	 else Tp_long = x_len;
	 
	 while(Tp_long)
	 {
		 SpiFlash_NormalPageProgram(StartAddress,u8DataBuffer,Tp_long);
		
		 x_len = x_len - Tp_long;
		 
		 if(x_len!=0)
		 {
			  u8DataBuffer = u8DataBuffer + Tp_long;
		    StartAddress = StartAddress + Tp_long;
		 }
		 
		 if(x_len>256) Tp_long=256;
	   else Tp_long = x_len;
	 }
}

void SpiFlash_NormalPageProgram_32(uint32_t StartAddress, uint8_t *u8DataBuffer,uint32_t x_len)
{
	uint16_t Tp_i,Tp_long;
	if(StartAddress%0x10000!=0)  return;
	if(x_len!=131072)             return;
	
	 for(Tp_i=0;Tp_i<32;Tp_i++)
	{
		
		SpiFlash_NormalPageProgram_16(StartAddress,u8DataBuffer,0x1000);
		if(Tp_i!=31)
		{
		StartAddress = StartAddress+0x1000;
		u8DataBuffer = u8DataBuffer + 0x1000;
		}
	}
}

/*
函数名：SpiFlash_NormalRead(uint32_t StartAddress, uint8_t *u8DataBuffer)
功能：读取SpiFlash数据
参数：	
			StartAddress：要读取数据的首地址 为256的倍数
			u8DataBuffer：读取数据存入指针
返回值：				无
*/
void SpiFlash_NormalRead(uint32_t StartAddress, uint8_t *u8DataBuffer,uint16_t x_len)
{
    uint32_t i;

    // /CS: 使能
    spiIoctl(0, SPI_IOC_ENABLE_SS, SPI_SS_SS0, 0);

    // 发送命令: 0x03, 读使能
    spiWrite(0, 0, 0x03);
    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
    while(spiGetBusyStatus(0));

    // 发送24-bit初始地址
    spiWrite(0, 0, (StartAddress>>16) & 0xFF);
    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
    while(spiGetBusyStatus(0));
    
    spiWrite(0, 0, (StartAddress>>8) & 0xFF);
    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
    while(spiGetBusyStatus(0));
    
    spiWrite(0, 0, StartAddress & 0xFF);
    spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
    while(spiGetBusyStatus(0));

   // delay_ms(1);
     
    // 数据读取
    for(i=0; i<x_len; i++) {
        spiWrite(0, 0, 0x00);
        spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
        while(spiGetBusyStatus(0));
        u8DataBuffer[i] = spiRead(0, 0);  
    }
		
		SpiFlash_WaitReady();
}

void SpiFlash_NormalRead_16(uint32_t StartAddress, uint8_t *u8DataBuffer,uint16_t x_len)
{
		uint16_t Tp_i,Tp_long;
	if(StartAddress%0x1000!=0)  return;
	if(x_len>4096)             return;
	
	 if(x_len>256) Tp_long=256;
	 else Tp_long = x_len;
	 
	 while(Tp_long)
	 {
		 SpiFlash_NormalRead(StartAddress,u8DataBuffer,Tp_long);
		 
		 x_len = x_len - Tp_long;
		 
		 if(x_len!=0)
		 {
			 StartAddress = StartAddress + Tp_long;
		   u8DataBuffer = u8DataBuffer + Tp_long;
		 }
		 
		 if(x_len>256) Tp_long=256;
	   else Tp_long = x_len;
	}
}

void SpiFlash_NormalRead_32(uint32_t StartAddress, uint8_t *u8DataBuffer,uint32_t x_len)
{
	uint16_t Tp_i,Tp_long;
	if(StartAddress%0x10000!=0)  return;
	if(x_len!=131072)             return;
	
	 for(Tp_i=0;Tp_i<32;Tp_i++)
	{
		
		SpiFlash_NormalRead_16(StartAddress,u8DataBuffer,0x1000);
		if(Tp_i!=31)
		{
		StartAddress = StartAddress+0x1000;
		u8DataBuffer = u8DataBuffer + 0x1000;
		}
	}
}


void W25Q128_Write(access_TYPE_E x_type)
{
	uint8_t Tp_data[36]={0};
	
	switch(x_type)
	{
		case accsee_BASEDATA_PARA_5INCH: 
			memcpy((void *)Tp_data,(void *)(BaseData_ARR+3*9),9);
	    memcpy((void *)(Tp_data+9),(void *)(BaseData_ARR+4*9),9);
	    memcpy((void *)(Tp_data+18),(void *)(BaseData_ARR+12*9),9);
	    memcpy((void *)(Tp_data+27),(void *)(BaseData_ARR+13*9),9);
	    SpiFlash_SectorErase(W_block255_sector15);
	    SpiFlash_NormalPageProgram(W_block255_sector15,Tp_data,36);
			break;
		case access_BLOCK_CHECKSUM:
			DATACLASS1_Check_Write();
			break;
		case access_BLOCK_0_BACKUP:
			//SpiFlash_SectorErase(W_block255_sector0);
		  SpiFlash_NormalPageProgram_16(W_block255_sector0,badmanage_str->BAD_MANAGE_arr,sizeof(badmanage_str->BAD_MANAGE_arr));
		  
			break;
		case access_BLOCK_1967_BASEDATA:
			//SpiFlash_SectorErase(W_block253_sector0);
			 SpiFlash_NormalPageProgram_16(W_block253_sector0,(uint8_t *)BaseData_ARR,logodata_Basedata_SIZE);
			break;
		case access_BLOCK_1966_UNIT:
			 //SpiFlash_SectorErase(W_block251_sector0);
		   SpiFlash_NormalPageProgram_16(W_block251_sector0,(uint8_t *)logodata_sdrambuffer_addr_arry[Field_unit],MAX_LOGO_UINT_NUM+2);
			break;
		case access_BLOCK_1532_TAB3:
			SpiFlash_NormalPageProgram_32(W_block249_sector0,(uint8_t *)(bmp_tab_BUFFER+3*131072),131072);
		  break;
		case access_BLOCK_1531_TAB2:
			SpiFlash_NormalPageProgram_32(W_block247_sector0,(uint8_t *)(bmp_tab_BUFFER+2*131072),131072);
		  break;
		case access_BLOCK_1530_TAB1:
			SpiFlash_NormalPageProgram_32(W_block245_sector0,(uint8_t *)(bmp_tab_BUFFER+1*131072),131072);
		  break;
		case access_BLOCK_1529_TAB0:
			SpiFlash_NormalPageProgram_32(W_block243_sector0,(uint8_t *)(bmp_tab_BUFFER),131072);
		  break;
		default:
			break;
	}
	
}

void W25Q128_Read(access_TYPE_E x_type)
{
	uint8_t Tp_data[36]={0};
	
	switch(x_type)
	{
		case accsee_BASEDATA_PARA_5INCH: 
			SpiFlash_NormalRead(W_block255_sector15,Tp_data,36);
	    memcpy((void *)(BaseData_ARR+3*9),(void *)(Tp_data),9);
	    memcpy((void *)(BaseData_ARR+4*9),(void *)(Tp_data+9),9);
	    memcpy((void *)(BaseData_ARR+12*9),(void *)(Tp_data+18),9);
	    memcpy((void *)(BaseData_ARR+13*9),(void *)(Tp_data+27),9);
			break;
		case access_BLOCK_CHECKSUM:
			DATACLASS1_Check_Read();
			break;
		case access_BLOCK_0_BACKUP:
			SpiFlash_NormalRead_16(W_block255_sector0,badmanage_str->BAD_MANAGE_arr,sizeof(badmanage_str->BAD_MANAGE_arr));
			break;
		case access_BLOCK_1967_BASEDATA:
			SpiFlash_NormalRead_16(W_block253_sector0,(uint8_t *)BaseData_ARR,logodata_Basedata_SIZE);
			break;
		case access_BLOCK_1966_UNIT:
			SpiFlash_NormalRead_16(W_block251_sector0,(uint8_t *)logodata_sdrambuffer_addr_arry[Field_unit],MAX_LOGO_UINT_NUM+2);
			break;
		case access_BLOCK_1532_TAB3:
			SpiFlash_NormalRead_32(W_block249_sector0,(uint8_t *)(bmp_tab_BUFFER+3*131072),131072);
			break;
		case access_BLOCK_1531_TAB2:
			SpiFlash_NormalRead_32(W_block247_sector0,(uint8_t *)(bmp_tab_BUFFER+2*131072),131072);
		  break;
		case access_BLOCK_1530_TAB1:
			SpiFlash_NormalRead_32(W_block245_sector0,(uint8_t *)(bmp_tab_BUFFER+1*131072),131072);
		  break;
		case access_BLOCK_1529_TAB0:
			SpiFlash_NormalRead_32(W_block243_sector0,(uint8_t *)(bmp_tab_BUFFER),131072);
		  break;
		default:
			break;
	}
	
}


void DATACLASS1_Check_init(void)
{
	uint16_t Tp_i;
	for(Tp_i=0;Tp_i<NAND_CHECKSUM_IN_SPIFLASH;Tp_i++)
	{
	Dataclass1_U.U32_ARRY[Tp_i]=0x1fe0000;
	}
}

void DATACLASS1_Check_Write(void)
{
	
	SpiFlash_SectorErase(W_block255_sector14);
	SpiFlash_NormalPageProgram(W_block255_sector14,Dataclass1_U.U8_ARRY,NAND_CHECKSUM_IN_SPIFLASH*4);
	#ifdef  SYSUARTPRINTF_ActionTimers 
	sysprintf("CHECKSUM Write=%X,%X,%X,%X,%X,%X\r\n",Dataclass1_U.U32_ARRY[0],Dataclass1_U.U32_ARRY[1],Dataclass1_U.U32_ARRY[2],
	                                 Dataclass1_U.U32_ARRY[3],Dataclass1_U.U32_ARRY[4],Dataclass1_U.U32_ARRY[5]);
	#endif
}

void DATACLASS1_Check_Read(void)
{
	SpiFlash_NormalRead(W_block255_sector14,Dataclass1_U.U8_ARRY,NAND_CHECKSUM_IN_SPIFLASH*4);
	#ifdef  SYSUARTPRINTF_ActionTimers 
	sysprintf("CHECKSUM Read=%X,%X,%X,%X,%X,%X\r\n",Dataclass1_U.U32_ARRY[0],Dataclass1_U.U32_ARRY[1],Dataclass1_U.U32_ARRY[2],
	                                 Dataclass1_U.U32_ARRY[3],Dataclass1_U.U32_ARRY[4],Dataclass1_U.U32_ARRY[5]);
	#endif
}


void W25Q128_test(void)
{
	uint8_t Tp_data_w[36]={0},Tp_data_r[36]={0};
	uint16_t Tp_i;
	
	memset((void *)BaseData_ARR,0xaa,logodata_Basedata_SIZE);
	W25Q128_Write(access_BLOCK_1967_BASEDATA);
	memset((void *)BaseData_ARR,0,logodata_Basedata_SIZE);
	W25Q128_Read(access_BLOCK_1967_BASEDATA);
	
	for(Tp_i=0;Tp_i<logodata_Basedata_SIZE;Tp_i++)
	{
		if(BaseData_ARR[Tp_i]!=0xaa)
		{
			LED_POWERLOW_ON();
	   	while(1);
		}
	}
	
	memset((void *)BaseData_ARR,0xbb,logodata_Basedata_SIZE);
	W25Q128_Write(access_BLOCK_1967_BASEDATA);
	memset((void *)BaseData_ARR,0,logodata_Basedata_SIZE);
	W25Q128_Read(access_BLOCK_1967_BASEDATA);
	
	for(Tp_i=0;Tp_i<logodata_Basedata_SIZE;Tp_i++)
	{
		if(BaseData_ARR[Tp_i]!=0xbb)
		{
			LED_FLASHERR_ON();
	   	while(1);
		}
	}
	
//	memset((void *)Tp_data_w,0x55,36);
//	SpiFlash_SectorErase(W_block255_sector15);
//	SpiFlash_NormalPageProgram(0xFFFF00,Tp_data_w,32);
//	memset((void *)Tp_data_r,0,36);
//	SpiFlash_NormalRead(0xFFFF00,Tp_data_r,32);
//	
//	if(memcmp((void *)Tp_data_w,(void *)Tp_data_r,32)!=0)
//	{
//		LED_FLASHERR_ON();
//		while(1);
//	}
//	
//	memset((void *)Tp_data_w,0xaa,36);
//	SpiFlash_SectorErase(W_block255_sector15);
//	SpiFlash_NormalPageProgram(0xFFFF00,Tp_data_w,32);
//	memset((void *)Tp_data_r,0,36);
//	SpiFlash_NormalRead(0xFFFF00,Tp_data_r,32);
//	
//	if(memcmp((void *)Tp_data_w,(void *)Tp_data_r,32)!=0)
//	{
//		LED_POWERLOW_ON();
//		while(1);
//	}
}


void W25Q128_earse(void)
{
	uint32_t Tp_i;
	for(Tp_i=W_block243_sector0;Tp_i<=W_block255_sector15;Tp_i=Tp_i+0x1000)
	{
		SpiFlash_SectorErase(Tp_i);
	}
	
	DATACLASS1_Check_init();
	DATACLASS1_Check_Write();
	
}
//void SPI_FLASH_CS (uint8_t cs) {

//  if (cs == CS_HIGH) {
//    REG_OPERATE(REG_SPI1_SSR,4,set);        // CS is inactive (High)
//  }
//  else {
//    REG_OPERATE(REG_SPI1_SSR,4,clear);        // CS is   active (Low)
//  }
//}

///*----------------------------------------------------------------------------
//  SPI Flash send a byte
// *----------------------------------------------------------------------------*/
//uint8_t SPI_FLASH_SendByte(uint8_t byte) {

//  while (inpw(REG_SPI1_CNTRL) & (1UL << 0));	                // wait until transfer is finished

//  //SPIM->TX[0]  = byte;                              // write data byte
//  outpw(REG_SPI1_TX0,byte);
//	//outpw(,(8UL       <<  3)|(1UL       <<  2)|(1UL       <<  1)|(1UL       <<  0));
//	REG_OPERATE(REG_SPI1_CNTRL,0x1,set);
//	
//	while (inpw(REG_SPI1_CNTRL) & (1UL << 0));	                // wait until transfer is finished

//  return ((uint8_t)inpw(REG_SPI1_RX0));
//}


///*----------------------------------------------------------------------------
//  SPI Flash Reaqd ID
// *----------------------------------------------------------------------------*/
//void SPI_FLASH_ReadID(uint8_t* pBuf, uint32_t sz) {

//  if (sz < 3) return;

//  SPI_FLASH_CS(CS_LOW);                             // CS is   active (Low)

//  SPI_FLASH_SendByte(CMD_RDID);                     // Send Read ID instruction
//  *(pBuf + 0) = SPI_FLASH_SendByte(0xFF);           // Read a byte from the FLASH
//  *(pBuf + 1) = SPI_FLASH_SendByte(0xFF);           // Read a byte from the FLASH
//  *(pBuf + 2) = SPI_FLASH_SendByte(0xFF);           // Read a byte from the FLASH

//  SPI_FLASH_CS(CS_HIGH);                            // CS is inactive (High)
//}









