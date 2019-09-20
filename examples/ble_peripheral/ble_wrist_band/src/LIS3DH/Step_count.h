

#ifndef _Step_count_H_
#define _Step_count_H_


#include <stdio.h> 
//#include "kx022_1020_hw.h"
#include <math.h>
#include "stdint.h"

#define ZEROTIME   20			                                          //0�ĳ���ʱ��
#define CMPMAX     200													  //�Ƚϵ�ʱ�䣨6ms*200=1.2s��
//#define CMPMIN     8		/*�û������ȵ���*/		//�Ƚϵ�ʱ�䣨6ms*10=60ms��
//#define TIME       25													  //�������ʱ��+5������㡱+���ȳ���ʱ��
#define TIME       10
//#define SMIN       50													  //��С�����
#define SMIN       100
//#define SMAX       500												      //�������
#define SMAX       500
//#define KMAX	  	 8		/*�û����ȵ���*/			//��������ֵ


typedef struct
{
	unsigned int StepCount; 			//�Ʋ����Ʋ���
	float Calorie;						//��·��
	float distance;						//·��
	float speed;						//�ٶ�
	unsigned int  ScCurTime;			//���ڱ���Ʋ�ʱ��
	unsigned short int  SCSetUpMenuHValue;	//�Ʋ����˵�����ߵ���ֵ
	unsigned short int  SCSetUpMenuWValue;	//�Ʋ����˵������ص���ֵ
	unsigned short int SCSetUpMenuEValue; 	//�Ʋ����˵������ȵ���ֵ
	unsigned char SCSetUpMenuSValue; 	//�Ʋ����˵��������ȵ���ֵ
} S_STEP_COUNTER;

typedef struct
{
	float Calorie;			    //��·��
	float distance;			    //·��
	float speed;				//�ٶ�
} S_STEP_COUNTER_OUT;


extern S_STEP_COUNTER SStepCounter;
extern volatile  unsigned char timer_on;
extern volatile  unsigned char two_second;	


/*�Ʋ�Ҫ���������Ԫ��*/
typedef struct
{
	uint8_t StepRecordIndex;//��¼����
	uint8_t StepMemoryDay;//��
	uint8_t StepMemoryMonth;//��
	uint8_t StepMemoryYear;//��
	uint16_t StepMemoryTime;//ʱ��(��λ:��)
	uint32_t StepMemoryStep;//����
	uint16_t StepMemoryCalories;//��·��(��λ:KAL)
	uint16_t StepMemoryDistance;//·��(��λ:KM)
}StepData;
extern StepData StepDataSave;


int GetAcceleration(unsigned char *step_data, unsigned char len);
void Speed_and_Calorie(S_STEP_COUNTER *SStepCounter);


#endif




