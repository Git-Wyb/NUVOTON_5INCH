#ifndef W25Q128_H
#define W25Q128_H 

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
#define W_block255_sector14 0xffe000
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
#endif


