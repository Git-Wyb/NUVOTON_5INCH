#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "sys.h"
#include "ff.h"
#include "BSP_Init.h"
#include "Command_all.h"
#include "2d.h"
#include "nand_nand.h"


extern  SYSTEMERRORSTATUS_TYPE  systerm_error_status;
extern layInfo gs_layer[3];
extern LAYER_SDRAM_STR display_layer_sdram;
uint8_t  Dma2d_busy=0;
extern uint8_t *RxBuffer; //[NAND_PAGE_SIZE];
uint32_t shift_pointer(uint32_t ptr, uint32_t align);
extern BMP_RPOTOCOL_TYPE  bmp_protocol_bak[3];
extern uint8_t flag_layer23_draw;


int GetSdramImgInfoByIdx(int idx, uint16_t *width, uint16_t *height, void **pBuf)  //根据idx为输入，判断内存中有没有对应图像，有则把图像的长宽内存位置返回给width height pbuf
{
	//int i = 0;
	//void *addr;
	
	//uint32_t * Tp_addr;

	
	*width = *(uint16_t *)(SDRAM_Q_TAB + 8*idx);
	
	*height= *(uint16_t *)(SDRAM_Q_TAB + 8*idx + 2);
	
	*pBuf = (uint32_t *)(SDRAM_Q_SHIFT_TAB + 4*idx);
	//* Tp_addr = shift_pointer(* Tp_addr ,32);
	
	//*pBuf = Tp_addr; //(uint32_t *)(SDRAM_Q_TAB + 8*idx + 4);
	#ifdef SYSUARTPRINTF
	sysprintf("*width=%x,*height=%x,*pBuf=%x\r\n",*width,*height,*pBuf);
	#endif
	
	
	if((*width ==0)||(*height == 0)||(*width ==0xffff)||(*height == 0xffff)||(*width>800)||(*height>480))
	{
		return 0;
	}
	
	return 1;
	
}

uint32_t CHECK_Same(uint16_t x,uint16_t y ,uint16_t width,uint16_t height,uint8_t layer)
{
	static uint8_t Tp_i=0;
	for(Tp_i=0;Tp_i<bmp_protocol_bak[layer].num;Tp_i++)
	{
		if((x==bmp_protocol_bak[layer].bmp[Tp_i].x)&&(y==bmp_protocol_bak[layer].bmp[Tp_i].y)&&(width==bmp_protocol_bak[layer].bmp[Tp_i].width)&&(height==bmp_protocol_bak[layer].bmp[Tp_i].height))
		{
			return Tp_i;
		}
	}
		return 0xffffffff;
}

//int GetSdramgInfoByIdx(int idx, int *width, int *height, void **pBuf)  //根据idx为输入，判断内存中有没有对应图像，有则把图像的长宽内存位置返回给width height pbuf
//{
//	int i = 0;
//	void *addr;
//	
//	

//	
//	*width = *(uint16_t *)(SDRAM_Q_TAB + 8*idx);
//	
//	*height= *(uint16_t *)(SDRAM_Q_TAB + 8*idx + 2);
//	
//	*pBuf =  (uint32_t *)(SDRAM_Q_TAB + 8*idx + 4);
//	
//	
//	
//	if((*width ==0)||(*height == 0)||(*width ==0xffff)||(*height == 0xffff))
//	{
//		return 0;
//	}
//	
//	return 1;

//	
//}



int GetBmpFIleSize_SDRAMfrist(uint16_t file_name, uint16_t *width, uint16_t *height)
{
	//FRESULT result;
	//UINT br;
	//FIL fsrc={0};
	//unsigned char buf[64];
	static void **Tp_p=0;
	
	
  if(GetSdramImgInfoByIdx((int)file_name,width,height,Tp_p)==1)
	{
		#ifdef  SYSUARTPRINTF 
		sysprintf("bmp in sdram\r\n");
		#endif
		return 1;
	}
	
	*width = *(__IO int16_t*)(bmp_tab_BUFFER+file_name*8);
	*height = *(__IO int16_t*)(bmp_tab_BUFFER+file_name*8 + 2);
	
	if((*width==0xffff)||(*height==0xffff)||(*width==0)||(*height==0))
	{
		#ifdef  SYSUARTPRINTF 
		sysprintf("width=%x,height=%x\r\n",*width,*height);
		#endif
		return 0;
	}
	#ifdef  SYSUARTPRINTF 
	sysprintf("width=%x,height=%x\r\n",*width,*height);
	#endif
	return 1;
	

}


//////uint8_t BMP_judge(int cnt , BmpFIleInfo *info)
//////{
//////	 uint8_t Tp_i ;
//////	 int width, height;
//////	 char dirPath[24]={0};
//////	 int bmp_cnt_layer[2];
//////	 int layer;
//////	 int sameIdx = -1;
//////	
//////	 bmp_cnt_layer[0] = gs_layer[1].bmpCnt;//2层
//////	 bmp_cnt_layer[1] = gs_layer[2].bmpCnt;//3层
//////	
//////	 for(Tp_i = 0; Tp_i< cnt;Tp_i++)
//////	 { 
//////	    if (info->layer < 1 || info->layer > 3) return 0;
//////		  if (info->x > 800 || info->y > 480)
//////	    {
//////		     systerm_error_status.bits.draw_coordinatebeyond_error=1; 
//////				return 0;
//////	    }
////////			if(info->name<0xff00)
////////			{
////////			sprintf(dirPath,"0:/BELMONT_BMP/%04X.bmp",info->name);
////////			}
////////			else
////////			{
////////			sprintf(dirPath,"1:/USER_BMP/%04X.bmp",info->name);	
////////			}
//////			if(GetBmpFIleSize_SDRAMfrist(info->name,&width,&height)==0)
//////			{
//////				 systerm_error_status.bits.draw_filenotfound_error=1;
//////				 return 0;
//////			}
//////		  if(((info->x+width)>800)||((info->y+height)>480))
//////			{
//////				 systerm_error_status.bits.draw_coordinatebeyond_error=1; 
//////  				return 0;
//////			}
////////////////////////		  layer = info->layer-1;
////////////////////////			if(layer != 0)
////////////////////////			{
////////////////////////			   if(CHECK_Same(info->x,info->y,width,height,layer))
////////////////////////			   {
////////////////////////				   bmp_cnt_layer[layer-1]++;
////////////////////////			   }
////////////////////////				
////////////////////////		  }
////////////////////////		  info++;//！！
//////	 }
//////	 if((bmp_cnt_layer[0] >LAY_MAX_BMP_CNT)||(bmp_cnt_layer[1] >LAY_MAX_BMP_CNT))
//////	 {
//////		 systerm_error_status.bits.layer_img_num_over_max=1;
//////		 return 0;
//////	 }
//////	 
//////	 return 1;
//////}


void DisplayBmpToFrame(int x, int y, int width, int height, void *pBuf, int frame) //x y 显示的位置　width height图像长和高 pbuf图像数据位置　frame表示层0~2表示一到3层
{
	static uint32_t addr = 0;
	switch (frame)
	{
	case 0:
		
		addr = bmp_layer1_BUFFER;
		break;
	case 1:
		
		addr = bmp_layer2_BUFFER;
		break;
	case 2:
		
		addr = bmp_layer3_BUFFER;
		break;
	default:
		addr = bmp_mixlayer_BUFFER;
		break;
	}
	#ifdef  SYSUARTPRINTF
	sysprintf("pBuf0=%X,%X,%X,%X,%X,%X\r\n",*(uint8_t *)((uint32_t)pBuf),*(uint8_t *)((uint32_t)pBuf+1),*(uint8_t *)((uint32_t)pBuf+2),*(uint8_t *)((uint32_t)pBuf+3),*(uint8_t *)((uint32_t)pBuf+4),*(uint8_t *)((uint32_t)pBuf+5));
	sysprintf("pBuf1=%X,%X,%X,%X,%X,%X\r\n",*(uint8_t *)((uint32_t)pBuf+6),*(uint8_t *)((uint32_t)pBuf+7),*(uint8_t *)((uint32_t)pBuf+8),*(uint8_t *)((uint32_t)pBuf+9),*(uint8_t *)((uint32_t)pBuf+10),*(uint8_t *)((uint32_t)pBuf+11));
	#endif
	ge2dSpriteBlt_Screen(x,y,width,height,pBuf,(void *)addr);
	//while(1);
	#ifdef  SYSUARTPRINTF
	sysprintf("layer1_BUFFER0=%X,%X,%X,%X,%X,%X\r\n",*(uint8_t *)(addr),*(uint8_t *)(addr+1),*(uint8_t *)(addr+2),*(uint8_t *)(addr+3),*(uint8_t *)(addr+4),*(uint8_t *)(addr+5));
	sysprintf("layer1_BUFFER1=%X,%X,%X,%X,%X,%X\r\n",*(uint8_t *)(addr+800*2),*(uint8_t *)(addr+1+800*2),*(uint8_t *)(addr+2+800*2),*(uint8_t *)(addr+3+800*2),*(uint8_t *)(addr+4+800*2),*(uint8_t *)(addr+5+800*2));
	#endif
	
	//DMA2D_Config(width, height, (uint32_t)pBuf, addr + (y * 800 + x) * 2, 0);
	
}


int GetSamePosPic(int x, int y, int width, int height,int layer)//判断对应图像有没有在对应的位置上； x y 显示的坐标; width height 图像的长和高　layer在哪层显示
{
	
	int i = 0;
	layBmpInfo *pBmpInfo;
	
	//layBmpInfo  temp_BmpInfo;  //qdhai add
	
	for (i = 0 ; i < gs_layer[layer].bmpCnt;i++)
	{
		pBmpInfo = &gs_layer[layer].bmp[i];
		if ((pBmpInfo->x == x) && (pBmpInfo->y == y) && (pBmpInfo->width == width) && (pBmpInfo->height == height))
		{
			

                  
                  //往前移有变化的放在最后，ReDrawLayer才不会有相同坐标有重叠现象
                  //qdhai add start
                   if(gs_layer[layer].bmpCnt>=2)
                   	{
	                   for (; i < (gs_layer[layer].bmpCnt-1);i++)
	                   	{
	                   		gs_layer[layer].bmp[i]=gs_layer[layer].bmp[i+1];
	                   	}
				return gs_layer[layer].bmpCnt-1;
                   	}

                   	else
                   		{
					return i;
                   		}
			                  //qdhai add end
			
			
			
		}
	}

	return -1;
}

void *GetSdramLastPos(void)
{



////判断怕有非法指针
//	if((gs_Sdram_wrtie_pos<SDRAM_BMP_ADDR) ||(gs_Sdram_wrtie_pos>=SDRAM_MAX_ADDR) )//qdhai add
//  {
//		gs_Sdram_wrtie_pos = SDRAM_BMP_ADDR; //qdhai add
//		//gs_sdram_bmp_cnt = 0;////qdhai add
//  }

	return (void *)SDRAM_NANDFLASH_TEMP;
}




uint8_t NANDFLASH_TO_SDRAM_RANDOM(uint32_t x_sdram_start,uint32_t x_nandflash_start,uint32_t x_sdram_size,uint32_t x_nandflash_size)
{
	NAND_ADDRESS_STR WriteReadAddr;
	
	uint32_t Tp_i;
	uint32_t Tp_nandflash_realaddr;//NANDFLASH???j??
	uint32_t Tp_sdram_nowaddr;
	
//	uint8_t Tp_flag=0;//1-???????
//	uint16_t Tp_delay;
//	uint32_t Tp_count = 0x195000; 
	uint32_t Tp_sdram_size_sum;//h?????NANDFLASH????????????
//	uint32_t Tp_badmanage; 
//	static uint8_t Tp_j; 
	
	if(x_sdram_size > x_nandflash_size*2048) //????????????????SDRAM
	{	
		sysprintf("ERRERR1\r\n");
	return 0;//???????????
	}
	
	if(x_sdram_size%2048 == 0)
	Tp_sdram_size_sum = x_sdram_size/2048;
	else
	Tp_sdram_size_sum = (x_sdram_size/2048)+1;
	
	
	Tp_nandflash_realaddr = x_nandflash_start;
	Tp_sdram_nowaddr =x_sdram_start; //LCD_FRAME_BUFFER;//x_sdram_start;
	for(Tp_i = 0;Tp_i<Tp_sdram_size_sum;Tp_i++)
	{
		WriteReadAddr.Zone = 0x00;
    WriteReadAddr.Block = 0x00;
	  WriteReadAddr.Page =Tp_nandflash_realaddr;
	
	   
//	BAD_BLOCK_CHANGE(&WriteReadAddr);
		

  NAND_ReadPage( WriteReadAddr.Page,0, RxBuffer,NAND_PAGE_SIZE);//NAND_RESULT=0x140????????????
	
	if(Tp_i < (Tp_sdram_size_sum-1))
	memcpy((uint8_t*)(Tp_sdram_nowaddr),(uint8_t*)RxBuffer,2048);
	else
	memcpy((uint8_t*)(Tp_sdram_nowaddr),(uint8_t*)RxBuffer,x_sdram_size-Tp_i*2048);//???????g
	
	Tp_nandflash_realaddr = Tp_nandflash_realaddr + 1;
	if(WriteReadAddr.Page >= (x_nandflash_start+x_nandflash_size))//????????k??
	{
    #ifdef  SYSUARTPRINTF      
		sysprintf("ERRERR2\r\n");
		#endif
		return 0;
	}
	Tp_sdram_nowaddr = Tp_sdram_nowaddr + 2048;
	}
	#ifdef SYSUARTPRINTF
	sysprintf("right\r\n");
	#endif
	return 1;
}


uint32_t nandflash_to_sdram_Q0(uint32_t x_nandflashstartaddr,uint32_t addr,uint32_t *x_bmp,uint32_t addrmax)
{
	uint32_t Tp_res;
	//uint8_t  Tp_high;
	#ifdef  SYSUARTPRINTF
	 sysprintf("addr = %x \r\n",addr);
	#endif
	
	if((addr + 2*x_bmp[0]*x_bmp[1])>addrmax)//???????????????SDRAM??????????????
	{	
	   #ifdef  SYSUARTPRINTF
		sysprintf("addrmax ng \r\n");
		#endif
		return 0xffffffff;
	}
	
	if(x_nandflashstartaddr>image_tab__nandflash_start)
	{ 
	   #ifdef  SYSUARTPRINTF
		sysprintf("x_nandflashstartaddr ng\r\n");
		#endif
		return 0xffffffff;
	}
	
	
	Tp_res = NANDFLASH_TO_SDRAM_RANDOM(addr,x_nandflashstartaddr,2*(x_bmp[0])*x_bmp[1],image_tab__nandflash_start-x_nandflashstartaddr);
  #ifdef  SYSUARTPRINTF
	sysprintf("addr2=%X,%X,%X,%X,%X,%X\r\n",*(uint8_t *)(addr),*(uint8_t *)(addr+1),*(uint8_t *)(addr+2),*(uint8_t *)(addr+3),*(uint8_t *)(addr+4),*(uint8_t *)(addr+5));
	sysprintf("addr3=%X,%X,%X,%X,%X,%X\r\n",*(uint8_t *)(addr+6),*(uint8_t *)(addr+7),*(uint8_t *)(addr+8),*(uint8_t *)(addr+9),*(uint8_t *)(addr+10),*(uint8_t *)(addr+11));

	
	
	 sysprintf("Tp_res = %x \r\n",Tp_res);
	#endif
	
	if(Tp_res == 0)
		return 0xffffffff;
	else
		return addr+2*x_bmp[0]*x_bmp[1];
}

int NandToSdramBmp(uint16_t x_order, uint16_t *x, uint16_t *y, void *pBuf) //file 图像id转的路径如0:/USER_BMP/0001.bmp  pbuf要读出的图像放的内存位置　x y 显示的坐标
{

  
	//NANDFLASH_TO_SDRAM(LCD_FRAME_BUFFER,1,6);
	uint32_t Tp_info[2];
	uint32_t Tp_nandadr;
	
	
	 Tp_nandadr =  READ_TAB_FROMSDRAM(x_order,0,Tp_info);
	*x = Tp_info[0];
	*y = Tp_info[1];
	#ifdef  SYSUARTPRINTF
	sysprintf("Tp_nandadr=%x\r\n",Tp_nandadr);
	#endif
	if(nandflash_to_sdram_Q0(Tp_nandadr,(uint32_t)pBuf,Tp_info,(uint32_t)pBuf+2*800*480)==0xffffffff)
	{
		return 0;
	}
	
	return 1 ;


}

int CpyNandToSdram(uint16_t idx, void *pbuf,uint16_t *width,uint16_t *height) //idx 图像id  pbuf要读出的图像放的内存位置　width height 显示的坐标
{
	static char file[128]={0};
	

	if (NandToSdramBmp(idx, width, height, pbuf))
	{
		
		//__set_PRIMASK(0);
		return 1;
	}
	
	//__set_PRIMASK(0);
	return 0;
}


void DisPlaySdramBmpByIdx(char Cmd, uint16_t idx, int x, int y,int layer)
{
	uint16_t width, height;
	void *pBuf;
	//uint32_t  ppp;
	layBmpInfo *pBmpInfo;
	int sameIdx = -1;
	void *addr;
	uint32_t ppp;
  //    int xx,yy; //qdhai add
	
	//if (layer < 1 || layer > 3) return;

	layer = layer - 1;

	

	if (GetSdramImgInfoByIdx(idx, &width, &height, &pBuf))  //????
	//if(0)
	{
            
		//pBuf = (uint32_t *)shift_pointer((uint32_t)pBuf,32);	  
		if (layer == 0)
		//if(0)
		{
			//if((gs_layer[layer].bmpCnt < LAY_MAX_BMP_CNT))
			//{
			ppp  = *(uint32_t *)pBuf;
			addr = (void *)ppp;
			DisplayBmpToFrame(x, y, width, height, addr, layer);
			gs_layer[layer].bmpCnt++;
			//}
		}
		else
		{	
		      if((sameIdx = GetSamePosPic(x,y,width,height,layer)) != -1)
			{
				pBmpInfo = &gs_layer[layer].bmp[sameIdx];
				pBmpInfo->x = x;
				pBmpInfo->y = y;
				pBmpInfo->width = width;
				pBmpInfo->height = height;
				ppp  = *(uint32_t *)pBuf;
				pBmpInfo->pBuf = (void *)ppp;
			pBmpInfo->idx = idx;
				DisplayBmpToFrame(pBmpInfo->x, pBmpInfo->y, pBmpInfo->width, pBmpInfo->height, pBmpInfo->pBuf, layer);
			}
			else if (gs_layer[layer].bmpCnt < LAY_MAX_BMP_CNT)
			{
				pBmpInfo = &gs_layer[layer].bmp[gs_layer[layer].bmpCnt];
				pBmpInfo->x = x;
				pBmpInfo->y = y;
				pBmpInfo->width = width;
				pBmpInfo->height = height;
				ppp  = *(uint32_t *)pBuf;
				pBmpInfo->pBuf = (void *)ppp;
				gs_layer[layer].bmpCnt++;
			pBmpInfo->idx = idx;
				DisplayBmpToFrame(pBmpInfo->x, pBmpInfo->y, pBmpInfo->width, pBmpInfo->height, pBmpInfo->pBuf, layer);
			}
			else
			{
				flag_layer23_draw = 1;
			}
			
    }			
			  

	}
	else
	{

		//if (CpyNandToSdram(idx, (void*)LCD_CACHE_BUFFER, &width, &height))
		if (CpyNandToSdram(idx, GetSdramLastPos(), &width, &height))
		{
                   
			  
			if (layer == 0)
			//if(0)
			{
				DisplayBmpToFrame(x, y, width, height, GetSdramLastPos(), layer);
				gs_layer[layer].bmpCnt++;
			}
			else 
			{	
			 
				if ((sameIdx = GetSamePosPic(x, y, width, height, layer)) != -1)
				{
					pBmpInfo = &gs_layer[layer].bmp[sameIdx];
					pBmpInfo->x = x;
					pBmpInfo->y = y;
					pBmpInfo->width = width;
					pBmpInfo->height = height;
					pBmpInfo->pBuf = 0;
					pBmpInfo->idx = idx;
					DisplayBmpToFrame(pBmpInfo->x, pBmpInfo->y, pBmpInfo->width, pBmpInfo->height, GetSdramLastPos(), layer);
				}
				else if (gs_layer[layer].bmpCnt < LAY_MAX_BMP_CNT)
				{
					pBmpInfo = &gs_layer[layer].bmp[gs_layer[layer].bmpCnt];
					pBmpInfo->x = x;
					pBmpInfo->y = y;
					pBmpInfo->width = width;
					pBmpInfo->height = height;
					pBmpInfo->pBuf = 0;
					pBmpInfo->idx = idx;
					gs_layer[layer].bmpCnt++;
					DisplayBmpToFrame(pBmpInfo->x, pBmpInfo->y, pBmpInfo->width, pBmpInfo->height, GetSdramLastPos(),layer);
				}
				else
				{

				  flag_layer23_draw = 1;
			
				}
				
		  }			
		}
		else
		{
			systerm_error_status.bits.draw_filenotfound_error = 1;
		}
		
		
	}



}


void DisCmdDisplay(char cmd, BmpFIleInfo *info1, uint16_t cnt1,BmpFIleInfo *info2, uint16_t cnt2,BmpFIleInfo *info3, uint16_t cnt3)
{
	static uint16_t i = 0;
	//uint8_t Tp_res;
	static BmpFIleInfo *Tp_info;


	//gs_layer[0].bmpCnt =0;
	//gs_layer[1].bmpCnt =0;
	//gs_layer[2].bmpCnt =0;
	////////ge2dInit(16, 800, 480, (void *)display_layer_sdram.LCD_CACHE_BUFFER);
	
		Tp_info = info1;
	for (i = 0; i < cnt1;i++)
	{
		Dma2d_busy = 1;
		DisPlaySdramBmpByIdx(cmd, info1->name, info1->x, info1->y, 1);
		//delay_ms(10);
		
		//while(Dma2d_busy);
		if(i!=(cnt1-1))
		info1++;
	}
  
	
	
	Tp_info = info2;
	for (i = 0; i < cnt2;i++)
	{
		Dma2d_busy = 1;
		DisPlaySdramBmpByIdx(cmd, info2->name, info2->x, info2->y, 2);
		//while(Dma2d_busy);
		//info2++;
		
		if(i!=(cnt2-1))
		info2++;
	}
  
	
	Tp_info = info3;
	for (i = 0; i < cnt3;i++)
	{
		Dma2d_busy = 1;
		DisPlaySdramBmpByIdx(cmd, info3->name, info3->x, info3->y, 3);
		//while(Dma2d_busy);
		//info3++;
		if(i!=(cnt3-1))
		info3++;
		
	}
  
	
	
	if (cmd == LCD_D0 || cmd == LCD_D1)
	{
		DisPlayLayer(LCD_FF, 0xff, 0xff);
	}
	
	////////ge2dReset();
}





