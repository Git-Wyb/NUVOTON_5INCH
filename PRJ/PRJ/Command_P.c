
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "sys.h"
#include "Command_all.h"
#include "ff.h"
#include "BSP_Init.h"
#include "nand_nand.h"
#include "display.h"
#include "Aprotocol.h"
#include "poweron.h"
#include "lcd.h"
#include "W25Q128.h"

CopyFileInfo gs_CpyInfo = {0};
extern char  USB_IMAGE_TYPE;
FIL   *g_file1,*g_file2;
/*__attribute__((aligned(8)))*/ uint8_t *bmpBuf_kkk=0,*bmpBuf_kkk_bak=0;
 SYSTEMERRORSTATUS_TYPE  systerm_error_status={0};
 static uint8_t Cover_Flag = 0;
extern LAYER_SDRAM_STR display_layer_sdram; 
extern uint8_t IMAGE_SEARCHED ;
// uint32_t NANDFLASH_CUSTOMER_INX;
//uint32_t NANDFLASH_USER_INX; 
extern uint8_t  USB_HAS_USABLE_IMG;
 uint8_t flag_checksum =1;
 uint32_t CHECK_SUM_USB=0,CHECK_SUM_NAND=0;
 BADMANAGE_TAB_TYPE_U badmanage_str[1]={0};
 extern uint32_t LOG_TIME;
extern uint8_t  LOG_FLAG;
uint8_t *RxBuffer =0;//[NAND_PAGE_SIZE];
//uint8_t *TxBuffer =0;//[NAND_PAGE_SIZE];
 
 uint8_t *BaseData_ARR=0;//[164][9]={0};
extern  int gs_usb_mount_flag;
uint16_t FLAG_SW3_BEGIN=0xffff;//�����־��0ʱ��ʾ����SW3������ͼƬ����
extern  uint8_t  command_xor;  //����USB����COPY��ͼƬӦ��
extern  AreaConfig gs_AreaInfo[16],Tp_gs_AreaInfo[16];
extern  Dataclass4567_Info Fieldx_Info[16];
extern  uint32_t logodata_basedata_BUFFER;
extern uint32_t logodata_sdrambuffer_addr_arry[16]; 
extern const uint32_t logodata_sdrambuffer_size_arry[];
extern uint8_t LOGO_ERR;
volatile uint8_t FLAG_NAND_busy = 0,FLAG_POWEROFF_wait=0;
extern   TAB_Checksum_SPI_U Dataclass1_U;
extern UINT32 volatile time1ms_count;
 
 
 const uint32_t checksum_addr_nand_Arr[NAND_CHECKSUM_IN_SPIFLASH]=
{image_tab__nandflash_start,
 image_tab__nandflash_start+64,
 image_tab__nandflash_start+128,
 image_tab__nandflash_start+192,
  unit_data__B_nandflash_start,
  baseA_data__nandflash_start};
 
/*????????????*/  
int tolower(int c)  
{
    if (c >= 'A' && c <= 'Z')  
    {  
        return c + 'a' - 'A';  
    }  
    else  
    {  
        return c;  
    }  
}  


int htoi(char s[])  
{  
    int i;  
    int n = 0;  
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X'))  
    {  
        i = 2;  
    }  
    else  
    {  
        i = 0;  
    }  
    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i)  
    {  
        if (tolower(s[i]) > '9')  
        {  
            n = 16 * n + (10 + tolower(s[i]) - 'a');  
        }  
        else  
        {  
            n = 16 * n + (tolower(s[i]) - '0');  
        }  
    }  
    return n;  
}  

uint8_t Clear_sdram(uint32_t x_Flag)
{
//	uint32_t index = 0;
	uint32_t SDRam_addrr;
	uint8_t Tp_i =0 ;
	
																 
	/*if(x_Flag&0x200000)
	{
		
		memset((uint8_t*)LCD_FRAME_BUFFER,0,800*480*2);

	}*/
	
	
					if(x_Flag&0x01)//����һ����??
					{
					 if(bmp_mixlayer_BUFFER) memset((uint8_t*)bmp_mixlayer_BUFFER,0,800*480*2);
				
						
					}
					if(x_Flag&0x02)//?�?��һ��
					{ 
					 if(bmp_layer1_BUFFER) memset((uint8_t*)bmp_layer1_BUFFER,0,800*480*2);
				
						
					}
					if(x_Flag&0x04)//?�?�ڶ���
					{ 
						if(bmp_layer2_BUFFER) memset((uint8_t*)bmp_layer2_BUFFER,0,800*480*2);
				
						
					}
					if(x_Flag&0x08)//?�?��??��
					{ 
					if(bmp_layer3_BUFFER)  memset((uint8_t*)bmp_layer3_BUFFER,0,800*480*2);
						
			
						
					}

	if(x_Flag&0x10)//?�?ͼ�����������
	{ 
	
  if(bmp_tab_BUFFER) memset((uint8_t*)bmp_tab_BUFFER,0,4*64*2048);

		
	}
	
	if(x_Flag&0x20)//?�?Q0 Q1?������?
	{
		if(SDRAM_Q_TAB) memset((uint8_t*)SDRAM_Q_TAB,0,65536*8);
		if(SDRAM_Q_SHIFT_TAB) memset((uint8_t*)SDRAM_Q_SHIFT_TAB,0,65536*4);
	}
	
	
	if(x_Flag&0x40)//?�?Һ����ʾ??
	{
		if(bmp_LCD_BUFFER) memset((uint8_t*)(bmp_LCD_BUFFER/*&(~0x80000000)*/),0,800*480*2);
	}
	
	if(x_Flag&0x80)//?�?�������??
	{
		//memset((uint8_t*)badmanage_tab_BUFFER,0,64*2048);
		memset((uint8_t*)badmanage_str,0,sizeof(badmanage_str));
	}
	
	for(Tp_i=0;Tp_i<16;Tp_i++)
  {
		if(x_Flag&(1<<(Tp_i+8)))
		{
				SDRam_addrr = logodata_sdrambuffer_addr_arry[Tp_i];
			if(SDRam_addrr==0) continue;
			if(Tp_i==0)
			{
				 memset((uint8_t*)SDRam_addrr,0x0,logodata_sdrambuffer_size_arry[Tp_i]);
				if(BaseData_ARR) memset(BaseData_ARR,0,164*9);
			}
			else
			{
			   memset((uint8_t*)SDRam_addrr,0xff,logodata_sdrambuffer_size_arry[Tp_i]);
			}

		}
	}
		return 1;
}	

void NANDFLASH_BADMANAGE_INIT(void)
{
	
	uint16_t Tp_i,Tp_j,Tp_x;
//	uint32_t Tp_block = backupdata_nandflash_start/64;
//	//tp_view1 = DAMADGE_CS();
//	if((*(__IO uint8_t*)(badmanage_tab_BUFFER+131071)!=DAMADGE_CS())
//		||(*(__IO uint8_t*)(badmanage_tab_BUFFER+131070)!=DAMADGE_CS())
//	  ||(*(__IO uint8_t*)(badmanage_tab_BUFFER+131069)!=0x36)
//	  ||(*(__IO uint8_t*)(badmanage_tab_BUFFER+131068)!=0x36))
//	{//֤������?��û�б���ʼ����?? ���������Ҳ�?����??
//				
//					Clear_sdram(0x80);
//					*(__IO uint8_t*)(badmanage_tab_BUFFER+131071)=DAMADGE_CS();
//					*(__IO uint8_t*)(badmanage_tab_BUFFER+131070)=DAMADGE_CS();
//					*(__IO uint8_t*)(badmanage_tab_BUFFER+131069)=0x36;
//					*(__IO uint8_t*)(badmanage_tab_BUFFER+131068)=0x36;
//					SDRAM_TO_NANDFLASH(badmanage_tab_BUFFER,badmanage_nandflash_start,1);//��ʼ��һ��  ����?�β��ȫ������?0
//	}
	//badmanage_str->flag = 0;
	#ifdef SYSUARTPRINTF
		sysprintf("badmanage_str->flag = 0x%8x\r\n",badmanage_str->BAD_MANAGE_str.flag);
	  sysprintf("badmanage_str->BAD_MANAGE_str.ERR_NUMBER = 0x%8x\r\n",badmanage_str->BAD_MANAGE_str.ERR_NUMBER);
		if(badmanage_str->BAD_MANAGE_str.ERR_NUMBER<=BAD_BLOCK_TOTAL)
		{
		for(Tp_x=0;Tp_x<badmanage_str->BAD_MANAGE_str.ERR_NUMBER;Tp_x++)
		{
			sysprintf("badmanage_str->BAD_MANAGE_str.ERR_BLOCK = 0x%8x\r\n",badmanage_str->BAD_MANAGE_str.ERR_BLOCK[Tp_x]);
			sysprintf("badmanage_str->BAD_MANAGE_str.BACKUP_BLOCK = 0x%8x\r\n",badmanage_str->BAD_MANAGE_str.BACKUP_BLOCK[Tp_x]);
		}
	  }
	#endif
		
	if(badmanage_str->BAD_MANAGE_str.flag!=BAD_BLOCK_LOCK)//û�б����ù�
	//if(1)
	{
		#ifdef SYSUARTPRINTF
		sysprintf("INIT BAD BLOCK\r\n");
		#endif
	
	 Clear_sdram(0xFFFFFFB0);
		
		badmanage_str->BAD_MANAGE_str.ERR_NUMBER = 0;//?????
		//badmanage_str->ERR_BLOCK[badmanage_str->ERR_NUMBER++] = base_data__nandflash_start/64;
		for(Tp_i=0;Tp_i<(backupdata_nandflash_start/64);Tp_i++)//????
	  {
		
		if(NAND_EraseBlock(Tp_i))
		{
			
			badmanage_str->BAD_MANAGE_str.ERR_BLOCK[badmanage_str->BAD_MANAGE_str.ERR_NUMBER++] = Tp_i;
			if(badmanage_str->BAD_MANAGE_str.ERR_NUMBER >= (BAD_BLOCK_TOTAL-1))
			{
				systerm_error_status.bits.nandflash_Write_error = 1; 
				sysprintf("nandflash_Write_error\r\n");
				break;
			}	
		}
    }
		
		#ifdef SYSUARTPRINTF
		sysprintf("badmanage_str->ERR_NUMBER = 0x%x\r\n",badmanage_str->BAD_MANAGE_str.ERR_NUMBER);
		#endif
		
		
		
		for(Tp_i=0;Tp_i<badmanage_str->BAD_MANAGE_str.ERR_NUMBER;Tp_i++)
		{
//			flag111:		
//			for(Tp_j=0;Tp_j<badmanage_str->ERR_NUMBER;Tp_j++)
//			{
//				if(badmanage_str->ERR_BLOCK[Tp_j]==Tp_block)//??????????
//				{
//					Tp_block=Tp_block + 1;
//					Tp_j = 0;
//					goto flag111;
//				}
//					
//			}
//			if(Tp_block>=nandflash______________end/64)
//			{
//				Tp_block = 2047;
//			}
			badmanage_str->BAD_MANAGE_str.BACKUP_BLOCK[Tp_i]= backupdata_nandflash_start/64 + Tp_i;
			//Tp_block=Tp_block + 1;
		}
		badmanage_str->BAD_MANAGE_str.flag=BAD_BLOCK_LOCK;//????????
		
		
		if((badmanage_str->BAD_MANAGE_str.NANDFLASH_CUSTOMER_INX < image_file_nandflash_start*2048) || (badmanage_str->BAD_MANAGE_str.NANDFLASH_CUSTOMER_INX >= image_user_nandflash_start*2048) )
	 {
		 badmanage_str->BAD_MANAGE_str.NANDFLASH_CUSTOMER_INX = (uint32_t)(image_file_nandflash_start*2048);
	 }
	 
	 if((badmanage_str->BAD_MANAGE_str.NANDFLASH_USER_INX <image_user_nandflash_start*2048)||(badmanage_str->BAD_MANAGE_str.NANDFLASH_USER_INX>=image_tab__nandflash_start*2048) )
	 {
		 badmanage_str->BAD_MANAGE_str.NANDFLASH_USER_INX = (uint32_t)(image_user_nandflash_start*2048);
	 }
		
		
		//SDRAM_TO_NANDFLASH((uint32_t)badmanage_str->BAD_MANAGE_arr,backup_tab_nandflash_start,1);//��ʼ��һ��  ����?�β��ȫ������?0
	  //NANDFLASH_backup_checksum();
	  //NAND_EraseBlock(backup_tab_nandflash_start);
	  //NAND_WritePage(backup_tab_nandflash_start,0,badmanage_str->BAD_MANAGE_arr,sizeof(badmanage_str->BAD_MANAGE_arr));
		NANDFLASH_P3PD_INX_SAVE();
	 
	  DATACLASS1_Check_init();
	  W25Q128_Write(access_BLOCK_CHECKSUM);
	 
	  *(uint8_t *)(BaseData_ARR+BASE_data_ErrBlock*9+8) = 8;
		sprintf((char *)(BaseData_ARR+BASE_data_ErrBlock*9),"%08X",badmanage_str->BAD_MANAGE_str.ERR_NUMBER );
		
		for (Tp_j = 0; Tp_j < 16;Tp_j++)
	  {
		    gs_AreaInfo[Tp_j].type = 0xff;
			  gs_AreaInfo[Tp_j].addr=0;
			  gs_AreaInfo[Tp_j].space=0;
			  gs_AreaInfo[Tp_j].size=0;
			  Fieldx_Info[Tp_j].cycle = 0;
			  Fieldx_Info[Tp_j].loop = 0;
			  Fieldx_Info[Tp_j].num = 0;
			  
	  }
		
		gs_AreaInfo[0].addr = 0x6000000;//base_data__nandflash_start*2048;
		gs_AreaInfo[0].space = 0x20000;
		gs_AreaInfo[0].type = 0;
		
		WriteAreaConfig_new();
		field_info_save();
		
		basedata_ram_to_sdram();
		
		SDRAM_TO_NANDFLASH(logodata_basedata_BUFFER ,baseA_data__nandflash_start,1);
	  SDRAM_TO_NANDFLASH(logodata_basedata_BUFFER ,baseB_data__nandflash_start,1);
	}		
		
	
}


uint32_t READ_TAB_FROMSDRAM(uint16_t x_order,uint8_t x_layer,uint32_t *x_bmp)
{
	
//	int8_t  Tp_res;
//	uint8_t Tp_i=0;
  
 uint16_t Tp_width_CYW;
	uint16_t Tp_hight_CYW;
	uint32_t Tp_strataddr_CYW;
	uint32_t SDRam_addrr;
	

	
	
	SDRam_addrr = display_layer_sdram.IMAGE_TAB_BUFFER;
	Tp_width_CYW =  *(volatile uint16_t*) (SDRam_addrr+x_order*8);
	Tp_hight_CYW =  *(volatile uint16_t*) (SDRam_addrr+x_order*8+2);
	Tp_strataddr_CYW = *(volatile uint32_t*) (SDRam_addrr+x_order*8+4);
	#ifdef  SYSUARTPRINTF 
	sysprintf("Tp_width_CYW=%X,Tp_hight_CYW=%X,Tp_strataddr_CYW=%X,x_order=%X\r\n",Tp_width_CYW,Tp_hight_CYW,Tp_strataddr_CYW,x_order);
	#endif
	if((Tp_width_CYW!=0)&&(Tp_hight_CYW!=0)&&(Tp_width_CYW<=800)&&(Tp_hight_CYW<=480)&&(Tp_strataddr_CYW<(image_tab__nandflash_start)))
	{
	x_bmp[0]=Tp_width_CYW;
	x_bmp[1]=Tp_hight_CYW;

	return Tp_strataddr_CYW;
	}
	else
	{
		return 0xffffffff;
	}
}

void SAVE_TAB_TOSDRAM(uint16_t x_order,uint32_t x_startaddr,uint16_t x_width,uint16_t x_hight)
{
		uint32_t SDRam_addrr;
	  SDRam_addrr = bmp_tab_BUFFER;
	
	*(__IO uint16_t*) (SDRam_addrr+x_order*8) = x_width;
	*(__IO uint16_t*) (SDRam_addrr+x_order*8+2) =x_hight;
	*(__IO uint32_t*) (SDRam_addrr+x_order*8+4) = (uint32_t)x_startaddr;
	
	 /* *(__IO uint8_t*) (SDRam_addrr+x_order*6) = (uint8_t)(x_width&0xff);
	  *(__IO uint8_t*) (SDRam_addrr+x_order*6+1) =  (uint8_t)(x_width>>8);
	  *(__IO uint8_t*) (SDRam_addrr+x_order*6+2) = (uint8_t)(x_hight&0xff);
	  *(__IO uint8_t*) (SDRam_addrr+x_order*6+3) = (uint8_t)(x_hight>>8);
	  *(__IO uint8_t*) (SDRam_addrr+x_order*6+4) = (uint8_t)(x_startaddr&0xff);
	  *(__IO uint8_t*) (SDRam_addrr+x_order*6+5) = (uint8_t)((x_startaddr>>8)&0xff);*/
	  
}
uint16_t BAD_BLOCK_CHANGE(uint16_t x_block)
{
	static uint8_t Tp_j=0;
	
	if(badmanage_str->BAD_MANAGE_str.flag==BAD_BLOCK_LOCK)
	 {
		for(Tp_j=0;Tp_j<badmanage_str->BAD_MANAGE_str.ERR_NUMBER;Tp_j++)
		{
			if(badmanage_str->BAD_MANAGE_str.ERR_BLOCK[Tp_j] ==x_block)
			{
				//return badmanage_str->BACKUP_BLOCK[Tp_j];
				//break;
				x_block = badmanage_str->BAD_MANAGE_str.BACKUP_BLOCK[Tp_j];
			}
		}
	 }
	 
	 return x_block;
}

void BAD_BLOCK_MARK(uint16_t x_block)
{
	//if(badmanage_str->flag)
		
	badmanage_str->BAD_MANAGE_str.ERR_BLOCK[badmanage_str->BAD_MANAGE_str.ERR_NUMBER]=x_block;
	badmanage_str->BAD_MANAGE_str.BACKUP_BLOCK[badmanage_str->BAD_MANAGE_str.ERR_NUMBER]= backupdata_nandflash_start/64 + badmanage_str->BAD_MANAGE_str.ERR_NUMBER;
	
	
	
	if(badmanage_str->BAD_MANAGE_str.ERR_NUMBER >= (BAD_BLOCK_TOTAL-1))
	{
				systerm_error_status.bits.nandflash_Write_error = 1; 
		return;
				
	}

	badmanage_str->BAD_MANAGE_str.ERR_NUMBER=(badmanage_str->BAD_MANAGE_str.ERR_NUMBER+1)%BAD_BLOCK_TOTAL;
		
	*(uint8_t *)(BaseData_ARR+BASE_data_ErrBlock*9+8) = 8;
	sprintf((char *)(BaseData_ARR+BASE_data_ErrBlock*9),"%08X",badmanage_str->BAD_MANAGE_str.ERR_NUMBER );
	
	
	NANDFLASH_P3PD_INX_SAVE();
	//SDRAM_TO_NANDFLASH(badmanage_tab_BUFFER,backup_tab_nandflash_start,1);
	
}


static uint8_t  TX_NEW[2048];
uint8_t SDRAM_TO_NANDFLASH(uint32_t x_sdram_start,uint32_t x_nandflash_start,uint16_t x_block_num)
{
	NAND_ADDRESS_STR WriteReadAddr;
	 uint32_t Tp_i=0,Tp_j=0;
	uint16_t  x_delay;
//	static uint16_t Tp_backup;
	//uint32_t sdram_start_backup = x_sdram_start,nandflash_start_backup=x_nandflash_start,block_num_backup=x_block_num;
	
	union 
	{
		uint32_t DATA_U32;
		uint8_t  DATA_U8[4];
	}Tp_CHECK;

	//#ifdef  SYSUARTPRINTF_ActionTimers 
	sysprintf("DDR TO NAND=0X%X\r\n",x_nandflash_start/64);
	//#endif
	
	
	rewrite:
	
	Tp_CHECK.DATA_U32 = 0;
	
	
	
	//x_nandflash_start = x_nandflash_start*64;
	
	if((x_nandflash_start%64)!=0)
	{
		return 0;
	}
	//HAL_NVIC_DisableIRQ(PVD_IRQn);
	//__set_PRIMASK(1);
	////sysSetLocalInterrupt(DISABLE_IRQ);
	FLAG_NAND_busy =1;
	
	
	for(Tp_i=0;Tp_i<64*x_block_num;Tp_i++)
	{
		
		WriteReadAddr.Zone = 0x00;
    WriteReadAddr.Block = 0x00;
    WriteReadAddr.Page = x_nandflash_start+Tp_i;
		
		
//	 BAD_BLOCK_CHANGE(&WriteReadAddr);
		
		
		if(WriteReadAddr.Page %64 == 0)
			NAND_EraseBlock(WriteReadAddr.Page/64);
		
//		for(x_delay = 0;x_delay<2048;x_delay++)
//		{
//			TxBuffer[x_delay] = *(__IO uint8_t*) (x_sdram_start+Tp_i*NAND_PAGE_SIZE+x_delay);
//		}
		//memcpy();
		
		memcpy(TX_NEW,(void *)(x_sdram_start+Tp_i*NAND_PAGE_SIZE),2048);
		//for(x_delay = 0;x_delay<2048;x_delay++);
		for(Tp_j=0;Tp_j<2048;Tp_j++)
		{
				Tp_CHECK.DATA_U32+=TX_NEW[Tp_j];
		}
		
		if(NAND_WritePage(WriteReadAddr.Page,0,TX_NEW, NAND_PAGE_SIZE))
		{

			//#ifdef  SYSUARTPRINTF 
			sysprintf("BAD BLOCK MARK");
			//#endif
			BAD_BLOCK_MARK(BAD_BLOCK_CHANGE(WriteReadAddr.Page/64));
			goto rewrite;
			//SDRAM_TO_NANDFLASH(x_sdram_start,x_nandflash_start,x_block_num);
			//BAD_BLOCK_MARK(BAD_BLOCK_CHANGE(WriteReadAddr.Page/64));
			//SDRAM_TO_NANDFLASH(x_sdram_start,x_nandflash_start,x_block_num);
//			  systerm_error_status.bits.nandflash_Write_error = 1;
//			if(badmanage_str->ERR_NUMBER<20)//����BACKUP�����Ǻõ�
//			{
//				badmanage_str->ERR_BLOCK[badmanage_str->ERR_NUMBER]=WriteReadAddr.Page-WriteReadAddr.Page%64;
//				if(badmanage_str->ERR_NUMBER == 0)
//				{
//					Tp_backup = backupdata_nandflash_start/64;
//				}
//				else
//				{
//					Tp_backup = badmanage_str->BACKUP_BLOCK[badmanage_str->ERR_NUMBER-1]+64;
//				}
//				badmanage_str->BACKUP_BLOCK[badmanage_str->ERR_NUMBER]=Tp_backup;
//				badmanage_str->ERR_NUMBER++;
//				Tp_i=Tp_i-Tp_i%64;//�ı�ѭ������ ���°�����д������
//				goto rewrite ;
//			}
		}
		
		
		for(x_delay = 0;x_delay<2048;x_delay++);
		
		NAND_ReadPage(WriteReadAddr.Page,0,RxBuffer, NAND_PAGE_SIZE);
		if(memcmp(TX_NEW,RxBuffer,NAND_PAGE_SIZE)!=0)
		{
			//#ifdef  SYSUARTPRINTF 
			sysprintf("BAD BLOCK MARK");
			//#endif
			BAD_BLOCK_MARK(BAD_BLOCK_CHANGE(WriteReadAddr.Page/64));
			goto rewrite;
			//SDRAM_TO_NANDFLASH(x_sdram_start,x_nandflash_start,x_block_num);
		}
//		if(NAND_GetECCresult(WriteReadAddr.Page,0))
//		{
//			BAD_BLOCK_MARK(WriteReadAddr.Page/64);
//			SDRAM_TO_NANDFLASH(x_sdram_start,x_nandflash_start,x_block_num);
//		}
		
		if(Tp_i%64==63)
		{
			for(Tp_j=0;Tp_j<NAND_CHECKSUM_IN_SPIFLASH;Tp_j++)
	    {
				if((x_nandflash_start+Tp_i-63)==checksum_addr_nand_Arr[Tp_j])
				{
                    sysprintf("----777----checksum_addr_nand_Arr[%d] = %08X\r\n",Tp_j,checksum_addr_nand_Arr[Tp_j]);
					switch(Tp_j)
					{
						case access_BLOCK_1967_BASEDATA:
							  LOG_FLAG|=0X01;
						    LOG_TIME = time1ms_count;
							  //W25Q128_Write(access_BLOCK_1967_BASEDATA);
							  break;
						case access_BLOCK_1966_UNIT:
							  LOG_FLAG|=0X02;
						     LOG_TIME = time1ms_count;
                             sysprintf("----888----access_BLOCK_1966_UNIT\r\n");
							  // W25Q128_Write(access_BLOCK_1966_UNIT);
							  break;
						case access_BLOCK_1532_TAB3:
			          W25Q128_Write(access_BLOCK_1532_TAB3);
			          break;
		        case access_BLOCK_1531_TAB2:
			          W25Q128_Write(access_BLOCK_1531_TAB2);
                break;
		        case access_BLOCK_1530_TAB1:
			          W25Q128_Write(access_BLOCK_1530_TAB1);
		            break;
		        case access_BLOCK_1529_TAB0:
			         W25Q128_Write(access_BLOCK_1529_TAB0);
		           break;
						default :
							  
							   break;
					}
					Dataclass1_U.U32_ARRY[Tp_j] = Tp_CHECK.DATA_U32;
					W25Q128_Write(access_BLOCK_CHECKSUM);
				}
	    }
			Tp_CHECK.DATA_U32=0;
		}
		
	}
	
	
	
	///sysSetLocalInterrupt(ENABLE_IRQ);
	FLAG_NAND_busy = 0;
	
	//__set_PRIMASK(0);
	//HAL_NVIC_EnableIRQ(PVD_IRQn);
	
	return 1;
}

uint8_t NANDFLASH_TO_SDRAM(uint32_t x_sdram_start,uint32_t x_nandflash_start,uint16_t x_block_num)
{
	NAND_ADDRESS_STR WriteReadAddr;
	 uint32_t Tp_i=0,Tp_j=0;
	uint16_t  x_delay;
	uint32_t Tp_nandflash_status;
	uint8_t flag_tab=0;
	
	union 
	{
		uint32_t DATA_U32;
		uint8_t  DATA_U8[4];
	}Tp_CHECK;
	
	//#ifdef  SYSUARTPRINTF_ActionTimers 
	sysprintf("---1111----NANDFLASH_TO_SDRAM,NAND TO DDR=0X%X\r\n",x_nandflash_start/64);
	//#endif
	
	
	Tp_CHECK.DATA_U32 = 0;
	//x_nandflash_start = x_nandflash_start*64;
	
	if((x_nandflash_start%64)!=0)
	{
		return 0;
	}
	if(x_block_num>1024)
	{
		return 0;
	}
	
	for(Tp_i=0;Tp_i<64*x_block_num;Tp_i++)
	{
		WriteReadAddr.Zone = 0x00;
    WriteReadAddr.Block = 0x00;
    WriteReadAddr.Page = x_nandflash_start+Tp_i;
		
	 // BAD_BLOCK_CHANGE(&WriteReadAddr);
		//for(x_delay = 0;x_delay<2048;x_delay++);
	
		
		Tp_nandflash_status = NAND_ReadPage(WriteReadAddr.Page,0,RxBuffer,NAND_PAGE_SIZE);
		if((Tp_nandflash_status&NAND_READY)!=NAND_READY)
		{
			//memset(RxBuffer,0,2048);
		}
		
		
		for(x_delay = 0;x_delay<2048;x_delay++)
		{
			*(__IO uint8_t*) (x_sdram_start+Tp_i*NAND_PAGE_SIZE+x_delay) = RxBuffer[x_delay];
		}
		
			for(Tp_j=0;Tp_j<2048;Tp_j++)
			{
				Tp_CHECK.DATA_U32+=RxBuffer[Tp_j];
			}
			
			
			if(Tp_i%64==63)
			{
				
				for(Tp_j=0;Tp_j<NAND_CHECKSUM_IN_SPIFLASH;Tp_j++)
	     {
				if((x_nandflash_start+Tp_i-63)==checksum_addr_nand_Arr[Tp_j])
				{
					if(Dataclass1_U.U32_ARRY[Tp_j] != Tp_CHECK.DATA_U32)
					{
                        sysprintf("---1112---Dataclass1_U.U32_ARRY[%d]=0X%x != Tp_CHECK.DATA_U320=X%X\r\n",Tp_j,Dataclass1_U.U32_ARRY[Tp_j],Tp_CHECK.DATA_U32);
						switch(Tp_j)
					  {
						   case access_BLOCK_1967_BASEDATA:
							      W25Q128_Read(access_BLOCK_1967_BASEDATA);
							      basedata_ram_to_sdram();
							  break;
							 case access_BLOCK_1966_UNIT:
								     W25Q128_Read(access_BLOCK_1966_UNIT);
							       unit_ram_to_sdram();
								break;
							 case access_BLOCK_1532_TAB3:
			          W25Q128_Read(access_BLOCK_1532_TAB3);
							 flag_tab = 1;
			          break;
		           case access_BLOCK_1531_TAB2:
			          W25Q128_Read(access_BLOCK_1531_TAB2);
							 flag_tab =1;
                break;
		            case access_BLOCK_1530_TAB1:
			          W25Q128_Read(access_BLOCK_1530_TAB1);
								flag_tab =1;
		            break;
		           case access_BLOCK_1529_TAB0:
			         W25Q128_Read(access_BLOCK_1529_TAB0);
							 flag_tab =1;
		           break;
						   default :
							   break;
					  }
						
						#ifdef  SYSUARTPRINTF_ActionTimers 
						sysprintf("CHECKSUM ERR=%X,read%X!=cal%X\r\n",Tp_j,Dataclass1_U.U32_ARRY[Tp_j],Tp_CHECK.DATA_U32);
						#endif
					}
				}
	      }
			  Tp_CHECK.DATA_U32=0;
			}
	//	for(x_delay = 0;x_delay<2048;x_delay++);
//		memcpy((void *)(x_sdram_start+Tp_i*NAND_PAGE_SIZE),RxBuffer,2048);
	}
	
	if(flag_tab==1) SDRAM_TO_NANDFLASH(bmp_tab_BUFFER,image_tab__nandflash_start,4);
	
	return 1;
}


uint16_t ORDER = 0;
uint32_t YYY;

uint32_t SAVE_IMAGE_20170816(uint32_t x_nandflah_start,uint32_t x_sdram_end,uint16_t x_order,uint8_t x_cover)
{
   uint32_t  Tp_sdram = bmp_layer3_BUFFER;
	uint16_t  x_delay;
	// uint32_t Tp_nandflash_status = 0; 
//	int Tp_nandflash_status = 0; 
	//unsigned int Tp_nandflash_status_abs=0;
	NAND_ADDRESS_STR WriteReadAddr;
	uint32_t Tp_addr = 0;
	//static uint8_t Tp_j;
	static uint8_t Tp_i=0,Tp_j=0;
	//uint8_t Tp_flag=0;//1-???????
  
	ORDER =  x_order;
	
	if(USB_IMAGE_TYPE < '4')
	{
	if((x_nandflah_start+x_sdram_end-bmp_layer3_BUFFER)>=(image_user_nandflash_start*2048))
	{
		 //LCD_DisplayStringLine( 30, "TOO MANY FILES     ");
		   //LCD_DisplayStringLine( 30, "Out Of Range       ");
		//SYSTEM_ERR_STATUS->bits.Image_OutOfRange_error=1;
		systerm_error_status.bits.image_outofrange_error = 1;
		return x_nandflah_start;
	}	
	}
	else
	{
	if((x_nandflah_start+x_sdram_end-bmp_layer3_BUFFER)>=(image_tab__nandflash_start*2048))
	{
		 //LCD_DisplayStringLine( 30, "TOO MANY FILES     ");
		
		systerm_error_status.bits.image_outofrange_error = 1;
		return x_nandflah_start;
	}
	}
	
	Tp_addr = x_nandflah_start;
	for(;Tp_sdram<x_sdram_end;Tp_sdram=Tp_sdram+2048)
	{
		   WriteReadAddr.Zone = 0x00;
       WriteReadAddr.Block = 0x00;
       WriteReadAddr.Page = (x_nandflah_start)/2048;
		 YYY =  WriteReadAddr.Page ;
	   // BAD_BLOCK_CHANGE(&WriteReadAddr);
		
		if((Tp_sdram == bmp_layer3_BUFFER)&&(x_cover ==2))//??????h??
			{
				if(WriteReadAddr.Page%64!=0)//????????????'??
				{
					Tp_i=WriteReadAddr.Page%64;
					WriteReadAddr.Page = WriteReadAddr.Page - WriteReadAddr.Page%64;
					NANDFLASH_TO_SDRAM(bmp_layer2_BUFFER,WriteReadAddr.Page,1);//????LAYER2
					
				//	Tp_j=0;
				 for(;Tp_i<64; Tp_i ++)
				 {
					  memcpy((uint8_t *)(bmp_layer2_BUFFER+Tp_i*2048),(uint8_t*)(Tp_sdram),2048);
					
					 Tp_sdram = Tp_sdram+2048;
					  Tp_addr = Tp_addr +2048;
			      x_nandflah_start = x_nandflah_start+2048;
					 if(Tp_sdram >= x_sdram_end)//?????h??????
					 {
						 goto flag111;
					 }
					// Tp_j ++;
				 }
				 SDRAM_TO_NANDFLASH(bmp_layer2_BUFFER,WriteReadAddr.Page,1);
				 Tp_sdram = Tp_sdram-2048;
				 continue;
				 
flag111:  
				 //WriteReadAddr.Page = WriteReadAddr.Page +64 +WriteReadAddr.Page%64;
				 //NANDFLASH_TO_SDRAM(bmp_layer1_BUFFER,WriteReadAddr.Page,1);//????LAYER2
				 //memcpy((uint8_t *)bmp_layer1_BUFFER,(uint8_t *)bmp_layer2_BUFFER,Tp_sdram - bmp_layer3_BUFFER);
				 //WriteReadAddr.Page = WriteReadAddr.Page -1;
				 //SDRAM_TO_NANDFLASH(bmp_layer1_BUFFER,WriteReadAddr.Page,1);
				 SDRAM_TO_NANDFLASH(bmp_layer2_BUFFER,WriteReadAddr.Page,1);
				 break;
				 
				}
			}
		
		
		
		  memcpy(bmpBuf_kkk,(uint8_t*)Tp_sdram,2048);
		  for(x_delay = 0;x_delay<1048;x_delay++);
	  
	    
		
		  if(WriteReadAddr.Page%64==0)
		 {
			 if(((Tp_sdram+(2048*64))>x_sdram_end)&&(x_cover ==2))//??????????h?????
			 {
				 NANDFLASH_TO_SDRAM(bmp_layer2_BUFFER,WriteReadAddr.Page,1);
				// memcpy((uint8_t *)bmp_layer2_BUFFER,(uint8_t *)(Tp_sdram),x_sdram_end-Tp_sdram);
				 Tp_i=0;
				 for(;Tp_sdram<x_sdram_end;Tp_sdram=Tp_sdram+2048)
				 {
					  memcpy((uint8_t *)(bmp_layer2_BUFFER+Tp_i*2048),(uint8_t*)Tp_sdram,2048);
					 Tp_i ++;
				 }
				 SDRAM_TO_NANDFLASH(bmp_layer2_BUFFER,WriteReadAddr.Page,1);
			//	 Tp_addr = Tp_addr + x_sdram_end-Tp_sdram;
				 break;
				 
			 }
			 NAND_EraseBlock(WriteReadAddr.Page/64);
			 
	  	}
		
	   for(x_delay = 0;x_delay<1048;x_delay++);
		
	   //NAND_WritePage( WriteReadAddr.Page,0,bmpBuf_kkk, 2048);
		 while(NAND_WritePage( WriteReadAddr.Page,0,bmpBuf_kkk, 2048)==NSTA_ERROR)
		 {
			// x_delay = x_delay;
			 BAD_BLOCK_MARK(BAD_BLOCK_CHANGE(WriteReadAddr.Page/64));
			 if(systerm_error_status.bits.nandflash_Write_error == 1)
			 {
				 break;
			 }
		 }
		    
		   Tp_addr = Tp_addr +2048;
			x_nandflah_start = x_nandflah_start+2048;
			
			if(USB_IMAGE_TYPE < '4')
	   {
			
			if( Tp_addr >= (image_user_nandflash_start*2048))//????????k??
	    {
         //LCD_DisplayStringLine( 30, "TOO MANY FILES     ");
				
			   //SYSTEM_ERR_STATUS->bits.Image_OutOfRange_error=1;
				systerm_error_status.bits.image_outofrange_error = 1;
		     return x_nandflah_start;
	    }
		 }
		 else
		 {
			 if( Tp_addr >= (image_tab__nandflash_start*2048))//????????k??
	    {
         //LCD_DisplayStringLine( 30, "TOO MANY FILES     ");
				
			  //SYSTEM_ERR_STATUS->bits.Image_OutOfRange_error=1;
				systerm_error_status.bits.image_outofrange_error = 1;
		     return x_nandflah_start;
	    }
		 }
	}
	
	return Tp_addr;//TAB??????????
	//	SDRAM_TO_NANDFLASH(badmanage_tab_BUFFER,badmanage_nandflash_start,1);
	
}

void check_sum_nand(uint32_t x_addr,uint16_t x_width,uint16_t x_height)
{
    NAND_ADDRESS_STR WriteReadAddr;
	uint32_t Tp_i=0,Tp_j=0;
	uint16_t Tp_numread = 0;
	uint16_t  x_delay;

	while(Tp_i<2*x_width*x_height)
	{

       WriteReadAddr.Zone = 0x00;
       WriteReadAddr.Block = 0x00;
	   WriteReadAddr.Page = (x_addr+Tp_i)/2048;
       
	   if((Tp_i+2048)<2*x_width*x_height)
	   {
	        Tp_numread = 2048;
	   }
		else
		{
			Tp_numread = 2*x_width*x_height  - Tp_i;
			for(x_delay = 0;x_delay<1048;x_delay++);
		}
		
       
       NAND_ReadPage( WriteReadAddr.Page,0, bmpBuf_kkk, NAND_PAGE_SIZE);

	   Tp_i = Tp_i + Tp_numread;

       for(Tp_j=0;Tp_j<Tp_numread;Tp_j++)
	   {
		   CHECK_SUM_NAND +=bmpBuf_kkk[Tp_j];
	   }

	}
}

uint8_t get_command_xor(void)
{
  return command_xor;

}

//uint8_t bmpBuf_kkk[IMAGE_BUFFER_SIZE];
//static uint8_t Tp_Image_Buf[1600];
uint16_t Tp_u16=0,Tp_u161=0;
static uint8_t *Tp_Image_Buf,*Tp_Image_Buf_noshift;//[1600]=0;

int cpoy_file(char *pSrc, char *pDst)
{
  FRESULT result;
	FIL fsrc;
	uint16_t x_order;
	uint16_t numOfReadBytes = 0;
	uint32_t Tp_bmpmainoffset=0;
	uint32_t Tp_img_width=0;
	uint32_t Tp_img_height=0;
	uint32_t x_bmp[2];
	uint8_t Tp_bit = 0;
	static uint32_t Tp_oneline_long = 0;
	static uint32_t Tp_oneline_real_long = 0;
	uint32_t Tp_NANDFLASH_INDEX=0;
	static uint32_t Tp_sdram_addr;
	uint16_t i = 0,j;
	uint8_t Tp_lcdhigh=0,Tp_lcdlow=0;
	uint8_t Tp_data;
	static uint8_t  Tp_high;
	uint32_t Tp_i;
	static uint32_t Tp_addr;
	
	#ifdef SYSUARTPRINTF_p
	 sysprintf("cpoy_file_start\r\n");
	#endif
	
	
    sysFlushCache(I_D_CACHE);
	if(bmpBuf_kkk==0)
	{
	#ifdef SYSUARTPRINTF_p
	 sysprintf("bmpBuf_kkk = %08X,bmpBuf_kkk_bak= %08X,malloc\r\n",bmpBuf_kkk,bmpBuf_kkk_bak);
	#endif	
	bmpBuf_kkk = (uint8_t *)(((uint32_t )malloc(IMAGE_BUFFER_SIZE+64)));
	bmpBuf_kkk_bak = bmpBuf_kkk;
	bmpBuf_kkk = 	(uint8_t *)(shift_pointer((uint32_t)bmpBuf_kkk,32)+32);
	bmpBuf_kkk = (uint8_t *)((uint32_t)bmpBuf_kkk|0x80000000	);
	}
	if(bmpBuf_kkk_bak==0)
	{
		#ifdef SYSUARTPRINTF_p
	 sysprintf("bmpBuf_kkk = %08X,bmpBuf_kkk_bak= %08X, malloc NG\r\n",bmpBuf_kkk,bmpBuf_kkk_bak);
	 #endif
		return 1;
	}
	
	#ifdef SYSUARTPRINTF_p
	 sysprintf("bmpBuf_kkk = %08X,bmpBuf_kkk_bak= %08X, malloc OK\r\n",bmpBuf_kkk,bmpBuf_kkk_bak);
	 #endif
	
//	if(Tp_Image_Buf==0)
//	{
//		Tp_Image_Buf = (uint8_t *)(((uint32_t )malloc(1600)+32));
//		Tp_Image_Buf = 	(uint8_t *)shift_pointer((uint32_t)Tp_Image_Buf,32);
//	  Tp_Image_Buf = (uint8_t *)((uint32_t)Tp_Image_Buf|0x80000000	);
//	}
//	if(Tp_Image_Buf==0)
//	{
//		return 1;
//	}
		if(Tp_Image_Buf_noshift==0)
	{
		
		#ifdef SYSUARTPRINTF_p
	  sysprintf("Tp_Image_Buf = %08X,Tp_Image_Buf_noshift= %08X,malloc\r\n",Tp_Image_Buf,Tp_Image_Buf_noshift);
  	#endif	
		Tp_Image_Buf_noshift = (uint8_t *)(((uint32_t )malloc(1600)+64));
		Tp_Image_Buf = 	(uint8_t *)(shift_pointer((uint32_t)Tp_Image_Buf_noshift,32)+32);
	  Tp_Image_Buf = (uint8_t *)((uint32_t)Tp_Image_Buf|0x80000000	);
	}
	if(Tp_Image_Buf_noshift==0)
	{
		#ifdef SYSUARTPRINTF_p
	  sysprintf("Tp_Image_Buf = %08X,Tp_Image_Buf_noshift= %08X, malloc NG\r\n",Tp_Image_Buf,Tp_Image_Buf_noshift);
	  #endif
		return 1;
	}
	
	#ifdef SYSUARTPRINTF_p
	 sysprintf("Tp_Image_Buf = %08X,Tp_Image_Buf_noshift= %08X, malloc OK\r\n",Tp_Image_Buf,Tp_Image_Buf_noshift);
	 #endif
	
	gs_CpyInfo.total++;
	
		if((USB_IMAGE_TYPE=='0')||(USB_IMAGE_TYPE=='1')||(USB_IMAGE_TYPE=='2')||(USB_IMAGE_TYPE=='3'))
	x_order = (uint16_t)htoi(pSrc+24); 
	
	if((USB_IMAGE_TYPE=='A')||(USB_IMAGE_TYPE=='B')||(USB_IMAGE_TYPE=='C')||(USB_IMAGE_TYPE=='D')||(USB_IMAGE_TYPE=='E'))
	x_order = (uint16_t)htoi(pSrc+3); 
	
		#ifdef SYSUARTPRINTF_p
	 sysprintf("bmp_name=%04X.bmp\r\n",x_order);
	#endif
	
	result = f_open(&fsrc, pSrc, FA_READ);
	
	#ifdef SYSUARTPRINTF
	sysprintf("f_open result=%d\r\n",result);
	#endif
	
	if(result!=FR_OK)
	{
		systerm_error_status.bits.image_filenobmp_error=1;
		return 1;
	}
	g_file1 = &fsrc;
	
	result = f_read(&fsrc, bmpBuf_kkk, 54, (void *)&numOfReadBytes);
	
		if((numOfReadBytes == 0) || (result != FR_OK)) /*EOF or Error*/
  { 
		systerm_error_status.bits.image_filenobmp_error=1;	
		return 1;//?????????????????
	}
	//mark
	if((*(uint8_t *)bmpBuf_kkk!=0x42)||(*(uint8_t *)(bmpBuf_kkk+1)!=0x4d))//???????BMP?l? ?????l????????24???????????????
	{ 
		systerm_error_status.bits.image_filenobmp_error=1;
		return 1;
	}
	
	Tp_bmpmainoffset = *(uint8_t *)(bmpBuf_kkk+10)|(*(uint8_t *)(bmpBuf_kkk+11)<<8)|(*(uint8_t *)(bmpBuf_kkk+12)<<16)|(*(uint8_t *)(bmpBuf_kkk+13)<<24);//?????'?j??
	Tp_img_width = *(uint8_t *)(bmpBuf_kkk+18)|(*(uint8_t *)(bmpBuf_kkk+19)<<8)|(*(uint8_t *)(bmpBuf_kkk+20)<<16)|(*(uint8_t *)(bmpBuf_kkk+21)<<24);
	
	if((Tp_img_width==0)||(Tp_img_width>800))//?????????????k??
	{
		//SYSTEM_ERR_STATUS->bits.Image_BMP800480_Large_error=1;
		systerm_error_status.bits.image_bmp800480_large_error = 1;
		return 1;
	}
	Tp_img_height = *(uint8_t *)(bmpBuf_kkk+22)|(*(uint8_t *)(bmpBuf_kkk+23)<<8)|(*(uint8_t *)(bmpBuf_kkk+24)<<16)|(*(uint8_t *)(bmpBuf_kkk+25)<<24);
	if((Tp_img_height == 0)||(Tp_img_height>480))//????????????k??
	{
		systerm_error_status.bits.image_bmp800480_large_error = 1;
		return 1;
	}
	//mark
	
	if(Cover_Flag == 2)
	{
	  READ_TAB_FROMSDRAM(x_order,0,x_bmp);
		if((x_bmp[0]!=Tp_img_width)||(x_bmp[1]!=Tp_img_height))
		{
		 systerm_error_status.bits.image_coversize_error = 1;
		 return 1;
		}
	}
	if(*(uint8_t *)(bmpBuf_kkk+28)==0x18)//24bit
	{
		Tp_bit = 1;
		Tp_oneline_long = 3*Tp_img_width;
		Tp_oneline_real_long = 4*((Tp_img_width*24+31)/32);
		#ifdef SYSUARTPRINTF
		sysprintf("Tp_oneline_real_long=%X\r\n",Tp_oneline_real_long);
	    #endif
	}
	if(*(uint8_t *)(bmpBuf_kkk+28)==0x10)//16bit
	{
		Tp_bit = 2;
		Tp_oneline_long = 2*Tp_img_width;
		Tp_oneline_real_long = 4*((Tp_img_width*16+31)/32);
	}
	
	if(Tp_bit==0)
	{
		systerm_error_status.bits.image_filenobmp_error=1;
		return 1;
	}
	
	if((IMAGE_SEARCHED == 0)&&(USB_IMAGE_TYPE=='0'))
	{
		badmanage_str->BAD_MANAGE_str.NANDFLASH_CUSTOMER_INX = (uint32_t)(image_file_nandflash_start*2048);
	}
	if((IMAGE_SEARCHED == 0)&&(USB_IMAGE_TYPE=='A'))
	{
		badmanage_str->BAD_MANAGE_str.NANDFLASH_USER_INX = (uint32_t)(image_user_nandflash_start*2048);
	}
	result = f_lseek (&fsrc, Tp_bmpmainoffset);
	#ifdef SYSUARTPRINTF
	sysprintf("Tp_bmpmainoffset=%d,RES=%d\r\n",Tp_bmpmainoffset,result);
    #endif	
	if((x_order >=0xff00)&&(Cover_Flag==1))//??? ????? ???
	   Tp_NANDFLASH_INDEX = (badmanage_str->BAD_MANAGE_str.NANDFLASH_USER_INX)/2048;//??????'?????
	if((x_order >=0xff00)&&(Cover_Flag==2))//? ????? ???	
		 Tp_NANDFLASH_INDEX = *(__IO uint32_t*)(bmp_tab_BUFFER+x_order*8+4);
	if((x_order <0xff00)&&(Cover_Flag==1))//??? ????? ???
	   Tp_NANDFLASH_INDEX = (badmanage_str->BAD_MANAGE_str.NANDFLASH_CUSTOMER_INX)/2048;//??????'?????
	if((x_order <0xff00)&&(Cover_Flag==2))//? ????? ???	
		 Tp_NANDFLASH_INDEX = *(__IO uint32_t*)(bmp_tab_BUFFER+x_order*8+4);
	
	Tp_sdram_addr = bmp_layer3_BUFFER;
	
	//mark
	while (1)
  {
		memset(bmpBuf_kkk,0,IMAGE_BUFFER_SIZE);
		#ifdef  SYSUARTPRINTF
		sysprintf("Tp_oneline_real_long=%d\r\n",Tp_oneline_real_long);
		#endif
		result = f_read(&fsrc, bmpBuf_kkk, /*1450*/Tp_oneline_real_long, (void *)&numOfReadBytes);//h???800*3?????? ?????h???????
		if(result != FR_OK)//20171025 ??????l? ?? ????????U????????
		{
			#ifdef  SYSUARTPRINTF
			sysprintf("f_read err,RES=%d\r\n",result);
			#endif
			systerm_error_status.bits.image_filenobmp_error=1;
			return 1;
		}
		
		if(numOfReadBytes == 0) /*EOF or Error*/
    {
			if((Tp_sdram_addr-bmp_layer3_BUFFER)!=(2*Tp_img_width*Tp_img_height))
			{
				systerm_error_status.bits.image_filenobmp_error=1;
			  return 1;
			}
			USB_HAS_USABLE_IMG= 1;
			SAVE_TAB_TOSDRAM(x_order,Tp_NANDFLASH_INDEX,Tp_img_width,Tp_img_height);
			#ifdef  SYSUARTPRINTF
			sysprintf("save tab\r\n");
			#endif
			//??h??????
			//NANDFLASH_CUSTOMER_INDEX = NANDFLASH_CUSTOMER_INDEX + 64*2048 - (NANDFLASH_CUSTOMER_INDEX%(64*2048));//???? unit BYTE ?????????h??BLOCK ?????????????????PAGE?????? 
			//NANDFLASH_CUSTOMER_INDEX = NANDFLASH_CUSTOMER_INDEX + 2048 - (NANDFLASH_CUSTOMER_INDEX%(2048));//???
			break; 
    }
		if(numOfReadBytes !=  Tp_oneline_real_long)//??h???????j? 
		{
			if((Tp_sdram_addr-bmp_layer3_BUFFER)!=(2*Tp_img_width*Tp_img_height))
			{
				systerm_error_status.bits.image_filenobmp_error=1;
			  return 1;
			}
			USB_HAS_USABLE_IMG= 1;
			SAVE_TAB_TOSDRAM(x_order,Tp_NANDFLASH_INDEX,Tp_img_width,Tp_img_height);
				#ifdef  SYSUARTPRINTF
			sysprintf("save tab\r\n");
			#endif
			//??h??????
		//	NANDFLASH_CUSTOMER_INDEX = NANDFLASH_CUSTOMER_INDEX + 64*2048 - (NANDFLASH_CUSTOMER_INDEX%(64*2048));//???? unit BYTE ?????????h??BLOCK 
			//NANDFLASH_CUSTOMER_INDEX = NANDFLASH_CUSTOMER_INDEX + 2048 - (NANDFLASH_CUSTOMER_INDEX%(2048));//???
			break; 
			
		}
		
		if(Tp_bit == 1)//24bit ???
		{
				for(i = 0 ; i < Tp_oneline_long; i+= 3)
				{
								//	*(__IO uint16_t*)(CurrentFrameBuffer + (2*i/3)+(CNT*2))=((uint16_t)((Image_Buf[i+2] >> 3) << 11) | (uint16_t)((Image_Buf[i+1] >> 2) << 5) | (uint16_t)(Image_Buf[i] >> 3));	
							
									Tp_u16 = ((uint16_t)((*(uint8_t *)(bmpBuf_kkk+i+2)>>3 ) << 11) | (uint16_t)((*(uint8_t *)(bmpBuf_kkk+i+1)>>2 ) << 5) | (uint16_t)(*(uint8_t *)(bmpBuf_kkk+i)>>3 ));
									Tp_lcdhigh = (uint8_t)(Tp_u16>>8);	
						Tp_lcdlow  =  (uint8_t)(Tp_u16%256);	
						*(uint8_t *)(Tp_Image_Buf+((Tp_oneline_long/3)*2-(i/3)*2-2)) =Tp_lcdlow;	
						*(uint8_t *)(Tp_Image_Buf+((Tp_oneline_long/3)*2-(i/3)*2-1)) =Tp_lcdhigh;
						#ifdef  SYSUARTPRINTF
					  // sysprintf("Tp_lcdlow=%d,Tp_lcdhigh=%d\r\n",Tp_lcdlow,Tp_lcdhigh);
					#endif
						//	Image_Buf[i -i/3] =Tp_lcdlow;	
						//	Image_Buf[i -i/3+1]=Tp_lcdhigh;
				} 
				memcpy(bmpBuf_kkk,Tp_Image_Buf,Tp_img_width*2);
	  }

	
		if(Tp_bit == 2) //16bit???         
		{
				for(i = 0 ; i < Tp_oneline_long; i+= 2)
					{
					Tp_u161 = (*(uint8_t *)(bmpBuf_kkk+i+1)<<8)|*(uint8_t *)(bmpBuf_kkk+i);
					//	Tp_u161 = 0x7fff;
					 Tp_u16 = ((uint16_t)(((Tp_u161&0x7c00 )>>10)<<11) | (uint16_t)(((Tp_u161&0x3e0)>>5) << 6) | (uint16_t)((Tp_u161&0x1f)));
						//?????????????16?????RGB555??ARM LTDC?RGB565
						
						Tp_lcdhigh = (uint8_t)(Tp_u16>>8);	
					Tp_lcdlow  =  (uint8_t)(Tp_u16%256);	
					//	Image_Buf[i] =Tp_lcdlow;	
						//Image_Buf[i+1]=Tp_lcdhigh;
						
							*(uint8_t *)(Tp_Image_Buf+(Tp_oneline_long-i-2)) =Tp_lcdlow;	
						  *(uint8_t *)(Tp_Image_Buf+(Tp_oneline_long-i-1))=Tp_lcdhigh;
						

					} 
					 memcpy(bmpBuf_kkk,Tp_Image_Buf,Tp_oneline_long);
	  }
		
		for(i=0;i<(Tp_img_width*2)/2;i+=2)
		{
			j = Tp_img_width*2 - 1 -i;
			Tp_data = *(uint8_t *)(bmpBuf_kkk+i+1);
			*(uint8_t *)(bmpBuf_kkk+i+1) = *(uint8_t *)(bmpBuf_kkk+j);
			 *(uint8_t *)(bmpBuf_kkk+j) = Tp_data;
			
			//j = Tp_img_width*2 - 1 -(i+1);
			j--;
			Tp_data = *(uint8_t *)(bmpBuf_kkk+i);
			*(uint8_t *)(bmpBuf_kkk+i) = *(uint8_t *)(bmpBuf_kkk+j);
			 *(uint8_t *)(bmpBuf_kkk+j) = Tp_data;			
		}
		
		
		if((Tp_sdram_addr+Tp_img_width*2) > (bmp_layer3_BUFFER + 800*480*2))
		{
			//LCD_DisplayStringLine( 30, "TOO LARGE FILES    ");
			systerm_error_status.bits.image_bmp800480_large_error = 1;
				#ifdef  SYSUARTPRINTF
			sysprintf("large err");
			#endif
			return 1 ;
		}
			#ifdef  SYSUARTPRINTF
		 sysprintf("Tp_sdram_addr=%X\r\n",Tp_sdram_addr);
		 sysprintf("bmpBuf_kkk=%X,%X,%X,%X,%X,%X\r\n",*(uint8_t *)(bmpBuf_kkk),*(uint8_t *)(bmpBuf_kkk+1),*(uint8_t *)(bmpBuf_kkk+2),*(uint8_t *)(bmpBuf_kkk+3),*(uint8_t *)(bmpBuf_kkk+4),*(uint8_t *)(bmpBuf_kkk+5));
		#endif
		memcpy((uint8_t*)Tp_sdram_addr,bmpBuf_kkk,Tp_img_width*2);
		Tp_sdram_addr = Tp_sdram_addr + Tp_img_width*2;
		
				
	}	
	#ifdef  SYSUARTPRINTF
	sysprintf("layer3_BUFFER0=%X,%X,%X,%X,%X,%X\r\n",*(uint8_t *)(bmp_layer3_BUFFER),*(uint8_t *)(bmp_layer3_BUFFER+1),*(uint8_t *)(bmp_layer3_BUFFER+2),*(uint8_t *)(bmp_layer3_BUFFER+3),*(uint8_t *)(bmp_layer3_BUFFER+4),*(uint8_t *)(bmp_layer3_BUFFER+5));
	sysprintf("layer3_BUFFER1=%X,%X,%X,%X,%X,%X\r\n",*(uint8_t *)(bmp_layer3_BUFFER+6),*(uint8_t *)(bmp_layer3_BUFFER+7),*(uint8_t *)(bmp_layer3_BUFFER+8),*(uint8_t *)(bmp_layer3_BUFFER+9),*(uint8_t *)(bmp_layer3_BUFFER+10),*(uint8_t *)(bmp_layer3_BUFFER+11));
	#endif
	for(Tp_high = 0;Tp_high < Tp_img_height/2;Tp_high++)
	{
		memcpy((uint8_t *)bmpBuf_kkk,(uint8_t *)(bmp_layer3_BUFFER+2*Tp_high*Tp_img_width),2*Tp_img_width);
		memcpy((uint8_t *)(bmp_layer3_BUFFER+2*Tp_high*Tp_img_width),(uint8_t *)(bmp_layer3_BUFFER+2*(Tp_img_height-Tp_high-1)*Tp_img_width),2*Tp_img_width);
		memcpy((uint8_t *)(bmp_layer3_BUFFER+2*(Tp_img_height-Tp_high-1)*Tp_img_width),(uint8_t *)bmpBuf_kkk,2*Tp_img_width);
	}
	#ifdef  SYSUARTPRINTF
	sysprintf("layer3_BUFFER2=%X,%X,%X,%X,%X,%X\r\n",*(uint8_t *)(bmp_layer3_BUFFER),*(uint8_t *)(bmp_layer3_BUFFER+1),*(uint8_t *)(bmp_layer3_BUFFER+2),*(uint8_t *)(bmp_layer3_BUFFER+3),*(uint8_t *)(bmp_layer3_BUFFER+4),*(uint8_t *)(bmp_layer3_BUFFER+5));
	sysprintf("layer3_BUFFER3=%X,%X,%X,%X,%X,%X\r\n",*(uint8_t *)(bmp_layer3_BUFFER+6),*(uint8_t *)(bmp_layer3_BUFFER+7),*(uint8_t *)(bmp_layer3_BUFFER+8),*(uint8_t *)(bmp_layer3_BUFFER+9),*(uint8_t *)(bmp_layer3_BUFFER+10),*(uint8_t *)(bmp_layer3_BUFFER+11));
#endif
	//while(1);
	//??????j??h??USB??CHECKSUN
	if(flag_checksum==1)
	{
	for(Tp_i=0;Tp_i<2*Tp_img_width*Tp_img_height;Tp_i++)
	{
          CHECK_SUM_USB+= *(uint8_t *)(bmp_layer3_BUFFER+Tp_i);
	}
	}

  	#ifdef  SYSUARTPRINTF
	sysprintf("Tp_NANDFLASH_INDEX=0x%x\n\r",Tp_NANDFLASH_INDEX);
	#endif
	 Tp_addr = SAVE_IMAGE_20170816(Tp_NANDFLASH_INDEX*2048,Tp_sdram_addr,x_order,Cover_Flag);
	
	if(systerm_error_status.bits.image_outofrange_error == 1)
	{
		return 1;
	}
	
  if((x_order >=0xff00)&&(Cover_Flag==1))//??? ????? ???
	   badmanage_str->BAD_MANAGE_str.NANDFLASH_USER_INX = Tp_addr;//??????'?????
	if((x_order <0xff00)&&(Cover_Flag==1))//??? ????? ???
	   badmanage_str->BAD_MANAGE_str.NANDFLASH_CUSTOMER_INX = Tp_addr;//??????'?????
	
	
	  IMAGE_SEARCHED = 1;	
		gs_CpyInfo.okCnt++;
	
    //??????h??CHECKSUM
	if(flag_checksum==1)
	{
	check_sum_nand(Tp_NANDFLASH_INDEX*2048,Tp_img_width,Tp_img_height);
	}
	#ifdef  SYSUARTPRINTF
	sysprintf("checksum=0x%x,0x%x\n\r",CHECK_SUM_USB,CHECK_SUM_NAND);
	#endif
	//if(bmpBuf_kkk) free(bmpBuf_kkk);
  //if(Tp_Image_Buf) free(Tp_Image_Buf);
  if(bmpBuf_kkk_bak) 
  {
		#ifdef SYSUARTPRINTF_p
	 sysprintf("bmpBuf_kkk=%08X,bmpBuf_kkk_bak=%08X, free\r\n",bmpBuf_kkk,bmpBuf_kkk_bak);
	 #endif
		free(bmpBuf_kkk_bak);
		bmpBuf_kkk=0;
		bmpBuf_kkk_bak=0;
		
	}
	else
	{
		#ifdef SYSUARTPRINTF_p
	 sysprintf("bmpBuf_kkk=%08X,bmpBuf_kkk_bak=%08X, not free\r\n",bmpBuf_kkk,bmpBuf_kkk_bak);
	 #endif
	}
	
  if(Tp_Image_Buf_noshift) 
	{	
		#ifdef SYSUARTPRINTF_p
	 sysprintf("Tp_Image_Buf=%08X,Tp_Image_Buf_noshift=%08X, free\r\n",Tp_Image_Buf,Tp_Image_Buf_noshift);
	 #endif
	free(Tp_Image_Buf_noshift);
		Tp_Image_Buf_noshift =0;
		Tp_Image_Buf=0;
	}
	else
	{
		#ifdef SYSUARTPRINTF_p
	 sysprintf("Tp_Image_Buf=%08X,Tp_Image_Buf_noshift=%08X, not free\r\n",Tp_Image_Buf,Tp_Image_Buf_noshift);
	 #endif
	}
	
	
	#ifdef SYSUARTPRINTF_p
	 sysprintf("cpoy_file_end\r\n");
	#endif
	
	
	
	return 0;
}

void NANDFLASH_backup_checksum(void)
{
	uint32_t Tp_check=0,Tp_i=0;
	for(Tp_i=0;Tp_i<(sizeof(badmanage_str->BAD_MANAGE_arr)-4);Tp_i++)
	{
		Tp_check = Tp_check + badmanage_str->BAD_MANAGE_arr[Tp_i];
	}
	badmanage_str->BAD_MANAGE_str.backup_checksum=Tp_check;
}

void NANDFLASH_P3PD_INX_SAVE(void)
{
	 //void *addr;
	 //addr = (void *)(badmanage_tab_BUFFER + 0x10000);
	 //*(uint32_t*)addr =badmanage_str->NANDFLASH_CUSTOMER_INX  ;
	 //addr = (void *)(badmanage_tab_BUFFER + 0x10004);
	 //*(uint32_t*)addr =badmanage_str->NANDFLASH_USER_INX ;
	 //SDRAM_TO_NANDFLASH((uint32_t)badmanage_str,backup_tab_nandflash_start,1);
//	sprintf("badmanage_str->BAD_MANAGE_str.NANDFLASH_USER_INX=%x,save\n\r",badmanage_str->BAD_MANAGE_str.NANDFLASH_USER_INX);
	NANDFLASH_backup_checksum();
	NAND_EraseBlock(backup_tab_nandflash_start);
	NAND_WritePage(backup_tab_nandflash_start,0,badmanage_str->BAD_MANAGE_arr,sizeof(badmanage_str->BAD_MANAGE_arr));
	W25Q128_Write(access_BLOCK_0_BACKUP);
}
	
int check_u_disk_img_file(char *udir)  //���u��BELMONT_BMPXX�ļ�����û��ͼ�� 0 ��ʾ�� 1��ʾ��
{
	FRESULT res;
//	char tmpFileName[64];
//	char nandFileName[64];
//	int fIdx = 0;
	FILINFO fileinfo;
	DIR dir;
	int ret = 0;

	res = f_opendir(&dir, udir); //��һ��Ŀ¼
	if (res == FR_OK)
	{
		while (1)
		{
			res = f_readdir(&dir, &fileinfo);                   //��ȡĿ¼�µ�һ���ļ�
			if (res != FR_OK || fileinfo.fname[0] == 0)
			{
				break;  //������/��ĩβ��,�˳�
			}
			if (fileinfo.fname[0] == '.')
			{
				continue;             //�����ϼ�Ŀ¼
			}
			if (fileinfo.fattrib & AM_DIR) //���ļ���
			{
				continue;
			}

			if (strstr(fileinfo.fname, ".bmp"))
			{
				if (strlen(fileinfo.fname) == 8)
				{
					ret=1;
					break;
				}
			}

			else if (strstr(fileinfo.fname, ".db"))//Thumbs.db  U��Ĭ��������ļ��������ص�
			{
			
				;
			}
			else   //
			{
					
			}
		}
		f_closedir(&dir);
	}

	return ret;
}

int check_u_disk_img(void) //  ���u����û��ͼ�� return  0 ��ʾû�У�1��ʾ��   
{
	FRESULT res;
	char dirPath[64]={0};
	FILINFO fileinfo={0};
	DIR dir={0};
	int ret = 0;

	res = f_readdir(&dir, NULL);
	
	
	res = f_opendir(&dir, "3:"); //��һ��Ŀ¼
	
	fileinfo.fsize = 60;
	#ifdef  SYSUARTPRINTF 
	sysprintf("f_opendir=%x\r\n",res);
	#endif
	if (res == FR_OK)
	{
		
		while (1)
		{
			res = f_readdir(&dir, &fileinfo);                   //��ȡĿ¼�µ�һ���ļ�
			if (res != FR_OK || fileinfo.fname[0] == 0)
			{
				break;  //������/��ĩβ��,�˳�
			}
			if (fileinfo.fname[0] == '.')
			{
				continue;             //�����ϼ�Ŀ¼
			}
			if (fileinfo.fattrib & AM_DIR) //���ļ���
			{
				
				if (memcmp(fileinfo.fname,"BELMONT_BMP",11) == 0)
				{
          #ifdef  SYSUARTPRINTF 
					sysprintf("found image\r\n");
					#endif
					sprintf(dirPath, "3:/%s", fileinfo.fname);
					if(strlen(dirPath)==23)
					ret = check_u_disk_img_file(dirPath);  //, "0:/BELMONT_BMP", flag);
				}
				
			}
		}
		f_closedir(&dir);
	}

	return ret;
}

void Clear_sdram_imagetab(void)
{
	  static uint16_t Tp_i;
	  switch(USB_IMAGE_TYPE)
		{
			case '0'://0000-FEFF
				for(Tp_i = 0; Tp_i < 0xff00;Tp_i++)
			  {
				   *(__IO uint16_t*) (bmp_tab_BUFFER+Tp_i*8) = 0;
					 *(__IO uint16_t*) (bmp_tab_BUFFER+Tp_i*8+2) = 0;
					 *(__IO uint32_t*) (bmp_tab_BUFFER+Tp_i*8+4) = 0;
				}
			//	SDRAM_TO_NANDFLASH(bmp_tab_BUFFER,image_tab__nandflash_start,3);
			//	????????????????
			 break;
			case 'A'://FF00-FFFF
				for(Tp_i = 0xff00; Tp_i>=0xff00 ;Tp_i++)
			  {
					 *(__IO uint16_t*) (bmp_tab_BUFFER+Tp_i*8) = 0;
					 *(__IO uint16_t*) (bmp_tab_BUFFER+Tp_i*8+2) = 0;
					 *(__IO uint32_t*) (bmp_tab_BUFFER+Tp_i*8+4) = 0;
				}
			//	SDRAM_TO_NANDFLASH(bmp_tab_BUFFER,image_tab__nandflash_start,3);
			//	????????????????
				break;
			default:
				break;
		}
}

FILINFO fileinfoooooo={0};

int CpyDirBmpFileToDir(char *udir,char *ndir,int flag)
{
	FRESULT res;
	char tmpFileName[64]={0};
	char nandFileName[64]={0};
	int fIdx = 0;
	
	DIR dir={0};
	int ret = 0;
	uint32_t x_bmp[2];
	
  
	
	res = f_opendir(&dir, udir); //??????
	if (res == FR_OK)
	{
		while (1)
		{
			//if(Get_volt_normal()==0)//??????????? ?????????
			//{
			//	 break;
			//}
//////////////////////////////////////////////////////////////////////			if(FLAG_SW3_BEGIN != 0xffff)
//////////////////////////////////////////////////////////////////////			{
//////////////////////////////////////////////////////////////////////				FLAG_SW3_BEGIN++;
//////////////////////////////////////////////////////////////////////				SetZuobiao(10, 400 + 40);
//////////////////////////////////////////////////////////////////////				if(FLAG_SW3_BEGIN%2==0)
//////////////////////////////////////////////////////////////////////				{
/////////////////////////??/////////////////////////////////////////					lcd_printf_new("Rewrite All Image");
//////////////////////////////////////////////////////////////////////				}
//////////////////////////////////////////////////////////////////////				if(FLAG_SW3_BEGIN%2==1)
//////////////////////////////////////////////////////////////////////				{
//////////////////////////////////////////////////////////////////////					lcd_printf_new("                 ");
//////////////////////////////////////////////////////////////////////				}
//////////////////////////////////////////////////////////////////////			}
			
			fileinfoooooo.fsize = 64;
			
			res = f_readdir(&dir, &fileinfoooooo);                   //??????????
		//	if ((res != 0) || (fileinfo.fname[0] == 0))
			if((res != 0) || (fileinfoooooo.fname[0] == 0x0))
			{
			//	res = f_readdir(&dir, &fileinfoooooo);
			//	if((res != 0) || (fileinfoooooo.fname[0] == 0x0))
			//	{
				break;  //???/????,??
			//	}
			}
			if (fileinfoooooo.fname[0] == '.')
			{
				continue;             //??????
			}
			if (fileinfoooooo.fattrib & AM_DIR) //????
			{
				continue;
			}

			if (strstr(fileinfoooooo.fname, ".bmp"))
			{
				if (strlen(fileinfoooooo.fname) == 8)
				{
					if (sscanf(fileinfoooooo.fname, "%04x", &fIdx) == 1)
					{
						
						sprintf(tmpFileName, "%s/%s", udir, fileinfoooooo.fname);
						//sprintf(nandFileName, "%s/%s", ndir, fileinfo.fname);
						SetZuobiao(10, 400 + 20);
						if(FLAG_SW3_BEGIN == 0)
						{
						lcd_printf_new(" %80s  "," ");
						}
						else
						{
						lcd_printf(" %80s  "," ");	
						}
						SetZuobiao(10, 400 + 20);
						if(FLAG_SW3_BEGIN == 0)
						{
						lcd_printf_new("find bmp file : %s          ", tmpFileName);
						}
						else
						{
						lcd_printf("find bmp file : %s          ", tmpFileName);		
						}

							
						if (flag == 1)
						{
							//if (CheckFile(nandFileName) == 0)
							if((((*(char *)(ndir))=='0')&&((uint16_t)htoi(fileinfoooooo.fname)>=0xff00))||
								(((*(char *)(ndir))=='1')&&((uint16_t)htoi(fileinfoooooo.fname)<0xff00)))
							{
								continue;
							}
							else if(READ_TAB_FROMSDRAM((uint16_t)htoi(fileinfoooooo.fname),0,x_bmp)==0xffffffff)
							
							{
								Cover_Flag = 1;
								ret = cpoy_file(tmpFileName, nandFileName);
								if (ret > 0) 
								{
									break;
								}
							}
							else
							{
								systerm_error_status.bits.image_samefilename_error = 1;
								lcd_printf("...skip", tmpFileName);
							}
						}
						else if(flag == 2)
						{
							if (READ_TAB_FROMSDRAM((uint16_t)htoi(fileinfoooooo.fname),0,x_bmp)!=0xffffffff)
							{
								Cover_Flag = 2;
								ret = cpoy_file(tmpFileName, nandFileName);
								if (ret > 0) break;
							}
						}
						else if(flag == 3)
						{
							 if(READ_TAB_FROMSDRAM((uint16_t)htoi(fileinfoooooo.fname),0,x_bmp)==0xffffffff)
							 {
								 Cover_Flag = 1;
								 ret = cpoy_file(tmpFileName, nandFileName);
								 if (ret > 0) 
								 {
								 	break;
								 }
							 }
							 else
							 {
								  Cover_Flag = 2;
								  ret = cpoy_file(tmpFileName, nandFileName);
								  if (ret > 0) break;
							 }
						}
						else if(flag == 0)
						{
							Cover_Flag = 1;
							ret = cpoy_file(tmpFileName, nandFileName);
							if (ret > 0) 
							{
								break;
							}
						}

					}
				}
			}

			else if (strstr(fileinfoooooo.fname, ".db"))//Thumbs.db  U????????,????
			{
			
;
			}
			else   //qdhai add 
			{
			//		systerm_error_status.bits.image_filenobmp_error=1; //qdhai add 
				systerm_error_status.bits.usb_cannot_find_image=1;
			}
		}
		f_closedir(&dir);
	}
  
	return ret;
}

int CheckUsbDirFile(int cmd,int flag) //(0, 0);
{
	FRESULT res;
	char dirPath[64]={0};
	FILINFO fileinfo;
	DIR dir;
	int ret = 0;

//	f_mount(0, "3:", 1);
//	res = f_mount(&usb_fs, "3:", 1); 
	
	
	res = f_opendir(&dir, "3:"); //��һ��Ŀ¼
	#ifdef  SYSUARTPRINTF 
	sysprintf("f_opendir=%X\r\n",res);
	#endif
	if (res == FR_OK)
	{
		while (1)
		{
			res = f_readdir(&dir, &fileinfo);                   //��ȡĿ¼�µ�һ���ļ�
			if (res != FR_OK || fileinfo.fname[0] == 0)
			{
				break;  //������/��ĩβ��,�˳�
			}
			if (fileinfo.fname[0] == '.')
			{
				continue;             //�����ϼ�Ŀ¼
			}
			if (fileinfo.fattrib & AM_DIR) //���ļ���
			{
				//if (strlen(fileinfo.fname) == 20)
				//{
					if (memcmp(fileinfo.fname,"BELMONT_BMP_",12) == 0)
					{
            #ifdef  SYSUARTPRINTF 
						sysprintf("BELMONT_BMP_find\r\n");
						#endif
						sprintf(dirPath, "3:/%s", fileinfo.fname);
						if(strlen(dirPath)==23)
						{
						  ret = CpyDirBmpFileToDir(dirPath, "0:/BELMONT_BMP", flag);
						}
						if(IMAGE_SEARCHED)
						{
							 SDRAM_TO_NANDFLASH(bmp_tab_BUFFER,image_tab__nandflash_start,4);
							 NANDFLASH_P3PD_INX_SAVE();
							 Clear_sdram(0X4F);
							 *(uint8_t *)(BaseData_ARR+BMP_Ver_index*9+8) = 8;
  							memcpy(BaseData_ARR+BMP_Ver_index*9, dirPath+15 , *(uint8_t *)(BaseData_ARR+BMP_Ver_index*9+8));
							  Updata_Basedata_No0_cyw();
			          basedata_ram_to_sdram();
			          SDRAM_TO_NANDFLASH(logodata_basedata_BUFFER,baseA_data__nandflash_start,1);
			          SDRAM_TO_NANDFLASH(logodata_basedata_BUFFER,baseB_data__nandflash_start,1);
							  W25Q128_Write(accsee_BASEDATA_PARA_5INCH);
							 break;
						}
//						if(ret == 0)
//						{
//							BaseData_Str->areaData[BMP_Ver_index].size = 8;
//							memcpy(BaseData_Str->areaData[BMP_Ver_index].data, dirPath+15 ,BaseData_Str->areaData[BMP_Ver_index].size);
//							SaveAreaData(0, 0);
//						}
					}
				//}
			}
		}
		f_closedir(&dir);
	}

	return ret;
}

int CheckFile(char *FileName)
{
	FRESULT result;
//	UINT br;
	FIL fsrc;
	 sysprintf("FileName = %s\r\n",FileName);
	//HAL_NVIC_DisableIRQ(PVD_IRQn);
	result = f_open(&fsrc, FileName, FA_READ);
	g_file1 = &fsrc;
	//HAL_NVIC_EnableIRQ(PVD_IRQn);
	
	if (result == FR_OK)
	{
		//HAL_NVIC_DisableIRQ(PVD_IRQn);
		#ifdef  SYSUARTPRINTF 
		sysprintf("OPEN OK\r\n");
		#endif
		f_close(&fsrc);
		//g_open_flag1 = 0;
   // HAL_NVIC_EnableIRQ(PVD_IRQn);
		return 1;
	}
	
	#ifdef  SYSUARTPRINTF 
   sysprintf("OPEN NG\r\n");
	#endif
	//g_open_flag1 = 0;
   return 0;
}

void UsbWriteNandFlash(char cmd, unsigned short *iFile, int cnt) 
{
  char tmpFileName[64];
	char nandFileName[64];
	FRESULT res;
	int ret = 0;
	char path[64];
	int i;
	uint32_t x_bmp[2];
	uint8_t ack;
	uint32_t Tp_idx ;
	
	if (gs_usb_mount_flag != 1)
	{
		systerm_error_status.bits.usb_unable_connect=1; 
		return;
	}
	
	if (cmd > U2N_PE) return;
	if ((cmd > U2N_P3) && (cmd < U2N_PA)) return;
	
	if (cmd <= U2N_P3)
	{
		if(check_u_disk_img( )==0)
		{
			///ack = 0X1C;
			///code_protocol_ack(get_command_xor(), 1, &ack,0);
			systerm_error_status.bits.usb_cannot_find_image = 1;
			#ifdef  SYSUARTPRINTF 
			sysprintf("check_u_disk_img=0\r\n");
			#endif
			return;
		}
	}
	
		if (cmd == U2N_P0)
	{
	  #ifdef  SYSUARTPRINTF 
		sysprintf("Clear_sdram_imagetab\r\n");
		#endif
		Clear_sdram_imagetab();
	}

	if (cmd == U2N_PA)
	{
	
		Clear_sdram_imagetab();
	}
	
	
	if (cmd <= U2N_P3)
	{
		sprintf(path, "%s", "3:/BELMONT_BMP");
  }
	else
	{
    sprintf(path, "%s", "3:");
	}
	
	gs_CpyInfo.total = 0;
	gs_CpyInfo.okCnt = 0;
	gs_CpyInfo.failedCnt = 0;
	
	if (cmd != U2N_PE)
	{
		if (cmd <= U2N_P3)
		{
			if (cmd == U2N_P1)//P1
			{
				ret =CheckUsbDirFile(0, 1);
			}
			else if (cmd == U2N_P2)//P2
			{
				ret =CheckUsbDirFile(0, 2);
			}
			else if(cmd == U2N_P3)//P0 P3
			{
				ret =CheckUsbDirFile(0, 3);
			}
			else if(cmd == U2N_P0)
			{
				ret =CheckUsbDirFile(0, 0);
			}
			
		}
		else
		{
			if (cmd == U2N_PB)//PB
			{
				ret = CpyDirBmpFileToDir("3:", "1:/USER_BMP", 1);
			}
			else if(cmd == U2N_PC)//PC
			{
				ret = CpyDirBmpFileToDir("3:", "1:/USER_BMP", 2);
			}
			else if(cmd == U2N_PD)//PA PD 
			{
				ret = CpyDirBmpFileToDir("3:", "1:/USER_BMP", 3);
			}
			else if(cmd == U2N_PA)
			{
				ret = CpyDirBmpFileToDir("3:", "1:/USER_BMP", 0);
			}
				
			
			if(IMAGE_SEARCHED)
			{
							 SDRAM_TO_NANDFLASH(bmp_tab_BUFFER+3*64*2048,image_tab__nandflash_start+3*64,1);
				       //NANDFLASH_TO_SDRAM(bmp_tab_BUFFER,image_tab__nandflash_start,4);
				       #ifdef  SYSUARTPRINTF 
				       sysprintf("TAB SAVE FINISH\r\n");
				       #endif
							 NANDFLASH_P3PD_INX_SAVE();
							 Clear_sdram(0X4F);
			}
		}
	}
	else if (cmd == U2N_PE)
	{
	   for (i = 0; i < cnt; i++)
		{
		sprintf(tmpFileName, "%s/%04x.bmp", path, *iFile);
			sprintf(nandFileName, "%s/%04x.bmp", "1:/USER_BMP", *iFile);
			if (*iFile >= 0xff00)
			{
		        if (CheckFile(tmpFileName) == 0)//���ﵼ��PE����ͼƬ��ʱ�� ֻҪ��һ���Ҳ����ͻᱨ206
				    {
					       systerm_error_status.bits.usb_cannot_find_image = 1;
					       break;
			     	}
            if(READ_TAB_FROMSDRAM(*iFile,0,x_bmp)==0xffffffff)
		       {
					      Cover_Flag = 1;
				   }
			   	else
				   {
					      Cover_Flag = 2;
				   }				
				   // Cover_Flag = 1;
		        ret = cpoy_file(tmpFileName, nandFileName);
		        if(IMAGE_SEARCHED)
			      {
							 SDRAM_TO_NANDFLASH(bmp_tab_BUFFER+3*64*2048,image_tab__nandflash_start+3*64,1);
							 NANDFLASH_P3PD_INX_SAVE();
							 Clear_sdram(0X4F);
			      }
			 	
				
				  if (ret > 0) break;
			 }
		     iFile++;
			}
	}
	
//		gs_Sdram_wrtie_pos = SDRAM_BMP_ADDR;
	//////////////////////////////////////////////////////////////////////////
  	for(Tp_idx=0;Tp_idx<65536;Tp_idx++)
		{
			//Tp_pBuf  = (SDRAM_Q_TAB + 8 *Tp_idx + 4);
			if(*(uint32_t *)(SDRAM_Q_TAB + 8 *Tp_idx + 4)!=0)
			{
				//free((void*)(SDRAM_Q_TAB + 8 *Tp_idx + 4));
				free((void*)(((*(uint32_t *)(SDRAM_Q_TAB + 8 *Tp_idx + 4))&~0x80000000)));
			}
		}
		memset((void *)SDRAM_Q_TAB,0,65536*8);
	  memset((void *)SDRAM_Q_SHIFT_TAB,0,65536*8);
	
	
	if(FLAG_SW3_BEGIN == 0)
	{
	//ClearLayerData(4);
	SetZuobiao(10, 400 + 40);	
	lcd_printf_new("copy bmp file end,total %d success %d failed %d",gs_CpyInfo.total,gs_CpyInfo.okCnt,gs_CpyInfo.failedCnt);
	}
	else
	{
	SetZuobiao(10, 400 + 40);	
	lcd_printf("copy bmp file end,total %d success %d failed %d",gs_CpyInfo.total,gs_CpyInfo.okCnt,gs_CpyInfo.failedCnt);
	}
	
	if (gs_CpyInfo.total == 0) systerm_error_status.bits.usb_cannot_find_image = 1;

	FLAG_SW3_BEGIN = 0xffff;
	
	if(gs_CpyInfo.okCnt==0)
	{
		systerm_error_status.bits.usb_cannot_find_image = 1;
		
	}
	
	//if(gs_CpyInfo.total ==gs_CpyInfo.okCnt)  //if (ret == 0)
	{

/*
uint32_t  image_samefilename_error:1;//23 ׷����дʱ�ļ����ظ�  204
uint32_t  image_coversize_error:1;//24 ���Ǵ�С����  205
uint32_t  image_filenobmp_error:1;//25 ����bmp�ļ�   203
uint32_t  usb_cannot_find_image:1;//16// u���Ҳ���ͼ���ļ�  206
*/
	if(systerm_error_status.bits.image_samefilename_error\
		||systerm_error_status.bits.image_coversize_error\
		||systerm_error_status.bits.image_filenobmp_error\
	  ||systerm_error_status.bits.image_outofrange_error\
	  ||systerm_error_status.bits.image_bmp800480_large_error)
		{
		  systerm_error_status.bits.usb_cannot_find_image=0;

		}
		
		if(systerm_error_status.bits.image_samefilename_error\
		||systerm_error_status.bits.image_coversize_error\
		||systerm_error_status.bits.image_filenobmp_error\
	  ||systerm_error_status.bits.image_outofrange_error\
	  ||systerm_error_status.bits.image_bmp800480_large_error\
		||systerm_error_status.bits.usb_cannot_find_image)
		{
		   
		}
     else	//if()
	 	{      	
			ack = 0X1C;
			code_protocol_ack(get_command_xor(), 1, &ack,0);
    }
	}
	
//	if(ret)//5��������λ��  201  ���ܲ���NAND��Ҳ����201
//	{
//		systerm_error_status.bits.image_outofrange_error=1; 
//	}
//	switch (ret)
//	{
//	case ERR_CREAT_FAILED:
//	case ERR_WRITE_FAILED:
//		systerm_error_status.bits.image_outofrange_error=1; 
//	   break;
//	case ERR_OPEN_FAILED:
//		systerm_error_status.bits.usb_cannot_find_hexortxt=1; 
//	   break;
//	default:
//		break;
//	}

}

void nandflash_format(void)
{
	
	badmanage_str->BAD_MANAGE_str.flag = 0;
	NANDFLASH_BADMANAGE_INIT();
	
	Clear_sdram(0x10);
	SDRAM_TO_NANDFLASH(bmp_tab_BUFFER,image_tab__nandflash_start,4);
	
	
	
	systerm_error_status.bits.nandflash_error =0;
	LOGO_ERR=0;
	BASEDATA_RAM_INIT();
	
//	Clear_sdram(0xFFFFFFFF);
//	SDRAM_TO_NANDFLASH(bmp_tab_BUFFER,image_tab__nandflash_start,4);
//	
//	
//	gs_AreaInfo[0].addr=0;
//	
////	memset(BaseData_ARR,0,164*9);
////	ReadAreaConfig_new();
//	
//	badmanage_str->BAD_MANAGE_str.flag = 0;
//	systerm_error_status.bits.nandflash_error = 0;
//	
//	SDRAM_DATA_INIT();
//	
//	
//	
////	NANDFLASH_BADMANAGE_INIT();
	
	
	
	
	
	
	
	

}

