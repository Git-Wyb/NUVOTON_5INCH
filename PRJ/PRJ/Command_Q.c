#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "sys.h"
#include "Command_all.h"
#include "BSP_Init.h"

extern LAYER_SDRAM_STR display_layer_sdram;
extern SYSTEMERRORSTATUS_TYPE  systerm_error_status;
uint32_t gs_Sdram_wrtie_pos ;
uint32_t shift_pointer(uint32_t ptr, uint32_t align);

int CpyNandToSdramCMd(char cmd, uint16_t *fileName,int cnt)
{
	//static char file[128];
	int i = 0;
	uint16_t x=0, y=0;
	uint16_t x_uint16,y_uint16;
	uint16_t width=0, height=0;
	void *pBuf;
	//char dirPath[24] = {0};
	void *addr;
	uint32_t size_tab[2];
	uint32_t *Tp_pBuf;
	uint32_t Tp_idx ;
	static uint8_t Tp_mark=0;
//	if()
	if ((cmd == N2S_Q0)&&(Tp_mark==1)) 
	{
			#ifdef  SYSUARTPRINTF_ActionTimers
		sysprintf("clear Q tab\r\n");
		#endif
		for(Tp_idx=0;Tp_idx<65536;Tp_idx++)
		{
			//Tp_pBuf  = (SDRAM_Q_TAB + 8 *Tp_idx + 4);
			if(*(uint32_t *)(SDRAM_Q_TAB + 8 *Tp_idx + 4)!=0)
			{
				#ifdef  SYSUARTPRINTF_ActionTimers
				sysprintf("clear Q=%X\r\n",Tp_idx);
				#endif
			//	sysSetGlobalInterrupt(DISABLE_ALL_INTERRUPTS);
				free((void*)(((*(uint32_t *)(SDRAM_Q_TAB + 8 *Tp_idx + 4))&~0x80000000)));
			//	sysSetGlobalInterrupt(ENABLE_ALL_INTERRUPTS);
				#ifdef SYSUARTPRINTF_ActionTimers
				sysprintf("clear Q=%X\r\n",Tp_idx);
				sysprintf("free addr=%x\r\n",(((*(uint32_t *)(SDRAM_Q_TAB + 8 *Tp_idx + 4))&~0x80000000))); 
				#endif
				
				#ifdef  SYSUARTPRINTF_ActionTimers
				sysprintf("clear Q=%X\r\n",Tp_idx);
				sysprintf("free addr=%x\r\n",*(uint32_t *)(SDRAM_Q_TAB + 8 *Tp_idx + 4)); 
				#endif
			}
		}
		Tp_mark = 0;
		memset((void *)SDRAM_Q_TAB,0,65536*8);
		memset((void *)SDRAM_Q_SHIFT_TAB,0,65536*4);
		
	//	gs_Sdram_wrtie_pos = SDRAM_BMP_ADDR;
	}
	
	for (i = 0; i < cnt; i++)//只有0 1 两种情况
	{

		Tp_mark = 1;
		if (GetSdramImgInfoByIdx(*fileName, &width, &height, &pBuf) == 0)//查找是否有图片，如果没有
		{
			 
			if(READ_TAB_FROMSDRAM(*fileName,0,size_tab) == 0xffffffff)
		   {
				systerm_error_status.bits.nand_canot_find_Q_bmp_error=1; 
				return 0;
		   }
		
			gs_Sdram_wrtie_pos = ((uint32_t )malloc((size_tab[0]+size_tab[0]%2 )* size_tab[1] * 2+64));
			 #ifdef SYSUARTPRINTF_ActionTimers
			 sysprintf("Q_fileName=%04X,gs_Sdram_wrtie_pos_noshift=%08X\r\n",*fileName,gs_Sdram_wrtie_pos); 
			 #endif
			 
			 //free((void *)gs_Sdram_wrtie_pos);
			 *(uint32_t *)(SDRAM_Q_SHIFT_TAB + 4 **fileName ) = (32+shift_pointer(gs_Sdram_wrtie_pos,32))|0x80000000;
			 
			  #ifdef SYSUARTPRINTF_ActionTimers
			 sysprintf("gs_Sdram_wrtie_pos_shift=%08X\r\n",*(uint32_t *)(SDRAM_Q_SHIFT_TAB + 4 **fileName )); 
			 #endif
			 
		//	gs_Sdram_wrtie_pos =  shift_pointer(gs_Sdram_wrtie_pos,32);
		//	gs_Sdram_wrtie_pos|=0x80000000;
			if((gs_Sdram_wrtie_pos==NULL)||(gs_Sdram_wrtie_pos>0x7b00000))//放不下
			{
				 if(gs_Sdram_wrtie_pos)
				 {
					 free((void *)gs_Sdram_wrtie_pos);
					 gs_Sdram_wrtie_pos = 0;
					 *(uint32_t *)(SDRAM_Q_SHIFT_TAB + 4 **fileName ) =0;
					 *(uint32_t *)(SDRAM_Q_TAB + 4 **fileName ) =0;
				 }
				systerm_error_status.bits.image_movenantosdram_error = 1;
				 return 0;
			}
			
			//gs_Sdram_wrtie_pos = gs_Sdram_wrtie_pos|0x80000000;
			#ifdef  SYSUARTPRINTF_ActionTimers
			sysprintf("gs_Sdram_wrtie_pos=%X,*fileName=%X\r\n",gs_Sdram_wrtie_pos,*fileName);
			#endif
//			
			if (NandToSdramBmp(*fileName, &x, &y, (void *)( *(uint32_t *)(SDRAM_Q_SHIFT_TAB + 4 **fileName ))))//移动图片
			{
				addr = (void *)(SDRAM_Q_TAB + (*fileName)*8);
				x_uint16 = (uint16_t )x;
				memcpy((void *)addr,(void *)&x_uint16,2);
				//*(uint16_t *)addr = (uint16_t )x;
				addr = (void *)(SDRAM_Q_TAB + (*fileName)*8  + 2);
				y_uint16 = (uint16_t )y;
				memcpy((void *)addr,(void *)&y_uint16,2);
				//*(uint16_t *)addr = (uint16_t )y;
				addr = (void *)(SDRAM_Q_TAB + (*fileName)*8  + 4);
				//*(uint32_t *)addr = (uint32_t)gs_Sdram_wrtie_pos;
				memcpy((void *)addr,(void *)&gs_Sdram_wrtie_pos,4);
				
				//gs_Sdram_wrtie_pos += x * y * 2;


				//判断怕有非法指针
//				if((gs_Sdram_wrtie_pos<SDRAM_BMP_ADDR) ||(gs_Sdram_wrtie_pos>=SDRAM_BMP_MAX_ADDR) )//qdhai add
//				{
//						 gs_Sdram_wrtie_pos = SDRAM_BMP_MAX_ADDR; //qdhai add
//						 systerm_error_status.bits.image_movenantosdram_error = 1;
//				     return 0;
//				}
				
			}
			else
			{
        systerm_error_status.bits.nand_canot_find_Q_bmp_error=1; 
				return 0;
			}
		}
		//fileName++;
	}


	return 1;

}