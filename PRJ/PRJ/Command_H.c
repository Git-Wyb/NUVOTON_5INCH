#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "sys.h"
#include "DIP_SW.h"
#include "Command_all.h"
#include "Aprotocol.h"
#include "ff.h"
#include "BSP_init.h"
#include "uart.h"
#include "DIP_SW.h"
#include "display.h"
#include "gpio.h"

extern DOWNLOAD_TYPE  download;
extern uint8_t updata_hex_fromSW2;
uint8_t ver_temp[8];
Download_ERR_U Download_code = {0};
extern SYSTEMERRORSTATUS_TYPE  systerm_error_status;
uint8_t   rx_finish_state_uart2;
static uint8_t    *tx_buff_uart2,*tx_buff_uart2_noshift;//[TX_BUFF_UART2_MAX_LEN];     //Ω” ’ª∫≥Â,◊Ó¥ÛUSART_REC_LEN∏ˆ◊÷Ω⁄.
uint16_t  tx_buff_uart2_num=0;
static uint8_t    *rrx_buff_uart2,*rrx_buff_uart2_noshift;//[RX_BUFF_UART2_MAX_LEN];     //Ω” ’ª∫≥Â,◊Ó¥ÛUSART_REC_LEN∏ˆ◊÷Ω⁄.
uint16_t  rrx_buff_uart2_cnt=0;
extern LAYER_SDRAM_STR display_layer_sdram;
extern uint8_t uart2_send_finish_state;
extern int gs_usb_mount_flag;
extern FATFS usb_fatfs;
const char filename_new_hex_const[60]="3:/BELMONT_HEX/TBXXXXXX.HEX";
extern uint8_t g_open_flag1,g_open_flag2;
static FIL   *g_file1,*g_file2;
extern uint8_t* BaseData_ARR;
extern uint8_t FLAG_SW_FINISH;
extern  UINT8 *RX_Test/*[500]*/;
//Õ®—∂÷ÿ÷√
uint8_t updata_hex_fromSW2=0;
uint32_t shift_pointer(uint32_t ptr, uint32_t align);

extern UART_T param;
extern volatile  uint32_t UART_TX_REAL,UART_TX_WANT;	


void UART_TOIO(void)
{
	//pe 2 3
	REG_OPERATE(REG_SYS_GPE_MFPL,0X0000FF00,clear);
 // REG_OPERATE(REG_SYS_GPE_MFPL,0X00009900,set);
	//PH8 9
	REG_OPERATE(REG_SYS_GPH_MFPH,0X000000FF,clear);
	//PI3 PI4
	REG_OPERATE(REG_SYS_GPI_MFPL,0X000FF000,clear);
	
	GPIO_OpenBit(GPIOE,BIT2|BIT3, DIR_OUTPUT, PULL_UP);
	GPIO_OpenBit(GPIOH,BIT8|BIT9, DIR_OUTPUT, PULL_UP);
	GPIO_OpenBit(GPIOI,BIT3|BIT4, DIR_OUTPUT, PULL_UP);
	
	GPIO_Clr(GPIOE, BIT2);
	GPIO_Clr(GPIOE, BIT3);
	GPIO_Clr(GPIOH, BIT8);
	GPIO_Clr(GPIOH, BIT9);
	GPIO_Clr(GPIOI, BIT3);
	GPIO_Clr(GPIOI, BIT4);
}

//œ¬‘ÿ≥Ã–Úµƒø⁄≥ı ºªØ
void download_gpio_set_output(void)  //≈‰÷√≥… ‰≥ˆ DOWNLOAD_HEX_INIT(void)
{

//	GPIO_InitTypeDef  GPIO_InitStruct;
//	__HAL_RCC_GPIOA_CLK_ENABLE();
//	/**********PAout**************************/
//	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;//GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStruct.Pull=GPIO_NOPULL; //GPIO_PULLUP;
//	GPIO_InitStruct.Speed =GPIO_SPEED_HIGH; // GPIO_SPEED_LOW;
//	GPIO_InitStruct.Pin = DOWNLOAD_TOOL_PIN|DOWNLOAD_RESET_PIN;
//	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	
	GPIO_CloseBit(GPIOF,BIT13|BIT14);
	
	REG_OPERATE(REG_SYS_GPF_MFPH,0X0FF00000,clear);
	GPIO_OpenBit(GPIOF,BIT13|BIT14, DIR_OUTPUT, NO_PULL_UP);
	 
	  DOWNLOAD_TOOL_CLR();
	  DOWNLOAD_RESET_CLR();
}
      
void download_gpio_set_input(void)   //≈‰÷√≥… ‰»ÎDOWNLOAD_HEX_IN(void) 
{
	
	
//	 GPIO_InitTypeDef  GPIO_InitStruct; 
//	__HAL_RCC_GPIOA_CLK_ENABLE();
//	 /**********PAinput*************************/
//	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
//	GPIO_InitStruct.Pull = GPIO_PULLUP;
//	GPIO_InitStruct.Pin = DOWNLOAD_TOOL_PIN|DOWNLOAD_RESET_PIN;	
//	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_CloseBit(GPIOF,BIT13|BIT14);
	
	REG_OPERATE(REG_SYS_GPF_MFPH,0X0FF00000,clear);
	GPIO_OpenBit(GPIOF,BIT13|BIT14, DIR_INPUT, PULL_UP);
	
	 
}


void delay_us1(int usec)
{
    volatile int  loop = 30 * usec;
    while (loop > 0) loop--;
}

void set_download_task(uint8_t a)
{
	download.task=a;
	tx_buff_uart2_noshift = malloc(TX_BUFF_UART2_MAX_LEN+64);
	tx_buff_uart2 = (uint8_t *)((32+(uint8_t   *)shift_pointer((uint32_t)tx_buff_uart2_noshift,32)));
	tx_buff_uart2 = (uint8_t *)((uint32_t)tx_buff_uart2|0x80000000);
	
	rrx_buff_uart2_noshift = malloc(RX_BUFF_UART2_MAX_LEN+64);
	rrx_buff_uart2 = (uint8_t *)((32+(uint8_t   *)shift_pointer((uint32_t)rrx_buff_uart2_noshift,32)));
	rrx_buff_uart2 = (uint8_t *)((uint32_t)rrx_buff_uart2|0x80000000);
}

//’‚∏ˆ «◊®√≈œ¬≥Ã–ÚµƒΩ”ø⁄
void uart2_send_buff(uint8_t *buff,uint16_t len)   
{

   while((REG_UART1_FSR&UART_FSR_TX_EMPTY_Msk))
	 {
		 //if((REG_UART1_FSR&UART_FSR_TX_EMPTY_Msk)==0)
		 //{
		//	 break;
		 //}
		  sysprintf("uart1 not empty\r\n");
	 }
////////	while(uart2_send_finish_state)
////////	{
////////		;
////////	}
//	 if(HAL_UART_Transmit_IT(&UART2_Handler, (uint8_t*)buff, len)!= HAL_OK)
//  {
//		
//	}
	uartWrite(UART1,(uint8_t*)buff, len);
	uart2_send_finish_state = 1;


}




void Power_application(void)
{
	uint16_t i;
	DOWNLOAD_TOOL_CLR();
	DOWNLOAD_RESET_CLR();  
	download_gpio_set_input();
	delay_us1(2300); //500);
	POWER_5V_OFF();//for test
      // uart2_IO( );//qdhai add for µππ‡1.9V
       for(i=0;i<400;i++)
       	{
			delay_us1(2300);  //	delay_us1(100000);
       	}

	download_gpio_set_output();
	DOWNLOAD_TOOL_SET();
	POWER_5V_ON();
	//uart2_init(38400);	//qdhai add
	
	delay_us1(2300);
	DOWNLOAD_TOOL_CLR();
	delay_us1(2300);
	 
}
void Mode_setting(void)
{
	  DOWNLOAD_RESET_SET();
	  delay_us1(2300);
	  DOWNLOAD_TOOL_SET();
	  delay_us1(2300);
	  DOWNLOAD_TOOL_CLR();
	  delay_us1(340);//78.125  115200BPS 00H
	  DOWNLOAD_TOOL_SET();
	  delay_us1(2300);
}


uint8_t cal_sum_check(uint8_t *x_data,uint16_t x_num)  //GET_SUMCHECK
{
     uint16_t Tp_i  =0;
     uint8_t Tp_sum = 0;
    Tp_sum =0;
     for(Tp_i = 1;Tp_i < x_num;Tp_i++)
     {
			 Tp_sum = Tp_sum - x_data[Tp_i];
		 }	
		 return Tp_sum;
		 
}


//ºÏ≤ÈRL78µƒªÿŒƒ
//0  ß∞‹
//1 ≥…π¶
uint8_t decode_ack(uint8_t *buff)   // 1 ±Ì æ≥…π¶ 0  ß∞‹  GET_RL78_ACK(uint8_t x_ACK_LONG)
{
	//if(download.use_uart2==0) return 0;
	
	if(buff[2]==0x6)
	{
		return 1;
	}
      else
      	{
				Download_code.bits.Ren_acknot06_err=1;	
		return 0;
      	}
}






uint8_t  Baud_setting(void)//01 03 9A 00 31 32 03 
{

	   uint32_t Tp_count  = 0x298D55; // 2s
	   uint8_t  Tp_ack    = 0;  
	uint16_t retval,Tp_rr;
	
	  download.bps= 115200;
	  uart2_init_download(115200);

	  //“‘±„Ω” ’”√
      
	
	
	  tx_buff_uart2_num = 0;
	  tx_buff_uart2[tx_buff_uart2_num++] = 0X01;
	  tx_buff_uart2[tx_buff_uart2_num++] = 0X03;
	  tx_buff_uart2[tx_buff_uart2_num++] = 0X9A;
	  tx_buff_uart2[tx_buff_uart2_num++] = 0X00;//115200
	  tx_buff_uart2[tx_buff_uart2_num++] = 0X31;//4.9v
	  tx_buff_uart2[tx_buff_uart2_num] = cal_sum_check((uint8_t *)tx_buff_uart2,tx_buff_uart2_num);
	  tx_buff_uart2_num++;
	  tx_buff_uart2[tx_buff_uart2_num++] = 0x03;
	  
		 rrx_buff_uart2_cnt=0;
	 //memset(bak_rx_buff_uart2,0x00,20);
	 rx_finish_state_uart2=0;
		uart2_send_buff(tx_buff_uart2,tx_buff_uart2_num); //SEND_DOWNLOAD_HEX_DATA(DOWNLOAD_DATA_TX,DOWNLOAD_DATA_COUNT);

       	memset(RX_Test,0,500);
	    retval = uartRead(param.ucUartNo, RX_Test,500);

	  //USART_TXTORX_DOWNLOAD(115200);
	  while(Tp_count)  //02 03 06 20 00 D7 03 
		{
				memset(RX_Test,0,500);
	    retval = uartRead(param.ucUartNo, RX_Test, 500);
			for(Tp_rr=0;Tp_rr<retval;Tp_rr++)
			{
			if(((rrx_buff_uart2_cnt+1)%RX_BUFF_UART2_MAX_LEN)!=0)
		{
			rrx_buff_uart2[rrx_buff_uart2_cnt++]=RX_Test[Tp_rr] ;
			sysprintf("%x\r\n",RX_Test[Tp_rr]);
		//	rx_buff_uart2_delay = 2;
		}
	    }
			if(rrx_buff_uart2_cnt==7)
				{
					//rx_finish_state_uart2=0;
					Tp_ack = decode_ack(rrx_buff_uart2);    //GET_RL78_ACK(7);
				}
			 Tp_count -- ;
			 if(Tp_count == 0)  
			 	{
					Download_code.bits.ACK_Baud_TIMEOUT = 1;
			 	 return 0;
			 	}
			 if(Tp_ack   == 1)  
			 	{
			 	 return 1;
			 	}
 	  }
	  //delay_us1(2300);
	  return 0;
}

//01 04 22 00 00 00 DA 03 
//01 04 22 00 04 00 D6 03 

void Block_erase(uint32_t x_addr)
{
	
	 tx_buff_uart2_num = 0;
	 tx_buff_uart2[tx_buff_uart2_num++] = 0x01;
	 tx_buff_uart2[tx_buff_uart2_num++] = 0x04;
	 tx_buff_uart2[tx_buff_uart2_num++] = 0x22;
	 tx_buff_uart2[tx_buff_uart2_num++] = x_addr&0xff;
	 tx_buff_uart2[tx_buff_uart2_num++] = (x_addr>>8)&0xff;
	 tx_buff_uart2[tx_buff_uart2_num++] = (x_addr>>16)&0xff;
	 tx_buff_uart2[tx_buff_uart2_num] = cal_sum_check((uint8_t *)tx_buff_uart2,tx_buff_uart2_num);
	  tx_buff_uart2_num++;
	 tx_buff_uart2[tx_buff_uart2_num++] = 0x03;
	
	 rrx_buff_uart2_cnt=0;
	 //memset(bak_rx_buff_uart2,0x00,20);
		 rx_finish_state_uart2=0;
	 uart2_send_buff(tx_buff_uart2,tx_buff_uart2_num); // SEND_DOWNLOAD_HEX_DATA(DOWNLOAD_DATA_TX,DOWNLOAD_DATA_COUNT);
}

//01 07 40 00 00 00 FF FF 01 BA 03 
uint8_t  Programme_start(void)
{
	//U32_TBHEX_LONG  = 1023;
	 uint32_t Tp_endaddr = 0;
   	uint32_t Tp_count  =  0x298D55; // 2s
	 uint8_t  Tp_ack    = 0;  
	
	uint16_t retval,Tp_rr;
	//download.bps = 115200;
	 //uart2_init_download(115200);
	if(download.hex_len%1024 == 0)
	{
	Tp_endaddr = (download.hex_len/1024)*1024 -1;
	}
	else
	{
	Tp_endaddr = ((download.hex_len/1024) + 1)*1024 -1;
	}
	
	
	tx_buff_uart2_num = 0;
	tx_buff_uart2[tx_buff_uart2_num++] = 0x01;
	tx_buff_uart2[tx_buff_uart2_num++] = 0x07;
	tx_buff_uart2[tx_buff_uart2_num++] = 0x40;
	tx_buff_uart2[tx_buff_uart2_num++] = 0;
	tx_buff_uart2[tx_buff_uart2_num++] = 0;
	tx_buff_uart2[tx_buff_uart2_num++] = 0;
	tx_buff_uart2[tx_buff_uart2_num++] = Tp_endaddr&0xff;
	tx_buff_uart2[tx_buff_uart2_num++] = (Tp_endaddr>>8)&0xff;
	tx_buff_uart2[tx_buff_uart2_num++] = (Tp_endaddr>>16)&0xff;
	tx_buff_uart2[tx_buff_uart2_num] = cal_sum_check((uint8_t *)tx_buff_uart2,tx_buff_uart2_num);
	tx_buff_uart2_num++;
	tx_buff_uart2[tx_buff_uart2_num++] = 0x03;
	
	 rrx_buff_uart2_cnt=0;
	// memset(bak_rx_buff_uart2,0x00,20);
         rx_finish_state_uart2=0;
	uart2_send_buff(tx_buff_uart2,tx_buff_uart2_num); //SEND_DOWNLOAD_HEX_DATA(DOWNLOAD_DATA_TX,DOWNLOAD_DATA_COUNT);
	//USART_TXTORX_DOWNLOAD(115200);

	 //“‘±„Ω” ’”√
  
	 
	  while(Tp_count)
		{
			memset(RX_Test,0,500);
	    retval = uartRead(param.ucUartNo, RX_Test, 500);
			for(Tp_rr=0;Tp_rr<retval;Tp_rr++)
			{
			if(((rrx_buff_uart2_cnt+1)%RX_BUFF_UART2_MAX_LEN)!=0)
		{
			rrx_buff_uart2[rrx_buff_uart2_cnt++]=RX_Test[Tp_rr] ;
			sysprintf("%x\r\n",RX_Test[Tp_rr]);
		//	rx_buff_uart2_delay = 2;
		}
	}
			if(rrx_buff_uart2_cnt==5)
				{
					rx_finish_state_uart2=0;
					Tp_ack = decode_ack(rrx_buff_uart2);    //GET_RL78_ACK(7);
				}
			 Tp_count -- ;
			 if(Tp_count == 0)  
			 {
			   Download_code.bits.ACK_ProS_TIMEOUT = 1;
				 return 0;
			 }
			 if(Tp_ack   == 1)  
			 {
			   sysprintf("Programme_start ok\r\n");
				 return 1;
			 }
 	  }
		
		return 0;
	//delay_us1(2300);
}


uint8_t Block_erase_N(void)
{
	 uint32_t Tp_i;
	  uint32_t Tp_count  =  0x298D55; // 2s;
	 uint8_t  Tp_ack    = 0; 
	uint16_t retval,Tp_rr;
	
	for(Tp_i =0;Tp_i < download.hex_len;)
	{
		Tp_count  = 0x298D55;
		//download.bps = 115200;		
	  	//uart2_init_download(115200);
		
		rrx_buff_uart2_cnt=0;
		 
		Block_erase(Tp_i);
		
		//USART_TXTORX_DOWNLOAD(115200);
		//LED_POWER_TOGGLE();
		//“‘±„Ω” ’”√
        	
	       Tp_ack=0;
	//	memset(RX_Test,0,sizeof(RX_Test));
	 //   retval = uartRead(param.ucUartNo, RX_Test, sizeof(RX_Test));
		while(Tp_count)
		{
			//feed_iwdg();//Œππ∑ œ÷‘⁄∫√œÒ”–30S
			memset(RX_Test,0,500);
	    retval = uartRead(param.ucUartNo, RX_Test, 500);
			for(Tp_rr=0;Tp_rr<retval;Tp_rr++)
			{
			if(((rrx_buff_uart2_cnt+1)%RX_BUFF_UART2_MAX_LEN)!=0)
		{
			rrx_buff_uart2[rrx_buff_uart2_cnt++]=RX_Test[Tp_rr] ;
			sysprintf("%x\r\n",RX_Test[Tp_rr]);
		//	rx_buff_uart2_delay = 2;
		}
	}
			
			if(rrx_buff_uart2_cnt==5)
				{
					rx_finish_state_uart2=0;
					Tp_ack = decode_ack(rrx_buff_uart2);    //GET_RL78_ACK(7);
				}
			Tp_count -- ;
			if(Tp_count == 0)  
			{
				Download_code.bits.ACK_Eraes_TIMEOUT=1;
				return 0;
			}
			if(Tp_ack   == 1)  
			{	
			   sysprintf("EARSE ACK OK\r\n");
				break;
			} 
			
		}
	//	LED_POWER_TOGGLE();
		Tp_i = Tp_i + 1024;
		//delay_us1(2300);
	}
	return 1;
}

uint8_t Programme_data_send(void)
{
	 uint32_t Tp_i =0;
	 uint16_t  Tp_data_real=0;
	 uint16_t Tp_j =0;
	 uint32_t Tp_count  =  0x298D55; // 2s;
	 uint8_t  Tp_ack    = 0; 
	uint16_t retval,Tp_rr;
	uint16_t Tp_send;
	//sysprintf("Programme_data_send,download.hex_len=%x\r\n",download.hex_len);
	for(Tp_i=0;Tp_i<download.hex_len;)
	{
		// sysprintf("Programme_data_send\r\n");
		//feed_iwdg();//Œππ∑ œ÷‘⁄∫√œÒ”–30S
		Tp_count  =  0x298D55; // 2s;
		Tp_ack    = 0; 
		 //download.bps = 115200;
	 	 //uart2_init_download(115200);
		
		tx_buff_uart2_num = 0;
		
		tx_buff_uart2[tx_buff_uart2_num++] =0x02;
		if((Tp_i+256)>=download.hex_len)
		{
			Tp_data_real = download.hex_len - Tp_i;
		}
		else
		{
			Tp_data_real = 256;
		}
		tx_buff_uart2[tx_buff_uart2_num++] =Tp_data_real%256;
		for(Tp_j = 0;Tp_j < Tp_data_real;Tp_j ++)
		{
			tx_buff_uart2[tx_buff_uart2_num++] =*(__IO uint8_t*)(DOWN_LOAD_DATA_BUFFER_START_ADD+Tp_i+Tp_j) ;
		}
		tx_buff_uart2[tx_buff_uart2_num] = cal_sum_check((uint8_t *)tx_buff_uart2,tx_buff_uart2_num);
	  	tx_buff_uart2_num++;
		if((Tp_i+256)>=download.hex_len)
		{
	  		tx_buff_uart2[tx_buff_uart2_num++] = 0x03;
		}
		else
		{
			tx_buff_uart2[tx_buff_uart2_num++] = 0x17;	
		}
			//“‘±„Ω” ’”√
     rrx_buff_uart2_cnt=0;
	// memset(bak_rx_buff_uart2,0x00,20);
             rx_finish_state_uart2=0; 
		
		sysprintf("Programme_data_send ,tx_buff_uart2_num=%X\r\n",tx_buff_uart2_num);
		for(Tp_send=0;Tp_send<tx_buff_uart2_num;Tp_send++)
		{
		uart2_send_buff(tx_buff_uart2+Tp_send,1); //SEND_DOWNLOAD_HEX_DATA(tx_buff_uart2,tx_buff_uart2_num);
		//uart2_send_buff(tx_buff_uart2+50,tx_buff_uart2_num-50);
		}
		//USART_TXTORX_DOWNLOAD(115200);

	
	 
		while(Tp_count)
		{
			//feed_iwdg();//Œππ∑ œ÷‘⁄∫√œÒ”–30S
			
     	memset(RX_Test,0,500);
	    retval = uartRead(param.ucUartNo, RX_Test, 500);
			for(Tp_rr=0;Tp_rr<retval;Tp_rr++)
			{
			if(((rrx_buff_uart2_cnt+1)%RX_BUFF_UART2_MAX_LEN)!=0)
		{
			rrx_buff_uart2[rrx_buff_uart2_cnt++]=RX_Test[Tp_rr] ;
			sysprintf("%x\r\n",RX_Test[Tp_rr]);
		//	rx_buff_uart2_delay = 2;
		}
	}
			if(rrx_buff_uart2_cnt==6)
				{
					rx_finish_state_uart2=0;
					Tp_ack = decode_ack(rrx_buff_uart2);    //GET_RL78_ACK(7);
				}
			Tp_count -- ;
			if(Tp_count == 0)  
			{
				Download_code.bits.ACK_ProData_TIMEOUT = 1;
				return 0;
			}
			if(Tp_ack   == 1)  
			{
				sysprintf("Programme_data_send ACK OK\r\n");
				break;
			}
		}
		Tp_i = Tp_i + Tp_data_real;
		//delay_us1(2300);
	}
	return 1;
}

//÷ÿ–¬∆Ù∂ØMCU
//void Restart_mcu(void)
//{
//	uint8_t i;
//	//Clear_sdram(0X41);//«Â¿Ìµ⁄“ª≤„∫Õ“∫æßœ‘ æ«¯”Ú 20171009 ≤√¥“≤≤ª«Â ƒø«∞÷ª÷ß≥÷750K¥Û–°µƒHEX ∂‡¡Àª≠√Ê“™≥ˆ¥Ì 
//	
//	//DOWNLOAD_TOOL_CLR();
//	//	DOWNLOAD_RESET_CLR();  
//	 
//	  delay_us1(2300);
//	  POWER_5V_OFF();
//	for(i=0;i<200;i++)
//       	{
//			delay_us1(2300);  //	delay_us1(100000);
//       	}	  	
//	// download_gpio_set_output();
//	download_gpio_set_input();
//	delay_us1(2300);
//	POWER_5V_ON();
//	delay_us1(2300);
//	DOWNLOAD_RESET_SET();
//}
void Restart_mcu(void)
{
	uint8_t i;
	//Clear_sdram(0X41);//??????h????????????? 20171009?Ù????? ?j????750K?????HEX ???????????? 
	
	//DOWNLOAD_TOOL_CLR();
	//	DOWNLOAD_RESET_CLR();  
	 UART_TOIO();
	 download_gpio_set_output();
	
		DOWNLOAD_TOOL_CLR();
		DOWNLOAD_RESET_CLR();  
	  delay_us1(2300);
	  POWER_5V_OFF();
	for(i=0;i<200;i++)
       	{
			delay_us1(2300);  //	delay_us1(100000);
       	}	  	

	//download_gpio_set_input();
	delay_us1(12300);
	POWER_5V_ON();
	delay_us1(2300);
	DOWNLOAD_RESET_SET();
}


void Calculate_real_long(void)
{
	static uint32_t Tp_i;
	Tp_i= download.hex_len;
	for( ;Tp_i>2;Tp_i--)
	{
		if(*(__IO uint8_t*)(DOWN_LOAD_DATA_BUFFER_START_ADD+Tp_i-2)==0xff)
		{
			download.hex_len --;
		}
		else
		{
			return;
		}
	}
}

static uint8_t extract_hex(FIL  file)   //FIL fsrc;    COPY_HEX_TO_SDRAM
{
	 uint16_t numOfReadBytes_hex = 0;
	 FRESULT res;
	
	 uint8_t Tp_Reallong = 0;
	 uint8_t Tp_i = 0;
	 uint8_t Tp_cs = 0;
	 uint32_t Tp_sub =0;
	 INTEL_HEX_TYPE  intel_hex;
	 uint8_t buff[48];  //Image_Buf 
	//download.hex_len = 0;
	while(1)
	{
		//feed_iwdg();//Œππ∑
		res = f_read(&file, buff, 3, (void *)&numOfReadBytes_hex);//∂¡»°–– ˝–≈œ¢
		if((numOfReadBytes_hex == 0) || (res != FR_OK))
		{
			return 0;
		}
		intel_hex.data_long = convet_2_hex_ASCII_to_hex(buff+1);
		intel_hex.data_maohao = buff[0];
		if(intel_hex.data_maohao!=':')
		{
			return 0;
		}
		if(intel_hex.data_long > 16)
		{
			return 0;
		}
		Tp_Reallong = intel_hex.data_long*2 + 10;
		res = f_read(&file, (uint8_t *)buff, Tp_Reallong, (void *)&numOfReadBytes_hex);//∂¡»°–– ˝–≈œ¢
		if((numOfReadBytes_hex == 0) || (res != FR_OK))
		{
			return 0;
		}
		intel_hex.data_addr = (convet_2_hex_ASCII_to_hex(buff)<<8)+convet_2_hex_ASCII_to_hex(buff+2);
		intel_hex.data_type = convet_2_hex_ASCII_to_hex(buff+4);
		for(Tp_i=0;Tp_i<intel_hex.data_long;Tp_i++)
	  	{
			intel_hex.data_arry[Tp_i] = convet_2_hex_ASCII_to_hex(buff + 6 + Tp_i*2);
		}
		intel_hex.data_cs = convet_2_hex_ASCII_to_hex(buff + 6 + Tp_i*2);
		
		intel_hex.data_over[0]=buff[8 + Tp_i*2];
		intel_hex.data_over[1]=buff[9 + Tp_i*2];
		
		if((intel_hex.data_over[0] != 0x0d)&&(intel_hex.data_over[0] != 0x0a))
		{
			return 0;
		}
		
		Tp_cs = 0;
		Tp_cs = intel_hex.data_long + (intel_hex.data_addr>>8) + (intel_hex.data_addr&0xff) + intel_hex.data_type;
		for(Tp_i=0;Tp_i<intel_hex.data_long;Tp_i++)
		{
			Tp_cs = Tp_cs + intel_hex.data_arry[Tp_i];
		}
		Tp_cs = ~(Tp_cs)+1;
		if(Tp_cs != intel_hex.data_cs)
		{
			return 0;
		}
		if(intel_hex.data_type == 0x01)//Œƒº˛µƒΩ· ¯
		{
			download.hex_len =Tp_sub;//0x10000;//Tp_sub;//0X1000;// Tp_sub;
			return 1;
		}
		else if(intel_hex.data_type == 0x03)//Œƒº˛µƒø™Õ∑
		{
			download.hex_len =0;
			Tp_sub = 0;
		}
		else if(intel_hex.data_type == 0)//÷ª”– ˝æ›≤≈“™
		{
		 
			///for(Tp_i =0 ; Tp_i < intel_hex.data_long;Tp_i++)
			//{
			//	if(intel_hex.data_arry[Tp_i]!=0xff)
			//	{
			//		break;
			//	}
			//}
			//if(Tp_i!=intel_hex.data_long)
			//{
			memcpy((uint8_t *)(DOWN_LOAD_DATA_BUFFER_START_ADD+Tp_sub),(uint8_t *)(intel_hex.data_arry),intel_hex.data_long);
			//memset((uint8_t *)(bmp_layer1_BUFFER+Tp_sub),0,10);
		  	Tp_sub = Tp_sub + intel_hex.data_long;
			if(Tp_sub >=0x232000)
			{
				return 0;
			}
			///download.hex_len=200;
			//}
		}
	}
		
}


extern uint8_t ver_temp[8];
int copy_download_hex_to_sdram(void)  //0 ≥…π¶ £≠1  ß∞‹
{
//	uint8_t ack;
	FRESULT res;
	//	int type = 0;
//	char tmpFileName[64];
//	char nandFileName[64];
//	char path[64];
	DIR dir ={0};
//	int i;
//	int ret = 0;
	
  char *fn=NULL;
	FIL fsrc;      	// file objects
  char filename_new_hex[60]="3:/BELMONT_HEX/TBXXXXXX.HEX";//’‚∏ˆ «”√¿¥¥Ê¥¢¥¯Œƒº˛º–µƒŒƒº˛¬∑æ∂µƒ
  FILINFO fno = {0};
	//static char lfn[_MAX_LFN +1];
 
	if (gs_usb_mount_flag != 1)
	{
		systerm_error_status.bits.usb_unable_connect=1;
		return  -1;
	}
	
	//res = f_readdir(&dir, NULL);
	f_mount(0, "3:", 1);
	res = f_mount(&usb_fatfs, "3:", 1); 
	
	res = f_readdir(&dir, NULL);
	
	res =f_opendir(&dir,"3:/BELMONT_HEX");
	if(res != FR_OK)//’“≤ªµΩŒƒº˛º–
	{
		//f_mkdir("1:/BELMONT_HEX");//…˙≥…ƒø¬º
		// ‰≥ˆ“ª∏ˆ’“≤ªµΩŒƒº˛µƒ¥ÌŒÛ
		systerm_error_status.bits.usb_cannot_find_hexortxt = 1;
		return -1;
	}
	else
	{
		f_closedir(&dir);
		res =f_opendir(&dir,"3:/BELMONT_HEX");  //?????????
		//f_closedir(&dir);
		
	}
	
	//fno.lfname =filename_new_bmp;
	//fno.fname = lfn;
	fno.fsize = 60;
	

	while(1)  //for (;;) 
	{
//feed_iwdg();//Œππ∑
		res = f_readdir(&dir, &fno);
		if (res != FR_OK || fno.fname[0] == 0) 
		{
			if(download.start==0) 
			systerm_error_status.bits.usb_cannot_find_hexortxt = 1;
			return -1;
		}

		if (fno.fname[0] == '.') continue;
		fn = fno.fname;
		// memset(MCU_VER_name3,0,sizeof(MCU_VER_name3));
		// strncpy(MCU_VER_name3,fn,8);
		if (fno.fattrib & AM_DIR) 
		{
			
			continue;
		} 
		else 
		{
			if((strstr(fn, ".hex")) || (strstr(fn, ".HEX")))//◊÷∑˚¥Æ∆•≈‰
			{				
				memcpy(filename_new_hex,filename_new_hex_const,sizeof(filename_new_hex_const));
				//memcpy(filename_new_hex+15,filename_new_bmp,strlen(filename_new_bmp));
				memcpy(filename_new_hex+15,fno.fname,12);
				//memset(ver_temp,0,8);
				if(strlen(fno.fname)!=12) return -1;
        memcpy(ver_temp,fno.fname,8);
				//øÕ»ÀÀµ≤ªπÊ∑∂Œƒº˛√˚TBXXXXXX.HEX
				//ƒ«√¥Œƒº˛√˚≥§∂»≤ªƒ‹¥Û”⁄45 ∑Ò‘Ú“™≥ˆ¥Ì
        
			////////////////	HAL_NVIC_DisableIRQ(PVD_IRQn);
				res = f_open(&fsrc, filename_new_hex, FA_OPEN_EXISTING | FA_READ);
				g_file1 = &fsrc;
			///////////////  HAL_NVIC_EnableIRQ(PVD_IRQn);
				
				
				if(res == FR_OK)
				{
					g_open_flag1 = 1;
					if(extract_hex(fsrc)==1)//≥…π¶æÕøΩ±¥∞Ê±æ
					{
						 download.start = 1;

					}//∞—HEXµƒ ˝æ›ƒ⁄»›øΩ±¥µΩSDRAM
				}
				//HEX_SEARCHED = 1;	
				//USB_OTG_BSP_mDelay(100);
//				ret = 0;
				/*
				while((HCD_IsDeviceConnected(&USB_OTG_Core)) && \
				(STM_EVAL_PBGetState (BUTTON_WAKEUP) == SET))
				{
				// Toggle_Leds();
				}
				*/
				if(g_open_flag1 == 1)
				{
				 //////////////// HAL_NVIC_DisableIRQ(PVD_IRQn);
				  f_close(&fsrc);
				  g_open_flag1 = 0;
			  //////////////////	HAL_NVIC_EnableIRQ(PVD_IRQn);
				}
				break;//÷ªÀ—À˜“ª∏ˆŒƒº˛
				//}
			}//HEX hex
		}
	}

	f_closedir(&dir);
	
 if(download.start)
 {
	 return 0;
 }
 else
 {
	
	 return -1;
 }


	
}



void download_process(void)  //COPY_SDRAM_TO_MCU(void)
{
	 uint8_t ack,Tp_res;
	static uint8_t read_hex_cnt=0;
	
    if(download.task)
   	{
    
			Download_code.val = 0;
		    ack=copy_download_hex_to_sdram(); //int copy_download_hex_to_sdram(void)  //0 ≥…π¶ £≠1  ß∞‹
        if(ack==0)
        {    
		      download.task=0;
						ack = 0X1C;
			     code_protocol_ack(get_command_xor(), 1, &ack,0);
        }	 
        else
		   {
          read_hex_cnt++;
			// change debug
		      if(read_hex_cnt>=3)//20181102
			    {
				     read_hex_cnt=0;
				     download.task=0;
						 Tp_res = 1;
						 Download_code.bits.COPY_HEX_err=1;
						 goto over_flag;
			    }				
		   }			
   	}
	 
	if( download.start == 1)
	{
		Touch_int_disable();
		read_hex_cnt = 0;
		
		///////////////stop_tim6( );  //NVIC_DisableIRQ(TIM7_IRQn);//Ω˚÷πTOUCH		
		///////////////HAL_NVIC_DisableIRQ(PVD_IRQn);
    		SetZuobiao(10, 400 + 20);      //
		lcd_printf("Update Software...");//LCD_DisplayStringLine( 30, "Update Software    ");				
    		Tp_res =0;		 								
	 	// delay_ms(10000);//10S	’‚∏ˆ ±º‰ «”√¿¥¡¨Ω”Õ®–≈µƒ ±º‰
	 	////////////feed_iwdg();//Œππ∑ œ÷‘⁄∫√œÒ”–30S
		Power_application();
		//feed_iwdg();//Œππ∑ œ÷‘⁄∫√œÒ”–30S
	  	Mode_setting();
		//feed_iwdg();//Œππ∑ œ÷‘⁄∫√œÒ”–30S
		//download.use_uart2=1;								  
	  	if(Baud_setting()==0) 
		{	
			Tp_res = 1;
			goto over_flag; 
		}
		delay_us1(2300);
		//////////feed_iwdg();//Œππ∑ œ÷‘⁄∫√œÒ”–30S
		
		if(Block_erase_N()==0) 
		{	
		  Tp_res = 1;
			goto over_flag; //for test
		}
		delay_us1(2300);
		///////////feed_iwdg();//Œππ∑ œ÷‘⁄∫√œÒ”–30S
	  	if(Programme_start()==0) 
		{
			Tp_res = 1;
			goto over_flag; 
		}
		delay_us1(2300);
		/////////////feed_iwdg();//Œππ∑ œ÷‘⁄∫√œÒ”–30S
		//Calculate_real_long();
		if(Programme_data_send()==0) 
		{	
			Tp_res = 1;
			goto over_flag;
		}
		delay_us1(2300);
		
over_flag:
		
	//	Clear_sdram(0X0F);
		if(Tp_res == 1)
		{			
			
			  delay_us1(2300);
			
       //////////HAL_UART_MspDeInit(&UART2_Handler);	//√ªº”’‚∏ˆœ¬√Ê≥ı ºªØπ˝≤ª»•£¨uart2_init“ª÷±”–÷–∂œΩ” ’
		  download.bps = 38400;
		  //uart2_init(38400);	 //USART_Config(38400);
			protocol_uart_init();
			
			lcd_printf("Update Failure     ");
       if(updata_hex_fromSW2 == 1)
       {
				 SetZuobiao(10, 400 + 20);
				 updata_hex_fromSW2 = 0;
				 lcd_printf_new("Update Failure         Code=0x%02x    ",Download_code.val);
				  FLAG_SW_FINISH = FLAG_SW_FINISH|0x02;
			 }
      
		 
      /////////////// TIM6_Init((50000-1),(108-1));	//for touch send  NVIC_EnableIRQ(TIM7_IRQn);
		  /////////////// start_tim6(50000);  
		   COMM_SOFT_DEINIT();
		  //////////////HAL_NVIC_EnableIRQ(PVD_IRQn); 
			 
		}
		else
		{
			
		  /////////feed_iwdg();//Œππ∑ œ÷‘⁄∫√œÒ”–30S
		  Restart_mcu();
		
		  ///////////HAL_UART_MspDeInit(&UART2_Handler);	//√ªº”’‚∏ˆœ¬√Ê≥ı ºªØπ˝≤ª»•£¨uart2_init“ª÷±”–÷–∂œΩ” ’
		  download.bps = 38400;
		  //uart2_init(38400);	 //USART_Config(38400);
			protocol_uart_init();
		
			lcd_printf("Update Success     ");			
			systerm_error_status.bits.comm_handle_finish = 1;//…˝º∂’˝≥£Ω· ¯
			if(updata_hex_fromSW2 == 1)
       {
				 SetZuobiao(10, 400 + 20);
				 updata_hex_fromSW2 = 0;
				 lcd_printf_new("Update Success     ");
				 FLAG_SW_FINISH = FLAG_SW_FINISH|0x01;
			 }
			// *(uint8_t *)(BaseData_ARR+Master_Ver_index*9+8) = 8;
  		// memcpy(BaseData_ARR+Master_Ver_index*9, ver_temp , *(uint8_t *)(BaseData_ARR+Master_Ver_index*9+8));
			 
      //////////////////// TIM6_Init((50000-1),(108-1));	//for touch send  NVIC_EnableIRQ(TIM7_IRQn);
		  //////////////// start_tim6(50000);  
		   COMM_SOFT_DEINIT();
		   ////////HAL_NVIC_EnableIRQ(PVD_IRQn); 

       			 
		}
		UART_TX_REAL = 0;
		UART_TX_WANT = 0;
		if(tx_buff_uart2_noshift) free(tx_buff_uart2_noshift);
		if(rrx_buff_uart2_noshift) free(rrx_buff_uart2_noshift);
		
		Touch_int_enable();
		 download.start =0;	
	}
	 
	  

	  //return 0;//‘› ±œ»∂ºªÿ0∞…
	  
	
	  
	
}
