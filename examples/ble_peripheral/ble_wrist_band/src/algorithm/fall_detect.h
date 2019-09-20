
#ifndef _FALL_DETECT_H_
#define _FALL_DETECT_H_
#include <math.h>
#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef void (*fall_state_callback)(uint32_t input);


enum
{
	FALL_DOWN_OCCUR = 0,
	FALL_DOWN_IDLE,
};


typedef struct
{
	fall_state_callback 	state_handler;
} FALL_MODULE_INIT;


/**
 * INVOKE NOTICE:
 * static uint32_t gsensor_timestamp_value;
 * gsensor_timestamp_value += Tperiod; Tperiod = 1000 / sample_rate;
 * fall_detector_handler(src_gsensor_data, gsensor_timestamp_value).
 */
//void fall_detector_handler(float gsensor_data, uint32_t gsensor_timestamp_value);
void fall_module_init(FALL_MODULE_INIT *input, uint8_t level); // 检测等级1 - 10 等级越高检测阈值越低
void fall_detector_state_handler(float input_composite_data, bool temp_device_is_sleep_flag);
void fall_set_switch(bool input);
bool fall_get_switch(void);


#ifdef __cplusplus
}
#endif /* end of __cplusplus */
#endif

