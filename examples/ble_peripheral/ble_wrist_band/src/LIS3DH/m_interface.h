

#ifndef __M_INTEFACE__h
#define __M_INTEFACE__h   



#include <stdint.h>
#include <stdbool.h>
#include <string.h>


/********************************************************************************************************************/
#define M_DRV_MC_UTL_AXIS_X      0
#define M_DRV_MC_UTL_AXIS_Y      1
#define M_DRV_MC_UTL_AXIS_Z      2
#define M_DRV_MC_UTL_AXES_NUM    3

#define FIFO_DEEP           32

typedef struct{
    int16_t X; 
    int16_t Y;
    int16_t Z;
}AxesRaw_t; 

enum
{
	CLICK_EVENT,
	TP_PRESS_SHORT,
};

typedef struct 
{
	short RawData[FIFO_DEEP][M_DRV_MC_UTL_AXES_NUM];
	short DataLen; 
	uint32_t TimeStamp[FIFO_DEEP]; //ms 
}   AccData;

typedef struct
{
	signed char      bSign[M_DRV_MC_UTL_AXES_NUM];
	unsigned char    bMap[M_DRV_MC_UTL_AXES_NUM];
}   S_M_DRV_MC_UTIL_OrientationReMap;

typedef enum
{
	E_M_DRV_UTIL_ORIENTATION_TOP_LEFT_DOWN = 0,
	E_M_DRV_UTIL_ORIENTATION_TOP_RIGHT_DOWN,
	E_M_DRV_UTIL_ORIENTATION_TOP_RIGHT_UP,
	E_M_DRV_UTIL_ORIENTATION_TOP_LEFT_UP,
	E_M_DRV_UTIL_ORIENTATION_BOTTOM_LEFT_DOWN,
	E_M_DRV_UTIL_ORIENTATION_BOTTOM_RIGHT_DOWN,
	E_M_DRV_UTIL_ORIENTATION_BOTTOM_RIGHT_UP,
	E_M_DRV_UTIL_ORIENTATION_BOTTOM_LEFT_UP,
	E_M_DRV_UTIL_ORIENTATION_TOTAL_CONFIG,
}   E_M_DRV_UTIL_OrientationConfig;
/********************************************************************************************************************/


void m_interface_module_before_run_init(void);
void m_interface_input_gsensor_source_data(AxesRaw_t xdata);


#endif




