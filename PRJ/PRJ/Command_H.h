#ifndef COMMAND_H_H
#define COMMAND_H_H


 


 




#define RX_BUFF_UART2_MAX_LEN  			(256*4)  	//定义最大接收字节数 200
#define  DOWN_LOAD_DATA_BUFFER_START_ADD      bmp_layer1_BUFFER 
typedef union 
{
   uint8_t val;
   struct 
	 {
		 uint8_t COPY_HEX_err:1;//low
		 uint8_t Ren_acknot06_err:1;
		 uint8_t ACK_Baud_TIMEOUT:1;
		 uint8_t ACK_Eraes_TIMEOUT:1;
		 uint8_t ACK_ProS_TIMEOUT:1;
		 uint8_t ACK_ProData_TIMEOUT:1;//high
	 }bits;		 
}Download_ERR_U;		

typedef struct
{
    uint8_t  data_maohao;      //冒号
    uint8_t  data_long;        //??
    uint32_t data_addr;        //??
    uint8_t  data_type;        //????,0-??,1-??
    uint8_t  data_arry[16];            //??
    uint8_t  data_cs;                //???
    uint8_t  data_over[2];     //0x0a0x0d
}INTEL_HEX_TYPE;
	


void set_download_task(uint8_t a);
void download_process(void);
void set_download_task(uint8_t a);
void download_gpio_set_input(void)  ;
void download_gpio_set_output(void) ;
void delay_us1(int usec);
void UART_TOIO(void);
#endif
