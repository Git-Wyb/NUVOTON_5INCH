#ifndef NAND_NAND_H
#define NAND_NAND_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

//NAND FLASH状态
#define NSTA_READY       	   	0X40		//nand已经准备好
#define NSTA_ERROR				0X01		//nand错误
#define NSTA_TIMEOUT        	0X02		//超时
#define NSTA_ECC1BITERR       	0X03		//ECC 1bit错误
#define NSTA_ECC2BITERR       	0X04		//ECC 2bit以上错误



/* NAND memory status */
#define NAND_VALID_ADDRESS         ((uint32_t)0x00000100U)
#define NAND_INVALID_ADDRESS       ((uint32_t)0x00000200U)
#define NAND_TIMEOUT_ERROR         ((uint32_t)0x00000400U)
#define NAND_BUSY                  ((uint32_t)0x00000000U)
#define NAND_ERROR                 ((uint32_t)0x00000001U)
#define NAND_READY                 ((uint32_t)0x00000040U)

void NAND_Init(void);

uint32_t NAND_ReadID(void);
uint8_t NAND_WritePage(uint32_t PageNum,uint16_t ColNum,uint8_t *pBuffer,uint16_t NumByteToWrite);
//void NAND_EARSE(void);
uint8_t NAND_EraseBlock(uint32_t BlockNum);
uint8_t NAND_ReadPage(uint32_t PageNum,uint16_t ColNum,uint8_t *pBuffer,uint16_t NumByteToRead);
void TEST_NANDFLASH(void);
#endif
