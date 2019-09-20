

#include <stdint.h>
#include <stdio.h>
#include "fall_detect.h"
#include "m_interface.h"
//#include "log.h"
#include "g_sensor_motion_lib.h"
//#include "lis3dh.h"

#if 1
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#define FLprintf				        NRF_LOG_INFO
#else
#define FLprintf
#endif
          

#define __MOTION_SENSOR_FALL_DETECT_SUPPORT__
#define SF_MODULE_LOG(...)
#define SENSOR_SAMPLE_RATE  ( SAMPLE_FREQ ) 


#ifdef __MOTION_SENSOR_FALL_DETECT_SUPPORT__

#define FALL_SAMPLE_RATE								( SENSOR_SAMPLE_RATE )
#define FALL_ONE_POINT_TIME_MS_SIZE						( 1000 / FALL_SAMPLE_RATE )
#define FALL_TIME_MS(X)   								( (X) / (FALL_ONE_POINT_TIME_MS_SIZE) )


#define FALL_STATIC_VALUE								( 1.0 )
#define FALL_STEP_ONE									( 0.3 )
#define FALL_STEP_TWO									( 1.8 )
#define FALL_STEP_CHECK_VALUE							        ( 0.4 )
#define FALL_DETECT_LEVEL   							        ( 9 ) 	// 检测等级1 - 9 等级越高检测阈值越低.

enum
{
	FALL_DETECTOR_IDLE,
//        FALL_WAIT_FOR_LOST_COMPLETED,
	FALL_DETECTOR_IMPACT,
	FALL_DETECTOR_STATIC,
    FALL_WAIT_FOR_SOME_TIME,
};

static float	m_first_value							= 0;
static uint32_t	m_active_counter						= 0;
//static uint32_t m_waitlost_counter                = 0;
static uint32_t	m_timeout_impact_counter				= 0;
static uint32_t	m_static_impact_counter					= 0;
//static uint32_t m_wait_for_counter                = 0;
static uint32_t	m_impact_counter						= 0;
//static uint32_t	m_noimpact_counter					= 0;
static uint32_t	m_static_counter						= 0;
static FALL_MODULE_INIT 							    m_fall_module;
static bool     m_FallDownflag 							= false;
static uint8_t	m_fall_detector_state				    = FALL_DETECTOR_IDLE;


const static float      m_lost_gravity_threshold_value[FALL_DETECT_LEVEL] = { 0.3,  0.33, 0.37, 0.41, 0.45, 0.49, 0.53, 0.57, 0.60 };
const static uint16_t   m_lost_gravity_time[FALL_DETECT_LEVEL]		      = { 345,  320,  295,  270,  245,  220,  195,  170 , 145 };
const static float      m_static_threshold_value[FALL_DETECT_LEVEL]       = { 0.13, 0.15, 0.20, 0.25, 0.30, 0.35, 0.40, 0.45, 0.50 };

typedef struct
{
  	float    lost_gravity_threshold_value;                  // 失重阈值.    
	uint16_t lost_gravity_time;			        // 跌落时间阈值.
        float    static_threshold_value;
} fall_detector_level_t;

static fall_detector_level_t m_fall_detector_level = 
{
    .lost_gravity_threshold_value       = 0.60,  
	.lost_gravity_time 	                = 145,
	.static_threshold_value             = 0.50,
};


void fall_detector_state_handler(float input_composite_data, bool temp_device_is_sleep_flag)  // range value +- 4G, 100Hz, 12bit.
{
//        static float max_impact_value = 0;

	switch ( m_fall_detector_state )
	{
		case FALL_DETECTOR_IDLE:
		{
			if ( input_composite_data < m_fall_detector_level.lost_gravity_threshold_value)
			{
                FLprintf("m_active_counter =%d \r\n" ,m_active_counter);
                
				if ( ++m_active_counter >= FALL_TIME_MS( m_fall_detector_level.lost_gravity_time ) )
				{
                   FLprintf("[Fall detect] counter = %d,  set_time = %d\r\n", m_active_counter, m_fall_detector_level.lost_gravity_time);
					m_active_counter 	= 0;
					m_impact_counter 	= 0;
					m_timeout_impact_counter= 0;    //碰撞超时.
                    m_static_impact_counter = 0;
                    //m_waitlost_counter      = 0;
					//m_noimpact_counter 	= 0;
                    m_static_counter        = 0;
                    // m_wait_for_counter      = 0;
					m_first_value		= input_composite_data;
                    //max_impact_value        = input_composite_data;
					m_fall_detector_state = FALL_DETECTOR_IMPACT;
					FLprintf("Go to FALL_DETECTOR_IMPACT!\r\n");
				}
			}
			else
			{
				m_active_counter = 0;
			}
		} break;

		case FALL_DETECTOR_IMPACT:
		{
			if ( input_composite_data >= 2.0 )
			{
                                if ( ++m_impact_counter >= 1 )
                                {
                                        m_impact_counter	= 0;
                                        m_static_counter	= 0;
                                        m_fall_detector_state	= FALL_DETECTOR_STATIC;
                                        FLprintf("Go to FALL_DETECTOR_STATIC.\r\n");
                                }
                                break;
                        }
                        else
			{
                        //       m_impact_counter = 0;
			}
                  //      if ( max_impact_value < input_composite_data ) max_impact_value = input_composite_data;
			if ( ++m_timeout_impact_counter > FALL_TIME_MS(1000) )
			{
				m_fall_detector_state = FALL_DETECTOR_IDLE;
                                m_active_counter = 0;
                        //        Nprintf(1,"[Fall detect] impact timeout, max_impact_value = %f, m_impact_counter = %d m_impact_counter = %d\r\n", max_impact_value, m_timeout_impact_counter, m_impact_counter);
                                break;
			}
		} break;

		case FALL_DETECTOR_STATIC:
		{
			if ( ++m_static_impact_counter > FALL_TIME_MS(4000) )
			{
				m_fall_detector_state = FALL_DETECTOR_IDLE;
                                m_active_counter = 0;
				FLprintf("[Fall detect] static timeout reback FALL_DETECTOR_IDLE, m_static_counter = %d\r\n", m_static_counter);
                                break;
			}
			if ( (FALL_STATIC_VALUE + m_fall_detector_level.static_threshold_value) > input_composite_data ) // 处于稳定阶段.
			{
				if ( true == temp_device_is_sleep_flag && ++m_static_counter > FALL_TIME_MS(2000) )
				{
					FLprintf("[Fall detect] input_composite_data = %f, m_first_value = %f\r\n", input_composite_data, m_first_value );
					if ( (input_composite_data > m_first_value) && (input_composite_data - m_first_value > FALL_STEP_CHECK_VALUE) )
					{
                                                m_fall_detector_state = FALL_WAIT_FOR_SOME_TIME; 
                                                //m_wait_for_counter = 0;
                                                FLprintf("[Fall detect] go to FALL_WAIT_FOR_SOME_TIME. \r\n");
					}
				}
			}
                        else
                        {
                                FLprintf("[Fall detect] not static now. \r\n");
                                -- m_static_counter;
                        }
		} break;

                case FALL_WAIT_FOR_SOME_TIME:
                {
                      {
                              FLprintf("Get the fall down action.\r\n");
                              if (NULL != m_fall_module.state_handler)
                              {
                                      m_fall_module.state_handler( FALL_DOWN_OCCUR );
                              }
                              m_fall_detector_state   = FALL_DETECTOR_IDLE; 
                              m_active_counter        = 0;
                              //m_wait_for_counter      = 0;
                      }
/*
    if ( true == temp_device_is_sleep_flag ) // 最终稳定.
    {
            if ( ++m_wait_for_counter >= FALL_TIME_MS(1200) )
            {
                    Nprintf(1, "Get the fall down action.\r\n");
                    if (NULL != m_fall_module.state_handler)
                    {
                            m_fall_module.state_handler( FALL_DOWN_OCCUR );
                    }
                    m_fall_detector_state   = FALL_DETECTOR_IDLE; 
                    m_active_counter        = 0;
                    m_wait_for_counter      = 0;
            }
    }
    else
    {
            m_fall_detector_state = FALL_DETECTOR_IDLE; 
            m_active_counter = 0;
            m_wait_for_counter = 0;
            Nprintf(1,"[Fall detect] static fail, FALL_DETECTOR_IDLE!\r\n");
    }
*/
                } break;

		default:
		{
			m_fall_detector_state = FALL_DETECTOR_IDLE;
                        m_active_counter = 0;
		} break;
	}
}


bool fall_get_switch(void)
{
	return m_FallDownflag;
}


void fall_set_switch(bool input)
{
	m_FallDownflag = input;
}

/*
void fall_config_interrupt(void) //init falldown interrupt
{
  lis3dh_write_byte( LIS3DH_INT1_THS, 0x16 );
  lis3dh_write_byte( LIS3DH_INT1_CFG, 0x95 );
  lis3dh_write_byte( LIS3DH_INT1_DURATION, 0x03 );
  lis3dh_write_byte( LIS3DH_CTRL_REG3, 0x40 ); // IA1 interrupt on INT1.
  
}
*/

void fall_module_init(FALL_MODULE_INIT *input, uint8_t level)   // 检测等级1 - 9 等级越高检测阈值越低.
{
	if ( NULL != input->state_handler )
	{
		m_fall_module.state_handler = input->state_handler;
	}
	if ( level >= 1 && level <= 9 )
	{
                uint8_t index = level - 1;
                m_fall_detector_level.lost_gravity_threshold_value = m_lost_gravity_threshold_value[index];
                m_fall_detector_level.lost_gravity_time = m_lost_gravity_time[index];
                m_fall_detector_level.static_threshold_value = m_static_threshold_value[index];
	}
}


#endif

