#ifndef W25Q128_H
#define W25Q128_H 

/*
W25Q128 将 16MB(128Mb)的容量分为256个块（0-255block），每块 64K 字节,每块分为 16 个扇区（sector），一扇区 4K 字节；
每扇区分为 16 个页（page），一页 256 字节。
W25Q128 的最小擦除单位为一个扇区，也就是每次必须擦除 4K 个字节。这样我们需要给 W25Q128 开辟一个至少 4K 的缓存区.
擦除之后值为0xFF.
*/
#define W_block128_sector0  0x800000
#define W_block243_sector0  0xf30000
#define W_block244_sector0  0xf40000
#define W_block245_sector0  0xf50000
#define W_block246_sector0  0xf60000
#define W_block247_sector0  0xf70000
#define W_block248_sector0  0xf80000
#define W_block249_sector0  0xf90000
#define W_block250_sector0  0xfa0000
#define W_block251_sector0  0xfb0000
#define W_block252_sector0  0xfc0000
#define W_block253_sector0  0xfd0000
#define W_block254_sector0  0xfe0000
#define W_block255_sector0  0xff0000
#define W_block255_sector14 0xffe000 //第255块的第14扇区
#define W_block255_sector15 0xfff000
/**/
typedef enum 
{
	access_BLOCK_1529_TAB0,
	access_BLOCK_1530_TAB1,
	access_BLOCK_1531_TAB2,
	access_BLOCK_1532_TAB3,
	access_BLOCK_1966_UNIT,
	access_BLOCK_1967_BASEDATA,
	access_BLOCK_0_BACKUP,
	access_BLOCK_CHECKSUM,
	accsee_BASEDATA_PARA_5INCH
}access_TYPE_E;


void W25Q128_init(void);
//void SPI_FLASH_ReadID(uint8_t* pBuf, uint32_t sz);
void W25Q128_Write(access_TYPE_E x_type);
void W25Q128_Read(access_TYPE_E x_type);
void W25Q128_test(void); 
void DATACLASS1_Check_init(void);
void W25Q128_earse(void);
void SpiFlash_NormalRead(uint32_t StartAddress, uint8_t *u8DataBuffer,uint16_t x_len);
#endif


