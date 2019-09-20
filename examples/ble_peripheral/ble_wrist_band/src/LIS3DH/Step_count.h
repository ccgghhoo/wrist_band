

#ifndef _Step_count_H_
#define _Step_count_H_


#include <stdio.h> 
//#include "kx022_1020_hw.h"
#include <math.h>
#include "stdint.h"

#define ZEROTIME   20			                                          //0的持续时间
#define CMPMAX     200													  //比较的时间（6ms*200=1.2s）
//#define CMPMIN     8		/*用户灵敏度调节*/		//比较的时间（6ms*10=60ms）
//#define TIME       25													  //波峰持续时间+5个“零点”+波谷持续时间
#define TIME       10
//#define SMIN       50													  //最小的面积
#define SMIN       100
//#define SMAX       500												      //最大的面积
#define SMAX       500
//#define KMAX	  	 8		/*用户力度调节*/			//波峰的最大值


typedef struct
{
	unsigned int StepCount; 			//计步器计步数
	float Calorie;						//卡路里
	float distance;						//路程
	float speed;						//速度
	unsigned int  ScCurTime;			//用于保存计步时间
	unsigned short int  SCSetUpMenuHValue;	//计步器菜单下身高的数值
	unsigned short int  SCSetUpMenuWValue;	//计步器菜单下体重的数值
	unsigned short int SCSetUpMenuEValue; 	//计步器菜单下力度的数值
	unsigned char SCSetUpMenuSValue; 	//计步器菜单下灵敏度的数值
} S_STEP_COUNTER;

typedef struct
{
	float Calorie;			    //卡路里
	float distance;			    //路程
	float speed;				//速度
} S_STEP_COUNTER_OUT;


extern S_STEP_COUNTER SStepCounter;
extern volatile  unsigned char timer_on;
extern volatile  unsigned char two_second;	


/*计步要保存的数据元素*/
typedef struct
{
	uint8_t StepRecordIndex;//记录索引
	uint8_t StepMemoryDay;//日
	uint8_t StepMemoryMonth;//月
	uint8_t StepMemoryYear;//年
	uint16_t StepMemoryTime;//时间(单位:分)
	uint32_t StepMemoryStep;//步数
	uint16_t StepMemoryCalories;//卡路里(单位:KAL)
	uint16_t StepMemoryDistance;//路程(单位:KM)
}StepData;
extern StepData StepDataSave;


int GetAcceleration(unsigned char *step_data, unsigned char len);
void Speed_and_Calorie(S_STEP_COUNTER *SStepCounter);


#endif




