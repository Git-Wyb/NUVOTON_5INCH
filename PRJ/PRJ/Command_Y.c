#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "sys.h"
#include "ff.h"
#include "Command_all.h"
#include "Aprotocol.h"


extern int gs_usb_mount_flag;
extern SYSTEMERRORSTATUS_TYPE  systerm_error_status;
uint8_t  LOGO_DOUBLE_VISIT = 0;
extern  AreaConfig gs_AreaInfo[16],Tp_gs_AreaInfo[16];
extern Dataclass4567_Info Fieldx_Info[16];
extern uint8_t* BaseData_ARR;
extern TIME_TYPE timeandday_now_app;
char* writeTextBuff=0; 
extern PARA_TYPE para;
extern uint32_t  time1ms_count_forrun;
extern uint32_t  time1ms_count_forrun_his;
extern uint32_t System_Runtime_Lasttime;
uint16_t System_Run_Cycle = 0;//
extern FIL file;
extern uint32_t logodata_sdrambuffer_addr_arry[16];
uint32_t Tp_data1;
extern struct LOGO_Y5Y6_TYPE LOGO_Y5Y6_str;
extern FATFS usb_fatfs;

void DIVIDED_DATA_DEC3BYTE(uint16_t x_DATA,uint8_t *x_BUFF)
{
	 x_BUFF[0]=(x_DATA/100)+'0';
	 x_DATA = x_DATA%100;
	 x_BUFF[1]=(x_DATA/10)+'0';
	 x_DATA = x_DATA%10;
	 x_BUFF[2]=(x_DATA)+'0';
}			

 void field_info_save(void)
{
		uint32_t Tp_i;
	uint32_t Tp_data;
	
	for(Tp_i= 0;Tp_i<48;Tp_i++)
	{
		switch(Tp_i%3)
		{
		  case 0:
			 Tp_data = Fieldx_Info[Tp_i/3].cycle;	
			break;
			case 1:				
			 Tp_data = Fieldx_Info[Tp_i/3].num;
			break;
			case 2:				
			 Tp_data = Fieldx_Info[Tp_i/3].loop;
			break;
			default:
			break;
		}
		sprintf((char *)(BaseData_ARR+(112+Tp_i)*9),"%08X",Tp_data);
	}
}
int GetUsbMountFlag(void)
{
	if(gs_usb_mount_flag!=1)
	{
		return 0;
	}
	return gs_usb_mount_flag;
}

void DIVIDED_DATA_HEX2BYTE(uint8_t x_FSC,uint8_t *x_BUFF)
{
	if((x_FSC/16)>9)
	{
		x_BUFF[0]='A'+(x_FSC/16-10);
	}
	else
	{
		x_BUFF[0]='0'+x_FSC/16;
	}
	if((x_FSC%16)>9)
	{
		x_BUFF[1]='A'+(x_FSC%16-10);
	}
	else
	{
		x_BUFF[1]='0'+(x_FSC%16);
	}
}

void DATATOSTR(char *x_des,uint8_t *x_sou,uint32_t x_long)
{
	uint32_t Tp_i = 0;
	uint32_t Tp_mirror_i=0;
	
	
	for(Tp_i =0 ;Tp_i <x_long;Tp_i++)
	{
		Tp_mirror_i =  x_long  - Tp_i - 1;//?????j  ???????????
		//Tp_mirror_i = Tp_i;//??????  ?????????
		if((x_sou[Tp_i]/16)<10)
		  x_des[Tp_mirror_i*2]=x_sou[Tp_i]/16+'0';
		else
			x_des[Tp_mirror_i*2]=x_sou[Tp_i]/16-10+'A';
		if((x_sou[Tp_i]%16)<10)
		  x_des[Tp_mirror_i*2+1]=x_sou[Tp_i]%16+'0';
		else
			x_des[Tp_mirror_i*2+1]=x_sou[Tp_i]%16-10+'A';
		
		//x_des[Tp_mirror_i*3+2]=32;
	}
}

static void Create_new_logo_file_name(uint8_t x_field,uint8_t *x_name)
{
		static uint8_t file_sub=0;
	  static uint8_t Tp_i;
    uint8_t *Tp_point;
	
	file_sub = 0;
	memset(x_name,0,60);
	x_name[file_sub++]='3';
	x_name[file_sub++]=':';
	x_name[file_sub++]='F';
	x_name[file_sub++]='U';
	x_name[file_sub++]='N';
	x_name[file_sub++]='C';
	x_name[file_sub++]='T';
	x_name[file_sub++]='I';
	x_name[file_sub++]='O';
	x_name[file_sub++]='N';
	x_name[file_sub++]='_';
	x_name[file_sub++]='M';
	x_name[file_sub++]='E';
	x_name[file_sub++]='M';
	x_name[file_sub++]='O';
	x_name[file_sub++]='R';
	x_name[file_sub++]='Y';
	x_name[file_sub++]='/';
	if(x_field == 0)
	{
	x_name[file_sub++]='B';
	x_name[file_sub++]='A';
	x_name[file_sub++]='S';
	x_name[file_sub++]='E';
	x_name[file_sub++]='D';
	x_name[file_sub++]='A';
	x_name[file_sub++]='T';
	x_name[file_sub++]='A';
	}
	else
	{
		x_name[file_sub++]='F';
	x_name[file_sub++]='I';
	x_name[file_sub++]='E';
	x_name[file_sub++]='L';
	x_name[file_sub++]='D';
		x_name[file_sub++]='_';
		if(x_field < 10)
		x_name[file_sub++]=x_field+'0';
		else
		x_name[file_sub++]=x_field-10+'A';
	}
	x_name[file_sub++]='_';
					//filename_new[file_sub++]='B';
					for(Tp_i = 0;Tp_i<8;Tp_i++)
					{
						if(((*(uint8_t*)(BaseData_ARR+IDNumber_High_index*9+Tp_i)>='A')&&
							 (*(uint8_t*)(BaseData_ARR+IDNumber_High_index*9+Tp_i)<='Z'))||
						   ((*(uint8_t*)(BaseData_ARR+IDNumber_High_index*9+Tp_i)>='0')&&
							 (*(uint8_t*)(BaseData_ARR+IDNumber_High_index*9+Tp_i)<='9')))
						{
							x_name[file_sub++] = *(uint8_t*)(BaseData_ARR+IDNumber_High_index*9+Tp_i);
						}
						else
						{
							break;
						}
					}
					for(Tp_i = 0;Tp_i<8;Tp_i++)
					{
						if(((*(uint8_t*)(BaseData_ARR+IDNumber_Low_index*9+Tp_i)>='A')&&
							 (*(uint8_t*)(BaseData_ARR+IDNumber_Low_index*9+Tp_i)<='Z'))||
						   ((*(uint8_t*)(BaseData_ARR+IDNumber_Low_index*9+Tp_i)>='0')&&
							 (*(uint8_t*)(BaseData_ARR+IDNumber_Low_index*9+Tp_i)<='9')))
						{
							x_name[file_sub++] = *(uint8_t*)(BaseData_ARR+IDNumber_Low_index*9+Tp_i);
						}
						else
						{
							break;
						}
					}
					x_name[file_sub++]='_';
				//	memcpy(filename_new+file_sub,(uint8_t *)timeandday_now,6);
					if(systerm_error_status.bits.lse_error == 0)
					{
						Tp_point = (uint8_t *)&(timeandday_now_app);
					for(Tp_i = 0;Tp_i<6;Tp_i++)
					{
					   DIVIDED_DATA_HEX2BYTE(*(Tp_point++)  ,(uint8_t *)(x_name+file_sub+Tp_i*2));
						
					}
				  }
					else
					{
						x_name[file_sub] = '0';
						x_name[file_sub+1] = '0';
						x_name[file_sub+2] = '0';
						x_name[file_sub+3] = '0';
						x_name[file_sub+4] = '0';
						x_name[file_sub+5] = '0';
						x_name[file_sub+6] = '9';
						x_name[file_sub+7] = '9';
						x_name[file_sub+8] = '9';
						x_name[file_sub+9] = '9';
						x_name[file_sub+10] = '9';
						x_name[file_sub+11] = '9';
						
					}
					
					file_sub = file_sub + 12;
					x_name[file_sub++]='.';
					x_name[file_sub++]='C';
					x_name[file_sub++]='S';
					x_name[file_sub++]='V';
					if(FF_USE_LFN == 0)//???l???
					{
						file_sub = 0;
						memset(x_name,0,44);
						if(x_field < 10)
		         x_name[file_sub++]=x_field+'0';
		        else
		        x_name[file_sub++]=x_field-10+'A';
						x_name[file_sub++]='.';
					  x_name[file_sub++]='C';
				   	x_name[file_sub++]='S';
					   x_name[file_sub++]='V';
				  	}	
}
void Printf_One_Line(const char  *x_name,uint8_t  *x_sou,uint8_t x_long,uint32_t x_num)
{
	
	const char char2_const[]={0X0D,0X0A};
	const char char1_const[]=",\0";
	uint16_t  bytesWritten,bytesToWrite;
	FRESULT res;
	static uint32_t Tp_i =0;
	
//	writeTextBuff= (char *)((uint32_t )malloc((0x2000+1)+32));
	memset(writeTextBuff,0,0x2000+1);
	strcpy(writeTextBuff,x_name);
	for(Tp_i=0 ;Tp_i<x_num;Tp_i++)
	{
	DATATOSTR(writeTextBuff+strlen(writeTextBuff),x_sou+Tp_i*x_long,x_long);
	strcpy(writeTextBuff+strlen(writeTextBuff),char1_const);
	}
  strcpy(writeTextBuff+strlen(writeTextBuff)-1,char2_const);
  bytesToWrite = strlen(writeTextBuff); 
  res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten); 
	
	if(res||(bytesToWrite != bytesWritten))//????????
	{
		systerm_error_status.bits.usb_canot_write_error=1;
		//while(1);
	}
//	free(writeTextBuff);
}



void Printf_Header(void)
{
	uint16_t Tp_i =0,Tp_j=0;
	uint16_t  bytesWritten,bytesToWrite=0;
	FRESULT res;
	uint8_t  Tp_byte;
	
	
	memset(writeTextBuff,0,0x2000+1);
	
	for(Tp_i=0;Tp_i<6;Tp_i++)
	{
	  Tp_byte =  HEXTODEC(*(uint8_t *)(BaseData_ARR+Tp_i));
		writeTextBuff[bytesToWrite++] = Tp_byte/10+'0';
		writeTextBuff[bytesToWrite++] = Tp_byte%10+'0';
	}
	writeTextBuff[bytesToWrite++] =',';
	
	for(Tp_i=0;Tp_i<9*15;Tp_i++)
	{
		if(Tp_i%9 == 8)
		{
			writeTextBuff[bytesToWrite++] = ',';
			Tp_j  = 0;
		}
		else
		{
			if((Tp_i%9)<(8-*(uint8_t  *)(BaseData_ARR+(Tp_i/9+1)*9+8)))
			{
					writeTextBuff[bytesToWrite++]=' ';
			}
			else
			{
			
			//writeTextBuff[bytesToWrite++] = *(uint8_t *)(logodata_sdrambuffer_addr_arry[0]+(Tp_i/9)*8 + Tp_i%9 + 8);
			writeTextBuff[bytesToWrite++] = *(uint8_t *)(BaseData_ARR + (Tp_i/9)*9 +9 +Tp_j);
				Tp_j ++;
			}
		}
//		if((uint8_t)writeTextBuff[bytesToWrite-1]==0xff)//??????????? 20171018
//		{
//			writeTextBuff[bytesToWrite-1]=' ';
//		}
	}
	bytesToWrite = bytesToWrite -1;
	writeTextBuff[bytesToWrite++] = 0x0D;
	writeTextBuff[bytesToWrite++] = 0x0A;
	
	for(Tp_i=0;Tp_i<4;Tp_i++)
	{
		writeTextBuff[bytesToWrite++] = ' ';
	}
	
	Tp_j  = 0;
	
	for(Tp_i=0;Tp_i<9*16;Tp_i++)
	{
		if(Tp_i%9 == 8)
		{
			writeTextBuff[bytesToWrite++] = ',';
			Tp_j  = 0;
		}
		else
		{
			if((Tp_i%9)<(8-*(uint8_t *)(BaseData_ARR+(Tp_i/9+16)*9+8)))
			{
					writeTextBuff[bytesToWrite++]=' ';
			}
			else
			{
			//writeTextBuff[bytesToWrite++] = *(uint8_t *)(logodata_sdrambuffer_addr_arry[0]+(Tp_i/9)*8 + Tp_i%9 + 8*16);
			   writeTextBuff[bytesToWrite++] = *(uint8_t *)(BaseData_ARR+ 9*16 +  (Tp_i/9)*9  +Tp_j);
				 Tp_j++;
			}
		}
//		if((uint8_t)writeTextBuff[bytesToWrite-1]==0xff)//??????????? 20171018
//		{
//			writeTextBuff[bytesToWrite-1]=' ';
//		}
	}
	bytesToWrite = bytesToWrite -1;
	writeTextBuff[bytesToWrite++] = 0x0D;
	writeTextBuff[bytesToWrite++] = 0x0A;
	
	res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten); 
	
	if(res||(bytesToWrite != bytesWritten))//????????
	{
		//SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
		systerm_error_status.bits.usb_canot_write_error=1;
		//while(1);
	}
//	free(writeTextBuff);
}

void DATATOSTR_LSB(char *x_des,uint8_t *x_sou,uint32_t x_long)
{
	uint32_t Tp_i = 0;
	uint32_t Tp_mirror_i=0;
	
	
	for(Tp_i =0 ;Tp_i <x_long;Tp_i++)
	{
		//Tp_mirror_i =  x_long  - Tp_i - 1;//?????j  ???????????
		Tp_mirror_i = Tp_i;//??????  ?????????
		if((x_sou[Tp_i]/16)<10)
		  x_des[Tp_mirror_i*2]=x_sou[Tp_i]/16+'0';
		else
			x_des[Tp_mirror_i*2]=x_sou[Tp_i]/16-10+'A';
		if((x_sou[Tp_i]%16)<10)
		  x_des[Tp_mirror_i*2+1]=x_sou[Tp_i]%16+'0';
		else
			x_des[Tp_mirror_i*2+1]=x_sou[Tp_i]%16-10+'A';
		
		//x_des[Tp_mirror_i*3+2]=32;
	}
}

void Printf_One_Line_TestAll(const char *x_name,uint8_t *x_sou,uint8_t x_long,uint32_t x_num,uint8_t x_flag)
{
	
	const char char2_const[]={0X0D,0X0A};
	const char char1_const[]=",\0";
	static char Tp_data[21]="2017/05/07,00:00:00,\0";
	uint16_t  bytesWritten,bytesToWrite;
	FRESULT res;
	static uint32_t Tp_i =0;
	static uint16_t Tp_ii; 
	Tp_ii = 0;
	memset(writeTextBuff,0,0x2000+1);
	strcpy(writeTextBuff,x_name);
	Tp_ii =  Tp_ii + strlen(writeTextBuff);
	for(Tp_i=0;Tp_i<6;Tp_i++)
	{
	
		DIVIDED_DATA_HEX2BYTE(x_sou[Tp_i],(uint8_t *)(Tp_data+2+Tp_i*3));
	}
	
  strcpy(writeTextBuff+Tp_ii,Tp_data);
	Tp_ii = Tp_ii + 20;
	
	for(Tp_i=0 ;Tp_i<x_num;Tp_i++)
	{
	DATATOSTR_LSB(writeTextBuff+Tp_ii,x_sou+Tp_i*x_long+6,x_long-6);
	if(x_flag == 0)
	{
   	strcpy(writeTextBuff+strlen(writeTextBuff),char1_const);
		
	}
	}
  strcpy(writeTextBuff+strlen(writeTextBuff)-1,char2_const);
  bytesToWrite = strlen(writeTextBuff); 
  res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten); 
	
	if(res||(bytesToWrite != bytesWritten))//????????
	{
		systerm_error_status.bits.usb_canot_write_error=1;
		//while(1);
	}
}
void Printf_One_Line_ASCII_right(const char *x_name,uint8_t *x_sou)
{
	const char char2_const[]={0X0D,0X0A,0};
	static uint32_t Tp_i =0;
	uint16_t  bytesWritten,bytesToWrite;
	FRESULT res;
	uint8_t Tp_long = x_sou[8];
	
	
	memset(writeTextBuff,0,0x2000+1);
	strcpy(writeTextBuff,x_name);
	
	
	
	if(Tp_long>8) 
	{
		writeTextBuff[strlen(writeTextBuff)] = 'E';
		writeTextBuff[strlen(writeTextBuff)] = 'r';
		writeTextBuff[strlen(writeTextBuff)] = 'r';
	}
	else
	{
		for(Tp_i=0;Tp_i<8-Tp_long;Tp_i++)
		{
			writeTextBuff[strlen(writeTextBuff)] = ' ';
		}
		for(Tp_i=0;Tp_i<Tp_long;Tp_i++)
		{
			writeTextBuff[strlen(writeTextBuff)] = x_sou[Tp_i];
		}
		
	}
	
		strcpy(writeTextBuff+strlen(writeTextBuff),char2_const);
  bytesToWrite = strlen(writeTextBuff); 
  res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten); 
	
	if(res||(bytesToWrite != bytesWritten))//????????
	{
		systerm_error_status.bits.usb_canot_write_error=1;
		//while(1);
	}
	
	

}


void Printf_One_Line_ASCII(const char *x_name,uint8_t *x_sou,uint8_t x_long,uint32_t x_num)
{
		const char char2_const[]={0X0D,0X0A};
	const char char1_const[]=",\0";
	uint16_t  bytesWritten,bytesToWrite;
	FRESULT res;
	static uint32_t Tp_i =0;
		static uint32_t Tp_j =0;
		memset(writeTextBuff,0,0x2000+1);
	strcpy(writeTextBuff,x_name);
	
	for(Tp_i=0 ;Tp_i<x_num;Tp_i++)
	{
	//DATATOSTR(writeTextBuff+strlen(writeTextBuff),x_sou+Tp_i*x_long,x_long);
	for(Tp_j = 0;Tp_j<x_long;Tp_j++)
	{
		if(((x_sou[Tp_i*(x_long) + Tp_j] >='0')&&(x_sou[Tp_i*(x_long) + Tp_j] <='9'))||((x_sou[Tp_i*(x_long) + Tp_j] >='A')&&(x_sou[Tp_i*(x_long) + Tp_j] <='Z')))
	  {
			writeTextBuff[strlen(writeTextBuff)] = x_sou[Tp_i*(x_long ) + Tp_j ];
		}
		else
		{
			writeTextBuff[strlen(writeTextBuff)] = ' ';
		}
	
			
	}
		
	strcpy(writeTextBuff+strlen(writeTextBuff),char1_const);
	}
	
	strcpy(writeTextBuff+strlen(writeTextBuff)-1,char2_const);
  bytesToWrite = strlen(writeTextBuff); 
  res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten); 
	
	if(res||(bytesToWrite != bytesWritten))//????????
	{
		systerm_error_status.bits.usb_canot_write_error=1;
		//while(1);
	}
	
		
}

void Printf_One_Line_NO96(const char *x_name,uint8_t *x_sou)
{
		const char char2_const[]={0X0D,0X0A};
//	const char char1_const[]=",";
	uint16_t  bytesWritten,bytesToWrite;
	FRESULT res;
	static uint32_t Tp_i =0;
		static uint32_t Tp_j =0;
		memset(writeTextBuff,0,0x2000+1);
	strcpy(writeTextBuff,x_name);
		for(Tp_i=0;Tp_i<16;Tp_i++)
		{
			if((x_sou[Tp_i*9+1]==0)&&(x_sou[Tp_i*9+2]==0)&&(x_sou[Tp_i*9+3]==0)&&(x_sou[Tp_i*9+4]==0)
				&&(x_sou[Tp_i*9+5]==0)&&(x_sou[Tp_i*9+6]==0)&&(x_sou[Tp_i*9+7]==0)&&(x_sou[Tp_i*9+8]==0))
			{
				for(Tp_j=0;Tp_j<8;Tp_j++) writeTextBuff[strlen(writeTextBuff)]='F';
			  writeTextBuff[strlen(writeTextBuff)] = ',';
			}
			else
			{
				for(Tp_j=0;Tp_j<8;Tp_j++)
				{
					if((x_sou[Tp_i*9+Tp_j+1]=='0')&&(Tp_j!=7)) writeTextBuff[strlen(writeTextBuff)]=' ';
					else break;
				}
				//if(Tp_j==8) Tp_j = 7;
				for(;Tp_j<8;Tp_j++)
				{
					writeTextBuff[strlen(writeTextBuff)]=x_sou[Tp_i*9+Tp_j+1];
				}
				writeTextBuff[strlen(writeTextBuff)] = ',';
			}
		}
		strcpy(writeTextBuff+strlen(writeTextBuff)-1,char2_const);
  bytesToWrite = strlen(writeTextBuff); 
  res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten); 
	
	if(res||(bytesToWrite != bytesWritten))//????????
	{
		systerm_error_status.bits.usb_canot_write_error=1;
		//while(1);
	}
}


void Printf_One_Line_ADDRANDSIZE(const char *x_name,uint8_t *x_sou,uint8_t x_long,uint32_t x_num)
{
	const char char2_const[]={0X0D,0X0A};
	const char char1_const[]=",\0";
	uint16_t  bytesWritten,bytesToWrite;
	FRESULT res;
	static uint32_t Tp_i =0;
		static uint32_t Tp_j =0;
		memset(writeTextBuff,0,0x2000+1);
	strcpy(writeTextBuff,x_name);
	
	for(Tp_i=0 ;Tp_i<x_num;Tp_i++)
	{
	//DATATOSTR(writeTextBuff+strlen(writeTextBuff),x_sou+Tp_i*x_long,x_long);
	if((x_sou[(Tp_i*4)*(x_long+1)+1] =='0')&&
		 (x_sou[(Tp_i*4)*(x_long+1)+2] =='0')&&
	   (x_sou[(Tp_i*4)*(x_long+1)+3] =='0')&&
	   (x_sou[(Tp_i*4)*(x_long+1)+4] =='0')&&
	   (x_sou[(Tp_i*4)*(x_long+1)+5] =='0')&&
	   (x_sou[(Tp_i*4)*(x_long+1)+6] =='0')&&
	   (x_sou[(Tp_i*4)*(x_long+1)+7] =='0')&&
	   (x_sou[(Tp_i*4)*(x_long+1)+8] =='0')
	)
	{
				 writeTextBuff[strlen(writeTextBuff)]='F';
		 writeTextBuff[strlen(writeTextBuff)]='F';
		 writeTextBuff[strlen(writeTextBuff)]='F';
		 writeTextBuff[strlen(writeTextBuff)]='F';
		 writeTextBuff[strlen(writeTextBuff)]='F';
		 writeTextBuff[strlen(writeTextBuff)]='F';
		 writeTextBuff[strlen(writeTextBuff)]='F';
		 writeTextBuff[strlen(writeTextBuff)]='F';
		strcpy(writeTextBuff+strlen(writeTextBuff),char1_const);
		continue;
	}
	for(Tp_j = 0;Tp_j<x_long;Tp_j++)
	{
	  if(((x_sou[(Tp_i*4)*(x_long+1) + Tp_j+1] >='0')&&(x_sou[(Tp_i*4)*(x_long+1) + Tp_j+1] <='9'))||((x_sou[(Tp_i*4)*(x_long+1) + Tp_j+1] >='A')&&(x_sou[(Tp_i*4)*(x_long+1) + Tp_j+1] <='Z')))
	  {
			writeTextBuff[strlen(writeTextBuff)] = x_sou[(Tp_i*4)*(x_long+1 ) + Tp_j +1];
		}
		else
		{
			writeTextBuff[strlen(writeTextBuff)] = ' ';
		}
	
			
	}
		
	strcpy(writeTextBuff+strlen(writeTextBuff),char1_const);
	}
	
	strcpy(writeTextBuff+strlen(writeTextBuff)-1,char2_const);
  bytesToWrite = strlen(writeTextBuff); 
  res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten); 
	
	if(res||(bytesToWrite != bytesWritten))//????????
	{
		systerm_error_status.bits.usb_canot_write_error=1;
		//while(1);
	}
}


void Printf_One_Line_FIELDnumber(void)
{
		const char char2_const[]={0X0D,0X0A};
	//const char char1_const[]=",";
   uint16_t  bytesWritten,bytesToWrite;
	FRESULT res;
	static uint32_t Tp_i =0;
		static uint32_t Tp_j =0;
		char  Tp_char[8];
		
	memset(writeTextBuff,0,0x2000+1);
	//strcpy(writeTextBuff,x_name);
		strcpy(writeTextBuff,"FIELD NUMBER      ,\0");
		
	for(Tp_i=0;Tp_i<16;Tp_i++)
	{
		if(gs_AreaInfo[Tp_i].type==0xff)
		{
			for(Tp_j=0;Tp_j<8;Tp_j++) writeTextBuff[strlen(writeTextBuff)]='F';
			writeTextBuff[strlen(writeTextBuff)] = ',';
		}
		else if((gs_AreaInfo[Tp_i].type<=7)&&(gs_AreaInfo[Tp_i].size<=99))
		{
			memset(Tp_char,0,8);
			sprintf(Tp_char,"%d",gs_AreaInfo[Tp_i].size);
			for(Tp_j=0;Tp_j<8-strlen(Tp_char);Tp_j++)  writeTextBuff[strlen(writeTextBuff)]=' ';
			for(Tp_j=0;Tp_j<strlen(Tp_char);Tp_j++)    writeTextBuff[strlen(writeTextBuff)]=*(Tp_char+Tp_j);
			writeTextBuff[strlen(writeTextBuff)] = ',';
		}
	}
  		strcpy(writeTextBuff+strlen(writeTextBuff)-1,char2_const);
  bytesToWrite = strlen(writeTextBuff); 
  res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten); 
	
	if(res||(bytesToWrite != bytesWritten))//????????
	{
		systerm_error_status.bits.usb_canot_write_error=1;
		//while(1);
	}
	
}

void Printf_One_Line_FIELDtype(void)
{
	const char char2_const[]={0X0D,0X0A};
	//const char char1_const[]=",";
   uint16_t  bytesWritten,bytesToWrite;
	FRESULT res;
	static uint32_t Tp_i =0;
		static uint32_t Tp_j =0;
		char Tp_char[8];
		
	memset(writeTextBuff,0,0x2000+1);
	strcpy(writeTextBuff,"FIELD DATA_CLASS  ,\0");
	#ifdef SYSUARTPRINTF
	//sysprintf("*x_name=%s\r\n",*x_name);
	#endif
		
	for(Tp_i=0;Tp_i<16;Tp_i++)
	{
		if(gs_AreaInfo[Tp_i].type==0xff)
		{
			//for(Tp_j=0;Tp_j<6;Tp_j++) writeTextBuff[strlen(writeTextBuff)]='0';
			for(Tp_j=0;Tp_j<8;Tp_j++) writeTextBuff[strlen(writeTextBuff)]='F';
			writeTextBuff[strlen(writeTextBuff)] = ',';
		}
		else if(gs_AreaInfo[Tp_i].type<=7)
		{
			memset(Tp_char,0,8);
			sprintf(Tp_char,"%d",gs_AreaInfo[Tp_i].type);
			for(Tp_j=0;Tp_j<8-strlen(Tp_char);Tp_j++)  writeTextBuff[strlen(writeTextBuff)]=' ';
			for(Tp_j=0;Tp_j<strlen(Tp_char);Tp_j++)    writeTextBuff[strlen(writeTextBuff)]=*(Tp_char+Tp_j);
			writeTextBuff[strlen(writeTextBuff)] = ',';
		}
	}
  		strcpy(writeTextBuff+strlen(writeTextBuff)-1,char2_const);
  bytesToWrite = strlen(writeTextBuff); 
  res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten); 
	
	if(res||(bytesToWrite != bytesWritten))//????????
	{
		systerm_error_status.bits.usb_canot_write_error=1;
		//while(1);
	}
}

//void Printf_One_Line_FIELDDATA(const char *x_name,uint8_t *x_sou,uint8_t x_long,uint32_t x_num)
//{
//	
//	const char char2_const[]={0X0D,0X0A};
//	const char char1_const[]=",";
//	uint16_t  bytesWritten,bytesToWrite;
//	FRESULT res;
//	static uint32_t Tp_i =0;
//		static uint32_t Tp_j =0;
//		memset(writeTextBuff,0,sizeof(writeTextBuff));
//	strcpy(writeTextBuff,x_name);
//	uint8_t Tp_mark=0;
//	
//	for(Tp_i=0 ;Tp_i<x_num;Tp_i++)
//	{
//	//DATATOSTR(writeTextBuff+strlen(writeTextBuff),x_sou+Tp_i*x_long,x_long);
//	Tp_mark = 0;
//	for(Tp_j = 0;Tp_j<x_long;Tp_j++)
//	{
//	  
//		if(((x_sou[(Tp_i*4)*(x_long+1) + Tp_j+1] =='0')&&(Tp_mark==0))&&(Tp_j!=(x_long-1)))
//		{
//			writeTextBuff[strlen(writeTextBuff)] = ' ';
//		}
//		else if(((x_sou[(Tp_i*4)*(x_long+1) + Tp_j+1] >='0')&&(x_sou[(Tp_i*4)*(x_long+1) + Tp_j+1] <='9'))||((x_sou[(Tp_i*4)*(x_long+1) + Tp_j+1] >='A')&&(x_sou[(Tp_i*4)*(x_long+1) + Tp_j+1] <='Z')))
//	  {
//			Tp_mark = 1;
//			writeTextBuff[strlen(writeTextBuff)] = x_sou[(Tp_i*4)*(x_long+1 ) + Tp_j +1];
//		}
//		else
//		{
//			writeTextBuff[strlen(writeTextBuff)] = ' ';
//		}
//	
//			
//	}
//		
//	strcpy(writeTextBuff+strlen(writeTextBuff),char1_const);
//	}
//	
//	strcpy(writeTextBuff+strlen(writeTextBuff)-1,char2_const);
//  bytesToWrite = strlen(writeTextBuff); 
//  res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten); 
//	
//	if(res||(bytesToWrite != bytesWritten))//????????
//	{
//		systerm_error_status.bits.usb_canot_write_error=1;
//		//while(1);
//	}
//	
//}

void Printf_One_Line_DATAF(const char *x_name,uint8_t *x_sou,uint8_t x_long,uint32_t x_num)
{
	const char char2_const[]={0X0D,0X0A};
	const char char1_const[]=",\0";
	uint16_t  bytesWritten,bytesToWrite;
	FRESULT res;
	static uint32_t Tp_i =0;
		static uint32_t Tp_j =0;
		memset(writeTextBuff,0,0x2000+1);
	strcpy(writeTextBuff,x_name);
	
	for(Tp_i=0 ;Tp_i<x_num;Tp_i++)
	{
	
	for(Tp_j = 0;Tp_j<x_long;Tp_j++)
	{
	  if(((x_sou[(Tp_i)*(x_long+1) + Tp_j+1] >='0')&&(x_sou[(Tp_i)*(x_long+1) + Tp_j+1] <='9'))||((x_sou[(Tp_i)*(x_long+1) + Tp_j+1] >='A')&&(x_sou[(Tp_i)*(x_long+1) + Tp_j+1] <='Z')))
	  {
			writeTextBuff[strlen(writeTextBuff)] = x_sou[(Tp_i)*(x_long+1 ) + Tp_j +1];
		}
		else
		{
			writeTextBuff[strlen(writeTextBuff)] = ' ';
		}
	
			
	}
		
	strcpy(writeTextBuff+strlen(writeTextBuff),char1_const);
	}
	if(x_num==16)
	strcpy(writeTextBuff+strlen(writeTextBuff)-1,char2_const);
  else if(x_num==1)
	writeTextBuff[strlen(writeTextBuff)-1] = 0;
	
	bytesToWrite = strlen(writeTextBuff); 
  res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten); 
	
	if(res||(bytesToWrite != bytesWritten))//????????
	{
		systerm_error_status.bits.usb_canot_write_error=1;
		//while(1);
	}
}

void Printf_One_Line_Dataclass12time(const char *x_name,uint8_t *x_sou,uint8_t x_long,uint32_t x_num)
{
	const char char2_const[]={0X0D,0X0A};
	const char char1_const[]=",\0";
	char Tp_char[]="2017/00\0";
	uint16_t  bytesWritten,bytesToWrite;
	FRESULT res;
	static uint32_t Tp_i =0;
	
	memset(writeTextBuff,0,0x2000+1);
	strcpy(writeTextBuff,x_name);
	for(Tp_i=0 ;Tp_i<x_num;Tp_i++)
	{
	//DATATOSTR(writeTextBuff+strlen(writeTextBuff),x_sou+Tp_i*x_long,x_long);
	if((x_sou[2]==0)&&(x_sou[3]==0))//???
	{
		DIVIDED_DATA_HEX2BYTE(x_sou[0],(uint8_t *)(Tp_char+5));
		DIVIDED_DATA_HEX2BYTE(x_sou[1],(uint8_t *)(Tp_char+2));
		memcpy(writeTextBuff+strlen(writeTextBuff),Tp_char,strlen(Tp_char));
	}
	else
  {
		DATATOSTR(writeTextBuff+strlen(writeTextBuff),x_sou+Tp_i*x_long,x_long);
	}
	strcpy(writeTextBuff+strlen(writeTextBuff),char1_const);
	}
  strcpy(writeTextBuff+strlen(writeTextBuff)-1,char2_const);
  bytesToWrite = strlen(writeTextBuff); 
  res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten); 
	
	if(res||(bytesToWrite != bytesWritten))//????
	{
	//	SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
		systerm_error_status.bits.usb_canot_write_error=1;
		//while(1);
	}
}
//void Printf_One_Line_ADDRANDSIZE(const char *x_name,uint8_t *x_sou,uint8_t x_long,uint32_t x_num)
//{
//	
//	const char char2_const[]={0X0D,0X0A};
//	const char char1_const[]=",";
//	uint16_t  bytesWritten,bytesToWrite;
//	FRESULT res;
//	static uint32_t Tp_i =0;
//	
//	memset(writeTextBuff,0,sizeof(writeTextBuff));
//	strcpy(writeTextBuff,x_name);
//	for(Tp_i=0 ;Tp_i<x_num;Tp_i++)
//	{
//	if((x_sou[Tp_i*x_long] == 0)&&(x_sou[Tp_i*x_long+1] == 0)&&(x_sou[Tp_i*x_long+2] == 0)&&(x_sou[Tp_i*x_long+3] == 0))
//	{
//		 writeTextBuff[strlen(writeTextBuff)]='F';
//		 writeTextBuff[strlen(writeTextBuff)]='F';
//		 writeTextBuff[strlen(writeTextBuff)]='F';
//		 writeTextBuff[strlen(writeTextBuff)]='F';
//		 writeTextBuff[strlen(writeTextBuff)]='F';
//		 writeTextBuff[strlen(writeTextBuff)]='F';
//		 writeTextBuff[strlen(writeTextBuff)]='F';
//		 writeTextBuff[strlen(writeTextBuff)]='F';
//	}
//  else
//	{		
//	DATATOSTR(writeTextBuff+strlen(writeTextBuff),x_sou+Tp_i*x_long,x_long);
//	}
//	strcpy(writeTextBuff+strlen(writeTextBuff),char1_const);
//	}
//  strcpy(writeTextBuff+strlen(writeTextBuff)-1,char2_const);
//  bytesToWrite = strlen(writeTextBuff); 
//  res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten); 
//	
//	if(res||(bytesToWrite != bytesWritten))//????????
//	{
//		systerm_error_status.bits.usb_canot_write_error=1;
//		//while(1);
//	}
//	
//}

//void Printf_One_Line_FIELDDATA(const char *x_name,uint8_t *x_sou,uint8_t x_long,uint32_t x_num)
//{
//	
//	const char char2_const[]={0X0D,0X0A};
//	const char char1_const[]=",";
//	uint16_t  bytesWritten,bytesToWrite;
//	FRESULT res;
//	static uint32_t Tp_i =0;
//	static uint8_t Tp_j =0 ;
//	static uint8_t Tp_sou[64];
//	
//	memset(writeTextBuff,0,sizeof(writeTextBuff));
//	strcpy(writeTextBuff,x_name);
//	for(Tp_i=0 ;Tp_i<x_num;Tp_i++)
//	{
//	for(Tp_j=0;Tp_j<x_long;Tp_j++)
//	{
//		Tp_sou[Tp_i*x_long+Tp_j]= DECTOHEX(x_sou[Tp_i*x_long+Tp_j]);
//	}
//	DATATOSTR(writeTextBuff+strlen(writeTextBuff),Tp_sou+Tp_i*x_long,x_long);
//	for(Tp_j=x_long*2;Tp_j>0;Tp_j--)
//	{
//		if(writeTextBuff[strlen(writeTextBuff)-Tp_j]=='0')
//		{
//			if(Tp_j!=1)
//			{
//			writeTextBuff[strlen(writeTextBuff)-Tp_j] = ' ';
//			}
//			else
//		  {
//				if(Tp_i!= 0)
//				{
//					writeTextBuff[strlen(writeTextBuff)-8] = 'F';
//					writeTextBuff[strlen(writeTextBuff)-7] = 'F';
//					writeTextBuff[strlen(writeTextBuff)-6] = 'F';
//					writeTextBuff[strlen(writeTextBuff)-5] = 'F';
//					writeTextBuff[strlen(writeTextBuff)-4] = 'F';
//					writeTextBuff[strlen(writeTextBuff)-3] = 'F';
//					writeTextBuff[strlen(writeTextBuff)-2] = 'F';
//					writeTextBuff[strlen(writeTextBuff)-1] = 'F';
//				}
//			}
//		}
//		else
//		{
//			break;
//		}
//		
//	}
//	strcpy(writeTextBuff+strlen(writeTextBuff),char1_const);
//	}
//  strcpy(writeTextBuff+strlen(writeTextBuff)-1,char2_const);
//  bytesToWrite = strlen(writeTextBuff); 
//  res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten); 
//	
//	if(res||(bytesToWrite != bytesWritten))//????????
//	{
//		systerm_error_status.bits.usb_canot_write_error=1;
//		//while(1);
//	}
//	
//}

//void Printf_One_Line_DATAF(const char *x_name,uint8_t *x_sou,uint8_t x_long,uint32_t x_num)
//{
//	
//	const char char2_const[]={0X0D,0X0A};
//	const char char1_const[]=",";
//	uint16_t  bytesWritten,bytesToWrite;
//	FRESULT res;
//	static uint32_t Tp_i =0;
//	static uint8_t Tp_j =0 ;
//	
//	memset(writeTextBuff,0,sizeof(writeTextBuff));
//	strcpy(writeTextBuff,x_name);
//	for(Tp_i=0 ;Tp_i<x_num;Tp_i++)
//	{
//	
//	DATATOSTR(writeTextBuff+strlen(writeTextBuff),x_sou+Tp_i*x_long,x_long);
//	for(Tp_j=x_long*2;Tp_j>0;Tp_j--)
//	{
//		if(writeTextBuff[strlen(writeTextBuff)-Tp_j]=='F')
//		{
//			if(Tp_j!=1)
//			{
//			writeTextBuff[strlen(writeTextBuff)-Tp_j] = ' ';
//			}
//			else
//		  {
//				
//					writeTextBuff[strlen(writeTextBuff)-8] = 'F';
//					writeTextBuff[strlen(writeTextBuff)-7] = 'F';
//					writeTextBuff[strlen(writeTextBuff)-6] = 'F';
//					writeTextBuff[strlen(writeTextBuff)-5] = 'F';
//					writeTextBuff[strlen(writeTextBuff)-4] = 'F';
//					writeTextBuff[strlen(writeTextBuff)-3] = 'F';
//					writeTextBuff[strlen(writeTextBuff)-2] = 'F';
//					writeTextBuff[strlen(writeTextBuff)-1] = 'F';
//				
//			}
//		}
//		else
//		{
//			break;
//		}
//		
//	}
//	strcpy(writeTextBuff+strlen(writeTextBuff),char1_const);
//	}
//  strcpy(writeTextBuff+strlen(writeTextBuff)-1,char2_const);
//  bytesToWrite = strlen(writeTextBuff); 
//  res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten); 
//	
//	if(res||(bytesToWrite != bytesWritten))//????????
//	{
//		systerm_error_status.bits.usb_canot_write_error=1;
//		//SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
//		//while(1);
//	}
//	
//}
void Data_Negate(uint8_t *x_des,uint8_t *x_sou,uint8_t x_num)
{
	static uint8_t Tp_i;
	static uint32_t Tpdata;
	for(Tp_i = 0;Tp_i<x_num ; Tp_i ++)
	{
		Tpdata = *(uint32_t *)(x_sou+Tp_i*4);
		
		Tpdata = ~Tpdata;
		*(uint32_t *)(x_des+Tp_i*4) = Tpdata;
	}
}

void Printf_One_Line_DATA0_DEC(const char *x_name,uint8_t *x_sou,uint8_t x_long,uint32_t x_num)
{
	
	const char char2_const[]={0X0D,0X0A};
	const char char1_const[]=",\0";
	uint16_t  bytesWritten,bytesToWrite;
	FRESULT res;
	static uint32_t Tp_i =0;
	static uint8_t Tp_j =0 ;
	
	
	static uint8_t Tp_ii;
	
	Tp_ii = 0;
	
	memset(writeTextBuff,0,0x2000+1);
	//strcpy(writeTextBuff,x_name);
	for(Tp_i=0 ;Tp_i<x_num;Tp_i++)
	{
	Tp_data1 = *(uint32_t *)(x_sou+Tp_i*x_long);
	writeTextBuff[Tp_ii+9]= Tp_data1%10+'0';
	Tp_data1 = Tp_data1 / 10;
  writeTextBuff[Tp_ii+8]= Tp_data1%10+'0';	
  Tp_data1 = Tp_data1 / 10;  
  writeTextBuff[Tp_ii+7]= Tp_data1%10+'0';
  Tp_data1 = Tp_data1 / 10; 
  writeTextBuff[Tp_ii+6]= Tp_data1%10+'0';
	Tp_data1 = Tp_data1 / 10; 
  writeTextBuff[Tp_ii+5]= Tp_data1%10+'0';
  Tp_data1 = Tp_data1 / 10; 
  writeTextBuff[Tp_ii+4]= Tp_data1%10+'0';
  Tp_data1 = Tp_data1 / 10; 
  writeTextBuff[Tp_ii+3]= Tp_data1%10+'0';	
  Tp_data1 = Tp_data1 / 10; 
  writeTextBuff[Tp_ii+2]= Tp_data1%10+'0';		
	Tp_data1 = Tp_data1 / 10; 
  writeTextBuff[Tp_ii+1]= Tp_data1%10+'0';	
  Tp_data1 = Tp_data1 / 10; 
  writeTextBuff[Tp_ii]= Tp_data1%10+'0';		
		Tp_ii = Tp_ii + 10;
	//DATATOSTR(writeTextBuff+strlen(writeTextBuff),x_sou+Tp_i*x_long,x_long);
	for(Tp_j=10;Tp_j>0;Tp_j--)
	{
		if(writeTextBuff[Tp_ii-Tp_j]=='0')
		{
			if(Tp_j!=1)
			{
			writeTextBuff[Tp_ii-Tp_j] = ' ';
			}
			else
		  {
				
			}
		}
		else
		{
			break;
		}
		
	}
	strcpy(writeTextBuff+Tp_ii,char1_const);
	Tp_ii ++;
	}
  strcpy(writeTextBuff+strlen(writeTextBuff)-1,char2_const);
  bytesToWrite = strlen(writeTextBuff); 
  res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten); 
	
	if(res||(bytesToWrite != bytesWritten))//????
	{
		//SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
		systerm_error_status.bits.usb_canot_write_error=1;
		//while(1);
	}
	
}

void Printf_One_Line_TestFinal(const char *x_name,uint8_t *x_sou,uint8_t x_long,uint32_t x_num)
{
	
	const char char2_const[]={0X0D,0X0A};
	const char char1_const[]=",\0";
	static char Tp_data[24]="2017/05/07,00:00:00,00,\0";
	uint16_t  bytesWritten,bytesToWrite;
	FRESULT res;
	static uint32_t Tp_i =0;
	static uint16_t  Tp_ii ;
	//static uint8_t Tp_long;
	
	memset(writeTextBuff,0,0x2000+1);
	//strcpy(writeTextBuff,x_name);
	for(Tp_i=0;Tp_i<7;Tp_i++)
	{
	
		DIVIDED_DATA_HEX2BYTE(x_sou[Tp_i],(uint8_t *)(Tp_data+2+Tp_i*3));
	}
	
  strcpy(writeTextBuff,Tp_data);
	Tp_ii = 23;
	
	for(Tp_i=0 ;Tp_i<x_num;Tp_i++)
	{
	DATATOSTR(writeTextBuff+Tp_ii,x_sou+Tp_i*x_long+7,x_long);
		Tp_ii = Tp_ii + 2;
	strcpy(writeTextBuff+Tp_ii,char1_const);
		Tp_ii = Tp_ii + 1;
	}
  strcpy(writeTextBuff+Tp_ii-1,char2_const);
  bytesToWrite = strlen(writeTextBuff); 
  res= f_write (&file, writeTextBuff, bytesToWrite, (void *)&bytesWritten); 
	
	if(res||(bytesToWrite != bytesWritten))//????
	{
		//SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
		systerm_error_status.bits.usb_canot_write_error=1;
		//while(1);
	}
}

void LOGOData_Write_To_USB(uint32_t x_flag)
{
//	uint16_t  bytesWritten,bytesToWrite;
	static uint8_t Tp_field;
//	FRESULT res;
	static uint32_t  Tp_j =0;
	uint32_t Tp_addr;
	char Tp_char[]=".....FIELD=0......DATACLASS=1\0";
	char Tp_char1[]="HISTORY           \0";//HISTROY???-?????j????
	char Tp_char2[]=" 000 ,\0";
//	char Tp_char3[]="No000000:";
	static FATFS Tp_fatfs[17];
	static uint8_t Tp_fatfs_num = 0;
	static uint32_t  Tp_i;
	static DATACLASS_TYPE  Tp_Type;
	static uint32_t  Tp_Time;
	static uint32_t  Tp_testfinal;
	static FRESULT Tp_res;
		
//	char filename[]="0:FIELD01.CSV";
  static char filename_new[60]={0};//	static char filename_new[44]={0};
//  static char basedata_foreword[]="0x0400_000*  ";
	static uint8_t Tp_linshi[64];
	static DIR tdir;	//????
//	static uint16_t  bytesToWrite;
	static uint16_t  bytesWritten;
//	static uint8_t Tp_data[9]={0};
	

	
	if(x_flag != 0)
	{
	   #ifdef PROMPT_ENABLE 
		LCD_DisplayStringLine( 30, "DOWNLOAD LOGO DATA ");
		#endif
	}
	
	Tp_res = f_mount(0, "3:", 1);
	Tp_res = f_mount(&usb_fatfs, "3:", 1); 
	sysprintf("USB f_mount Tp_res=%D\r\n",Tp_res);
	
	Tp_fatfs_num = 0;
	LOGO_DOUBLE_VISIT = 0;
	
  Tp_res =	f_opendir(&tdir,"3:/FUNCTION_MEMORY");
	if(Tp_res == FR_NO_PATH)//??????l???
	{
		sysprintf("USB f_mkdir\r\n");
		Tp_res =f_mkdir("3:/FUNCTION_MEMORY");
		sysprintf("USB f_mkdir Tp_res=%D\r\n",Tp_res);
		Tp_res = f_opendir(&tdir,"3:/FUNCTION_MEMORY");  //?????????
		sysprintf("USB f_opendir Tp_res=%D\r\n",Tp_res);
	}
  
	
	field_info_save();
	sysprintf("field_info_save over\r\n");
	
	for(Tp_field=0;Tp_field<16;Tp_field++)
	{
    
		//if(x_flag&(1<<Tp_field))
		if(Tp_field == x_flag)
		{
		Tp_Type = (DATACLASS_TYPE)gs_AreaInfo[Tp_field].type;
		switch(Tp_Type)
		{
			case Base_DATACLASS:
			  if(Tp_field == 0)
		   /* Write buffer to file */
				{
					
				
					
					
					Create_new_logo_file_name(Tp_field,(uint8_t *)filename_new);
					
					
					if(f_mount(&Tp_fatfs[Tp_fatfs_num++],"",0)!=FR_OK)
					{
						systerm_error_status.bits.usb_canot_write_error=1;
						break;
					}
					
	        if( f_open(&file, (const char *)filename_new/*"0:BASEDATA.CSV"*/,FA_CREATE_ALWAYS | FA_WRITE)!= FR_OK)
					//if(f_open(&file, "0:BASEDATA.CSV",FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
	        {
		          // return;
						systerm_error_status.bits.usb_canot_write_error=1;
						break;
						//????????h??USB?J???
	        }
					
					
					if(f_lseek(&file,0)!=FR_OK)//??????z????l???????
					{
						systerm_error_status.bits.usb_canot_write_error=1;
						
					}
					
					//IWDG_Reset();//
				  Printf_One_Line(".....FIELD=0......DATACLASS=0\0",(uint8_t *)BaseData_ARR,0,1);
					
					Printf_Header();//?????? 20171011
					
					
//					memset(writeTextBuff,0,sizeof(writeTextBuff));
//			    memcpy(Tp_data,BaseData_ARR[0],8);
//		      sprintf((char *)writeTextBuff,"%03d,%02d/%02d/%02d,%02d:%02d:%02d%c%c",
//			       0,HEXTODEC(Tp_data[0]),HEXTODEC(Tp_data[1]),HEXTODEC(Tp_data[2]),HEXTODEC(Tp_data[3]),
//		         HEXTODEC(Tp_data[4]),HEXTODEC(Tp_data[5]),13,10);
//			     Tp_res = f_write(&file,writeTextBuff,strlen((char *)writeTextBuff),(void *)&bytesToWrite);
//		
//		     for(Tp_i=1;Tp_i<164;Tp_i++)
//		     {
//			     memset(writeTextBuff,0,sizeof(writeTextBuff));
//			     memcpy(Tp_data,BaseData_ARR[Tp_i],8);
//			     sprintf((char *)writeTextBuff,"%03d,%8s%c%c",Tp_i,Tp_data,13,10);
//			     Tp_res = f_write(&file,writeTextBuff,strlen((char *)writeTextBuff),(void *)&bytesToWrite);
//		     }     
//				 
//					 memset(writeTextBuff,0,sizeof(writeTextBuff));
//				 sprintf((char *)writeTextBuff,"**************************************************************%c%c",13,10);
//					  Tp_res = f_write(&file,writeTextBuff,strlen((char *)writeTextBuff),(void *)&bytesToWrite);
				 
					    Printf_One_Line_TestAll("YY/MM/DD HH:MM:SS ,\0",(uint8_t *)BaseData_ARR,6,1,0);
					      Printf_One_Line_ASCII_right("RESERVED          ,\0",(uint8_t *)BaseData_ARR+9);
					      Printf_One_Line_ASCII_right("LCD MCU VER       ,\0",(uint8_t *)BaseData_ARR+LCD_Ver_index*9);
					      Printf_One_Line_ASCII_right("MCU VER           ,\0",(uint8_t *)BaseData_ARR+Master_Ver_index*9);
					      Printf_One_Line_ASCII_right("IMAGE FILE VER    ,\0",(uint8_t *)BaseData_ARR+BMP_Ver_index*9);
					      Printf_One_Line_ASCII_right("RESERVED          ,\0",(uint8_t *)BaseData_ARR+45);
					      Printf_One_Line_ASCII_right("SERIAL_NO1        ,\0",(uint8_t *)BaseData_ARR+IDNumber_High_index*9);
					      Printf_One_Line_ASCII_right("SERIAL_NO2        ,\0",(uint8_t *)BaseData_ARR+IDNumber_Low_index*9);
					for(Tp_j=0; Tp_j<24; Tp_j++)
					{
						    Printf_One_Line_ASCII_right("RESERVED          ,\0",(uint8_t *)BaseData_ARR+72+Tp_j*9);
					}
					//Printf_One_Line_ADDRANDSIZE("FIELD START_ADDR  ,",(uint8_t *)logodata_sdrambuffer_addr_arry[0]+Basedata_fieldinformation,4,16);
				  Printf_One_Line_ADDRANDSIZE("FIELD START_ADDR  ,\0",(uint8_t *)BaseData_ARR+32*9-1,8,16);
					Printf_One_Line_ADDRANDSIZE("FIELD SIZE        ,\0",(uint8_t *)BaseData_ARR+33*9-1,8,16);
				    Printf_One_Line_FIELDnumber();
				    Printf_One_Line_FIELDtype();
                Printf_One_Line_NO96("FIELD MANAGE1     ,\0",(uint8_t *)BaseData_ARR+96*9-1);
				        Printf_One_Line_DATAF("FIELD MANAGE2     ,\0",(uint8_t *)BaseData_ARR+112*9-1,8,16);
				        Printf_One_Line_DATAF("FIELD MANAGE3     ,\0",(uint8_t *)BaseData_ARR+128*9-1,8,16);
				        Printf_One_Line_DATAF("FIELD MANAGE4     ,\0",(uint8_t *)BaseData_ARR+144*9-1,8,16);
					      Printf_One_Line_ASCII("RESERVED          ,\0",(uint8_t *)BaseData_ARR+BASE_data_ErrBlock*9,8,1);
				        Printf_One_Line_DATAF("RTC POWER OFF     ,\0",(uint8_t *)BaseData_ARR+BASE_data_Dataclass12RTCPowerOffTimer*9-1,8,1);
					
					
					
					
				
					//Printf_One_Line(" ",(uint8_t *)logodata_sdrambuffer_addr_arry[0]+Basedata_power,0,0);
				  if(f_close(&file)!=FR_OK)
					{
						//SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
						systerm_error_status.bits.usb_canot_write_error=1;
					}
				  if(f_mount( NULL,"",0)!=FR_OK)
					{
						//SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
						systerm_error_status.bits.usb_canot_write_error=1;
					}
				}
				
				    


	     
				
			 // writeTextBuff
			
				break;
				//first debug basedata
			case ActionNumber_DATACLASS://????????
			case ActionTimers_DATACLASS://???????
				Create_new_logo_file_name(Tp_field,(uint8_t *)filename_new);
			   if(para.dataclass_1_2_action_count < MAX_LOGO_ACTION_MONTH)//20170510
				 {
				 if(Tp_Type == ActionTimers_DATACLASS)
				 {
					  Tp_Time = System_Run_Cycle*(0xffffffff/100) + time1ms_count_forrun/100;
					  Tp_Time = System_Runtime_Lasttime-Tp_Time;
				     memcpy((void *)(logodata_sdrambuffer_addr_arry[Tp_field]+(para.dataclass_1_2_action_count%16)*4*2048+4),(void *)(&Tp_Time),4);
//			    *(__IO uint32_t*)(logodata_sdrambuffer_addr_arry[Tp_field]+(para.dataclass_1_2_action_count%16)*4*2048+4) =  System_Runtime_Lasttime -  Tp_Time;
				 }
			  }
			 /* Write buffer to file */
				  //????????????NANDFLASH ?????Basedata_actionnumbertotal?????j????????SDRAM????????
		    
   			 if(f_mount(&Tp_fatfs[Tp_fatfs_num++],"",0)!=FR_OK)
				 {
					   //SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
					   systerm_error_status.bits.usb_canot_write_error = 1;
					   break;
				 }
			  
			   //filename[7]=Tp_field/10+'0';
			   //filename[8]=Tp_field%10+'0';
			   if(f_open(&file,(const char *)filename_new,FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
				 
				 //if(f_open(&file,(const char *)filename,FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
	        {
		           //????????h??USB?J???
						//SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
						systerm_error_status.bits.usb_canot_write_error = 1;
						break;
						  //return;
	        }	
					if(f_lseek(&file,0)!=FR_OK)//??????z????l???????
					{
						//SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
						systerm_error_status.bits.usb_canot_write_error = 1;
						break;
					}
					if(Tp_field < 10)
					{
					Tp_char[11]=Tp_field+'0';
					}
					else
					{
					Tp_char[11]=Tp_field - 10 +'A';	
					}
				 Tp_char[28]=Tp_Type+'0';
				Printf_One_Line((const char *)Tp_char,(uint8_t *)BaseData_ARR,0,1);//????l??......FIELD1......DATACLASS=1
					Printf_Header();//?????? 20171011
					if(para.dataclass_1_2_action_count < MAX_LOGO_ACTION_MONTH)//20170510
					{
			  Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + ((para.dataclass_1_2_action_count*4)/64)*64;		
			  SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
				LOGO_DOUBLE_VISIT = Tp_field;//????????		
					}
					
			 for(Tp_i=0;Tp_i<MAX_LOGO_ACTION_MONTH;Tp_i++)
				 {
					 
				
					 //IWDG_Reset();//??? ?????????30S
			  // filename[7] = Tp_field+'0';
				//	filename[9] =Tp_i/100 + '0' ;
				/// filename[10] =(Tp_i/10)%10 + '0' ;	
        //  filename[11] =(Tp_i%10) + '0' ;	
		
					
			
					 
          if(Tp_i%16==0)//16????h??
					{
					Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + (Tp_i*4)/64*64;
					NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1); 
					}
					 
				 	Printf_One_Line_Dataclass12time((const char *)Tp_char1,(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+(Tp_i%16)*4*2048),4,1);	//	HISTROY000-MY    :00 00 00 00 
				//	Printf_One_Line("                       0            1            2            3            4            5            6            7            8            9            A            B            C            D            E            F",
				//	                (uint8_t *)logodata_sdrambuffer_addr_arry[0],0,1);
					
					for(Tp_j=4;Tp_j<logodata_ActionNumTim_SIZE;)
					{
			     // Tp_char1[11]=(Tp_j/64)/10 + '0' ;
					//	Tp_char1[12]=(Tp_j/64)%10 + '0';
					//	DIVIDED_DATA_HEX2BYTE(Tp_j/64,(uint8_t *)Tp_char1+11);
						// Tp_char1[13]='*';
						if(Tp_j>0x800) { Tp_j = Tp_j + 64;	continue;}
						if((Tp_j +64) <logodata_ActionNumTim_SIZE)  
						{ 
							
						Data_Negate((uint8_t *)Tp_linshi,(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+Tp_j+(Tp_i%16)*4*2048),16);
						Printf_One_Line_DATA0_DEC("\0",(uint8_t *)Tp_linshi,4,16);//HISTROYXXX-XX    :
						}
						else
						{
					//	Data_Negate((uint8_t *)Tp_linshi,(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+Tp_j+(Tp_i%16)*4*2048),16);
							memset(Tp_linshi,0,sizeof(Tp_linshi));
						Printf_One_Line_DATA0_DEC("\0",(uint8_t *)Tp_linshi,4,16/*(logodata_ActionNumber_SIZE-Tp_j)/4*/);
						}
						Tp_j = Tp_j + 64;	
					}
					//????
					//Printf_One_Line(" ",(uint8_t *)logodata_sdrambuffer_addr_arry[0]+Basedata_power,0,0);
				  f_write(&file,"\r\n\r\n",4, (void *)&bytesWritten);
					
					
					if(Tp_i >= para.dataclass_1_2_action_count)
					{
						break;
					}
				 
				 
				 }
				 
				  if(f_close(&file)!=FR_OK)
					{
						systerm_error_status.bits.usb_canot_write_error = 1;
					}
				  if(f_mount(NULL,"",0)!=FR_OK)
					{
						systerm_error_status.bits.usb_canot_write_error = 1;
					}
				 
				 if(para.dataclass_1_2_action_count < MAX_LOGO_ACTION_MONTH)//20170510
				 {
			    Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + (para.dataclass_1_2_action_count*4)/64*64;
				  NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1); 
					LOGO_DOUBLE_VISIT = 0;
				 }
			  
				break;
				 case UnitPara_DATACLASS://????
        Create_new_logo_file_name(Tp_field,(uint8_t *)filename_new);
				 if(f_mount(&Tp_fatfs[Tp_fatfs_num++],"",0)!=FR_OK)
				 {
					 //SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
					 systerm_error_status.bits.usb_canot_write_error = 1;
					 break;
				 }
			  
			  // filename[7]=Tp_field/10+'0';
			   //filename[8]=Tp_field%10+'0';
			  if(f_open(&file,(const char *)filename_new,FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
				  //if(f_open(&file,(const char *)filename,FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
	        {
		           //????????h??USB?J???
					    //return;
					//	SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
						 systerm_error_status.bits.usb_canot_write_error = 1;
						break;
	        }	
				if(f_lseek(&file,0)!=FR_OK)//????????????
				{
					  //SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
					 systerm_error_status.bits.usb_canot_write_error = 1;
						break;
				}
				if(Tp_field < 10)
					{
					Tp_char[11]=Tp_field+'0';
					}
					else
					{
					Tp_char[11]=Tp_field - 10 +'A';	
					}
				  Tp_char[28]=UnitPara_DATACLASS+'0';
				  Printf_One_Line((const char *)Tp_char,(uint8_t *)BaseData_ARR,0,1);//?????......FIELD1......DATACLASS=1
				  Printf_Header();//?????? 20171011
					//	Printf_One_Line("          0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F",(uint8_t *)logodata_sdrambuffer_addr_arry[0],0,1);
					
					//for(Tp_i=1;Tp_i<=MAX_LOGO_UINT_NUM;Tp_i++)//20170628
					for(Tp_i=0;Tp_i<=MAX_LOGO_UINT_NUM;Tp_i++)//20170628
				 {
					 
					 //IWDG_Reset();//??
					 /*Tp_char2[2] = Tp_i/100+'0';
					 Tp_char2[3] = (Tp_i/10)%10+'0';
					 Tp_char2[4] = (Tp_i%10)+'0';
					 Tp_char2[6] = (Tp_i+15)/100+'0';
					 Tp_char2[7] = ((Tp_i+15)/10)%10+'0';
					 Tp_char2[8] = ((Tp_i+15)%10)+'0';
					 if((Tp_i+15)>MAX_LOGO_UINT_NUM)
					 {
						  Tp_char2[6] = (999)/100+'0';
					    Tp_char2[7] = ((999)/10)%10+'0';
					    Tp_char2[8] = ((999)%10)+'0';
					 }*/
					 DIVIDED_DATA_DEC3BYTE(Tp_i,(uint8_t *)Tp_char2+1);
					 Printf_One_Line((const char *)Tp_char2,(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+Tp_i),1,1);
					 
				 }
					if(f_close(&file)!=FR_OK)
				  {
							//SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
						 systerm_error_status.bits.usb_canot_write_error = 1;
					}
				  if(f_mount(NULL,"",0)!=FR_OK)
					{
						//SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
						 systerm_error_status.bits.usb_canot_write_error = 1;
					}
				 
				break;
				 case TestFinalData_DATACLASS:
					 Create_new_logo_file_name(Tp_field,(uint8_t *)filename_new);
					  
				  if(f_mount(&Tp_fatfs[Tp_fatfs_num++],"",0)!=FR_OK)
					{
						//SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
						 systerm_error_status.bits.usb_canot_write_error = 1;
						break;
					}
			  
			   //filename[7]=Tp_field/10+'0';
			  // filename[8]=Tp_field%10+'0';
			   if(f_open(&file,(const char *)filename_new,FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
				  //if(f_open(&file,(const char *)filename,FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
	        {
		           //????????h??USB?J???
						    //return;
						    //SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
						 systerm_error_status.bits.usb_canot_write_error = 1;
                 break;
	        }	
					if(f_lseek(&file,0)!=FR_OK)//??????z????l???????
					{
						  // SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
						 systerm_error_status.bits.usb_canot_write_error = 1;
                 break;
					}
					if(Tp_field < 10)
					{
					Tp_char[11]=Tp_field+'0';
					}
					else
					{
					Tp_char[11]=Tp_field - 10 +'A';	
					}
				 Tp_char[28]=TestFinalData_DATACLASS+'0';
				Printf_One_Line((const char *)Tp_char,(uint8_t *)BaseData_ARR,0,1);//????l??......FIELD1......DATACLASS=1
				Printf_Header();//?????? 20171011
					// viewcyw = *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]);
			  Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Fieldx_Info[Tp_field].cycle*64;		
			  SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
					LOGO_DOUBLE_VISIT = Tp_field;
					for(Tp_i=0;Tp_i<(gs_AreaInfo[Tp_field].space/0x20000);Tp_i++)
					{
							//IWDG_Reset();//??
						  Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Tp_i*64;
					    NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1); 
						  for(Tp_j=0;Tp_j<(0x20000/MAX_LOGO_FINALTEST_ONEPACKET);Tp_j++)
						  {
								Tp_testfinal =  (0x20000/MAX_LOGO_FINALTEST_ONEPACKET)*Tp_i+ Tp_j;
								//IWDG_Reset();//??
					 
							if(Fieldx_Info[Tp_field].loop==0)
								{
									//if(Tp_testfinal >= ( (0x20000/MAX_LOGO_FINALTEST_ONEPACKET)*sdram_TestFinalCyclex+ sdram_TestFinalNumx))
									if(Tp_testfinal > ( (0x20000/MAX_LOGO_FINALTEST_ONEPACKET)*Fieldx_Info[Tp_field].cycle+ Fieldx_Info[Tp_field].num))//20170629
									{
										break;
									}
								}//???????????h?????
								
								Printf_One_Line_TestFinal("",(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+Tp_j*MAX_LOGO_FINALTEST_ONEPACKET),1,99);//MAX_LOGO_FINALTEST_ONEPACKET,1);
						  }
						 
					}
					
			  Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Fieldx_Info[Tp_field].cycle*64;		
			  NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
					LOGO_DOUBLE_VISIT = 0;
					if( f_close(&file)!=FR_OK)
					{
						// SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
						systerm_error_status.bits.usb_canot_write_error = 1;
					}
				  if(f_mount(NULL,"",0)!=FR_OK)
					{
						 //SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
						systerm_error_status.bits.usb_canot_write_error = 1;
					}
				 
				break;
				case TestAllData_DATACLASS:
				case AnomalyRecord_DATACLASS:
			 
					Create_new_logo_file_name(Tp_field,(uint8_t *)filename_new);
				
					if( f_mount(&Tp_fatfs[Tp_fatfs_num++],"",0)!=FR_OK)
					{
						//SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
						systerm_error_status.bits.usb_canot_write_error = 1;
						 break;
					}
			  
			   //filename[7]=Tp_field/10+'0';
			  // filename[8]=Tp_field%10+'0';
			   if(f_open(&file,(const char *)filename_new,FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
				  //if(f_open(&file,(const char *)filename,FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
	        {
		           //????????h??USB?J???
						   //return;
						  //SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
						systerm_error_status.bits.usb_canot_write_error = 1;
						  break;
	        }	
					if(f_lseek(&file,0)!=FR_OK)//??????z????l???????
					{
						 // SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
						systerm_error_status.bits.usb_canot_write_error = 1;
						  break;
					}
					if(Tp_field < 10)
					{
					Tp_char[11]=Tp_field+'0';
					}
					else
					{
					Tp_char[11]=Tp_field - 10 +'A';	
					}
				 Tp_char[28]=Tp_Type+'0';
				Printf_One_Line((const char *)Tp_char,(uint8_t *)BaseData_ARR,0,1);//????l??......FIELD1......DATACLASS=1
			  Printf_Header();//?????? 20171011
					//	 viewcyw = *(__IO uint8_t*)(logodata_sdrambuffer_addr_arry[Tp_field]);
			  Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Fieldx_Info[Tp_field].cycle*64;		
			  SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
					LOGO_DOUBLE_VISIT = Tp_field;
					
					for(Tp_i=0;Tp_i<(gs_AreaInfo[Tp_field].space/0x20000);Tp_i++)
					{
							//IWDG_Reset();//??
						  Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Tp_i*64;
					    NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1); 
						  for(Tp_j=0;Tp_j<(0x20000/MAX_LOGO_ALLTEST_ONEPACKET(Tp_field));Tp_j++)
						  {
								Tp_testfinal =  (0x20000/MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))*Tp_i+ Tp_j;
								//IWDG_Reset();//??
					     // Tp_char3[2] = (Tp_testfinal/100000)%10+'0';
					     // Tp_char3[3] = (Tp_testfinal/10000)%10+'0';
					     // Tp_char3[4] = (Tp_testfinal/1000)%10+'0';
					     // Tp_char3[5] = (Tp_testfinal/100)%10+'0';
					     // Tp_char3[6] = (Tp_testfinal/10)%10+'0';
							//	Tp_char3[7] = (Tp_testfinal%10)+'0';
					     //??????????????????
							/*	if(*(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+Tp_j*MAX_LOGO_ALLTEST_ONEPACKET)==0xff)
								{
										 break;
							  }*/
									if(Fieldx_Info[Tp_field].loop==0)
								{
									if(Tp_testfinal >= ( (0x20000/MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))*Fieldx_Info[Tp_field].cycle+ Fieldx_Info[Tp_field].num))
									{
										break;
									}
								}
								
								Printf_One_Line_TestAll("\0",(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+Tp_j*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field)),MAX_LOGO_ALLTEST_ONEPACKET(Tp_field),1,gs_AreaInfo[Tp_field].size%2);
						  }
						  
					}
					
			  Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Fieldx_Info[Tp_field].cycle*64;		
			  NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
					LOGO_DOUBLE_VISIT = 0;
					
					if( f_close(&file)!=FR_OK)
					{
						// SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
							systerm_error_status.bits.usb_canot_write_error = 1;
						  
					}
				  if(f_mount(NULL,"",0)!=FR_OK)
					{
						//SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
							systerm_error_status.bits.usb_canot_write_error = 1;
					}
         break;	
        case ActionRecord_DATACLASS://????????
        	Create_new_logo_file_name(Tp_field,(uint8_t *)filename_new);
				
					 if(f_mount(&Tp_fatfs[Tp_fatfs_num++],"",0)!=FR_OK)
					 {
						 //SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
						 	systerm_error_status.bits.usb_canot_write_error = 1;
						 break;
					 }
				//filename[7]=Tp_field/10+'0';
			//   filename[8]=Tp_field%10+'0';
			   if(f_open(&file,(const char *)filename_new,FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
				  //if(f_open(&file,(const char *)filename,FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
	        {
		           //????????h??USB?J???
						    //return;
						//SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
							systerm_error_status.bits.usb_canot_write_error = 1;
						break;
	        }	
					
					if(f_lseek(&file,0)!=FR_OK)//??????z????l???????
					{
						//SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
							systerm_error_status.bits.usb_canot_write_error = 1;
						break;
					}
					if(Tp_field < 10)
					{
					Tp_char[11]=Tp_field+'0';
					}
					else
					{
					Tp_char[11]=Tp_field - 10 +'A';	
					}
				 Tp_char[28]=Tp_Type+'0';
				Printf_One_Line((const char *)Tp_char,(uint8_t *)BaseData_ARR,0,1);//????l??......FIELD1......DATACLASS=1
			 Printf_Header();//?????? 20171011
					Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Fieldx_Info[Tp_field].cycle*64;		
			  SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
					LOGO_DOUBLE_VISIT = Tp_field;
					for(Tp_i=0;Tp_i<((gs_AreaInfo[Tp_field].space)/0x20000);Tp_i++)
					{
							//IWDG_Reset();//??
						  Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Tp_i*64;
					    NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1); 
						  for(Tp_j=0;Tp_j<(0x20000/MAX_LOGO_ALLTEST_ONEPACKET(Tp_field));Tp_j++)
						  {
								Tp_testfinal =  (0x20000/MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))*Tp_i+ Tp_j;
								//IWDG_Reset();//??
									
								if(Fieldx_Info[Tp_field].loop==0)
								{
									if(Tp_testfinal >= ( (0x20000/MAX_LOGO_ALLTEST_ONEPACKET(Tp_field))*Fieldx_Info[Tp_field].cycle+ Fieldx_Info[Tp_field].num))
									{
										break;
									}
								}
								if(*(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+Tp_j*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field)) == 0xEF)//??h??????????
								{
									Tp_linshi[0] = *(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+Tp_j*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field) + 1);
									Tp_linshi[1] = *(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+Tp_j*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field) + 2);
									Tp_linshi[2] = *(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+Tp_j*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field) + 3);
									Tp_linshi[3] = *(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+Tp_j*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field) + 4);
									Tp_linshi[4] = *(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+Tp_j*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field) + 5);
									Tp_linshi[5] = *(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+Tp_j*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field) + 6);
									Tp_linshi[6] = 0x00;
									Tp_linshi[7] = 0x1E;
									Tp_linshi[8] = 0xF0;
									Printf_One_Line_TestAll("\0",(uint8_t *)Tp_linshi,9,1,1);
						 
								}
								else if(*(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+Tp_j*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field)) == 0xE0)//??h???????????
								{
									Tp_linshi[0] = *(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+Tp_j*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field) + 1);
									Tp_linshi[1] = *(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+Tp_j*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field) + 2);
									Tp_linshi[2] = *(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+Tp_j*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field) + 3);
									Tp_linshi[3] = *(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+Tp_j*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field) + 4);
									Tp_linshi[4] = *(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+Tp_j*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field) + 5);
									Tp_linshi[5] = *(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+Tp_j*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field) + 6);
									Tp_linshi[6] = 0x00;
									Tp_linshi[7] = 0x1E;
									Tp_linshi[8] = 0x00;
									Printf_One_Line_TestAll("\0",(uint8_t *)Tp_linshi,9,1,1);
								}	 
								else
								{
								Printf_One_Line_TestAll("\0",(uint8_t *)(logodata_sdrambuffer_addr_arry[Tp_field]+Tp_j*MAX_LOGO_ALLTEST_ONEPACKET(Tp_field)),MAX_LOGO_ALLTEST_ONEPACKET(Tp_field),1,gs_AreaInfo[Tp_field].size%2);
						    }
						}
						  
					}
					
			  Tp_addr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Fieldx_Info[Tp_field].cycle*64;		
			  NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_addr,1);
					LOGO_DOUBLE_VISIT = 0;
					
					if( f_close(&file)!=FR_OK)
					{
						//SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
						systerm_error_status.bits.usb_canot_write_error = 1;
					}
				  if(f_mount(NULL,"",0)!=FR_OK)
					{
						//SYSTEM_ERR_STATUS->bits.FATFS_SYSTEM_error =1;
						systerm_error_status.bits.usb_canot_write_error = 1;
					}
					
				break;					
			default:
				break;
		}
  
   }
  }
	LOGO_DOUBLE_VISIT = 0;
	
	//???²????????
////	delay_us(500000);//???0KB????????
////	UINT bytesWritten;
////	 f_mount(&Tp_fatfs[16],"",0);
////	f_lseek(&file,0);
////	f_open(&file,"CYW.~TXT",FA_CREATE_ALWAYS | FA_WRITE);
////	 f_write (&file, writeTextBuff, 1, (void *)&bytesWritten);
////	 f_close(&file);
////	f_unlink("CYW.~TXT");
////	 f_mount(NULL,"",0);
	
	
	
 if(x_flag != 0)
 {
	//LCD_DisplayStringLine( 30, "FINISH...          ");
	 #ifdef PROMPT_ENABLE 
	  LCD_DisplayStringLine( 30, "LOGO DATA FINISH   ");
	 #endif
 }
}

int AreaDataToUsbCmd(char cmd, int idx)
{
	int i = 0;

	
	if (GetUsbMountFlag() == 0)
	{
		
		systerm_error_status.bits.usb_unable_connect = 1;
		sysprintf("USB NOT connect\r\n");
		return 0;
	}

	if (cmd == LCD_Y0)
	{
		for (i = 0; i < 16;i++)
		{
			LOGOData_Write_To_USB(i);
			
				//continue;
			
		}

		return 1;
	}
	else if(cmd == LCD_Y1)
	{
		LOGOData_Write_To_USB(idx);
		
			//return 0;
		
		return 1;
	}

	return 0;
}


uint8_t GET_LOGODATA_Y5Y6_20170919(uint8_t *x_data,uint8_t x_FSC,uint8_t x_datasource)
{
	uint8_t Tp_field;
  static uint32_t Tp_itemno_oneblock;//1?BLOCK?????
	static uint32_t Tp_itemno_all;//??? 
	static uint8_t  Tp_Flag_full;//??????? 0 - ???    1-??1?
	static uint32_t Tp_now_nandaddr = 0;//????????? block
	static uint32_t Tp_requie_nandaddr = 0;//????????? block
	static uint32_t Tp_requie_sdramaddr = 0;//??????sdram???
	static uint32_t Tp_now_sdramaddr = 0;//??????sdram???
	static uint8_t  Tp_onepacketlong = 0;
  static uint32_t Tp_block_all=0;
	uint8_t Tp_i=0;
	
	Tp_field = LOGO_Y5Y6_str.AREA_NO;
	
	switch(LOGO_Y5Y6_str.DATA_Type)
	{
		case TestFinalData_DATACLASS://??sdram_TestFinalNumx
		  Tp_onepacketlong = MAX_LOGO_FINALTEST_ONEPACKET;
		  Tp_itemno_oneblock = 0x20000/Tp_onepacketlong;
		  Tp_itemno_all      = Tp_itemno_oneblock*Fieldx_Info[LOGO_Y5Y6_str.AREA_NO].cycle  + Fieldx_Info[LOGO_Y5Y6_str.AREA_NO].num;
		  if(Fieldx_Info[LOGO_Y5Y6_str.AREA_NO].loop!=0)
		  Tp_Flag_full = 1;
			else
			Tp_Flag_full = 0;
			
			
			if(Tp_itemno_all >= LOGO_Y5Y6_str.ITEM_NOW)//???????
			{
			   Tp_now_nandaddr = (gs_AreaInfo[LOGO_Y5Y6_str.AREA_NO].addr+logodata_2gbit_change_addr)/2048 + Fieldx_Info[LOGO_Y5Y6_str.AREA_NO].cycle *64;
				 Tp_requie_nandaddr = Tp_itemno_all - LOGO_Y5Y6_str.ITEM_NOW;
			   Tp_requie_nandaddr = Tp_requie_nandaddr/Tp_itemno_oneblock;
			   Tp_requie_nandaddr = (gs_AreaInfo[LOGO_Y5Y6_str.AREA_NO].addr+logodata_2gbit_change_addr)/2048 + Tp_requie_nandaddr*64;
			   if(Tp_now_nandaddr == Tp_requie_nandaddr)//???? ????
			   {
				   Tp_now_sdramaddr  = logodata_sdrambuffer_addr_arry[Tp_field] + Fieldx_Info[LOGO_Y5Y6_str.AREA_NO].num*Tp_onepacketlong;
					 Tp_requie_sdramaddr = Tp_now_sdramaddr - LOGO_Y5Y6_str.ITEM_NOW*Tp_onepacketlong+ 6 +LOGO_Y5Y6_str.DATACLASS4_ITEMNO;
			     x_data[0] = *(uint8_t *)(Tp_requie_sdramaddr);
					 memcpy(x_data+1,(uint8_t *)(Tp_requie_sdramaddr-6-LOGO_Y5Y6_str.DATACLASS4_ITEMNO),6);
					// LED_LOGO_CONTROL_ON();
					 if(x_datasource == COMM_DATA_UARST)
					 {
				       code_protocol_ack(x_FSC,7,x_data,0);
						   return 1;
					 }
			     if(x_datasource == COMM_DATA_IIC)//IIC
					 {
				       code_protocol_ack_IIC(x_FSC,7,x_data,0); 	
							 return 1;
					 }
					 
					
				 }
		   	 else//???
			   {
				     //???????
					   SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],Tp_now_nandaddr,1);
					   LOGO_DOUBLE_VISIT = Tp_field;
					   //???????
					   NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_requie_nandaddr,1);
					   //????
					   Tp_requie_sdramaddr = Tp_itemno_all - LOGO_Y5Y6_str.ITEM_NOW;
					   Tp_requie_sdramaddr = Tp_requie_sdramaddr%Tp_itemno_oneblock;
					   Tp_requie_sdramaddr = Tp_requie_sdramaddr*Tp_onepacketlong;
					   Tp_requie_sdramaddr = logodata_sdrambuffer_addr_arry[Tp_field] + Tp_requie_sdramaddr+ 6 +LOGO_Y5Y6_str.DATACLASS4_ITEMNO;
					   x_data[0] = *(uint8_t *)(Tp_requie_sdramaddr);
					   memcpy(x_data+1,(uint8_t *)(Tp_requie_sdramaddr-6-LOGO_Y5Y6_str.DATACLASS4_ITEMNO),6);
					    //???????
					   NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_now_nandaddr,1);
						 LOGO_DOUBLE_VISIT = 0;
					   //LED_LOGO_CONTROL_ON();
					   if(x_datasource == COMM_DATA_UARST)
					   {
				       code_protocol_ack(x_FSC,7,x_data,0);
						   return 1;
					   }
			       if(x_datasource == COMM_DATA_IIC)//IIC
					   {
				       code_protocol_ack_IIC(x_FSC,7,x_data,0); 	
							 return 1;
					   }
					 
					  
			   }
		  }
      else
			{
				if(Tp_Flag_full == 1)//???????
				{
					  Tp_block_all = gs_AreaInfo[Tp_field].space/0x20000;
					
					if((LOGO_Y5Y6_str.ITEM_NOW)>=(Tp_block_all*Tp_itemno_oneblock))
					{
						//return 0;//????????
						//x_data[0]=0xff;
						memset(x_data,0xff,7);
						//LED_LOGO_CONTROL_ON();
					   if(x_datasource == COMM_DATA_UARST)
					   {
				       code_protocol_ack(x_FSC,7,x_data,0);
						   return 1;
					   }
			       if(x_datasource == COMM_DATA_IIC)//IIC
					   {
				       code_protocol_ack_IIC(x_FSC,7,x_data,0); 	
							 return 1;
					   }
					}
					//???NAN??
					Tp_now_nandaddr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + ((Fieldx_Info[Tp_field].num)/Tp_itemno_oneblock)*64;
					//???NAN??
					Tp_requie_nandaddr = (Tp_block_all*Tp_itemno_oneblock - (LOGO_Y5Y6_str.ITEM_NOW  - Tp_itemno_all))/Tp_itemno_oneblock;//???
					Tp_requie_nandaddr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Tp_requie_nandaddr*64;//(Tp_block_all-1)*64;
					
					//???????
					SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],Tp_now_nandaddr,1);
					LOGO_DOUBLE_VISIT = Tp_field;//????????????
					//???????
					 NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_requie_nandaddr,1);
					//???SDRAM??
					Tp_requie_sdramaddr = (Tp_block_all*Tp_itemno_oneblock - (LOGO_Y5Y6_str.ITEM_NOW  - Tp_itemno_all))%Tp_itemno_oneblock;
					Tp_requie_sdramaddr = Tp_requie_sdramaddr*Tp_onepacketlong + logodata_sdrambuffer_addr_arry[Tp_field] + 6 +LOGO_Y5Y6_str.DATACLASS4_ITEMNO;
				  x_data[0] = *(uint8_t *)(Tp_requie_sdramaddr);
				  memcpy(x_data+1,(uint8_t *)(Tp_requie_sdramaddr-6-LOGO_Y5Y6_str.DATACLASS4_ITEMNO),6);
					
					//???????
					 NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_now_nandaddr,1);
					LOGO_DOUBLE_VISIT=0;
					 //LED_LOGO_CONTROL_ON();
					 if(x_datasource == COMM_DATA_UARST)
					 {
				     code_protocol_ack(x_FSC,7,x_data,0);
						 return 1;
					 }
			     if(x_datasource == COMM_DATA_IIC)//IIC
					 {
				     code_protocol_ack_IIC(x_FSC,7,x_data,0); 	
						 return 1;
					  }
				}
				else
				{
					  // x_data[0]=0xff;
					memset(x_data ,0xff,7);
						//LED_LOGO_CONTROL_ON();
					   if(x_datasource == COMM_DATA_UARST)
					   {
				       code_protocol_ack(x_FSC,7,x_data,0);
						   return 1;
					   }
			       if(x_datasource == COMM_DATA_IIC)//IIC
					   {
				       code_protocol_ack_IIC(x_FSC,7,x_data,0); 	
							 return 1;
					   }
				}
			}		
		  break;
		case TestAllData_DATACLASS:
		case AnomalyRecord_DATACLASS:
		case ActionRecord_DATACLASS:
			Tp_onepacketlong = MAX_LOGO_ALLTEST_ONEPACKET(Tp_field);
			Tp_itemno_oneblock = 0x20000/Tp_onepacketlong;
		  Tp_itemno_all      = Tp_itemno_oneblock*Fieldx_Info[Tp_field].cycle  + (Fieldx_Info[Tp_field].num-1);
		  if(Fieldx_Info[Tp_field].loop!=0)
		  Tp_Flag_full = 1;
			else
			Tp_Flag_full = 0;
			
			if((Fieldx_Info[Tp_field].cycle==0)&&(Fieldx_Info[Tp_field].loop==0)&&(Fieldx_Info[Tp_field].num==0))
			{
				goto flag_blank;
			}
			//yy = Fieldx_Info[Tp_field].num;//ceshi
			if(Tp_itemno_all >= LOGO_Y5Y6_str.ITEM_NOW)//???????
			{
			   Tp_now_nandaddr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + (Tp_itemno_all/Tp_itemno_oneblock)*64;
				 Tp_requie_nandaddr = Tp_itemno_all - LOGO_Y5Y6_str.ITEM_NOW;
			   Tp_requie_nandaddr = Tp_requie_nandaddr/Tp_itemno_oneblock;
			   Tp_requie_nandaddr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Tp_requie_nandaddr*64;
			   if(Tp_now_nandaddr == Tp_requie_nandaddr)//???? ????
			   {
				   Tp_now_sdramaddr  = logodata_sdrambuffer_addr_arry[Tp_field] + (Fieldx_Info[Tp_field].num-1)*Tp_onepacketlong;
					 
					 
					 Tp_requie_sdramaddr = Tp_now_sdramaddr - LOGO_Y5Y6_str.ITEM_NOW*Tp_onepacketlong;
			    // if(gs_AreaInfo[Tp_field].size%2==0)
					 if(0)
					 {
					 memcpy(x_data,(uint8_t *)(Tp_requie_sdramaddr+6),Tp_onepacketlong-6);
					 memcpy(x_data+Tp_onepacketlong-6,(uint8_t *)(Tp_requie_sdramaddr),6);
					 }
					 else
					 {
						 if(*(uint8_t *)(Tp_requie_sdramaddr)==0xe0)
						 {
							 sprintf((char *)(x_data),"%s%c","001E0",'\0');
							 sprintf((char *)(x_data+5),"%02X%02X%02X%02X%02X%02X%c",*(uint8_t *)(Tp_requie_sdramaddr+1),
							                            *(uint8_t *)(Tp_requie_sdramaddr+2),*(uint8_t *)(Tp_requie_sdramaddr+3),
																						*(uint8_t *)(Tp_requie_sdramaddr+4),*(uint8_t *)(Tp_requie_sdramaddr+5),
																							 *(uint8_t *)(Tp_requie_sdramaddr+6),'\0');
						 }
						 else if(*(uint8_t *)(Tp_requie_sdramaddr)==0xef)
						 {
							 sprintf((char *)(x_data),"%s%c","001EF",'\0');
							 sprintf((char *)(x_data+5),"%02X%02X%02X%02X%02X%02X%c",*(uint8_t *)(Tp_requie_sdramaddr+1),
							                            *(uint8_t *)(Tp_requie_sdramaddr+2),*(uint8_t *)(Tp_requie_sdramaddr+3),
																						*(uint8_t *)(Tp_requie_sdramaddr+4),*(uint8_t *)(Tp_requie_sdramaddr+5),
																							 *(uint8_t *)(Tp_requie_sdramaddr+6),'\0');
						 }
						 else
						 {
						 for(Tp_i=0;Tp_i<(Tp_onepacketlong-6);Tp_i++)
						 sprintf((char *)(x_data+2*Tp_i),"%02X",*(uint8_t *)(Tp_requie_sdramaddr+6+Tp_i));
						 if(gs_AreaInfo[Tp_field].size%2==1)
						 {
						 sprintf((char *)(x_data+2*Tp_i-1),"%02X%02X%02X%02X%02X%02X%c",*(uint8_t *)(Tp_requie_sdramaddr),
							                            *(uint8_t *)(Tp_requie_sdramaddr+1),*(uint8_t *)(Tp_requie_sdramaddr+2),
																						*(uint8_t *)(Tp_requie_sdramaddr+3),*(uint8_t *)(Tp_requie_sdramaddr+4),
																							 *(uint8_t *)(Tp_requie_sdramaddr+5),'\0');
					   }
						 else
						 {
							sprintf((char *)(x_data+2*Tp_i),"%02X%02X%02X%02X%02X%02X%c",*(uint8_t *)(Tp_requie_sdramaddr),
							                            *(uint8_t *)(Tp_requie_sdramaddr+1),*(uint8_t *)(Tp_requie_sdramaddr+2),
																						*(uint8_t *)(Tp_requie_sdramaddr+3),*(uint8_t *)(Tp_requie_sdramaddr+4),
																							 *(uint8_t *)(Tp_requie_sdramaddr+5),'\0'); 
						 }
					  }
					 }
					// LED_LOGO_CONTROL_ON();
					 if(x_datasource == COMM_DATA_UARST)
					 {
				       //if(gs_AreaInfo[Tp_field].size%2==0)//????????
						   if(0)    
						      code_protocol_ack(x_FSC,Tp_onepacketlong,x_data,0);
							 else//?????????
								  code_protocol_ack(x_FSC,strlen((char *)x_data),x_data,1);
						   return 1;
					 }
			     if(x_datasource == COMM_DATA_IIC)//IIC
					 {
				       //if(gs_AreaInfo[Tp_field].size%2==0)//????????
						   if(0)   
						     code_protocol_ack_IIC(x_FSC,Tp_onepacketlong,x_data,0); 	
							 else
							   code_protocol_ack_IIC(x_FSC,strlen((char *)x_data),x_data,1); 	
							 return 1;
					 }
					 
				 }
		   	 else//???
			   {
				    //???????
					   SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],Tp_now_nandaddr,1);
					   LOGO_DOUBLE_VISIT = Tp_field;
					   //???????
					   NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_requie_nandaddr,1);
					   //????
					   Tp_requie_sdramaddr = Tp_itemno_all - LOGO_Y5Y6_str.ITEM_NOW;
					   Tp_requie_sdramaddr = Tp_requie_sdramaddr%Tp_itemno_oneblock;
					   Tp_requie_sdramaddr = Tp_requie_sdramaddr*Tp_onepacketlong;
					   Tp_requie_sdramaddr = logodata_sdrambuffer_addr_arry[Tp_field] + Tp_requie_sdramaddr;
					  //if(gs_AreaInfo[Tp_field].size%2==0)
					 if(0)
					 {
   					 memcpy(x_data,(uint8_t *)(Tp_requie_sdramaddr+6),Tp_onepacketlong-6);
					   memcpy(x_data+Tp_onepacketlong-6,(uint8_t *)(Tp_requie_sdramaddr),6);
					 }
					 else
					 {
						 if(*(uint8_t *)(Tp_requie_sdramaddr)==0xe0)
						 {
							 sprintf((char *)(x_data),"%s%c","001E0",'\0');
							 sprintf((char *)(x_data+5),"%02X%02X%02X%02X%02X%02X%c",*(uint8_t *)(Tp_requie_sdramaddr+1),
							                            *(uint8_t *)(Tp_requie_sdramaddr+2),*(uint8_t *)(Tp_requie_sdramaddr+3),
																						*(uint8_t *)(Tp_requie_sdramaddr+4),*(uint8_t *)(Tp_requie_sdramaddr+5),
																							 *(uint8_t *)(Tp_requie_sdramaddr+6),'\0');
						 }
						 else if(*(uint8_t *)(Tp_requie_sdramaddr)==0xef)
						 {
							 sprintf((char *)(x_data),"%s%c","001EF",'\0');
							 sprintf((char *)(x_data+5),"%02X%02X%02X%02X%02X%02X%c",*(uint8_t *)(Tp_requie_sdramaddr+1),
							                            *(uint8_t *)(Tp_requie_sdramaddr+2),*(uint8_t *)(Tp_requie_sdramaddr+3),
																						*(uint8_t *)(Tp_requie_sdramaddr+4),*(uint8_t *)(Tp_requie_sdramaddr+5),
																							 *(uint8_t *)(Tp_requie_sdramaddr+6),'\0');
						 }
						 else
						 {
						 for(Tp_i=0;Tp_i<(Tp_onepacketlong-6);Tp_i++)
						 sprintf((char *)(x_data+2*Tp_i),"%02X",*(uint8_t *)(Tp_requie_sdramaddr+6+Tp_i));
						if(gs_AreaInfo[Tp_field].size%2==1)
						{
						 sprintf((char *)(x_data+2*Tp_i-1),"%02X%02X%02X%02X%02X%02X%c",*(uint8_t *)(Tp_requie_sdramaddr),
							                            *(uint8_t *)(Tp_requie_sdramaddr+1),*(uint8_t *)(Tp_requie_sdramaddr+2),
																						*(uint8_t *)(Tp_requie_sdramaddr+3),*(uint8_t *)(Tp_requie_sdramaddr+4),
																							 *(uint8_t *)(Tp_requie_sdramaddr+5),'\0');
					  }
						else
						{
							 sprintf((char *)(x_data+2*Tp_i),"%02X%02X%02X%02X%02X%02X%c",*(uint8_t *)(Tp_requie_sdramaddr),
							                            *(uint8_t *)(Tp_requie_sdramaddr+1),*(uint8_t *)(Tp_requie_sdramaddr+2),
																						*(uint8_t *)(Tp_requie_sdramaddr+3),*(uint8_t *)(Tp_requie_sdramaddr+4),
																							 *(uint8_t *)(Tp_requie_sdramaddr+5),'\0');
						}
					  }
					 }
					   //???????
					   NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_now_nandaddr,1);
						 LOGO_DOUBLE_VISIT = 0;
					  // LED_LOGO_CONTROL_ON();
					   if(x_datasource == COMM_DATA_UARST)
					   {
				       //if(gs_AreaInfo[Tp_field].size%2==0)//????????
							 if(0)  
							   code_protocol_ack(x_FSC,Tp_onepacketlong,x_data,0);
							 else
								 code_protocol_ack(x_FSC,strlen((char *)x_data),x_data,1);
						   return 1;
					   }
			       if(x_datasource == COMM_DATA_IIC)//IIC
					   {
				       //if(gs_AreaInfo[Tp_field].size%2==0)//????????
							 if(0)   
							    code_protocol_ack_IIC(x_FSC,Tp_onepacketlong,x_data,0);
							 else
								  code_protocol_ack_IIC(x_FSC,strlen((char *)x_data),x_data,1);
							 return 1;
					   }
						 
			   }
		  }
      else
			{
				if(Tp_Flag_full == 1)//???????
				{
					Tp_block_all = gs_AreaInfo[Tp_field].space/0x20000;
					
					if(LOGO_Y5Y6_str.ITEM_NOW>=(Tp_block_all*Tp_itemno_oneblock))
					{
						//return 0;//????????
						if(gs_AreaInfo[Tp_field].size%2==0)
						{
						memset(x_data,0xff,Tp_onepacketlong);
						}
						else
						{
						memset(x_data,0,Tp_onepacketlong*2);	
						memset(x_data,'F',Tp_onepacketlong*2-1);
						}
					//	LED_LOGO_CONTROL_ON();
					   if(x_datasource == COMM_DATA_UARST)
					   {
				       //if(gs_AreaInfo[Tp_field].size%2==0)//????????
							  if(0)  
							     code_protocol_ack(x_FSC,Tp_onepacketlong,x_data,0);
							 else
								  code_protocol_ack(x_FSC,strlen((char *)x_data),x_data,1);
						   return 1;
					   }
			       if(x_datasource == COMM_DATA_IIC)//IIC
					   {
				       //if(gs_AreaInfo[Tp_field].size%2==0)//????????
							  if(0)  
							    code_protocol_ack_IIC(x_FSC,Tp_onepacketlong,x_data,0);
               else
                  code_protocol_ack_IIC(x_FSC,strlen((char *)x_data),x_data,1); 								 
							 return 1;
					   }
						
					}
					//???NAN??
					Tp_now_nandaddr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + ((Fieldx_Info[Tp_field].num-1)/Tp_itemno_oneblock)*64;
					//???NAN??
					Tp_requie_nandaddr = (Tp_block_all*Tp_itemno_oneblock - (LOGO_Y5Y6_str.ITEM_NOW  - Tp_itemno_all))/Tp_itemno_oneblock;//???
					Tp_requie_nandaddr = (gs_AreaInfo[Tp_field].addr+logodata_2gbit_change_addr)/2048 + Tp_requie_nandaddr*64;//(Tp_block_all-1)*64;
					
					//???????
					SDRAM_TO_NANDFLASH(logodata_sdrambuffer_addr_arry[Tp_field],Tp_now_nandaddr,1);
					LOGO_DOUBLE_VISIT = Tp_field;//????????????
					//???????
					 NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_requie_nandaddr,1);
					//???SDRAM??
					Tp_requie_sdramaddr = (Tp_block_all*Tp_itemno_oneblock - (LOGO_Y5Y6_str.ITEM_NOW - Tp_itemno_all))%Tp_itemno_oneblock;
					Tp_requie_sdramaddr = Tp_requie_sdramaddr*Tp_onepacketlong + logodata_sdrambuffer_addr_arry[Tp_field];
					 //if(gs_AreaInfo[Tp_field].size%2==0)
					if(0)
					 {
					memcpy(x_data,(uint8_t *)(Tp_requie_sdramaddr+6),Tp_onepacketlong-6);
					memcpy(x_data+Tp_onepacketlong-6,(uint8_t *)(Tp_requie_sdramaddr),6);
					 }
					 else
					 {
						 if(*(uint8_t *)(Tp_requie_sdramaddr)==0xe0)
						 {
							 sprintf((char *)(x_data),"%s%c","001E0",'\0');
							 sprintf((char *)(x_data+5),"%02X%02X%02X%02X%02X%02X%c",*(uint8_t *)(Tp_requie_sdramaddr+1),
							                            *(uint8_t *)(Tp_requie_sdramaddr+2),*(uint8_t *)(Tp_requie_sdramaddr+3),
																						*(uint8_t *)(Tp_requie_sdramaddr+4),*(uint8_t *)(Tp_requie_sdramaddr+5),
																							 *(uint8_t *)(Tp_requie_sdramaddr+6),'\0');
						 }
						 else if(*(uint8_t *)(Tp_requie_sdramaddr)==0xef)
						 {
							 sprintf((char *)(x_data),"%s%c","001EF",'\0');
							 sprintf((char *)(x_data+5),"%02X%02X%02X%02X%02X%02X%c",*(uint8_t *)(Tp_requie_sdramaddr+1),
							                            *(uint8_t *)(Tp_requie_sdramaddr+2),*(uint8_t *)(Tp_requie_sdramaddr+3),
																						*(uint8_t *)(Tp_requie_sdramaddr+4),*(uint8_t *)(Tp_requie_sdramaddr+5),
																							 *(uint8_t *)(Tp_requie_sdramaddr+6),'\0');
						 }
						 else
						 {
						 for(Tp_i=0;Tp_i<(Tp_onepacketlong-6);Tp_i++)
						 sprintf((char *)(x_data+2*Tp_i),"%02X",*(uint8_t *)(Tp_requie_sdramaddr+6+Tp_i));
							 if(gs_AreaInfo[Tp_field].size%2==1)
							 {
						 sprintf((char *)(x_data+2*Tp_i-1),"%02X%02X%02X%02X%02X%02X%c",*(uint8_t *)(Tp_requie_sdramaddr),
							                            *(uint8_t *)(Tp_requie_sdramaddr+1),*(uint8_t *)(Tp_requie_sdramaddr+2),
																						*(uint8_t *)(Tp_requie_sdramaddr+3),*(uint8_t *)(Tp_requie_sdramaddr+4),
																							 *(uint8_t *)(Tp_requie_sdramaddr+5),'\0');
					    }
							 else
							 {
								 sprintf((char *)(x_data+2*Tp_i),"%02X%02X%02X%02X%02X%02X%c",*(uint8_t *)(Tp_requie_sdramaddr),
							                            *(uint8_t *)(Tp_requie_sdramaddr+1),*(uint8_t *)(Tp_requie_sdramaddr+2),
																						*(uint8_t *)(Tp_requie_sdramaddr+3),*(uint8_t *)(Tp_requie_sdramaddr+4),
																							 *(uint8_t *)(Tp_requie_sdramaddr+5),'\0');
							 }
					   }
					 }
					//???????
					 NANDFLASH_TO_SDRAM(logodata_sdrambuffer_addr_arry[Tp_field],Tp_now_nandaddr,1);
					LOGO_DOUBLE_VISIT=0;
					// LED_LOGO_CONTROL_ON();
					 if(x_datasource == COMM_DATA_UARST)
					 {
				     //if(gs_AreaInfo[Tp_field].size%2==0)//????????
						 if(0)   
						     code_protocol_ack(x_FSC,Tp_onepacketlong,x_data,0);
						 else 
							  code_protocol_ack(x_FSC,strlen((char *)x_data),x_data,1);
						 return 1;
					 }
			     if(x_datasource == COMM_DATA_IIC)//IIC
					 {
				     //if(gs_AreaInfo[Tp_field].size%2==0)//????????
						 if(0)   
						    code_protocol_ack_IIC(x_FSC,Tp_onepacketlong,x_data,0); 
             else
                code_protocol_ack_IIC(x_FSC,strlen((char *)x_data),x_data,1);  							 
						 return 1;
					  }
					 
				}
				else
				{
					//return 0;//??????
flag_blank:				
					 //if(gs_AreaInfo[Tp_field].size%2==0)
					  if(0)
						{
						memset(x_data,0xff,Tp_onepacketlong);
						}
						else
						{
							if(gs_AreaInfo[Tp_field].size%2==1)
							{
						memset(x_data,0,Tp_onepacketlong*2);	
						memset(x_data,'F',Tp_onepacketlong*2-1);
							}
							else
							{
						memset(x_data,0,Tp_onepacketlong*2);	
						memset(x_data,'F',Tp_onepacketlong*2);
							}
						}
					
					//LED_LOGO_CONTROL_ON();
					   if(x_datasource == COMM_DATA_UARST)
					   {
				       //if(gs_AreaInfo[Tp_field].size%2==0)//????????
							 if(0)
							 {
							    code_protocol_ack(x_FSC,Tp_onepacketlong,x_data,0);
							 }
							 else
							 {
								  code_protocol_ack(x_FSC,strlen((char *)x_data),x_data,1);
							 }
						   return 1;
					   }
			       if(x_datasource == COMM_DATA_IIC)//IIC
					   {
				       //if(gs_AreaInfo[Tp_field].size%2==0)//????????
							 if(0)
							 {
							 code_protocol_ack_IIC(x_FSC,Tp_onepacketlong,x_data,0); 
							 }
               else			
               {
							 code_protocol_ack_IIC(x_FSC,strlen((char *)x_data),x_data,1); 
							 }								 
							 return 1;
					   }
				}
			}		
			
			break;
		
			
		
		default:
			break;
	}
	return 0;
}






