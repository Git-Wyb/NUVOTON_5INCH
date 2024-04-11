#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nuc970.h"
#include "sys.h"
#include "Aprotocol.h"
#include "Command_all.h"
#include "rtc.h"
#include "W25Q128.h"

extern AreaConfig gs_AreaInfo[16];
extern uint32_t logodata_sdrambuffer_addr_arry[16];
extern  uint8_t *BaseData_ARR;
 extern uint32_t logodata_basedata_BUFFER,logodata_field1_BUFFER,logodata_field2_BUFFER,logodata_field3_BUFFER,logodata_field4_BUFFER,\
          logodata_field5_BUFFER,logodata_field6_BUFFER,logodata_field7_BUFFER,logodata_field8_BUFFER,logodata_field9_BUFFER,\
					logodata_field10_BUFFER,logodata_field11_BUFFER,logodata_field12_BUFFER,logodata_field13_BUFFER,logodata_field14_BUFFER,\
					logodata_field15_BUFFER;
FieldArea4All_cyw *pArea4;
FieldArea3All_cyw *pArea3;
FieldArea1All_cyw *pArea1;
FieldArea2All_cyw *pArea2;
extern SYSTEMERRORSTATUS_TYPE  systerm_error_status;
 uint16_t One_data_long[16] = {0}; //1�����ݵĳ���
uint16_t One_space_num[16] = {0}; //1��SDRAM�ռ�0x20000����Ŀ��





//��������0~3 �������ڴ浱�У�Ҫ���������ȡ
//��������0~3 ����ȡ��U��ʱ��Ҫ���ڴ�ͼ�¼ͳһһ�£�������ȡʱҪ����һ��
//void SaveAreaData(uint8_t type , uint8_t x_field)  // 1=���籣�棻0����������0~3 ����
//{
//	void *addr;
//	//BaseAreaAll *pAreaBase;
//	
//	
//	
//	int i = 0;
//	uint8_t Tp_i;
//	//for (Tp_i = 0; Tp_i < 16;Tp_i++)
//	//{
//	//	i = 15 - Tp_i;
//	  i = x_field;
//		addr = (void *)(AREA_FILED_START + 0X2000 * i);
//		switch (gs_AreaInfo[i].type)
//		{
//		case 0:
//			field_info_save();//��BASEDATA�������ݸ��� 
//		  WriteAreaConfig_new();
//			//pAreaBase = (BaseAreaAll *)addr;
//			WriteAreaData(i, BaseData_ARR, sizeof(BaseData_ARR), 1);
//			break;
//		case 1:
//			pArea1 = (FieldArea1All_cyw *)addr;
//			if(pArea1->num<DATA_TYPE_1_2_MAX_NUM)
//			{
//			  //WriteAreaData(i, pArea1, sizeof(FieldArea1All_cyw), 2);
//				WriteAreaDataclass4567(i, pArea1, sizeof(FieldArea1All_cyw), pArea1->num*sizeof(FieldArea1All_cyw));
//			}
//			break;
//		case 2:
//			
//			pArea2 = (FieldArea2All_cyw *)addr;
//			if(pArea2->num<DATA_TYPE_1_2_MAX_NUM)
//			{
//			  //WriteAreaData(i, pArea2, sizeof(FieldArea2All_cyw), 2);
//				WriteAreaDataclass4567(i, pArea2, sizeof(FieldArea2All_cyw), pArea2->num*sizeof(FieldArea2All_cyw));
//			} 
//			break;
//		case 3:
//			if(type==0)
//			{
//			   pArea3 = (FieldArea3All_cyw *)addr;
//			   WriteAreaData(i, pArea3, sizeof(FieldArea3All_cyw), 1);
//			}
//			break;
//		
//		case 4:
//			if(type==0)//�����
//			{
//				One_data_long[i] = sizeof(FieldArea4All_cyw);
//				One_space_num[i] = 0x2000/One_data_long[i];
//			  addr = (void *)(AREA_FILED_START + 0x2000 * i);
//			  WriteAreaDataclass4567(i, addr, One_data_long[i]*Fieldx_Info[i].num,Fieldx_Info[i].cycle*One_space_num[i]*One_data_long[i]);
//			}
//			
//			if(type==1)//�����
//			{	
//				//pArea4 = (FieldArea4All *)addr;
//				//WriteAreaData(i, pArea4, sizeof(FieldArea4All), 0);

//				//gs_AreaWeadTotalNum[i]=gs_AreaInfo[i].space/sizeof(FieldArea4All);  //(gs_AreaInfo[i].size + 12);
//			
//				//WriteAreaDataclass4567(i, pArea4, sizeof(FieldArea4All),gs_AreaWriteIdx[i]);

//				//gs_AreaWriteIdx[i]++;
//				//if(gs_AreaWriteIdx[i]>=gs_AreaWeadTotalNum[i])
//				//{
//				//	gs_AreaWriteIdx[i]=0;
//				//}
//				
//				
//				One_data_long[i] = sizeof(FieldArea4All_cyw);
//				One_space_num[i] = 0x2000/One_data_long[i];
//				
//        
//			
//					 
//					WriteAreaDataclass4567(i, addr, One_data_long[i]*(Fieldx_Info[i].num+1),Fieldx_Info[i].cycle*One_space_num[i]*One_data_long[i]);
//				
//			}
//			
//			break;
//		case 5:
//	  case 6:
//			if((type==1)||(type==0))
//			{	
//				One_data_long[i] = gs_AreaInfo[i].size + 6;
//				One_space_num[i] = 0x2000/One_data_long[i];
//				addr = (void *)(AREA_FILED_START + 0x2000 * i);
//				WriteAreaDataclass4567(i, addr, One_data_long[i]*Fieldx_Info[i].num,Fieldx_Info[i].cycle*One_space_num[i]*One_data_long[i]);
//			}
//			break;
//		case 7:	
//			if(type==0)//�����
//			{
//				One_data_long[i] = gs_AreaInfo[i].size + 6;
//				One_space_num[i] = 0x2000/One_data_long[i];
//				addr = (void *)(AREA_FILED_START + 0x2000 * i);
//				WriteAreaDataclass4567(i, addr, One_data_long[i]*Fieldx_Info[i].num,Fieldx_Info[i].cycle*One_space_num[i]*One_data_long[i]);
//			}
//		  if(type==1)//�����
//			{	
//				DataClass7PowerOffRecord(i);
//			  One_data_long[i] = gs_AreaInfo[i].size + 6;
//				One_space_num[i] = 0x2000/One_data_long[i];
//			  addr = (void *)(AREA_FILED_START + 0x2000 * i);
//				WriteAreaDataclass4567(i, addr, One_data_long[i]*Fieldx_Info[i].num,Fieldx_Info[i].cycle*One_space_num[i]*One_data_long[i]);
//			}	
//			break;
//		default:
//			break;
//		}
//	//}

//	//save_data_class4567_write_poisition( );//data_c_4567_wr_p.txt ������
//}


int AreaWriteCmd(char cmd, int field, int idx,void *data,int len)		//���� FILED  ���� ���� ����
{
	void *addr,*addr1;
	//BaseAreaAll *pAreaBase,pAreaBaseCom;
	uint8_t Tp_return = 1;
	FieldArea3All_cyw *pArea3Com;
	uint16_t Tp_i,checksumwrite=0,checksumread=0;
	
	//FieldArea3All *pArea3;
	
	TIME_TYPE time;
	static char buf[256];
	//	int i;
	//unsigned char *p=(unsigned char *)data;

	if (field >= 16) return 0;
	if (gs_AreaInfo[field].type == 0xff) return 0;
	addr = (void *)logodata_sdrambuffer_addr_arry[field];//(void *)(AREA_FILED_START + 0x2000 * field);
	switch (gs_AreaInfo[field].type)
	{
	case 0:
		if ((len <= 8) && (idx < 32))
		{
			//pAreaBase = (BaseAreaAll *)addr;
			//pAreaBase->areaData[idx].size = len;
		//	memcpy(pAreaBase->areaData[idx].data, data, len);
			//read_rtc(&time);
			//memcpy(pAreaBase->areaData[0].data, &time, 6);
			//pAreaBase->areaData[0].size = 6;
			
			*(uint8_t *)(BaseData_ARR+idx*9+8) = len;
			memset(BaseData_ARR+idx*9,0,8);
			memcpy(BaseData_ARR+idx*9, data, len);
			#ifdef SYSUARTPRINTF
			sysprintf("1.BaseData_ARR[idx] = %s\r\n",(char *)(BaseData_ARR+idx*9));
			#endif
			Updata_Basedata_No0_cyw();
			#ifdef SYSUARTPRINTF
			sysprintf("2.BaseData_ARR[idx] = %s\r\n",(char *)(BaseData_ARR+idx*9));
			#endif
			basedata_ram_to_sdram();
			#ifdef SYSUARTPRINTF
			sysprintf("3.BaseData_ARR[idx] = %s\r\n",(char *)(BaseData_ARR+idx*9));
			#endif
			SDRAM_TO_NANDFLASH(logodata_basedata_BUFFER,baseA_data__nandflash_start,1);
			SDRAM_TO_NANDFLASH(logodata_basedata_BUFFER,baseB_data__nandflash_start,1);
			#ifdef SYSUARTPRINTF
			sysprintf("4.BaseData_ARR[idx] = %s\r\n",(char *)(BaseData_ARR+idx*9));
			#endif
			
			if((idx==Master_Ver_index)||(idx==BMP_Ver_index)||(idx==PCB_Checked_5inch)||(idx==VBAT_FLAG_5inch))
			{
				W25Q128_Write();
			}
			return 1;
			
			//SaveAreaData(0, 0);
			
	    //GetAreaData(0, 0,pAreaBaseCom, sizeof(BaseAreaAll));
//			for(Tp_i=0;Tp_i<sizeof(BaseAreaAll);Tp_i++)
//			{
//				checksumwrite =checksumwrite + *(uint8_t *)(BaseData_ARR+Tp_i);
//			}
//			SDRAM_TO_NANDFLASH(logodata_field0_BUFFER,base_data__nandflash_start,1);
//			
//			NANDFLASH_TO_SDRAM(logodata_field0_BUFFER,base_data__nandflash_start,1);
//			for(Tp_i=0;Tp_i<sizeof(BaseAreaAll);Tp_i++)
//			{
//				checksumread =checksumread + *(uint8_t *)(logodata_field0_BUFFER+Tp_i);
//			}
//			if(checksumwrite==checksumread)
//			{
//			   return 1;//�ɹ�
//			}
//			else
//			{ 
//				
//				systerm_error_status.bits.nandflash_Write_error = 1; 
//				//BAD_BLOCK_MARK(BLOCK_WRITE);
//				//SDRAM_TO_NANDFLASH(logodata_field0_BUFFER,base_data__nandflash_start,1);
//				return 0;
//			}
			
		}
		break;
	case 1:
		if (idx <= 512)
		{
			//read_rtc(&time);
			pArea1 = (FieldArea1All_cyw *)addr;
			pArea1->areaData[idx - 1].data++;
			//���л����Ѿ���ֵ��
			//pArea1->year = pwr_on_time.year;
			//pArea1->month = pwr_on_time.month;
			return 1;
		}
		break;
	case 2:
		if (idx <= 512)
		{
			//read_rtc(&time);		
			pArea2 = (FieldArea2All_cyw *)addr;
			pArea2->areaData[idx - 1].data += (unsigned int)data;
                    //���л����Ѿ���ֵ��
			//pArea2->year = pwr_on_time.year;
			//pArea2->month = pwr_on_time.month;
			return 1;
		}
		break;
	case 3:
//		if (idx < 1000)
//		{
//			
//			pArea3 = (FieldArea3All_cyw *)addr;
//			pArea3->areaData[idx].data= (unsigned int)data;// *p;//(unsigned char*)data;//  (unsigned int)data; //(unsigned char*)data;// (unsigned char)data;
//			//pArea3->areaData[idx].data=(unsigned char*)data;
//			
//			SaveAreaData(0, field);
//			
//			GetAreaData(field ,0, pArea3Com, sizeof(FieldArea3All_cyw));
//			
//			if(memcmp(pArea3,pArea3Com,sizeof(FieldArea3All_cyw))==0)
//			{
//				 return 1;
//			}
//			else
//			{
//				 systerm_error_status.bits.nandflash_Write_error = 1; 
//				 return 0;
//			}
//		}
		break;
	case 4:
//		if (idx < 100)
//		{
//			//read_rtc(&time);	
//			One_data_long[field] = sizeof(FieldArea4All_cyw);
//			One_space_num[field] = 0x2000/One_data_long[field];
//      addr = (void *)(AREA_FILED_START + 0x2000 * field + One_data_long[field]*Fieldx_Info[field].num);			
//			pArea4 = (FieldArea4All_cyw *)addr;			
//			pArea4->areaData[idx].data = (unsigned int)data; 
//			/*pArea4->year = time.year;
//			pArea4->month = time.month;
//			pArea4->day = time.day;
//			pArea4->hour = time.hour;
//			pArea4->min = time.minute;
//			pArea4->sec = time.second;
//			*/
//			//pArea4->rev[0]=get_vbat_average_ad_value( );	//
//			return 1;
//		}
		break;
	case 5:
//		if (len <= gs_AreaInfo[field].size)
//		{
//      //gs_AreaWeadTotalNum[filed]=gs_AreaInfo[filed].space/(gs_AreaInfo[filed].size + 12);//1��������ռ�Ŀռ�
//			One_data_long[field] = gs_AreaInfo[field].size + 6;
//			
//			read_rtc(&time);
//			//��ӡ��SDRAM
//			//sprintf((char *)(addr+ gs_AreaWeadTotalNum[filed]*Fieldx_Info[filed].num), "%02x%02x%02x%02x%02x%02x%-99s", time.year, time.month, time.day, time.hour, time.minute, time.second, (char *)data);
//			addr = (void *)(AREA_FILED_START + 0x2000 * field + One_data_long[field] *Fieldx_Info[field].num);
//			//sprintf(addr, "%02x%02x%02x%02x%02x%02x%-99s", time.year, time.month, time.day, time.hour, time.minute, time.second, (char *)data);
//			memcpy(addr,&time,6);
//			
//			addr = (void *)(AREA_FILED_START + 0x2000 * field + One_data_long[field] *Fieldx_Info[field].num + 6);
//			memcpy(addr,(uint8_t *)data,strlen(data));
//			
//			
//			Fieldx_Info[field].num ++;
//			 if(((Fieldx_Info[field].num+1)*One_data_long[field])>0x2000)//��ʾд���� +1Ϊ����ǰ��ҳ �������һ��������
//			{
//						 //����ʱ��Ҫ��¼��NAND
//				     addr = (void *)(AREA_FILED_START + 0x2000 * field);
//						 Tp_return = WriteAreaDataclass4567(field, addr, One_data_long[field]*Fieldx_Info[field].num,One_data_long[field]*Fieldx_Info[field].num*Fieldx_Info[field].cycle);
//				     //
//						 Fieldx_Info[field].num =0;
//						 Fieldx_Info[field].cycle = Fieldx_Info[field].cycle  +1;
//						 if((Fieldx_Info[field].cycle  * 0x2000)>=gs_AreaInfo[field].space)
//						 {
//							 Fieldx_Info[field].cycle = 0;
//							 Fieldx_Info[field].loop = Fieldx_Info[field].loop  +1;
//						 }
//						// Clear_sdram(1<<(Tp_field+8));//AREANO
//						   memset((void *)(AREA_FILED_START + 0x2000 * field) , 0x00, 0x2000);
//			}
//			
//			

//			return Tp_return;
//		}
		break;
	case 6:
//		if (len <= gs_AreaInfo[field].size)
//		{
//      One_data_long[field] = gs_AreaInfo[field].size + 6;
//			read_rtc(&time);
//			addr = (void *)(AREA_FILED_START + 0x2000 * field + One_data_long[field] *Fieldx_Info[field].num);
//			
//			//sprintf(addr, "%02x%02x%02x%02x%02x%02x%-99s", time.year, time.month, time.day, time.hour, time.minute, time.second, (char *)data);
//			memcpy(addr,&time,6);
//	   	addr = (void *)(AREA_FILED_START + 0x2000 * field + One_data_long[field] *Fieldx_Info[field].num + 6);
//			memcpy(addr,(uint8_t *)data,strlen(data));
//			
//			
//			Fieldx_Info[field].num ++;
//			 if(((Fieldx_Info[field].num+1)*One_data_long[field])>0x2000)//��ʾд���� +1Ϊ����ǰ��ҳ �������һ��������
//			{
//						 //����ʱ��Ҫ��¼��NAND
//				     addr = (void *)(AREA_FILED_START + 0x2000 * field);
//						 Tp_return = WriteAreaDataclass4567(field, addr, One_data_long[field]*Fieldx_Info[field].num,One_data_long[field]*Fieldx_Info[field].num*Fieldx_Info[field].cycle);
//				     //
//						 Fieldx_Info[field].num =0;
//						 Fieldx_Info[field].cycle = Fieldx_Info[field].cycle  +1;
//						 if((Fieldx_Info[field].cycle  * 0x2000)>=gs_AreaInfo[field].space)
//						 {
//							 Fieldx_Info[field].cycle = 0;
//							 Fieldx_Info[field].loop = Fieldx_Info[field].loop  +1;
//						 }
//						// Clear_sdram(1<<(Tp_field+8));//AREANO
//						   memset((void *)(AREA_FILED_START + 0x2000 * field) , 0x00, 0x2000);
//			}
//			

//			
//			return Tp_return;
//		}
		break;
	case 7:
//		if (len <= gs_AreaInfo[field].size)
//		{

//			One_data_long[field] = gs_AreaInfo[field].size + 6;
//			
//			read_rtc(&time);
//			addr = (void *)(AREA_FILED_START + 0x2000 * field + One_data_long[field] *Fieldx_Info[field].num);
//			//sprintf(addr, "%02x%02x%02x%02x%02x%02x%-99s", time.year, time.month, time.day, time.hour, time.minute, time.second, (char *)data);
//			memcpy(addr,&time,6);
//			//addr = (void *)(addr + 6);
//			//memcpy(addr,(uint8_t *)data,strlen(data));
//			addr = (void *)(AREA_FILED_START + 0x2000 * field + One_data_long[field] *Fieldx_Info[field].num + 6);
//			memcpy(addr,(uint8_t *)data,strlen(data));
//			
//			Fieldx_Info[field].num ++;
//			 if(((Fieldx_Info[field].num+1)*One_data_long[field])>0x2000)//��ʾд���� +1Ϊ����ǰ��ҳ �������һ��������
//			{
//						 //����ʱ��Ҫ��¼��NAND
//				     addr = (void *)(AREA_FILED_START + 0x2000 * field);
//						 Tp_return = WriteAreaDataclass4567(field, addr, One_data_long[field]*Fieldx_Info[field].num,One_data_long[field]*Fieldx_Info[field].num*Fieldx_Info[field].cycle);
//				     //
//						 Fieldx_Info[field].num =0;
//						 Fieldx_Info[field].cycle = Fieldx_Info[field].cycle  +1;
//						 if((Fieldx_Info[field].cycle  * 0x2000)>=gs_AreaInfo[field].space)
//						 {
//							 Fieldx_Info[field].cycle = 0;
//							 Fieldx_Info[field].loop = Fieldx_Info[field].loop  +1;
//						 }
//						// Clear_sdram(1<<(Tp_field+8));//AREANO
//						   memset((void *)(AREA_FILED_START + 0x2000 * field) , 0x00, 0x2000);
//			}
//			
//			return Tp_return;
//		}
		break;
	default:
		break;
	}
	return 0;
}