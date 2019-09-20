
#include "m_interface.h"
#include "m_step.h"
#include "gsflag.h"
#include "fall_detect.h"


#if 0
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#define MD_LOG				    NRF_LOG_DEBUG
#else
#define MD_LOG(...)
#endif 

static volatile AccData GensorData;
const S_M_DRV_MC_UTIL_OrientationReMap
				g_MDrvUtilOrientationReMap[E_M_DRV_UTIL_ORIENTATION_TOTAL_CONFIG] =
				{
					{{  1,  1,  1 }, { M_DRV_MC_UTL_AXIS_X, M_DRV_MC_UTL_AXIS_Y, M_DRV_MC_UTL_AXIS_Z }},
					{{ -1,  1,  1 }, { M_DRV_MC_UTL_AXIS_Y, M_DRV_MC_UTL_AXIS_X, M_DRV_MC_UTL_AXIS_Z }},
					{{ -1, -1,  1 }, { M_DRV_MC_UTL_AXIS_X, M_DRV_MC_UTL_AXIS_Y, M_DRV_MC_UTL_AXIS_Z }},
					{{  1, -1,  1 }, { M_DRV_MC_UTL_AXIS_Y, M_DRV_MC_UTL_AXIS_X, M_DRV_MC_UTL_AXIS_Z }},

					{{ -1,  1, -1 }, { M_DRV_MC_UTL_AXIS_X, M_DRV_MC_UTL_AXIS_Y, M_DRV_MC_UTL_AXIS_Z }},
					{{  1,  1, -1 }, { M_DRV_MC_UTL_AXIS_Y, M_DRV_MC_UTL_AXIS_X, M_DRV_MC_UTL_AXIS_Z }},
					{{  1, -1, -1 }, { M_DRV_MC_UTL_AXIS_X, M_DRV_MC_UTL_AXIS_Y, M_DRV_MC_UTL_AXIS_Z }},
					{{ -1, -1, -1 }, { M_DRV_MC_UTL_AXIS_Y, M_DRV_MC_UTL_AXIS_X, M_DRV_MC_UTL_AXIS_Z }},
				};

const S_M_DRV_MC_UTIL_OrientationReMap *_ptOrienMap = &g_MDrvUtilOrientationReMap[E_M_DRV_UTIL_ORIENTATION_TOP_RIGHT_UP];
volatile float accMpss[3] ={0}; 


/**
 * @Description - judge is sleep now. by ......2018.4.12
 *
 * @param[] - none.
 *
 * @return - return 1 sleep  return 0 sport.
 */
bool IsKeeprelative(short *data)
{
	static    short   accMpssbuf[3]; 
	static    uint8_t times =0 ;	
	if (
		(((accMpssbuf[0]-60)< data[0])&&(data[0]<(accMpssbuf[0]+60)))&&
	    (((accMpssbuf[1]-60)< data[1])&&(data[1]<(accMpssbuf[1]+60)))&&
	    (((accMpssbuf[2]-60)< data[2])&&(data[2]<(accMpssbuf[2]+60))))
	{
		times ++;
		if (times >= 30)
		{
			times = 30 ;

			for(uint8_t i = 0 ; i < 3;i++)
			{
				accMpssbuf[i] = data[i];
			}
			// now status if sleep
			//if(GetSleepflag() == 1)
			//{
			//	return 0 ;
			//}
			return 1;
		}
		// memcpy(&accMpssbuf[0],&data[0],3);
	}
	else
	{
		times = 0;
		for (uint8_t i = 0 ; i < 3;i++)
		{
			accMpssbuf[i] = data[i];
		}
		// memcpy(&accMpssbuf[0],&data[0],sizeof(accMpssbuf)/sizeof(uint8_t));
	}
	return 0 ;
}


/**
 * @Description - the sport step of the people, by ......2018.4.12
 *                when have step occur, invole this function.
 *          
 * @param[] - none  .
 *
 * @return - none.
 */
void  SmartWear_SportHealth_Sport_Step_Update_Callback(uint32_t sport_step)
{
//	if(stepCount != Get_User_Sport_steps())
	{
		loc_set_step_count(sport_step);
	}
}


/**
 * @Description - the sport state of the people, by ......2018.4.12
 *                when have step occur, invole this function.
 *          
 * @param[] - none  .
 *
 * @return - none.
 */
void  SmartWear_SportHealth_Sport_State_Update_Callback(uint8_t sport_state)
{
	if (SPORT_STATE_RUN == sport_state)
	{	
		MD_LOG("\r\n[interface] run now \r\n");
	}
	else if (SPORT_STATE_WALK == sport_state)
	{	
		MD_LOG("\r\n[interface] walk now \r\n");
	}
        
        if ( SPORT_STATE_NONE != sport_state )
        {
    
        }
}


/**
 * @Description - input the gsensor source data, by ......2018.4.12
 *                the important :+/-4g, 12bit  25Hz sample rate   
 *
 * @param[] - none  .
 *
 * @return - none.
 */
#define   XXXX    0
void m_interface_input_gsensor_source_data(AxesRaw_t xdata)
{
	uint8_t stepflag = 1;//the status of now sport status.
	//unsigned char buf[6];
	static uint8_t fss_len = 0; 
	static AxesRaw_t raw_buffer[FIFO_DEEP + 2];
	raw_buffer[fss_len++] = xdata;
	if (fss_len >= FIFO_DEEP)
	{
		fss_len = 0;
		for (unsigned char i = 0; i < FIFO_DEEP; ++ i)
		{
			GensorData.RawData[i][M_DRV_MC_UTL_AXIS_X] =  	raw_buffer[i].X >> XXXX;//note! the sign of high it not change.
			GensorData.RawData[i][M_DRV_MC_UTL_AXIS_Y] =    raw_buffer[i].Y >> XXXX;
			GensorData.RawData[i][M_DRV_MC_UTL_AXIS_Z] =    raw_buffer[i].Z >> XXXX;
			if(IsKeeprelative((short *)&GensorData.RawData[i][0]) == 1)
			{
				stepflag = 0; //sport
			}
			else
			{
				stepflag = 1;  //sleep
			}
			//sign and map
			GensorData.RawData[i][M_DRV_MC_UTL_AXIS_X] = _ptOrienMap->bSign[M_DRV_MC_UTL_AXIS_X] * GensorData.RawData[i][_ptOrienMap->bMap[M_DRV_MC_UTL_AXIS_X]];
			GensorData.RawData[i][M_DRV_MC_UTL_AXIS_Y] = _ptOrienMap->bSign[M_DRV_MC_UTL_AXIS_Y] * GensorData.RawData[i][_ptOrienMap->bMap[M_DRV_MC_UTL_AXIS_Y]];
			GensorData.RawData[i][M_DRV_MC_UTL_AXIS_Z] = _ptOrienMap->bSign[M_DRV_MC_UTL_AXIS_Z] * GensorData.RawData[i][_ptOrienMap->bMap[M_DRV_MC_UTL_AXIS_Z]];
			GensorData.RawData[i][M_DRV_MC_UTL_AXIS_X] = -GensorData.RawData[i][M_DRV_MC_UTL_AXIS_X];
			GensorData.RawData[i][M_DRV_MC_UTL_AXIS_Y] = -GensorData.RawData[i][M_DRV_MC_UTL_AXIS_Y];
		}
		for (unsigned char i = 0; i < FIFO_DEEP; ++ i)
		{
			accMpss[0] =  (float)GensorData.RawData[i][M_DRV_MC_UTL_AXIS_X] * 9.807f/512.0f;
			accMpss[1] =  (float)GensorData.RawData[i][M_DRV_MC_UTL_AXIS_Y] * 9.807f/512.0f;
			accMpss[2] =  (float)GensorData.RawData[i][M_DRV_MC_UTL_AXIS_Z] * 9.807f/512.0f;
			gsensor_data_t  temp;
			temp.mpss[0] = accMpss[0];
			temp.mpss[1] = accMpss[1];
			temp.mpss[2] = accMpss[2];
			if(stepflag == 1)
			{
				//step count.
				static uint32_t        GSENSOR_TimeStamp_Counter = 0;
				GSENSOR_TimeStamp_Counter = GSENSOR_TimeStamp_Counter+36;
				SmartWear_SportHealth_Sport_Algo_Handler(temp,GSENSOR_TimeStamp_Counter);
			}
		}
	}
}


/**
 * @Description - init all Algorithm module. by ......2018.4.12
 *
 * @param[] - none.
 *
 * @return - return 1 sleep  return 0 sport.
 */
void m_interface_module_before_run_init(void)
{
	//the sport module of people.
	SmartWear_SportHealth_Sport_Algo_Init();
	SmartWear_SportHealth_Sport_Step_Reset();
}


/********************************************************************************************************************/

