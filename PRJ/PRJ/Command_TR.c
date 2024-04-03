
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "sys.h"
#include "gpio.h"
#include "BSP_Init.h"
#include "Command_TR.h"
#include "display.h"
#include "Aprotocol.h"
#include "etimer.h"


#define GT911 0
#define IL2117A 1
#define TOUCH_IC IL2117A 

#define CT_INT_GPIO_PORT GPIOE
#define CT_INT_GPIO_PIN BIT7

#define SET_SDA()  SET_IIC_SDA
#define SET_CLK()  SET_IIC_SCL
#define CLR_SDA()  CLR_IIC_SDA
#define CLR_CLK()  CLR_IIC_SCL

//IO操作函数	 
#define SET_IIC_SCL    GPIO_Set(GPIOE, BIT9) //SCL
#define SET_IIC_SDA    GPIO_Set(GPIOE, BIT8) //SDA	

#define CLR_IIC_SCL    GPIO_Clr(GPIOE, BIT9) //SCL
#define CLR_IIC_SDA    GPIO_Clr(GPIOE, BIT8) //SDA	


#define CT_READ_SDA  		 GPIO_ReadBit(GPIOE, BIT8)  //输入SDA 


#define CT_CMD_WR	   	0X4C    //0X70	//写数据命令 0110 000 0
#define CT_CMD_RD   	0X4D   //0X71	//读数据命令

#define CT_MAX_TOUCH    10		//电容触摸屏最大支持的点数
#define MAX_CTP_POINT   3

void delay_us(int usec);
static volatile unsigned int sTime = 0;
extern volatile uint32_t time1ms_count;
uint8_t touch_sensibility_cnt=1;    //
uint8_t touck_effective_press=0;
extern  uint16_t command_D8_D9_time;
uint8_t  FLAG_KEY_RELASE = 0;
static tpInfo gs_tpInfo_copy[16];
tpInfo gs_tpInfo[16] = { 0 };
extern uint8_t screen_reverse_bit;
extern PARA_TYPE para;
extern uint8_t*  UART2_TX_BUFF_NEW;
extern uint16_t  TX_COUNT_IN,TX_COUNT_OUT;
volatile uint8_t Flag_int = 0;
uint8_t touch_send_imm = 0;
extern uint32_t Flag_Tim_50ms;
uint32_t shift_pointer(uint32_t ptr, uint32_t align);
E_INT_STATUS touch_control=int_enable;

void set_touchint(E_INT_STATUS x_status)
{
	touch_control = x_status;
}

void Touch_int_enable(void)
{
	///////////////////sysEnableInterrupt(ETMR2_IRQn);
	//////////////////sysEnableInterrupt(TMR0_IRQn);
	// ETIMER_EnableInt(2);
	//sysSetLocalInterrupt(ENABLE_IRQ);
	if(touch_control== int_wait) 
	{
		set_touchint(int_enable);
		check_tp_send();
	}
	set_touchint(int_enable);
}
void Touch_int_disable(void)
{
	set_touchint(int_disable);
	//////////////////sysDisableInterrupt(ETMR2_IRQn);
	/////////////////sysDisableInterrupt(TMR0_IRQn);
//	ETIMER_DisableInt(2);
	//sysSetLocalInterrupt(DISABLE_IRQ);
}


#define delay_us delay_us_new

void delay_us_new(int usec)
{
    volatile int  loop = 100 * usec;
    while (loop > 0) loop--;
}

vu32 GPIOECallback(UINT32 status, UINT32 userData)
{
	 if(status & BIT7)
	 {
		 Flag_int = 1;
		 sTime = time1ms_count;
		 sysprintf("TOUCH INT \r\n");
	 }
	  //清除中断标志位
    GPIO_ClrISRBit(GPIOE,status);
    return 0;
}
//初始化IIC
void IIC_Init(void)
{					     
    //PE8 PE9
	  REG_OPERATE(REG_SYS_GPE_MFPH,0X000000FF,clear);
	
	  GPIO_OpenBit(GPIOE,BIT9, DIR_OUTPUT, PULL_UP);
    GPIO_OpenBit(GPIOE,BIT8, DIR_OUTPUT, PULL_UP);
		SET_IIC_SCL; 
	  SET_IIC_SDA;
	  //delay_ms(500);
	
	  //PE7-INT
	  REG_OPERATE(REG_SYS_GPE_MFPL,0XF0000000,clear);
	  GPIO_OpenBit(GPIOE,BIT7, DIR_INPUT, PULL_UP);
	
	  GPIO_EnableTriggerType(GPIOE, BIT7,BOTH_EDGE);
	  GPIO_EnableInt(GPIOE, (GPIO_CALLBACK)GPIOECallback, 0);
	
	  //PB-11 RESET
	   REG_OPERATE(REG_SYS_GPB_MFPH,0X0000F000,clear);
	  GPIO_OpenBit(GPIOB,BIT11, DIR_OUTPUT, PULL_UP);
	
	  TOUCH_IC_RESET();
	  delay_ms(2);
	  TOUCH_IC_SET();
	
}

void GT911_InitHard(void)
{
	  ;
}
void IL2117A_InitHard(void)
{
	
	;
}
void FT5x06_Init(void)
{
	uint8_t Tp_i=0;
	
	IIC_Init();
	#if(TOUCH_IC == GT911)
	GT911_InitHard();
	#endif
	#if(TOUCH_IC == IL2117A)
	IL2117A_InitHard();
	#endif
	
	for (Tp_i = 0; Tp_i < 10;Tp_i++)
   {
	   gs_tpInfo[Tp_i].sendCnt = 10;
   }
	
	 
}

void CT_SDA_IN()
{
  GPIO_SetBitDir(GPIOE,BIT8, DIR_INPUT);
}

void CT_SDA_OUT()
{
  GPIO_SetBitDir(GPIOE,BIT8, DIR_OUTPUT);
}

//产生ACK应答
void CT_I2C_Ack(void)
{
//	CLR_CLK();
//	CT_SDA_OUT();
//	CLR_SDA();
//	delay_us(5);
//	SET_CLK();
//	delay_us(5);
//	CLR_CLK();
	
	
 CT_SDA_OUT();//SDAIO = 1;
 delay_us(1);//Delayus(1);
 CLR_SDA();//if(a==0)SDA = 0;
           //else  SDA = 1;
 delay_us(5);//Delayus(5);  //>4.7us
 SET_CLK();//SCL = 1;
 delay_us(4);//Delayus(4);//>4us
 CLR_CLK();//SCL = 0;	
}
//不产生ACK应答		    
void CT_I2C_NAck(void)
{
//	CLR_CLK();
//	CT_SDA_OUT();
//	SET_SDA();
//	delay_us(5);
//	SET_CLK();
//	delay_us(5);
//	CLR_CLK();

	
 CT_SDA_OUT();//SDAIO = 1;
 delay_us(1);//Delayus(1);
 SET_SDA();//if(a==0)SDA = 0;
           //else  SDA = 1;
 delay_us(5);//Delayus(5);  //>4.7us
 SET_CLK();//SCL = 1;
 delay_us(4);//Delayus(4);//>4us
 CLR_CLK();//SCL = 0;	
}				

//产生I2C起始信号
void CT_I2C_Start(void)
{
//	CT_SDA_OUT();     //sda线输出
//	SET_SDA();	  	  
//	SET_CLK();
//	delay_us(5);
// 	CLR_SDA();//START:when CLK is high,DATA change form high to low 
//	delay_us(5);
//	CLR_CLK();//钳住I2C总线，准备发送或接收数据 

	
 SET_SDA();//SDA=1; //发送启始条件的数据信号
 delay_us(1);//Delayus(1);
 SET_CLK();//SCL=1;
 delay_us(5);//Delayus(5);  //>4.7us
 CLR_SDA();//SDA=0; //发送起始信号
 delay_us(4);//Delayus(4);  //>4us
 CLR_CLK();//SCL=0; //钳住I2C总线,准备发送数据或接收数据
 delay_us(1);//Delayus(1);
}	  

//产生I2C停止信号
void CT_I2C_Stop(void)
{
//	CT_SDA_OUT();//sda线输出
//	SET_CLK();
//	CLR_SDA();//STOP:when CLK is high DATA change form low to high
// 	delay_us(5);
//	SET_CLK();
//	SET_SDA();//发送I2C总线结束信号
//	delay_us(5);					


 CLR_SDA();//SDA = 0; //发送结束条件的数据信号
 delay_us(5);//Delayus(5);  //>4.7us
 SET_CLK();//SCL = 1;
 delay_us(4);//Delayus(4);//>4us
 SET_SDA();//SDA = 1;
 delay_us(5);//Delayus(5);  //>4.7us	
}

uint8_t CT_I2C_Wait_Ack(void)
{
//	u16 ucErrTime=0;
//	//CT_SDA_OUT();     //SDA设置为输入  
//	//SET_SDA();   delay_us(1);	
//	CT_SDA_IN(); 
//	delay_us(5);
//	SET_CLK();delay_us(5);	
//	while(CT_READ_SDA)
//	{
//		ucErrTime++;
//		if(ucErrTime>250)
//		{
//			CT_I2C_Stop();
//			return 1;
//		}
//	}
//	CLR_CLK();//时钟输出0 	 
//  delay_us(5);  
//	return 0;  

	
uint8_t ack = 0;//应答信号	
 //SDA = 1;
 delay_us(1);//Delayus(1);
 CT_SDA_IN();//SDAIO = 0;
 delay_us(5);//Delayus(5); //>4.7us
 SET_CLK();//SCL = 1;
 delay_us(4);//Delayus(4); //>4us
 //SDAIO = 0;
 if(CT_READ_SDA == 1)//if(SDA_I == 1)
     ack = 0;
 else
     ack = 1; //用ASK=1为有应答信号
 CLR_CLK();//SCL = 0;
 delay_us(1);//Delayus(1);
 CT_SDA_OUT();//SDAIO = 1;	
 return ack;  //qdhai add
} 

void CT_I2C_Send_Byte(uint8_t txd)
{                        
//    u8 t;   
//	CT_SDA_OUT(); 	 
//    delay_us(5);	
//    CLR_CLK();//拉低时钟开始数据传输
//    for(t=0;t<8;t++)
//    {              
////        CT_I2C_SDA=(txd&0x80)>>7;
//		if((txd&0x80)>>7)
//		SET_SDA();
//		else	
//		CLR_SDA();
//		
//		txd<<=1; 	      
//		SET_CLK();
//		delay_us(5);
//		CLR_CLK();	
//		delay_us(5);
//    }	 
	
	
 uint8_t bit_count;
 for(bit_count = 0;bit_count < 8;bit_count++)
     {
      if((txd << bit_count) & 0x80) SET_SDA();//if((c << bit_count) & 0x80) SDA = 1;
      else  CLR_SDA(); //else  SDA = 0;
      delay_us(5);//Delayus(5); //>4.7us
      SET_CLK();//SCL = 1;
      delay_us(4);//Delayus(4); //>4us
      CLR_CLK();//SCL = 0;
     }
 CT_I2C_Wait_Ack();//Mack();
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t CT_I2C_Read_Byte(unsigned char ack)
{
//	unsigned char i,receive=0;
// 	CT_SDA_IN();//SDA设置为输入
//    for(i=0;i<8;i++ )
//	{
//    CLR_CLK(); 	    	   
//		delay_us(5);
//		SET_CLK();
//    receive<<=1;
//		delay_us(5);
//    if(CT_READ_SDA)receive++;   
//	}	  				 
//	if (!ack)CT_I2C_NAck();//发送nACK
//	else CT_I2C_Ack(); //发送ACK   
//	
// 	return receive;
	

 uint8_t retc, bit_count;
 retc = 0;
 CT_SDA_IN();
 for(bit_count = 0;bit_count < 8;bit_count++)
     {
      delay_us(1);//Delayus(1);
      CLR_CLK();//SCL = 0;
      delay_us(5);//Delayus(5);  //>4.7us
      //SDA =1;
      SET_CLK();//SCL = 1;
      delay_us(5);//Delayus(5); //>4.7us
      retc = retc << 1;
      if(CT_READ_SDA == 1)//if(SDA_I == 1)
          retc = retc + 1;
     }
 CLR_CLK();//SCL = 0;
 delay_us(1);//Delayus(1);
 return(retc);	
}

void FT5x06_RD_Reg(uint8_t reg,uint8_t *buf,uint8_t len)
{
//	const uint32_t Tp_timedelay_data_const = 0x15000;//0x15000;//0x15000;
//	uint32_t Tp_time = Tp_timedelay_data_const;
	uint8_t Tp_error=0;
	
 uint8_t i;// ,j;
	
 CT_I2C_Start();//start_i2c(); //启动总线
 CT_I2C_Send_Byte(CT_CMD_WR);
	
/*while(CT_I2C_Wait_Ack())
{
	Tp_time--;
	if(Tp_time == 0 )
	{
		Tp_error = 1;
		break;
	}
}*/
	
 CT_I2C_Send_Byte(reg);

/*Tp_time = Tp_timedelay_data_const;
	while(CT_I2C_Wait_Ack())
{
	Tp_time--;
	if(Tp_time == 0 )
	{
		Tp_error = 1;
		break;
	}
}*/
	CT_I2C_Stop();//stop_i2c(); //发送停止位	
	
//	DELAY_1MS_touch = time1ms_count;
//	 while(get_timego(DELAY_1MS_touch)<=1)
//	 {
//		 ;
//	 }
delay_us(2);
	
 CT_I2C_Start();
 CT_I2C_Send_Byte(CT_CMD_RD);
 	/*Tp_time = Tp_timedelay_data_const;
	while(CT_I2C_Wait_Ack())
{
	Tp_time--;
	if(Tp_time == 0 )
	{
		Tp_error = 1;
		break;
	}
}*/
 *buf = CT_I2C_Read_Byte(1);
 if(*buf!=0x5a)
	{
		CT_I2C_NAck();//N0ack(1); //发送非应答位
    CT_I2C_Stop();//stop_i2c(); //发送停止位	
	  return;
	}
 buf++;
 CT_I2C_Ack();
	
 for(i = 1;i < len - 1;i++)
     {
      //SDAIO = 0;
      *buf = CT_I2C_Read_Byte(1);//*s = receive_byte(); //获得数据
      buf++;//s++;
      CT_I2C_Ack();//N0ack(0); //发送应答信号
     }
 //SDAIO = 0;
 *buf = CT_I2C_Read_Byte(1);//*s = receive_byte(); //获得数据
 CT_I2C_NAck();//N0ack(1); //发送非应答位
 CT_I2C_Stop();//stop_i2c(); //发送停止位	
		 
  if(Tp_error==1)
	{
		 
		///////////////////// systerm_error_status.bits.ctpiic_error=1;
			memset(buf,0,len);//如果IIC总线错误 全部回零
	}
	
		 
}

int check_same_pt(int x, int y, int cx, int cy)
{
	if ((x <= (cx + 50)) && (x >= (cx - 50)))
	{
		if ((y <= (cy + 50)) && (y >= (cy - 50)))
		{
			return 1;
		}
	}

	return 0;
}

int add_touch_key(int x, int y)
{
	int i = 0;
	for (i = 0; i < 10;i++)
	{
		if ((gs_tpInfo[i].state == 0)&&(gs_tpInfo[i].sendCnt>=2))
		{
			gs_tpInfo[i].state = 1;
			
      
			gs_tpInfo[i].x = x;
			gs_tpInfo[i].y = y;
			
			//TestCtp(x, y);
//			gs_tpInfo[i].cnt = 0;
			gs_tpInfo[i].sendCnt = 0;
			gs_tpInfo[i].T_flag_new = 0;
			return i;
 		}
	}
	return 0xf;
}


void touch_dev_poll(void)
{
	static uint8_t buf[64];//cyw
	static int s_touch_io_state = 0;
	static short  tKey[16][2];//cyw
	
	int io_state = 0;
	uint16_t tmpX,tmpY;
	uint8_t i = 0;
	uint8_t j = 0;
	uint8_t kCnt = 0;
	uint16_t tFlag = 0;
	//LED_FLASHERR_ON();
//	

//	//LED_FLASHERR_TOGGLE();
//	
//	io_state = GPIO_ReadBit(CT_INT_GPIO_PORT, CT_INT_GPIO_PIN);
//	if(io_state==0)
//	{
//	       if(touck_effective_press==0)
//		     {
//			       touck_effective_press=1; //qdhai add
//			       touch_sensibility_cnt=0;  //qdhai add
//	       }
//	}
//	if (io_state == s_touch_io_state)
//	{
//		if (get_timego(sTime) > 70)
//		{
//			  if((command_D8_D9_time)/*||(COMM_STEP!=COMM_IDLE)*/) 
//			  //if(0)
//	      {
//					
//			  }
//		    else if(FLAG_KEY_RELASE == 0)
//				{
//			   for (i = 0 ; i < 10;i++)
//			   {
//				   //////////////////////HAL_NVIC_DisableIRQ(TIM6_DAC_IRQn);
//					// Touch_tim_stop();
//					 gs_tpInfo[i].state = 0;
//					// Touch_tim_start();
//					 /////////////////////HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
//			   }
//			  }
//				else
//				{
//					FLAG_KEY_RELASE = 0;
//					s_touch_io_state = io_state;
//	        sTime = time1ms_count;
//				}
//		}
//    //LED_FLASHERR_OFF();
//		return;
//	}
//	s_touch_io_state = io_state;
//	sTime = time1ms_count;
//	
//	if((command_D8_D9_time)/*||(COMM_STEP!=COMM_IDLE)*/) 
//	//if(0)
//	{	
//		// LED_FLASHERR_OFF();
//		//Tp_flag = 1;
//    FLAG_KEY_RELASE = 1;
//		return;
//	}
	
	if(Flag_int == 0) 
	{
		if(get_timego(sTime) > 100)
		{
			for (i = 0 ; i < 10;i++)
			{
				gs_tpInfo[i].state = 0;
			}
			sTime = time1ms_count;
			touck_effective_press = 0;
		}
		return;
  }
	
	touck_effective_press = 1;
	Flag_int = 0;
	//////////////HAL_NVIC_DisableIRQ(TIM6_DAC_IRQn);
//	Touch_tim_stop();
	/////////Touch_int_disable();
	FT5x06_RD_Reg(0, buf, 42);
	/////////Touch_int_enable();
//	Touch_tim_start();
	///////////////HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

	if (buf[0] == 0x5A)
	{
		
		kCnt = 0;
		if(screen_reverse_bit)//cyw
		{
		
		for (i = 0; i < 10; i++)
		{
			/*
			tmpX = ((buf[1 + i * 4] & 0xf0) << 4) + buf[2 + i * 4];
			tmpY = ((buf[1 + i * 4] & 0x0f) << 8) + buf[3 + i * 4];
			tKey[i][0] = (tmpX) * 800 / 2048;
			tKey[i][1] = (tmpY) * 480 / 2048;
			if (tmpX == 0xfff) break;
			if (tmpY == 0xfff) break;
			*/
			//qdhai change
			//根据陈工修改两个几乎同时按(要有一定的间隔)一先一后松开的时候
				//检测不出松开时间，在最后一个松开时报的是两个一起松的
				
			tmpX = ((buf[1 + i * 4] & 0xf0) << 4) + buf[2 + i * 4];
			tmpY = ((buf[1 + i * 4] & 0x0f) << 8) + buf[3 + i * 4];
			if (tmpX >= 0xfff) continue;
			if (tmpY >= 0xfff) continue;
			
			if(READ_PRODUCTTYPE==PORDUCT_7INCH)
			{
			tKey[kCnt][0] = (tmpX) ;
			tKey[kCnt][1] = (tmpY) ;
			if(tKey[kCnt][0] >800) tKey[kCnt][0] = 800;
			if(tKey[kCnt][1] >480) tKey[kCnt][0] = 480;
			}
			
      if(READ_PRODUCTTYPE==PORDUCT_5INCH)
			{				
			tKey[kCnt][0] = (tmpX) * 800 / 2048;
			tKey[kCnt][1] = (tmpY) * 480 / 2048;
			if(tKey[kCnt][0] >800) tKey[kCnt][0] = 800;
			if(tKey[kCnt][1] >480) tKey[kCnt][0] = 480;
			tKey[kCnt][0] = 800 - tKey[kCnt][0];
			tKey[kCnt][1] = 480 - tKey[kCnt][1];
			}
			
			kCnt++;
		}
	  }
		else//不需要翻转
		{
			 for (i = 0; i < 10; i++)
		{
			/*
			tmpX = ((buf[1 + i * 4] & 0xf0) << 4) + buf[2 + i * 4];
			tmpY = ((buf[1 + i * 4] & 0x0f) << 8) + buf[3 + i * 4];
			tKey[i][0] = (tmpX) * 800 / 2048;
			tKey[i][1] = (tmpY) * 480 / 2048;
			if (tmpX == 0xfff) break;
			if (tmpY == 0xfff) break;
			*/
			//qdhai change
			//根据陈工修改两个几乎同时按(要有一定的间隔)一先一后松开的时候
				//检测不出松开时间，在最后一个松开时报的是两个一起松的
				
			tmpX = ((buf[1 + i * 4] & 0xf0) << 4) + buf[2 + i * 4];
			tmpY = ((buf[1 + i * 4] & 0x0f) << 8) + buf[3 + i * 4];
			if (tmpX >= 0xfff) continue;
			if (tmpY >= 0xfff) continue;
			if(READ_PRODUCTTYPE==PORDUCT_5INCH)
			{
			tKey[kCnt][0] = (tmpX) * 800 / 2048;
			tKey[kCnt][1] = (tmpY) * 480 / 2048;
			if(tKey[kCnt][0] >800) tKey[kCnt][0] = 800;
			if(tKey[kCnt][1] >480) tKey[kCnt][0] = 480;
			}
			if(READ_PRODUCTTYPE==PORDUCT_7INCH)
			{
			tKey[kCnt][0] = (tmpX) ;
			tKey[kCnt][1] = (tmpY) ;
			if(tKey[kCnt][0] >800) tKey[kCnt][0] = 800;
			if(tKey[kCnt][1] >480) tKey[kCnt][0] = 480;
			}
			kCnt++;
		}
		}
	}
	if (kCnt == 0) return;
	
  
	
////////////////	HAL_NVIC_DisableIRQ(TIM6_DAC_IRQn);
	Touch_int_disable();
	for (i = 0; i < kCnt;i++)
	{
		for (j = 0 ; j < 10;j++)
		{
			if (gs_tpInfo[j].state)
			{
				if (check_same_pt(tKey[i][0], tKey[i][1], gs_tpInfo[j].x, gs_tpInfo[j].y))
				{
					tFlag |= 1 << j;
				  break;
				}
			}
		}
		if (j == 10)
		{
			tFlag |= (1 << add_touch_key(tKey[i][0], tKey[i][1]));
		}
	}

	for (i = 0 ; i < 10;i++)
	{
		if ((tFlag & (1 << i)) == 0)
		{
			if ((gs_tpInfo[i].state == 2)||(gs_tpInfo[i].state == 3))//2表示已经T过了
			{
				gs_tpInfo[i].state = 0;
				//gs_tpInfo[i].sendCnt =0;//cyw
			}			
		}
	}
	Touch_int_enable();
	
	if(touch_send_imm == 0)
	{
		touch_send_imm = 1;
		 touck_effective_press=1; //qdhai add
		touch_sensibility_cnt=0;  //qdhai add
		check_tp_send();
		Flag_Tim_50ms = time1ms_count;
		Touch_tim_init();
		Touch_tim_start();
		 //TIM6_Init((50000-1),(108-1));	//for touch send 
   //start_tim6(50000);  //test
	}
 ////////////////HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

  //LED_FLASHERR_OFF();


	
}


void send_tcp_xy_mul(tpSendData *pData, int cnt)
{
	int i;
	static char buf[84]={0};
	static  char tmpBuf[16]={0};
	uint8_t *p=0,*p_shift=0;
	uint16_t Tp_num;
	if (cnt == 0) return;
	
	
	memset(buf,0,84);
	sprintf(buf, "@%02X", cnt * 8);
	
	for (i = 0 ; i < cnt;i++)
	{
		sprintf(tmpBuf, "%c%03d%03d", pData->state, pData->x, pData->y);
		strcat(buf, tmpBuf);
		if (i != (cnt -1))
		{
			strcat(buf, ",");
		}
		pData++;
	}
	strcat(buf, "*");
	#ifdef  SYSUARTPRINTF  
		sysprintf("buf=%s\r\n",buf);
	#endif
		
	if(TX_COUNT_IN == TX_COUNT_OUT)
	{
		#ifdef  SYSUARTPRINTF  
		//sysprintf("para.lcd_back_light.brightness=%X\r\n",para.lcd_back_light.brightness);
		#endif
		
	for(i = 0 ; i < (uint16_t)(8 * cnt + 3);i++)
	{
		*(uint8_t *)(UART2_TX_BUFF_NEW+TX_COUNT_IN) = buf[i];
		TX_COUNT_IN++;
	  TX_COUNT_IN = TX_COUNT_IN % TX_BUFF_MAX_UART;
		#ifdef  SYSUARTPRINTF  
		//sysprintf("	1=%c, 2=%c\r\n",*(uint8_t *)(UART2_TX_BUFF_NEW+TX_COUNT_IN),buf[i]);
	  #endif
	}
	#ifdef  SYSUARTPRINTF 
	//sysprintf("data to sendbuf over\r\n");
	#endif
	
  }
	else
	{
////////		Tp_num = GET_READ_RXNUM(TX_COUNT_IN,TX_COUNT_OUT,TX_BUFF_MAX_UART);
////////		p = malloc(Tp_num+64);
////////		p_shift = (uint8_t *)(shift_pointer((uint32_t)p,32)+32);
////////		i =0;
////////		while(TX_COUNT_IN!=TX_COUNT_OUT)
////////		{
////////			*(p_shift+i)=UART2_TX_BUFF_NEW[TX_COUNT_IN];
////////			i++;
////////			TX_COUNT_IN++;
////////	    TX_COUNT_IN = TX_COUNT_IN % TX_BUFF_MAX_UART;
////////		}
////////		for(i = 0 ; i < (uint16_t)(8 * cnt + 3);i++)
////////	  {
////////		UART2_TX_BUFF_NEW[TX_COUNT_IN] = buf[i];
////////		TX_COUNT_IN++;
////////	  TX_COUNT_IN = TX_COUNT_IN % TX_BUFF_MAX_UART;
////////	  }
////////		for(i = 0;i<Tp_num;i++)
////////		{
////////			UART2_TX_BUFF_NEW[TX_COUNT_IN]=*(p_shift+i);
////////		}
////////		free(p);
	}
		SEND_data();
	//uart2_send_buff((uint8_t*)buf, (uint16_t)(8 * cnt + 3));

}


void check_tp_send(void)
{
	
//	static int sTimes = 0;
//	static int sSendCnt = 0;
	static tpSendData tpData[16];//cyw
	static uint8_t tpCnt = 0/*,Tp_T=0*/,Tp_mao=0;
	static uint8_t Tp_i=0;
	#if (MAX_CTP_POINT>10)
	#error CTP_INT_PIN define error
	#endif

	#ifdef  SYSUARTPRINTF 
	sysprintf("touch 50ms int\r\n");
	#endif
	if(touch_control==int_disable)
	{
	set_touchint(int_wait);
	return;	
	}
	
  
if(touck_effective_press)
{
		touch_sensibility_cnt++;
		      if(touch_sensibility_cnt>=para.touch_sensibility)
		      	{
				touch_sensibility_cnt=para.touch_sensibility+1;
				
		      	}
}			  
  tpCnt = 0;
  //Tp_T = 0;
  Tp_mao=0;
//	   	for (i = 0; i < 10; i++)
//   {
//			if((gs_tpInfo[i].state==0)&&(gs_tpInfo[i].sendCnt==2))//R two over
//			{
//				memcpy(&gs_tpInfo_copy[0],*(&gs_tpInfo+(i+1)*12),(9-i)*12);
//				memcpy(*(&gs_tpInfo+i*12),&gs_tpInfo_copy[0],(9-i)*12);//delete
//			}
//		}

   
    for (Tp_i = 0; Tp_i < 10; Tp_i++)//cyw 他的软件只支持3个键 MAX
		{
			 
			if (gs_tpInfo[Tp_i].state==0)
			{
				if (gs_tpInfo[Tp_i].sendCnt < 2)
				{
					if(gs_tpInfo[Tp_i].T_flag_new==1)
					{
				  
					gs_tpInfo[Tp_i].sendCnt++;
					tpData[tpCnt].state = 'R';
					tpData[tpCnt].x = gs_tpInfo[Tp_i].x;
					tpData[tpCnt].y = gs_tpInfo[Tp_i].y;
					tpCnt++;
					if(gs_tpInfo[Tp_i].sendCnt==2) gs_tpInfo[Tp_i].T_flag_new = 0;
					}
				}
			}
		}
		
    if(tpCnt==3)
		{
			Tp_i=255;
			goto mark1;
		}
		
    for (Tp_i = 0; Tp_i < 10; Tp_i++)//cyw 他的软件只支持3个键 MAX
		{
			if ((gs_tpInfo[Tp_i].state==1)||(gs_tpInfo[Tp_i].state==2))
			{
				gs_tpInfo[Tp_i].sendCnt = 0;
				tpData[tpCnt].state = 'T';
				tpData[tpCnt].x = gs_tpInfo[Tp_i].x;
				tpData[tpCnt].y = gs_tpInfo[Tp_i].y;
 				tpCnt++;
				gs_tpInfo[Tp_i].state = 2;
				gs_tpInfo[Tp_i].T_flag_new = 1;
			//	Tp_T++;
				//if(Tp_T>=MAX_CTP_POINT) break;
				if(tpCnt>=3) break;
			}
			
		}
		mark1:	
		for (Tp_i++; Tp_i < 10; Tp_i++)//cyw 他的软件只支持3个键 MAX
		{
			if ((gs_tpInfo[Tp_i].state==1)||(gs_tpInfo[Tp_i].state==2))
			{
				gs_tpInfo[Tp_i].sendCnt = 0;
				gs_tpInfo[Tp_i].state = 3;
				gs_tpInfo[Tp_i].T_flag_new = 0;
			}
		}
		
	
		
		// sysprintf("I AM HERE\r\n");
		
//		if (tpCnt > MAX_CTP_POINT) 
//		{
//			
//			tpCnt = MAX_CTP_POINT;
//		}	
		if (tpCnt > 0) 
		{
       
			if(touch_sensibility_cnt>=para.touch_sensibility)
        {
					
					send_tcp_xy_mul(tpData, tpCnt);
        }
        //先按先出
					#ifdef  SYSUARTPRINTF 
	         //sysprintf("tr send over\r\n");
	        #endif
  
				
       for(Tp_i = 0; Tp_i < 10; Tp_i++)
		   {
			 if(gs_tpInfo[Tp_i].T_flag_new == 1)
			 {
				gs_tpInfo_copy[Tp_mao].sendCnt=gs_tpInfo[Tp_i].sendCnt;
				 gs_tpInfo_copy[Tp_mao].state=gs_tpInfo[Tp_i].state;
				 gs_tpInfo_copy[Tp_mao].T_flag_new=gs_tpInfo[Tp_i].T_flag_new;
				 gs_tpInfo_copy[Tp_mao].x=gs_tpInfo[Tp_i].x;
				  gs_tpInfo_copy[Tp_mao].y=gs_tpInfo[Tp_i].y;
				 Tp_mao++;
			 }
		   }
			 #ifdef  SYSUARTPRINTF 
	         //sysprintf("gs_tpInfo 1\r\n");
	        #endif
			 
       for(Tp_i = 0; Tp_i < 10; Tp_i++)
		   {
			 if(gs_tpInfo[Tp_i].T_flag_new == 0)
			  {
				 	gs_tpInfo_copy[Tp_mao].sendCnt=gs_tpInfo[Tp_i].sendCnt;
				 gs_tpInfo_copy[Tp_mao].state=gs_tpInfo[Tp_i].state;
				 gs_tpInfo_copy[Tp_mao].T_flag_new=gs_tpInfo[Tp_i].T_flag_new;
				 gs_tpInfo_copy[Tp_mao].x=gs_tpInfo[Tp_i].x;
				  gs_tpInfo_copy[Tp_mao].y=gs_tpInfo[Tp_i].y;
				 Tp_mao++;
			  }
		   }
			 #ifdef  SYSUARTPRINTF 
	        // sysprintf("gs_tpInfo 2\r\n");
	        #endif
//        for(i = 0; i < 10; i++)
//			 {
//				 gs_tpInfo[i].sendCnt=gs_tpInfo_copy[i].sendCnt;
//				 gs_tpInfo[i].state=gs_tpInfo_copy[i].state;
//				 gs_tpInfo[i].T_flag_new=gs_tpInfo_copy[i].T_flag_new;
//				 gs_tpInfo[i].x=gs_tpInfo_copy[i].x;
//				  gs_tpInfo[i].y=gs_tpInfo_copy[i].y;
//			 }
			 
			 memcpy(&gs_tpInfo[0],&gs_tpInfo_copy[0],16*10); 
			 
			  #ifdef  SYSUARTPRINTF 
	       //  sysprintf("gs_tpInfo 3\r\n");
	        #endif
			 		
		}
	  else
	  {
			Touch_tim_stop();
			touch_sensibility_cnt=0;
			touck_effective_press=0;
			touch_send_imm=0;
	  }
   
    

  #ifdef  SYSUARTPRINTF 
	        // sysprintf("50ms over\r\n");
	        #endif

	
	
}



void ETMR2_IRQHandler(void)
{
	#ifdef  SYSUARTPRINTF
	sysprintf("ETMR2 Come\r\n");
	#endif
	
	check_tp_send();
	ETIMER_ClearIntFlag(2);

}



