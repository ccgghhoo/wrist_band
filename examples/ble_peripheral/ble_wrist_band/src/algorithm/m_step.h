

#ifndef __SMARTWEAR_SPORTHEALTH_SPORT_H__
#define __SMARTWEAR_SPORTHEALTH_SPORT_H__

#include "stdint.h"

typedef struct
{
	float 	mpss[3];
} gsensor_data_t;

typedef enum
{
	SPORT_STATE_NONE,   //
	SPORT_STATE_WALK,   //
	SPORT_STATE_RUN,    //
}SPORT_STATE_TYPE;


void  SmartWear_SportHealth_Sport_Algo_Init(void); // init
void  SmartWear_SportHealth_Sport_Algo_Handler(gsensor_data_t gsensor_data,uint32_t time_stamp_value); // input
void  SmartWear_SportHealth_Sport_Step_Reset(void); // Step Reset
void  SmartWear_SportHealth_Sport_Step_Update_Callback(uint32_t sport_step);  // output
void  SmartWear_SportHealth_Sport_State_Update_Callback(uint8_t sport_state); // output


#endif



