
#ifndef _TILT_DETECT_H_
#define _TILT_DETECT_H_
#include <math.h>
#include <stdint.h>
#include "g_sensor_motion_lib.h"


#ifdef __cplusplus
extern "C" {
#endif

  
#define   TILT_VERTICAL_ACCEL_POINT_X           0
#define   TILT_VERTICAL_ACCEL_POINT_Y           1        
#define   TILT_VERTICAL_ACCEL_POINT_Z           2
    
#define TILT_VERTICAL_ACCEL_S        TILT_VERTICAL_ACCEL_POINT_X  //chen  

typedef struct
{
	uint16_t  			user_alarm_angle_value; 	//  用户设置的倾斜状态角度.	
	uint16_t  			user_alarm_time_value; 		//  用户设置的倾斜状态持续时间 UNIT = S.
} tilt_user_config_t;


enum
{
	TILE_MODULE_STATUE_IDLE  = 0x00,
	TILE_MODULE_STATUE_OCCUR = 0x01,
} ;


typedef void (*p_tilt_state_callback)(uint32_t input_state);


typedef struct
{
	p_tilt_state_callback 	tilt_state_handler;
} TILT_MODULE_INIT;


void	 tilt_module_init(TILT_MODULE_INIT *input);//init.
bool     tilt_module_config_angle_alarm(tilt_user_config_t *input);
void     tilt_moudle_gsensor_data_handler(vector3_t *input, bool temp_device_is_sleep_flag);
bool	 tilt_get_switch(void);
void	 tilt_set_switch(bool input);


#ifdef __cplusplus
}
#endif /* end of __cplusplus */
#endif

