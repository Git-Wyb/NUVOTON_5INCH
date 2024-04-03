

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Table.h"
#include <stdarg.h>

#include "nuc970.h"
#include "sys.h"
#include "display.h"
#include "BSP_init.h"
#include "Command_P.h"

extern UINT32 volatile time1ms_count;
static int gs_font_x=1;
static int gs_font_y=0;
static UINT32 gs_font_color=0xffffff;
extern LAYER_SDRAM_STR display_layer_sdram;

static unsigned short *gs_pLcdBuf ;//= (unsigned short *)(bmp_LCD_BUFFER + 2 * 800 *480);

void PutPixel(UINT32 x, UINT32 y, UINT32 c)
{
	
	gs_pLcdBuf = (unsigned short *)(bmp_LCD_BUFFER );
	if ((x < 800) && (y < 480))
	{
		//x = 800 - x;
		//y = 480 - y; 
		gs_pLcdBuf[y * 800 + x] = ((((c >> 16) & 0xff) & 0xF8) << 7) | ((((c >> 8) & 0xff) & 0xF8) << 2) | ((((c >> 0) & 0xff) & 0xF8) >> 3);
	}


}



//*******************************************
//字体输出程序,写一个字节
//*******************************************

void Out_Byte(int x,int y,unsigned char Byte,int color)
{
  int i,temp,ch;
  ch = Byte;
  for (i=0;i<8;i++)
    {
    temp = (ch&0x80);
	if (temp)
	{
		PutPixel(x + i, y, color);
	}
	else
	{
		PutPixel(x + i, y, 0x0);
	}
    ch = ch << 1;
    }
}

void Out_Font0816(unsigned char *font,int Scolor)
{
  int i;
	
  for (i=0;i<16;i++)
  {
		
  Out_Byte(gs_font_x,gs_font_y+i,*(font+i),gs_font_color);
		
  }
}

void Font_Print(unsigned char word)
{
  unsigned char c;
  c=word-32;
  Out_Font0816((unsigned char *)Table0816[c],gs_font_color);

}


void Print(unsigned char *s)
 {
//  U16 ID;
  while(*s)
  {
	
   if ((*s)>0xA0)
    {
           if (gs_font_x>(800 - 16))
         {
             gs_font_x=0;
             gs_font_y+=16;
         }

        	
    //ID = Count_ID(s);
    //Text_Out((unsigned char *)HZ1616[ID]);
    s++;
    s++;
    }
    
   else
    {
    if ((*s)==0x0d) s++;
    if ((*s)=='\n')
		{
		gs_font_x=0;
		gs_font_y+=16;
		s++;
		}
	else {	
		
	
		if (gs_font_x>800)
         		{
            	 gs_font_x=0;
             	gs_font_y+=16;
         		}

         
     Font_Print(*s);
     s++;
     gs_font_x+=8;
     	}
     	
    }
  }
 }
 
 
void SetZuobiao(uint16_t x_x,uint16_t x_y)
{
	 gs_font_x=x_x;
     gs_font_y=x_y;
}

void lcd_printf(const char *fmt,...)
{
	
}

void lcd_printf_new(const char *fmt,...)
{
	  va_list ap;
    static char string[256];

    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    Print((unsigned char *)string);
    va_end(ap);
}

uint32_t get_timego(uint32_t x_data_his)
{
	uint32_t time_pass=0;
	if(time1ms_count >= x_data_his)
	{
		time_pass = time1ms_count - x_data_his;
	}
	else
	{
		time_pass = time1ms_count + 0xffffffff - x_data_his + 1;
	}
	return time_pass;
}





