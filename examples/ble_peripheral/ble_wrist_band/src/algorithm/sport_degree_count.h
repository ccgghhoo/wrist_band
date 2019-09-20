#ifndef _SPORT_DEGREE_COUNT_
#define _SPORT_DEGREE_COUNT_

#include <stdint.h>
#include <stdbool.h>
#include "m_interface.h"
#include <stdio.h>
#include "g_sensor_motion_lib.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__CC_ARM)
    #pragma push
    #pragma anon_unions
#elif defined(__ICCARM__)
    #pragma language=extended
#elif defined(__GNUC__)
    // anonymous unions are enabled by default
#endif

typedef struct
{
	uint32_t utc;
	uint32_t gsensor_point_value;
} md_gsensor_degree_t;

extern md_gsensor_degree_t 	 m_new_degree_value;

void md_module_input_gsensor_raw_handle(vector3_t *p_input_gsensor_raw);
void md_module_period_one_second(void);
void md_module_period_save(void);    
    
#if defined(__CC_ARM)
    #pragma pop
#elif defined(__ICCARM__)
    // leave anonymous unions enabled
#elif defined(__GNUC__)
    // anonymous unions are enabled by default
#endif

#ifdef __cplusplus
}
#endif

#endif
