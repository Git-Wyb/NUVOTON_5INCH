


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "nand_nand.h"
#include "nand.h"
#include "BSP_Init.h"
#include "Command_all.h"


void delay_us(int usec);
#define _RB0_READYBUSY   (0x01 << 13)//pc13
#define _DMA_READYBUSY   (0X01 << 2)
uint8_t NAND_ID[5];
uint8_t STATUS = 0;
extern volatile uint8_t Flag_int;
extern BADMANAGE_TAB_TYPE_U badmanage_str[1];
extern SYSTEMERRORSTATUS_TYPE  systerm_error_status;

int nuc970_dev_ready(void)
{
    return ((inpw(REG_GPIOC_DATAIN) & _RB0_READYBUSY) ? 1 : 0);
}

int nuc970_dev_dmaover(void)
{
	return ((inpw(REG_NANDCTL) & _DMA_READYBUSY) ? 1 : 0);
}

u8 NAND_WaitRB(vu8 rb)
{
    vu16 time=0;  
	
	while(time<10000)
	{
		time++;
		if(nuc970_dev_ready()==rb)return 0;
	}
	return 1;
}

u8 NAND_ReadStatus(void)
{
    vu8 data=0; 
   outpw(REG_NANDCMD,inpw(REG_NANDCMD)&0xffffff00|(uint8_t)NAND_CMD_STATUS);
  data = inpw(REG_NANDDATA)&0x000000ff;
	
    return data;
}


u8 NAND_GetBusy(void)
{
	  u32 status=0;
    vu32 time=0; 
	  u8  data;
	while((status != NAND_READY) &&( time <0x1FFFF))						//等待ready
	{
	   data=NAND_ReadStatus();	//获取状态值
		
	  if((data & NAND_READY) == NAND_READY)
     {
        status = NAND_READY;
     }
     else
     {
        status = NAND_BUSY; 
      }
		 time++; 
  }
	
  if(time >= 0x1FFFF)	
	{
		 return  NSTA_TIMEOUT;
		
	}
		
  return NSTA_READY;//准备好
}


u8 NAND_GetStatus(void)
{
    u32 status=0;
    vu32 time=0; 
	  u8  data;
	while((status != NAND_READY) &&( time <0x1FFFF))						//等待ready
	{
		data=NAND_ReadStatus();	//获取状态值
		
		if((data & NAND_ERROR) == NAND_ERROR)//IO1
    {
        status = NAND_ERROR;//1-FAIL
    } 
    else if((data & NAND_READY) == NAND_READY)//IO6
    {
       status = NAND_READY;//1 READY
    }
    else
    {
       status = NAND_BUSY; //0 BUSY
    }
		
		time++; 
	}

  if(time >= 0x1FFFF)	
	{
		 return  NSTA_TIMEOUT;
		
	}
		
		 return NSTA_READY;//准备好
} 



uint32_t NAND_ReadID(void)
{
	//REG_OPERATE(REG_NANDINTSTS,1<<10,set);
	outpw(REG_NANDCMD,inpw(REG_NANDCMD)&0xffffff00|(uint8_t)NAND_CMD_READID);
	outpw(REG_NANDADDR, (inpw(REG_NANDADDR)&0xffffff00)|0x80000000);
	//while(nuc970_dev_ready()==0);
	//REG_OPERATE(REG_NANDINTSTS,1<<10,set);
	
	NAND_ID[0] = inpw(REG_NANDDATA)&0x000000ff;
	#ifdef SYSUARTPRINTF
	sysprintf("%x\r\n",NAND_ID[0]);
	#endif
	NAND_ID[1]= inpw(REG_NANDDATA)&0x000000ff;
	#ifdef SYSUARTPRINTF
	sysprintf("%x\r\n",NAND_ID[1]);
	#endif
	NAND_ID[2] = inpw(REG_NANDDATA)&0x000000ff;
	#ifdef SYSUARTPRINTF
	sysprintf("%x\r\n",NAND_ID[2] );
	#endif
	NAND_ID[3] = inpw(REG_NANDDATA)&0x000000ff;
	#ifdef SYSUARTPRINTF
	sysprintf("%x\r\n",NAND_ID[3]);
	#endif
	NAND_ID[4] = inpw(REG_NANDDATA)&0x000000ff;
	#ifdef SYSUARTPRINTF
	sysprintf("%x\r\n",NAND_ID[4]);
	#endif
	
	//REG_OPERATE(REG_NANDCTL,1<<16,set);
	//REG_OPERATE(REG_NANDCTL,1<<17,clear);
	
 return 	0x5555;
}

u8 NAND_EraseBlock(u32 BlockNum)
{
	#ifdef  SYSUARTPRINTF  
		sysprintf("earse_BlockNum=0x%X\r\n",BlockNum);
	#endif
	//REG_OPERATE(REG_NANDINTSTS,1<<10,set);
//////	uint32_t Tp_busy=0;
//////	
//////	REG_OPERATE(REG_NANDCTL,1,set);
//////	delay_us(3);
//////	REG_OPERATE(REG_NANDECTL,1,set);//WP HIGH
//////	delay_us(3);
//////	
//////	outpw(REG_NANDCMD,inpw(REG_NANDCMD)&0xffffff00|(uint8_t)NAND_CMD_ERASE1);
//////	delay_us(3);
//////	outpw(REG_NANDADDR, ((inpw(REG_NANDADDR)&0x7fffff00)));
//////	delay_us(3);
//////	//outpw(REG_NANDADDR, (inpw(REG_NANDADDR)&0x7fffff00));
//////	outpw(REG_NANDADDR, (inpw(REG_NANDADDR)&0xffffff00)|0x80000000);
//////	delay_us(3);
//////	outpw(REG_NANDCMD,inpw(REG_NANDCMD)&0xffffff00|(uint8_t)NAND_CMD_ERASE2);
//////	
//////	while(nuc970_dev_ready()==0)  Tp_busy++;
//////	sysprintf("Tp_busy = %x\n",Tp_busy );
//////	//REG_OPERATE(REG_NANDINTSTS,1<<10,set);
//////	
//////	outpw(REG_NANDCMD,inpw(REG_NANDCMD)&0xffffff00|(uint8_t)NAND_CMD_STATUS);
//////	STATUS = inpw(REG_NANDDATA)&0x000000ff;
//////	
//////	REG_OPERATE(REG_NANDECTL,1,clear);//WP LOW
//////	
//////	
//////	sysprintf("STATUS = %x\n",STATUS );
//////	
  /////////////REG_OPERATE(REG_NANDECTL,1,set);//WP HIGH
  
	BlockNum = BAD_BLOCK_CHANGE(BlockNum);
	
	BlockNum<<=6;

	outpw(REG_NANDCMD,0x60);
	//outpw(REG_NANDADDR,0);
	//outpw(REG_NANDADDR,0);
	outpw(REG_NANDADDR,((inpw(REG_NANDADDR)&0x7fffff00))|(u8)BlockNum);
	outpw(REG_NANDADDR,((inpw(REG_NANDADDR)&0x7fffff00))|(u8)(BlockNum>>8));
	outpw(REG_NANDADDR,((inpw(REG_NANDADDR)&0x7fffff00))|(u8)(BlockNum>>16));
	outpw(REG_NANDADDR,((u8)(BlockNum>>24))|0x80000000);
	outpw(REG_NANDCMD,0xd0);
	//while(nuc970_dev_ready()==0);
	//////////REG_OPERATE(REG_NANDECTL,1,clear);//WP LOW
	
	if(NAND_GetStatus()!=NSTA_READY)
	{
     #ifdef SYSUARTPRINTF
		sysprintf("BlockNum erase error = %x\n\r",BlockNum );
		#endif
		systerm_error_status.bits.nandflash_Write_error = 1;
		return NSTA_ERROR;
	}
	else
	{
    #ifdef SYSUARTPRINTF
		sysprintf("Erase ok = %x\n\r",BlockNum);
		#endif
		return 0;	//成功   
	}
	
	
}


//void NAND_WRITE_dma(void)
//{
//	void *Tp_p = NAND_WRITE_BUF;
//	memset(Tp_p,0x00,2112);
//	memset(Tp_p,0x55,2048);
//	outpw(REG_FMI_DMASA,(uint32_t )NAND_WRITE_BUF);
//	//NAND_WRITE_BUF[2050] = 0xff;
//	//NAND_WRITE_BUF[2051] = 0xff;
//	outpw(REG_NANDRA0, 0x0000FFFF);
//	outpw(REG_NANDCMD,inpw(REG_NANDCMD)&0xffffff00|(uint8_t)NAND_CMD_SEQIN);
//	outpw(REG_NANDADDR, (inpw(REG_NANDADDR)&0xffffff00));
//	outpw(REG_NANDADDR, (inpw(REG_NANDADDR)&0xffffff00));
//	outpw(REG_NANDADDR, (inpw(REG_NANDADDR)&0xffffff00));
//	outpw(REG_NANDADDR, (inpw(REG_NANDADDR)&0xffffff00)|0x80000000);
//	
//	REG_OPERATE(REG_NANDINTSTS,1<<0,set);
//	REG_OPERATE(REG_NANDINTSTS,1<<2,set);
//	REG_OPERATE(REG_NANDINTSTS,1<<3,set);
//	
//	REG_OPERATE(REG_NANDCTL,1<<4,set);
//	REG_OPERATE(REG_NANDCTL,1<<2,set);
//	
//	while(nuc970_dev_dmaover()==1);
//	outpw(REG_NANDCMD,inpw(REG_NANDCMD)&0xffffff00|(uint8_t)NAND_CMD_PAGEPROG);
//	sysprintf("DMA DWR_EN bit Clear");
//	
//	while(nuc970_dev_ready()==0);
//	REG_OPERATE(REG_NANDINTSTS,1<<10,set);
//	sysprintf("RB0_IFbit until it was set");
//	
//	outpw(REG_NANDCMD,inpw(REG_NANDCMD)&0xffffff00|(uint8_t)NAND_CMD_STATUS);
//	STATUS = inpw(REG_NANDDATA)&0x000000ff;
//	
//	sysprintf("STATUS = %x\n",STATUS );
//}


u8 NAND_WritePage(u32 PageNum,u16 ColNum,u8 *pBuffer,u16 NumByteToWrite)
{
	
	uint16_t Tp_i;
	////////////REG_OPERATE(REG_NANDECTL,1,set);//WP HIGH
	PageNum = BAD_BLOCK_CHANGE(PageNum/64)*64  +  PageNum%64;
//	memset(NAND_WRITE_BUF,0x55,2048);
	
	outpw(REG_NANDCMD,inpw(REG_NANDCMD)&0xffffff00|(uint8_t)NAND_CMD_SEQIN);
	outpw(REG_NANDADDR, ((inpw(REG_NANDADDR)&0x7fffff00))|((u8)ColNum));
	outpw(REG_NANDADDR, ((inpw(REG_NANDADDR)&0x7fffff00))|((u8)(ColNum>>8)));
	outpw(REG_NANDADDR, ((inpw(REG_NANDADDR)&0x7fffff00))|((u8)PageNum));
	outpw(REG_NANDADDR, ((inpw(REG_NANDADDR)&0x7fffff00))|((u8)(PageNum>>8)));
	outpw(REG_NANDADDR, ((inpw(REG_NANDADDR)&0x7fffff00))|((u8)(PageNum>>16)));
	outpw(REG_NANDADDR, ((inpw(REG_NANDADDR)&0xffffff00)|0x80000000)|((u8)(PageNum>>24)));
	delay_us(1);
	
	for(Tp_i=0;Tp_i<NumByteToWrite;Tp_i++)
	   outpw(REG_NANDDATA,pBuffer[Tp_i]);
	
	outpw(REG_NANDCMD,inpw(REG_NANDCMD)&0xffffff00|(uint8_t)NAND_CMD_PAGEPROG);
	
	//while(nuc970_dev_ready()==0);
	//REG_OPERATE(REG_NANDINTSTS,1<<10,set);
	//sysprintf("RB0_IFbit until it was set");
	//delay_us(300);
	//outpw(REG_NANDCMD,inpw(REG_NANDCMD)&0xffffff00|(uint8_t)NAND_CMD_STATUS);
	//STATUS = inpw(REG_NANDDATA)&0x000000ff;
	
 //////////// REG_OPERATE(REG_NANDECTL,1,clear);//WP LOW
	
	if(NAND_GetStatus()!=NSTA_READY)
	{
		#ifdef SYSUARTPRINTF
		sysprintf("WRITE PAGE ERROR = %x\n",PageNum );
		#endif
		systerm_error_status.bits.nandflash_error = 1;
		return NSTA_ERROR;//失败
	}

    return 0;//成功   
	
	
	

}

u8 NAND_ReadPage(u32 PageNum,u16 ColNum,u8 *pBuffer,u16 NumByteToRead)
{
	uint16_t Tp_i;
	u8 errsta=0;
	u8 res=0;
	
	PageNum = BAD_BLOCK_CHANGE(PageNum/64)*64  +  PageNum%64;
	
	//memset(NAND_WRITE_BUF,0x0,2048);
	outpw(REG_NANDCMD,inpw(REG_NANDCMD)&0xffffff00|(uint8_t)NAND_CMD_READ0);
	outpw(REG_NANDADDR, ((inpw(REG_NANDADDR)&0x7fffff00))|((u8)ColNum));
	outpw(REG_NANDADDR, ((inpw(REG_NANDADDR)&0x7fffff00))|((u8)(ColNum>>8)));
	outpw(REG_NANDADDR, ((inpw(REG_NANDADDR)&0x7fffff00))|((u8)PageNum));
	outpw(REG_NANDADDR, ((inpw(REG_NANDADDR)&0x7fffff00))|((u8)(PageNum>>8)));
	outpw(REG_NANDADDR, ((inpw(REG_NANDADDR)&0x7fffff00))|((u8)(PageNum>>16)));
	outpw(REG_NANDADDR, ((inpw(REG_NANDADDR)&0xffffff00)|0x80000000)|((u8)(PageNum>>24)));
  //delay_us(300);
	outpw(REG_NANDCMD,inpw(REG_NANDCMD)&0xffffff00|(uint8_t)NAND_CMD_READSTART);
	
//	delay_us(300);
	res=NAND_WaitRB(0);			//等待RB=0 
    if(res)
		{
			sysprintf("TIMEOUT ERROR1 = %x\n" );
			return NSTA_TIMEOUT;	//超时退出
		}                                             
    //下面2行代码是真正判断NAND是否准备好的
	res=NAND_WaitRB(1);			//等待RB=1 
    if(res)
		{
			sysprintf("TIMEOUT ERROR2 = %x\n" );
			return NSTA_TIMEOUT;	//超时退出
		}
//	delay_us(300);
//	REG_OPERATE(REG_NANDINTSTS,1<<10,set);
	//delay_us(30);
	for(Tp_i=0;Tp_i<NumByteToRead;Tp_i++)
	   pBuffer[Tp_i] = inpw(REG_NANDDATA)&0x000000ff;
	
	
 if(NAND_GetBusy()!=NSTA_READY)
 {
	sysprintf("READ PAGE ERROR = %x\n" );
	 errsta=NSTA_ERROR;	//失败
 }
    return errsta;	//成功  
	//delay_us(300);
	//outpw(REG_NANDCMD,inpw(REG_NANDCMD)&0xffffff00|(uint8_t)NAND_CMD_STATUS);
	//STATUS = inpw(REG_NANDDATA)&0x000000ff;
	
	//sysprintf("STATUS = %x\n\r",STATUS );
	//sysprintf("data = %x\n\r",NAND_WRITE_BUF[0] );
}

u8 Tp_Buff_linshi[2112];
u8 Tp_Buff_rx[2112];

	uint32_t Tp_bad = 0; //坏块个数
void TEST_NANDFLASH(void)
{
	//NAND_ADDRESS WriteReadAddr;
//	uint32_t Q0_addr_1;
	uint16_t  x_delay;
	uint16_t Tp_block = 0;

	char Tp_arr[15] = {'B','A','D',' ','B','L','O','C','K',' ','=',' ',' ',0,0};
	uint32_t Tp_page;
	u8 Tp_res,Tp_ddd;
	u16 Tp_NumByteEqual;
	u8 Tp_data=0;
	static uint8_t buf[64];
	

	badmanage_str->BAD_MANAGE_str.ERR_NUMBER = 0;
	//Tp_res = NAND_EraseBlock(0x600);
	//while(1)
	//{
	for(Tp_block=0;Tp_block<2048;Tp_block++)
	{
	   if(Flag_int==1)
		{
				Flag_int = 0;
				FT5x06_RD_Reg(0, buf, 42);
		}
		//Tp_block = 0x600;
		Tp_res = NAND_EraseBlock(Tp_block);
		sysprintf("Tp_res = %d\n\r",Tp_res);
		 if(Tp_res!=0)
		 {
			 Tp_bad ++;
			 
			 continue;
			 
		 }
		sysprintf("Tp_bad = %x\n\r",Tp_bad);
		 //这个FOR貌似要花很多的时间，有没有必要要呢？
//		 for(Tp_page=0;Tp_page<64;Tp_page++)
//		 {
//	      Tp_data++;
//			 memset(Tp_Buff_linshi,Tp_data,2112);
//			 
//			 for(x_delay = 0;x_delay<1048;x_delay++);
//			 
//			 Tp_res = NAND_WritePage( Tp_block*64+Tp_page ,0 , Tp_Buff_linshi ,2048);
//			 
//			 memset(Tp_Buff_rx,0xff,2112);	
//			 
//			 	for(x_delay = 0;x_delay<1048;x_delay++);
//			 
//			 Tp_res = NAND_ReadPage(Tp_block*64+Tp_page ,0 , Tp_Buff_rx ,2048);
//			 
//			 if( memcmp(Tp_Buff_linshi,Tp_Buff_rx,2048)	!=0	)	
//	     {
//				  
//	         sysprintf( "nandflash R&W error");
//				  // Tp_bad ++;
//				    while(1);
//			 }

//		 
//		 }
		 
		 //NAND_EraseBlock(Tp_block);
		
	}
 //  }
	
	
//	while(1)
//	{
//		memset(Tp_Buff_linshi,Tp_ddd++,2112);
//		 for(Tp_page=0;Tp_page<64;Tp_page++)
//		 {
//	      for(x_delay = 0;x_delay<1048;x_delay++);
//			 
//			 Tp_res = NAND_WritePage( 0x7d8*64+Tp_page ,0 , Tp_Buff_linshi ,2048);
//			 
//			 memset(Tp_Buff_rx,0xff,2112);	
//			 
//			 	for(x_delay = 0;x_delay<1048;x_delay++);
//			 
//			 Tp_res = NAND_ReadPage(0x7d8*64+Tp_page ,0 , Tp_Buff_rx ,2048);
//			 
//			 if( memcmp(Tp_Buff_linshi,Tp_Buff_rx,2048)	!=0	)	
//	     {
//				   SetZuobiao(10, 400 + 20);
//	         lcd_printf( "nandflash R&W error");
//				  // Tp_bad ++;
//				    while(1);
//			 }
//		 }
//		
//	}
	sysprintf("Tp_bad = %x\n\r",Tp_bad);
	if(Tp_bad > 39)
	{
		//SetZuobiao(10, 400 + 20);
	  sysprintf( "nandflash errblock");
		
		while(1);
	}
	else
	{
			//Tp_arr[11]  = Tp_bad/10 + '0';
	   // Tp_arr[12]  = Tp_bad%10 + '0';
		  //SetZuobiao(10, 400 + 20);
	    sysprintf("Tp_bad = %x\n\r",Tp_bad);
	    
	
	}
	
	
		
		
		
}





void NAND_Init(void)
{
	
}









