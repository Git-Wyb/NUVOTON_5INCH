

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "sys.h"
#include "BSP_Init.h"
#include "logo_IIC.h"
#include "i2c.h"


#define I2CNUM_1	1 

void LOGO_IIC_Init(void)
{
  int32_t rtval;
	
	outpw(REG_SYS_APBIPRST1, (1<<1));
	outpw(REG_SYS_APBIPRST1, 0);
	//pi3 pi4 iic1
	// REG_OPERATE(REG_CLK_PCLKEN1, 1<<1,set);
	
	REG_OPERATE(REG_SYS_GPI_MFPL,0x000ff000,clear);
	REG_OPERATE(REG_SYS_GPI_MFPL,0x00088000,set);
	
	i2cInit(1);	
	
	rtval = i2cOpen((PVOID)I2CNUM_1);
	if(rtval < 0)
	{
		sysprintf("Open I2C0 error!\n");
		//goto exit_test;
	}	
	
	i2cIoctl(I2CNUM_1, I2C_IOC_SET_DEV_ADDRESS, 0xA6, 0);
	i2cIoctl(I2CNUM_1, I2C_IOC_SET_SPEED, 100, 0);
	
	
	
}





