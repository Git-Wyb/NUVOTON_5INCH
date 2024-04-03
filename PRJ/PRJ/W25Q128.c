#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "sys.h"
#include "W25Q128.h"
#include "BSP_Init.h"
#include "spi.h"



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
extern  uint8_t *BaseData_ARR;
 
 
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
void SpiFlash_NormalPageProgram(uint32_t StartAddress, uint8_t *u8DataBuffer)
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
    for(i=0;i<36;i++)
    {
        spiWrite(0, 0, u8DataBuffer[i]);
        spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
        while(spiGetBusyStatus(0));
    }
    // /CS: 失能
    spiIoctl(0, SPI_IOC_DISABLE_SS, SPI_SS_SS0, 0);
		
		SpiFlash_WaitReady();
}
/*
函数名：SpiFlash_NormalRead(uint32_t StartAddress, uint8_t *u8DataBuffer)
功能：读取SpiFlash数据
参数：	
			StartAddress：要读取数据的首地址 为256的倍数
			u8DataBuffer：读取数据存入指针
返回值：				无
*/
void SpiFlash_NormalRead(uint32_t StartAddress, uint8_t *u8DataBuffer)
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
    for(i=0; i<36; i++) {
        spiWrite(0, 0, 0x00);
        spiIoctl(0, SPI_IOC_TRIGGER, 0, 0);
        while(spiGetBusyStatus(0));
        u8DataBuffer[i] = spiRead(0, 0);  
    }
		
		SpiFlash_WaitReady();
}


void W25Q128_Write(void)
{
	uint8_t Tp_data[36]={0};
	memcpy((void *)Tp_data,(void *)(BaseData_ARR+3*9),9);
	memcpy((void *)(Tp_data+9),(void *)(BaseData_ARR+4*9),9);
	memcpy((void *)(Tp_data+18),(void *)(BaseData_ARR+12*9),9);
	memcpy((void *)(Tp_data+27),(void *)(BaseData_ARR+13*9),9);
	SpiFlash_SectorErase(0xFFFF00);
	SpiFlash_NormalPageProgram(0xFFFF00,Tp_data);
}

void W25Q128_Read(void)
{
	uint8_t Tp_data[36]={0};
	
	SpiFlash_NormalRead(0xFFFF00,Tp_data);
	memcpy((void *)(BaseData_ARR+3*9),(void *)(Tp_data),9);
	memcpy((void *)(BaseData_ARR+4*9),(void *)(Tp_data+9),9);
	memcpy((void *)(BaseData_ARR+12*9),(void *)(Tp_data+18),9);
	memcpy((void *)(BaseData_ARR+13*9),(void *)(Tp_data+27),9);
	
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









