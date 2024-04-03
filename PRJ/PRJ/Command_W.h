

#ifndef COMMAND_W_H
#define COMMAND_W_H

typedef enum DATACLASS5INCH
{ 
	BASE_DATA_CLASS,          //0 �������� //BASE_DATA_CLASS,          //
	ACTION_NUMBER_DATA_CLASS,   //1 ��������//ACTION_NUMBER_DATA_CLASS,  //
	ACTION_TIMERS_DATA_CLASS,   //2 ����ʱ��//ACTION_TIMERS_DATA_CLASS,  // //2 
	UNIT_PARA_DATA_CLASS,       //3 ��Ԫ����//UNIT_PARA_DATA_CLASS,//
	TEST_FINAL_DATA_DATA_CLASS,  //4 �ⶨֵ��������//TEST_FINAL_DATA_DATA_CLASS,//
	TEST_ALL_DATA_DATA_CLASS,    //5 �ⶨֵȫ������//TEST_ALL_DATA_DATA_CLASS,    //5 
	ERROR_RECORD_DATACLASS,  //6 ��������//ERROR_RECORD_DATACLASS,  //
	ACTION_RECORD_DATACLASS,    //7 ��������//ACTION_RECORD_DATACLASS,    //7  
	//BLANK_DATACLASS
}DATACLASS_TYPE5INCH; 


typedef struct
{
	unsigned char data;
}FieldArea3;

typedef struct
{
	FieldArea3 areaData[1024];
}FieldArea3All_cyw;



typedef struct
{
	unsigned short data;
}FieldArea1;
typedef struct
{
	short year;
	short month;
	int    num; //�������0~239
	FieldArea1 areaData[512];
}FieldArea1All_cyw;

typedef struct
{
	unsigned int data;
}FieldArea2;

typedef struct
{
	short year;
	short month;
	int    num; //�������
	FieldArea2 areaData[512];
}FieldArea2All_cyw;

typedef struct
{

	unsigned char data;
}FieldArea4;
typedef struct
{
	char year;
	char month;
	char day;
	char hour;
	char min;
	char sec;
	char rev[2]; //rev[0] vbat ad
	FieldArea4 areaData[100];
}FieldArea4All_cyw;



//void SaveAreaData(uint8_t type , uint8_t x_field);
int AreaWriteCmd(char cmd, int field, int idx,void *data,int len);
#endif







