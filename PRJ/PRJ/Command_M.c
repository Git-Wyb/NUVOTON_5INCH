#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "pwm.h"
#include "ff.h"
#include "Command_ALL.h"
#include "Aprotocol.h"
#include "2d.h"
#include "BSP_Init.h"



PPT_TYPE ppt={0};
static uint16_t  file_name[10]={0};
static uint8_t  file_name_num=0;
static uint8_t  now_display_num=0;
static uint32_t light_time_cnt=0;;
extern PARA_TYPE para;
extern SYSTEMERRORSTATUS_TYPE  systerm_error_status;
static FIL   *g_file1,*g_file2;
uint8_t g_open_flag1=0,g_open_flag2=0;
extern LAYER_SDRAM_STR display_layer_sdram;

void PWM_Change(uint8_t x_level)
{
	typePWMVALUE pwmvalue;
    typePWMSTATUS PWMSTATUS;
	
		static uint32_t PULSE2_VALUE;
	static float  Tp_percent;
  x_level = 255-x_level;
	Tp_percent = (x_level*100.0/255.0);

	PULSE2_VALUE = (uint32_t)(999*Tp_percent/100.0);
	
		pwmvalue.field.cnr=999;
			//�ı�ռ�ձ� ��0%��100% �ɱ�
		pwmvalue.field.cmr=PULSE2_VALUE;
		pwmWrite(PWM_TIMER1, (PUCHAR)(&pwmvalue), sizeof(pwmvalue));
}

void Backlinght_Control_Init_HARDV4(uint8_t x_level)  //0 ���� 255�,0 brightest 255 darkest
{
	
	 typePWMVALUE pwmvalue;
    typePWMSTATUS PWMSTATUS;
	
		static uint32_t PULSE2_VALUE;
	static float  Tp_percent;
  x_level = 255-x_level;
	Tp_percent = (x_level*100.0/255.0);

	PULSE2_VALUE = (uint32_t)(999*Tp_percent/100.0);

	
	
	//ʹ��PWMʱ�� ���Ĵ���˵���� NUC970_TRM.pdf 160ҳ��
    outpw(REG_CLK_PCLKEN1, inpw(REG_CLK_PCLKEN1) | (1 << 27)); // Enable PWM engine clock

	
    PWMSTATUS.PDR=0;
    PWMSTATUS.InterruptFlag=FALSE;
	
	   pwmInit();
		
    
    pwmOpen(PWM_TIMER1);
		//���ö�����Ƶ
    pwmIoctl(PWM_TIMER1, SET_CSR, 0, CSRD1);
		//��������Ƶ
   // pwmIoctl(PWM_TIMER1, SET_CP, 0, 3);//19.23K
		pwmIoctl(PWM_TIMER1, SET_CP, 0, 2);//25K
    pwmIoctl(PWM_TIMER1, SET_DZI, 0, 0);
    pwmIoctl(PWM_TIMER1, SET_INVERTER, 0, PWM_INVOFF);
    pwmIoctl(PWM_TIMER1, SET_MODE, 0, PWM_TOGGLE);
    pwmIoctl(PWM_TIMER1, DISABLE_DZ_GENERATOR, 0, 0);
    
    pwmIoctl(PWM_TIMER1, ENABLE_PWMGPIOOUTPUT, PWM_TIMER1, PWM1_GPB3);

    pwmvalue.field.cnr=999;
    pwmvalue.field.cmr=0;
    pwmWrite(PWM_TIMER1, (PUCHAR)(&pwmvalue), sizeof(pwmvalue));
    pwmIoctl(PWM_TIMER1, START_PWMTIMER, 0, 0);		
	//	sysprintf("PB3���PWM 24.75K HZ���� ռ�ձ� ��0%��100% ����. \r\n");	
		
		pwmvalue.field.cnr=999;
			//�ı�ռ�ձ� ��0%��100% �ɱ�
			pwmvalue.field.cmr=PULSE2_VALUE;
			pwmWrite(PWM_TIMER1, (PUCHAR)(&pwmvalue), sizeof(pwmvalue));
	
}

void StopPpt(void)
{
	DisPlayLayer(LCD_G0, 0xff, 0xff);
}


void end_ppt(void)
{     
   ppt.step=PPT_OVER;
}

void start_ppt(void)
{
  //////////////DisPlayLayer(0,3,0x0f); //display 3 layer
   //ppt.keep_time_cnt=1; //ppt.keep_time;
   light_time_cnt=0;
   now_display_num=0;
   ppt.step=PPT_START;
}

void DisPlayPptBmp(int fileName)
{
	uint16_t width, height;
	void *pBuf;
	uint32_t ppp;
	void *addr;
	
	if (GetSdramImgInfoByIdx(fileName, &width, &height, &pBuf))
	{
		sysprintf("\r\nfind bmp in sdram\r\n");
		ppp = *(uint32_t *)pBuf;
		addr = (void *)ppp;
		//DisplayBmpToFrame(0, 0, width, height,addr, 4);
		ge2dSpriteBlt_Screen(0,0,width,height,addr,(void *)bmp_LCD_BUFFER);
	}
}

int GetBmpFIleSize(char *FileName, int *width, int *height)
{
	FRESULT result;
	UINT br;
	FIL fsrc;
	unsigned char buf[64];
	
  
	
	
	
	////////////////HAL_NVIC_DisableIRQ(PVD_IRQn);
	result = f_open(&fsrc, FileName, FA_READ);
	g_file1 = &fsrc;
	///////////////HAL_NVIC_EnableIRQ(PVD_IRQn);
	
	if (result == FR_OK)
	{
		g_open_flag1 = 1;
		f_read(&fsrc, buf, 0x36, &br);
		if (br == 0x36)
		{
			*width = (buf[21] << 24) | (buf[20] << 16) | (buf[19] << 8) | (buf[18] << 0);
			*height = (buf[25] << 24) | (buf[24] << 16) | (buf[23] << 8) | (buf[22] << 0);
			//cyw
			//////////// HAL_NVIC_DisableIRQ(PVD_IRQn);
			f_close(&fsrc);//cyw  ��OPENû��close
			g_open_flag1 = 0;
			//////////////HAL_NVIC_EnableIRQ(PVD_IRQn);
			return 1;
		}
		 ////////////HAL_NVIC_DisableIRQ(PVD_IRQn);
		f_close(&fsrc);//cyw  ��OPENû��close
		g_open_flag1 = 0;
		////////////////HAL_NVIC_EnableIRQ(PVD_IRQn);
	}
	
	g_open_flag1 = 0;
	
	if(result == FR_DISK_ERR)
	{
	/////////////	HAL_NVIC_DisableIRQ(PVD_IRQn);
	result = f_open(&fsrc, FileName, FA_READ);
	g_file1 = &fsrc;
	///////////////HAL_NVIC_EnableIRQ(PVD_IRQn);
	
	if (result == FR_OK)
	{
		g_open_flag1 = 1;
		f_read(&fsrc, buf, 0x36, &br);
		if (br == 0x36)
		{
			*width = (buf[21] << 24) | (buf[20] << 16) | (buf[19] << 8) | (buf[18] << 0);
			*height = (buf[25] << 24) | (buf[24] << 16) | (buf[23] << 8) | (buf[22] << 0);
			//cyw
			///////////// HAL_NVIC_DisableIRQ(PVD_IRQn);
		   f_close(&fsrc);//cyw  ��OPENû��close
		   g_open_flag1 = 0;
		   /////////////HAL_NVIC_EnableIRQ(PVD_IRQn);
			return 1;
		}
		///////////// HAL_NVIC_DisableIRQ(PVD_IRQn);
		f_close(&fsrc);//cyw  ��OPENû��close
		g_open_flag1 = 0;
		/////////////HAL_NVIC_EnableIRQ(PVD_IRQn);
	}
	}
	
	
		
	g_open_flag1 = 0;
	
	
	return 0;
}

 //PPT_FILES[]={0XFF00,0XFF01,0XFF02,0XFF03,0XFF04,0XFF05,0XFF06,0XFF07,0XFF08,0XFF09};
void ppt_find_file_and_Q(char cmd)
{	 
	//findfile	
	 uint8_t i;
	 uint32_t Tp_size=0;
//	char nandFileName[64];
	
	uint16_t Tp_filename ;
	int Tp_Width,Tp_High;
  static char file[128];
	void *Tp_daram;
	int Tp_width_sdram,Tp_height_sdram;
	
	file_name_num=0;
	
//	for(i=0;i<10;i++)
//	{
//	  Tp_filename = (0xff00 + i); 
//		sprintf(file, "1:/USER_BMP/%04x.bmp", Tp_filename);
//		if(GetBmpFIleSize(file,&Tp_Width,&Tp_High)==1)
//		{
//			if(GetSdramImgInfoByIdx(Tp_filename,&Tp_width_sdram,&Tp_height_sdram,&Tp_daram)==0)
//			{
//				Tp_size =Tp_size+ 2*Tp_Width*Tp_High;
//			}
//			
//		}
//		
//	}
//	if(Tp_size >= (SDRAM_BMP_MAX_ADDR-gs_Sdram_wrtie_pos))
//	{
//		systerm_error_status.bits.image_movenantosdram_error = 1;
//		return;
//	}
	
	for(i=0;i<10;i++)
	{
    
		file_name[file_name_num] = (0xff00 + i);
		if (CpyNandToSdramCMd(cmd, &file_name[file_name_num], 1) == 1)
		{
			file_name_num++;
		}
		
		
		//20180904
		systerm_error_status.bits.nand_canot_find_Q_bmp_error=0; //pptûͼƬ��ֱ�����������ñ���
	}

	sysprintf("file_name_num= %d",file_name_num);
}

	 
void ppt_process(void)  //1//1 ms run one
{
    

	
//	BmpFIleInfo  temp_bmp_file;
 //      uint16_t *file_name;
//	uint16_t file=0xff00;
	
	
	 // file_name=&file;
	//file_name=0xFF00|a;


//�ӵ�Q0
//




	light_time_cnt++;
	
	 switch(ppt.step)  
	 {
		 
		 case PPT_START:
		 	
			if(file_name_num==0)
				{
					Backlinght_Control_Init_HARDV4(255);//����ȫ��
					ppt.step = PPT_IDLE;
					break;
				}
			Backlinght_Control_Init_HARDV4(255);//����ȫ��
		     
		   	ppt.step = FADE_IN;//���ļ�
		   	light_time_cnt=0;
			 break;		
		 
		 case FADE_IN://����
			if(light_time_cnt==1)
			{		
				/*
				temp_bmp_file.x=0;
				temp_bmp_file.y=0;
				temp_bmp_file.layer=4;
				temp_bmp_file.name=file_name[now_display_num];
				*/
			     //DisCmdDisplay(4,&temp_bmp_file,1);
				DisPlayPptBmp(file_name[now_display_num]);
			}             
			 if(light_time_cnt>1000)
			 {
				  //Backlinght_Control_Init_HARDV4(0);//����ȫ��
				   Backlinght_Control_Init_HARDV4(para.lcd_back_light.brightness);
				 light_time_cnt=0;
				  ppt.step = DISPLAY_HOLD;
			 }
			 else
			 {
				  PWM_Change(255-(((255-para.lcd_back_light.brightness)*light_time_cnt)/1000));   //255 - (uint8_t)((get_timego(LCD_Timer)*255)/1000));//����ʱ����������Ļ���� 255-light_time_cnt/4 
			 }	 
			 break;
		 case DISPLAY_HOLD://��ʾά��״̬
			if(light_time_cnt>((uint32_t)(ppt.keep_time)*1000) )   //if(get_timego(LCD_Timer)>(uint32_t)(PPT_KEEP_TIME*1000))//һ��ͼƬ����Ļά�ֵ�ʱ��
       		{
				  light_time_cnt=0;
				 ppt.step = FADE_OUT;
			}	 
			      		  						 
   		 break;
		 case FADE_OUT://����
			 if(light_time_cnt>1000)
			 {
				  Backlinght_Control_Init_HARDV4(255);//����ȫ��
				  now_display_num++;
				   if(now_display_num >=file_name_num)
				   	{
						 now_display_num = 0;
				   	}	 
				  ppt.step = FADE_IN;
				  light_time_cnt=0;
			 }
			 else
			 {
				  //Backlinght_Control_Init_HARDV4(light_time_cnt/4); //(uint8_t)((get_timego(LCD_Timer)*255)/1000));//����ʱ����������Ļ����light_time_cnt/4      255-  1000/4ms
			    PWM_Change(para.lcd_back_light.brightness+(((255-para.lcd_back_light.brightness)*light_time_cnt)/1000));
			 }	
		   break;
		 case PPT_OVER://PPT����
		    //�ѻ�ϲ���ʾ����
		    //DMA2D_Config(800,480,bmp_mixlayer_BUFFER,LCD_FRAME_BUFFER,0);//3.2MS
	           //Paint_Bmp_DMA2D();
		   //����ظ���ԭ��
		   //LCD_CONTROL_BACKLIGHT_ON();			
			 StopPpt();
		  	 ppt.step = PPT_IDLE;
		   	 Backlinght_Control_Init_HARDV4(para.lcd_back_light.brightness); //lcd_back_light.state=0; 
			 break;
		 default:
			 break;
	 }

	
}


