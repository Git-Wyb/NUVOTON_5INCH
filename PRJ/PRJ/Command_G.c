
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"

#include "Command_all.h"
#include "BSP_Init.h"
#include "2d.h"

extern const unsigned char image_bmp1[];
extern const unsigned char image_bmp4[];
extern const unsigned char image_bmp3[];
extern const unsigned char image_bmp2[];
extern const unsigned char image_bmp6[];

int gs_layFlag = 0;
layInfo gs_layer[3] = { 0 };
extern BMP_RPOTOCOL_TYPE  bmp_protocol[3];
extern LAYER_SDRAM_STR display_layer_sdram;
extern UINT32 _ClipTL, _ClipBR;
extern BOOL _ClipEnable;
extern uint8_t Tp_layerover;


void CLearCacheData(void)
{
	memset((void *)bmp_LCD_BUFFER, 0, 800 * 480 * 2);
}

//1 2 layer有效　１层图像3８　１层有5张　layCnt=2   zeroCnt=0
int CheckLayerCnt(int *lay)//return 1是返回只显示一层的，如多层，只有一层才有数据有显示 lay值即表示对应的层数 return　0 表示多层要显示的
{
	int i = 0;
	int zeroCnt = 0;
	int layCnt = 0;
	for (i = 0; i < 3;i++)
	{
		if (gs_layFlag & (1 << i))
		{
			layCnt++;
			if (gs_layer[i].bmpCnt == 0)
			{
				zeroCnt++;
			}
			else
			{
				*lay = i;
			}
		}
	}
	if ((layCnt - zeroCnt) == 1) return 1;
	return 0;
}
void DisplayLayerBmpToCache(int x, int y, int width, int height,int layer)
{
	unsigned int addr = 0;
	uint16_t  Tp_i;
	switch (layer)
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
	//DMA2D_Config(width, height, (uint32_t)(addr + ( y * 800 + x) * 2), LCD_CACHE_BUFFER + (y * 800 + x) * 2, 800 - width);
	/////////ge2dSpriteBlt_Screen(0,0,width, height,(uint8_t *)(addr + ( y * 800 + x) * 2), (uint8_t *)(bmp_mixlayer_BUFFER + (y * 800 + x) * 2));
	//ge2dSpriteBlt_Screen(x,y,width, height,(uint8_t *)(addr ), (uint8_t *)(bmp_mixlayer_BUFFER + (y * 800 + x) * 2));
  
	if((width>400)&&(height>240))
	{
	_ClipEnable=TRUE;
	_ClipTL=x+(y<<16);
	_ClipBR=x+width+((y+height)<<16);
	ge2dSpriteBlt_Screen(x,y,width, height,(uint8_t *)(addr ), (uint8_t *)(bmp_mixlayer_BUFFER));
  _ClipEnable = FALSE;
	}
////////////////////////////////////////////  屏蔽的原因是速度变慢了
//  _ClipEnable=TRUE;
//	_ClipTL=0;
//	_ClipBR=width+((height)<<16);
//	ge2dSpriteBlt_Screen(0,0,800-x, height,(uint8_t *)(addr +y*800*2), (uint8_t *)(bmp_mixlayer_BUFFER+y*800*2));
//	_ClipEnable = FALSE;
	//ge2dSpriteBlt_Screen(0,0,width, height,(uint8_t *)(addr + ( y * 800 + x) * 2), (uint8_t *)(bmp_mixlayer_BUFFER + (y * 800 + x) * 2));
	
	//ge2dSpriteBlt_Screen(0,0,width,height,(void *)addr,(void *)(bmp_mixlayer_BUFFER + (y * 800 + x) * 2));
	
//	ge2dSpriteBlt_Screen(0,0,width, height,(uint8_t *)(addr + ( y * 800 + x) * 2), (uint8_t *)(bmp_mixlayer_BUFFER + (y * 800 + x) * 2));
	else
	{
		for(Tp_i=0;Tp_i<height;Tp_i++)
	{
		memcpy((uint8_t *)(bmp_mixlayer_BUFFER+  (y * 800 + x) * 2+Tp_i*2*800),(uint8_t *)(addr+  (y * 800 + x) * 2+Tp_i*2*800),2*width);
	}
  }
}

void DisLayerToLcd(int layer)
{
	unsigned int addr = 0;
	switch (layer)
	{
	case 0:
		addr = display_layer_sdram.LCD_FRAME1_BUFFER;
		break;
	case 1:
		addr = display_layer_sdram.LCD_FRAME2_BUFFER;
		break;
	case 2:
		addr = display_layer_sdram.LCD_FRAME3_BUFFER;
		break;
	case 0xff:
		if(Tp_layerover) Clear_sdram(0x40);
		return;
		break;
	default:
		addr = display_layer_sdram.LCD_FRAME_BUFFER;
		break;
	}
	// ge2dSpriteBlt_Screen(0,0,800,480,(void *)image_bmp2);
		#ifdef  SYSUARTPRINTF  
	sysprintf("addr = %X\r\n",(uint32_t)addr);
	sysprintf("addr0=%X,%X,%X,%X,%X,%X\r\n",*(uint8_t *)((uint32_t)addr),*(uint8_t *)((uint32_t)addr+1),*(uint8_t *)((uint32_t)addr+2),*(uint8_t *)((uint32_t)addr+3),*(uint8_t *)((uint32_t)addr+4),*(uint8_t *)((uint32_t)addr+5));
	sysprintf("addr1=%X,%X,%X,%X,%X,%X\r\n",*(uint8_t *)((uint32_t)addr+6),*(uint8_t *)((uint32_t)addr+7),*(uint8_t *)((uint32_t)addr+8),*(uint8_t *)((uint32_t)addr+9),*(uint8_t *)((uint32_t)addr+10),*(uint8_t *)((uint32_t)addr+11));
  #endif
	ge2dSpriteBlt_Screen(0,0,800,480,(void *)addr,(void *)bmp_LCD_BUFFER);
	
	
	
}
void DisCacheToLcd(void)
{
	//DMA2D_Config(800, 480, LCD_CACHE_BUFFER, LCD_FRAME_BUFFER, 0);
	ge2dSpriteBlt_Screen(0,0,800,480,(void *)bmp_mixlayer_BUFFER,(void *)bmp_LCD_BUFFER);
}

void DisplayBmpNew(int x, int y, int width, int height, void *pBuf)
{
	//DMA2D_Config(width, height, (uint32_t)pBuf, LCD_CACHE_BUFFER + (y * 800 + x) * 2, 0);
	ge2dSpriteBlt_Screen(0,0,width,height,(void *)pBuf,(void *)(bmp_mixlayer_BUFFER + (y * 800 + x) * 2));
	//ge2dSpriteBlt_Screen(x,y,width,height,(void *)pBuf,(void *)(bmp_mixlayer_BUFFER ));
}

void ClearLayerData(int layer)
{
	unsigned int addr = 0;
	switch (layer)
	{
	case 0:
		addr = display_layer_sdram.LCD_FRAME1_BUFFER;
		break;
	case 1:
		addr = display_layer_sdram.LCD_FRAME2_BUFFER;
		break;
	case 2:
		addr = display_layer_sdram.LCD_FRAME3_BUFFER;
		break;
	default:
		addr = display_layer_sdram.LCD_FRAME_BUFFER;
		break;
	}
	memset((void *)addr, 0, 800 * 480 * 2);
}

void ReDrawLayer(int layer)
{
	int iLay = 0;
	layBmpInfo *pBmpInfo;
//	int width, height;
	for (iLay = 0; iLay < gs_layer[layer].bmpCnt; iLay++)
	{
		pBmpInfo = &gs_layer[layer].bmp[iLay];
		if (pBmpInfo->pBuf == 0)
		{
			//CpyNandToSdram(pBmpInfo->idx, GetSdramLastPos(), &width, &height);
			//DisplayBmpNew(pBmpInfo->x, pBmpInfo->y, pBmpInfo->width, pBmpInfo->height, GetSdramLastPos());
			DisplayLayerBmpToCache(pBmpInfo->x, pBmpInfo->y, pBmpInfo->width, pBmpInfo->height, layer);
		}
		else
		{
			DisplayBmpNew(pBmpInfo->x, pBmpInfo->y, pBmpInfo->width, pBmpInfo->height, pBmpInfo->pBuf);
		}
	}
}

void DisPlayLayer(char cmd, int layer1, int layer2) //
{
	int i = 0;
	int lay;
	switch (cmd)
	{
	case LCD_G0:
		if (layer1 < 4 && layer1 > 0) gs_layFlag |= 1 << (layer1 - 1);
		if (layer2 < 4 && layer2 > 0) gs_layFlag |= 1 << (layer2 - 1);
		break;
	case LCD_G1:
		if (layer1 < 4 && layer1 > 0) gs_layFlag &= ~(1 << (layer1 - 1));
		if (layer2 < 4 && layer2 > 0) gs_layFlag &= ~(1 << (layer2 - 1));
		break;
	case LCD_G2:
		gs_layFlag = 0x07;
		break;
	case LCD_G3:
		gs_layFlag = 0x00;
		break;
	case LCD_G4:
		if ((layer1 < 4) && (layer1 > 0))
		{
			//gs_layFlag &= ~(1 <<(layer1 - 1));
			gs_layer[layer1 - 1].bmpCnt = 0;
			ClearLayerData(layer1 - 1);
			bmp_protocol[layer1 - 1].num=0;
		}
		if ((layer2 < 4)&& (layer2 > 0))
		{
			//gs_layFlag &= ~(1 << (layer2 - 1));
			gs_layer[layer2 - 1].bmpCnt = 0;
			ClearLayerData(layer2 - 1);
			bmp_protocol[layer2 - 1].num=0;
		}
		break;
	case LCD_G5:
		//gs_layFlag = 0x00;
		gs_layer[0].bmpCnt = 0;
		gs_layer[1].bmpCnt = 0;
		gs_layer[2].bmpCnt = 0;
	  bmp_protocol[0].num=0;
	  bmp_protocol[1].num=0;
	  bmp_protocol[2].num=0;
		ClearLayerData(0);
		ClearLayerData(1);
		ClearLayerData(2);
		break;

	case LCD_G6:
		if ((layer1 < 4) && (layer1 > 0))
		{
			//gs_layFlag &= ~(1 <<(layer1 - 1));
			gs_layer[layer1 - 1].bmpCnt = 0;
			ClearLayerData(layer1 - 1);
			bmp_protocol[layer1 - 1].num=0;
		}
		if ((layer2 < 4)&& (layer2 > 0))
		{
			//gs_layFlag &= ~(1 << (layer2 - 1));
			gs_layer[layer2 - 1].bmpCnt = 0;
			ClearLayerData(layer2 - 1);
			bmp_protocol[layer2 - 1].num=0;
		}
		return ; //break;

	case LCD_G7:
		//gs_layFlag = 0x00;
		gs_layer[0].bmpCnt = 0;
		gs_layer[1].bmpCnt = 0;
		gs_layer[2].bmpCnt = 0;
	  bmp_protocol[0].num=0;
	  bmp_protocol[1].num=0;
	  bmp_protocol[2].num=0;
		ClearLayerData(0);
		ClearLayerData(1);
		ClearLayerData(2);
	
		return ; //break;	
	default:
		break;
	}
	
	#ifdef  SYSUARTPRINTF
	sysprintf("gs_layFlag=0x%X\r\n",gs_layFlag);
	#endif
	
  if(gs_layFlag==0)
	{
		DisLayerToLcd(0xff);
	}
	else if ((gs_layFlag == 0x1) || (gs_layFlag == 0x02) || (gs_layFlag == 0x04))  //单层时
	{
		for (i = 0; i < 3; i++)
		{
			if (gs_layFlag & (1 << i))
			{
				DisLayerToLcd(i);
			}
		}
	}
	else //多层
	{
		if (CheckLayerCnt(&lay))
		{
			DisLayerToLcd(lay);
		}
		else
		{
			//CLearCacheData();
			switch (gs_layFlag)
			{
				case 0x03://layer1 2 有效
					DisplayBmpNew(0, 0, 800, 480, (void *)bmp_layer1_BUFFER);
					ReDrawLayer(1);// 2 layer draw
					break;
				case 0x05: //layer1 3 有效
					DisplayBmpNew(0, 0, 800, 480, (void *)bmp_layer1_BUFFER);
					ReDrawLayer(2);// 3 layer draw
					break;
				case 0x06://layer 2 3 有效
					DisplayBmpNew(0, 0, 800, 480, (void *)bmp_layer2_BUFFER);
					ReDrawLayer(2);// 3 layer draw
					break;
				case 0x07://layer1 2 3 有效
					DisplayBmpNew(0, 0, 800, 480, (void *)bmp_layer1_BUFFER);
					ReDrawLayer(1);// 2 layer draw
					ReDrawLayer(2);// 3 layer draw
					break;
				default:
					break;
			}
			DisCacheToLcd();
		}
//////////		if (CheckLayerCnt(&lay))
//////////		{
//////////			DisLayerToLcd(lay);
//////////		}
//////////		else
//////////		{
//////////			CLearCacheData();
//////////			switch (gs_layFlag)
//////////			{
//////////				case 0x03://layer1 2 有效
//////////					DisplayBmpNew(0, 0, 800, 480, (void *)LCD_FRAME1_BUFFER);
//////////					ReDrawLayer(1);// 2 layer draw
//////////					break;
//////////				case 0x05: //layer1 3 有效
//////////					DisplayBmpNew(0, 0, 800, 480, (void *)LCD_FRAME1_BUFFER);
//////////					ReDrawLayer(2);// 3 layer draw
//////////					break;
//////////				case 0x06://layer 2 3 有效
//////////					DisplayBmpNew(0, 0, 800, 480, (void *)LCD_FRAME2_BUFFER);
//////////					ReDrawLayer(2);// 3 layer draw
//////////					break;
//////////				case 0x07://layer1 2 3 有效
//////////					DisplayBmpNew(0, 0, 800, 480, (void *)LCD_FRAME1_BUFFER);
//////////					ReDrawLayer(1);// 2 layer draw
//////////					ReDrawLayer(2);// 3 layer draw
//////////					break;
//////////				default:
//////////					break;
//////////			}
//////////			DisCacheToLcd();
//////////		}

		
	}



}
