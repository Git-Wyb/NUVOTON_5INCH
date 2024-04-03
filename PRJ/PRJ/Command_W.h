

#ifndef COMMAND_W_H
#define COMMAND_W_H

typedef enum DATACLASS5INCH
{ 
	BASE_DATA_CLASS,          //0 基本参数 //BASE_DATA_CLASS,          //
	ACTION_NUMBER_DATA_CLASS,   //1 动作次数//ACTION_NUMBER_DATA_CLASS,  //
	ACTION_TIMERS_DATA_CLASS,   //2 动作时间//ACTION_TIMERS_DATA_CLASS,  // //2 
	UNIT_PARA_DATA_CLASS,       //3 单元参数//UNIT_PARA_DATA_CLASS,//
	TEST_FINAL_DATA_DATA_CLASS,  //4 测定值最终数据//TEST_FINAL_DATA_DATA_CLASS,//
	TEST_ALL_DATA_DATA_CLASS,    //5 测定值全部数据//TEST_ALL_DATA_DATA_CLASS,    //5 
	ERROR_RECORD_DATACLASS,  //6 错误履历//ERROR_RECORD_DATACLASS,  //
	ACTION_RECORD_DATACLASS,    //7 动作履历//ACTION_RECORD_DATACLASS,    //7  
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
	int    num; //保存分数0~239
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
	int    num; //保存分数
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







