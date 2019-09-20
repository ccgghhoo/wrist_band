
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "m_step.h"
#include "stdbool.h"

/************************* SportAlgo ***********************/
#define SPORT_HEALTH_SPORT_CFG     1 

#if      SPORT_HEALTH_SPORT_CFG

#define  SPORT_INIT_THR_VALUE            1
#define  SPORT_STATE_THR_VALUE           2
#define  SPORT_DATA_BUFF_SIZE            4

#define  SPORT_STEP_UP_COUNTER           2
#define  SPORT_STEP_UP_DATA             26

#define  SPORT_STEP_ENTER_VALUE          5

#define  SPORT_STEP_MIN_TIME           200
#define  SPORT_RUN_MAX_TIME            366
#define  SPORT_WALK_MIN_TIME           400
#define  SPORT_STEP_DIFF1_TIME        1200
#define  SPORT_STEP_DIFF2_TIME        1600

#define  GSENSOR_VOLLEY_OVER_VALUE      25
#define  GSENSOR_VOLLEY_CHANGE_VALUE    30
#define  GSENSOR_VOLLEY_CHANGE_COUNTER  15
#define  GSENSOR_COMPOSITE_BUFF_SIZE     8

typedef struct
{
	uint8_t sport_peak_volley_over_flag:1;
	uint8_t sport_state_last_flag      :1;
	uint8_t sport_state_up_flag        :1;
	uint8_t sport_mode_run_flag        :1;
	uint8_t sport_state_reserve        :4;
}sport_state_flag_t;


static sport_state_flag_t   sport_state_flag;static uint8_t  gsensor_composite_data_cur = 0;
static uint16_t gsensor_continue_up_counter       = 0;
static uint16_t gsensor_state_up_counter          = 0;

static uint8_t  sport_peak_value                  = 0;
static uint8_t  sport_valley_value                = 0;
  
static uint16_t sport_state_step_counter          = 0;
static uint8_t  gsensor_peak_volley_counter       = 0;
static uint32_t sport_time_stamp                  = 0;
static uint32_t sport_peak_last_time              = 0;
static uint32_t sport_peak_this_time              = 0;
static uint32_t sport_peak_new_time               = 0;

static uint8_t  sport_threshold_value             = SPORT_STATE_THR_VALUE;
static uint8_t  sport_diff_data_buffer[SPORT_DATA_BUFF_SIZE];
static uint8_t  sport_diff_data_index             = 0;

static uint8_t  sport_state_cur                   = 0;
static uint32_t sport_step_counter                = 0;
static uint8_t  step_mode_counter                 = 0;

static uint8_t  gsensor_composite_buffer[GSENSOR_COMPOSITE_BUFF_SIZE];
static uint8_t  gsensor_composite_index           = 0;
static uint8_t  gsensor_composite_data_size       = GSENSOR_COMPOSITE_BUFF_SIZE;
static uint8_t  gsensor_composite_data_counter    = 0;

static uint8_t  gsensor_max_value                 = 0;
static uint8_t  gsensor_min_value                 = 0;


static uint32_t app_algo_sqrt_hander(uint32_t data_value)
{
	uint32_t op  = data_value;
	uint32_t res = 0;
	uint32_t one = 1uL << 30;
	
	while (one > op)	 one >>= 2;
	while (one != 0) 
	{
		if (op >= res + one) 
		{
			op = op - (res + one);
			res = res +  2 * one;
		}
		res >>= 1;
		one >>= 2;
	} 	
	return res;
}


void  SmartWear_SportHealth_Sport_Algo_Init(void)
{
	memset(&sport_state_flag,0,sizeof(sport_state_flag));
}


static bool  app_sport_detect_peak_algo_handler(uint8_t gsensor_composite_data_new)
{
	bool sport_peak_flag  = false;
	sport_state_flag.sport_state_last_flag = sport_state_flag.sport_state_up_flag;
	if( gsensor_composite_data_new > gsensor_composite_data_cur )
	{
		sport_state_flag.sport_state_up_flag = true;
		gsensor_continue_up_counter ++;
	}
	else if( gsensor_composite_data_new < gsensor_composite_data_cur )
	{
		sport_state_flag.sport_state_up_flag  = false;
		gsensor_state_up_counter    = gsensor_continue_up_counter;
		gsensor_continue_up_counter = 0x00;		
	}			
	if( ( sport_state_flag.sport_state_up_flag == false && sport_state_flag.sport_state_last_flag == true ) &&
	( gsensor_state_up_counter >= SPORT_STEP_UP_COUNTER || gsensor_composite_data_cur >= SPORT_STEP_UP_DATA ) )
	{
		sport_peak_value = gsensor_composite_data_cur;
		sport_peak_flag  = true;
	}
	else if( sport_state_flag.sport_state_up_flag == true && sport_state_flag.sport_state_last_flag == false )
	{
		sport_valley_value = gsensor_composite_data_cur;
	}   
	return sport_peak_flag;
}


static uint8_t  app_sport_threshold_value_get(uint8_t sport_diff_value)
{
	uint8_t threshold_value = 2;

	if( sport_diff_value >= 30 )      threshold_value = 8;
	else if( sport_diff_value >= 20 ) threshold_value = 5;
	else if( sport_diff_value >= 8  ) threshold_value = 3;
	return threshold_value;
}


static uint8_t  app_sport_threshold_with_peak_valley_handler(uint8_t sport_diff_value)
{
	uint8_t sport_threadhold_data = sport_diff_value;
	if( sport_diff_data_index < SPORT_DATA_BUFF_SIZE ) sport_diff_data_buffer[sport_diff_data_index++] = sport_diff_value;
	else
	{
		uint16_t sport_data = 0x00;
		for(int index = 0; index < SPORT_DATA_BUFF_SIZE; index ++) sport_data += sport_diff_data_buffer[index];
		sport_threadhold_data = (uint8_t)(sport_data/SPORT_DATA_BUFF_SIZE);
		for(int index = 1; index < SPORT_DATA_BUFF_SIZE; index ++) sport_diff_data_buffer[index-1] = sport_diff_data_buffer[index]; 
		sport_diff_data_buffer[SPORT_DATA_BUFF_SIZE-1] = sport_diff_value;
	}   
	return  app_sport_threshold_value_get(sport_threadhold_data);
}


void  SmartWear_SportHealth_Sport_Step_Reset(void)
{
	sport_step_counter = 0x00;
}


static void  app_sport_detect_algo_handler(uint8_t gsensor_composite_data)
{
	if( gsensor_composite_data_cur )
	{
		bool sport_state_reset_flag = false;
		if( app_sport_detect_peak_algo_handler(gsensor_composite_data) == true )
		{
			sport_peak_last_time  = sport_peak_this_time;
			sport_peak_new_time   = sport_time_stamp;
			if( ( (sport_peak_new_time - sport_peak_last_time) >= SPORT_STEP_MIN_TIME ) &&
			    (sport_peak_value - sport_valley_value) >= sport_threshold_value )
			{
				if( ++sport_state_step_counter >= 10 )
				{
					if( sport_state_cur == 0 )
					{
						sport_state_cur       = 1;
						sport_step_counter   += 10;
					}
					else sport_step_counter ++;
					SmartWear_SportHealth_Sport_Step_Update_Callback(sport_step_counter);
				}	
				if( sport_state_step_counter >= SPORT_STEP_ENTER_VALUE )
				{
					if( ( sport_peak_new_time - sport_peak_last_time ) < SPORT_RUN_MAX_TIME )
					{
						sport_state_flag.sport_mode_run_flag  = true;
						step_mode_counter = 0;
					}
					else if( sport_state_flag.sport_mode_run_flag == true && ( sport_peak_new_time - sport_peak_last_time ) >= SPORT_WALK_MIN_TIME )
					{
						if( ++step_mode_counter >= 3 ) sport_state_flag.sport_mode_run_flag = false;
					}
					if( sport_state_flag.sport_mode_run_flag == true )
					{
						SmartWear_SportHealth_Sport_State_Update_Callback(SPORT_STATE_RUN);
					}
					else SmartWear_SportHealth_Sport_State_Update_Callback(SPORT_STATE_WALK); 											
				}
				sport_peak_this_time = sport_peak_new_time;
			}
			if( ( sport_peak_new_time - sport_peak_last_time ) >= SPORT_STEP_MIN_TIME  &&
			( sport_peak_value    - sport_valley_value   ) >= SPORT_INIT_THR_VALUE )
			{
				sport_threshold_value = app_sport_threshold_with_peak_valley_handler(sport_peak_value-sport_valley_value);
			}   
			if( ( sport_peak_new_time - sport_peak_last_time ) >= SPORT_STEP_DIFF1_TIME ) sport_state_reset_flag = true;
		}
		if( ( sport_time_stamp - sport_peak_this_time ) >= SPORT_STEP_DIFF2_TIME )	sport_state_reset_flag = true;
		if( sport_state_step_counter < SPORT_STEP_ENTER_VALUE ) SmartWear_SportHealth_Sport_State_Update_Callback(SPORT_STATE_NONE);
		if( sport_state_reset_flag == true )
		{
			sport_state_step_counter      = 0;
			sport_state_cur      	      = 0;
			step_mode_counter             = 0;								
			sport_state_flag.sport_mode_run_flag         = false;
			sport_state_flag.sport_peak_volley_over_flag = false;
		}
	}   
	gsensor_composite_data_cur =  gsensor_composite_data;
}

static void  app_gsensor_peak_volley_state_handler(uint8_t gesensor_composite_data)
{
	if( sport_state_flag.sport_peak_volley_over_flag == false )
	{
		if( sport_valley_value >= GSENSOR_VOLLEY_OVER_VALUE )
		{
			sport_state_flag.sport_peak_volley_over_flag = true;
			gsensor_composite_data_size   = 1;
			gsensor_peak_volley_counter   = 0;
		}			
	}
	else
	{
		if( gesensor_composite_data < GSENSOR_VOLLEY_CHANGE_VALUE )
		{
			if( ++gsensor_peak_volley_counter >= GSENSOR_VOLLEY_CHANGE_COUNTER )
			{
				gsensor_composite_data_size    = GSENSOR_COMPOSITE_BUFF_SIZE;
				gsensor_composite_index        = 0;
				gsensor_composite_data_counter = 0;
				sport_state_flag.sport_peak_volley_over_flag = false;
			} 
		}
		else
		{
			gsensor_peak_volley_counter   = 0;
		}
	}
}

static void  app_gsensor_maxmin_value_get(uint8_t gsensor_composite_data)
{
	if( gsensor_max_value < gsensor_composite_data ) gsensor_max_value = gsensor_composite_data;
	else if( gsensor_min_value > gsensor_composite_data ) gsensor_min_value = gsensor_composite_data;
}

uint8_t gsensor_composite_data = 0;
void  SmartWear_SportHealth_Sport_Algo_Handler(gsensor_data_t gsensor_data,uint32_t time_stamp_value)
{
	uint32_t gsensor_composite_value =
			      (uint32_t)gsensor_data.mpss[0] * (uint32_t)gsensor_data.mpss[0]+
			      (uint32_t)gsensor_data.mpss[1] * (uint32_t)gsensor_data.mpss[1]+
			      (uint32_t)gsensor_data.mpss[2] * (uint32_t)gsensor_data.mpss[2];
	gsensor_composite_data = (uint8_t)app_algo_sqrt_hander(gsensor_composite_value);
	app_gsensor_peak_volley_state_handler(gsensor_composite_data);
	app_gsensor_maxmin_value_get(gsensor_composite_data);	    
	if( gsensor_composite_data_size == 1 )  gsensor_composite_data = gsensor_composite_data;
	else
	{
		gsensor_composite_buffer[gsensor_composite_index] = gsensor_composite_data;
		if( ++gsensor_composite_index >= gsensor_composite_data_size ) gsensor_composite_index = 0;
		if( gsensor_composite_data_counter < gsensor_composite_data_size ) gsensor_composite_data_counter ++;
		uint16_t sum = 0;
		for(int index = 0; index < gsensor_composite_data_counter; index ++)
		{
			sum += gsensor_composite_buffer[index];
		}  
		gsensor_composite_data = (uint8_t)(sum / gsensor_composite_data_counter);			
	}			
	sport_time_stamp  =  time_stamp_value;			
	app_sport_detect_algo_handler(gsensor_composite_data);			
}  


#endif


