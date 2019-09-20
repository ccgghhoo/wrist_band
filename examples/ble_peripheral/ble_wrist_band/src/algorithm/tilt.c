

#include "tilt.h"
#include <string.h>
#include <stdbool.h>
//#include "log.h"

#if 0
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#define Nprintf				        NRF_LOG_INFO
#else
#define Nprintf
#endif
   

#define __MOTION_SENSOR_TILT_SUPPORT__
#ifdef __MOTION_SENSOR_TILT_SUPPORT__


#define SHORT_ABS(x)        									(( ((x) & 0x8000) > 0) ? ((~x) + 1)  : (x) )
#define TILT_SAMPLE_RATE										( SAMPLE_FREQ )			// default 100Hz.


#define TILT_ONE_POINT_TIME_MS_SIZE								( 1000 / TILT_SAMPLE_RATE )
#define TILT_TIME_MS_REQ(X)   									( (X) / (TILT_ONE_POINT_TIME_MS_SIZE) )
#define DEVICE_TILT_ANGLE_MAX_VALUE_ALLOW 						( 90 )
#define DEVICE_TILT_START_ANGLE_OFFSET    						( 30 )

#define USER_ALARM_ANGLE_VALUE_MAX                     		( DEVICE_TILT_ANGLE_MAX_VALUE_ALLOW - DEVICE_TILT_START_ANGLE_OFFSET )
#define USER_ALARM_ANGLE_VALUE_MIN				0

#define USER_ALARM_TIME_VALUE_SET_MAX							( 1800 )
#define USER_ALARM_TIME_VALUE_SET_MIN							( 5 )// S

#define DEFAULT_USER_ALARM_ANGLE_VALUE							( 45 )// 0 - 70 用户可设置的报警倾斜角的范围.
#define DEFAULT_USER_ALARM_TIME_VALUE							( 5 )// 默认时间是5 S.


enum
{
    TILT_PREPARE,   //准备.
    TILT_COUNT,             //状态计量.
    TILT_STATIC,    //产品静止.
    TILT_REBACK_FIRST,
};


static TILT_MODULE_INIT 	m_tilt_module						= { .tilt_state_handler = NULL };

static uint32_t  			m_tilt_vaild_state_counter			= 0;
static uint32_t	                        m_state_timeout_counter				= 0;
static uint32_t  			m_user_war_prepare_counter			= 0;
static uint32_t  			m_user_tilt_static_counter			= 0;            //tilt保持时间，超过用户设置的数值.
static bool      			m_device_have_first_angle			= false;        //要有起始角度，起始角度由90度偏移.
static uint8_t   			m_device_current_state				= TILT_PREPARE;
static uint16_t                         m_device_shake_threshold_counter	= 0;
static tilt_user_config_t               m_user_config_alarm_value			= { .user_alarm_angle_value =  DEFAULT_USER_ALARM_ANGLE_VALUE, .user_alarm_time_value = DEFAULT_USER_ALARM_TIME_VALUE };
static bool     			m_flag 								= false;





#define SIN_INTERVAL_NUM 91
#define SIN_0   0
#define SIN_1   9
#define SIN_2   18
#define SIN_3   27
#define SIN_4   36
#define SIN_5   45
#define SIN_6   53
#define SIN_7   62
#define SIN_8   71
#define SIN_9   80
#define SIN_10  89
#define SIN_11  98
#define SIN_12  106
#define SIN_13  115
#define SIN_14  124
#define SIN_15  132
#define SIN_16  141
#define SIN_17  150
#define SIN_18  158
#define SIN_19  167
#define SIN_20  175
#define SIN_21  183
#define SIN_22  192
#define SIN_23  200
#define SIN_24  208
#define SIN_25  216
#define SIN_26  224
#define SIN_27  232
#define SIN_28  240
#define SIN_29  248
#define SIN_30  256
#define SIN_31  264
#define SIN_32  271
#define SIN_33  279
#define SIN_34  286
#define SIN_35  294
#define SIN_36  301
#define SIN_37  308
#define SIN_38  315
#define SIN_39  322
#define SIN_40  329
#define SIN_41  336
#define SIN_42  342
#define SIN_43  349
#define SIN_44  356
#define SIN_45  362
#define SIN_46  368
#define SIN_47  374
#define SIN_48  380
#define SIN_49  386
#define SIN_50  392
#define SIN_51  398
#define SIN_52  403
#define SIN_53  409
#define SIN_54  414
#define SIN_55  419
#define SIN_56  424
#define SIN_57  429
#define SIN_58  434
#define SIN_59  439
#define SIN_60  443
#define SIN_61  448
#define SIN_62  452
#define SIN_63  456
#define SIN_64  460
#define SIN_65  464
#define SIN_66  468
#define SIN_67  471
#define SIN_68  475
#define SIN_69  476
#define SIN_70  478
#define SIN_71  479
#define SIN_72  480
#define SIN_73  481
#define SIN_74  482
#define SIN_75  484
#define SIN_76  485
#define SIN_77  486
#define SIN_78  487
#define SIN_79  489
#define SIN_80  490
#define SIN_81  491
#define SIN_82  492
#define SIN_83  493
#define SIN_84  494
#define SIN_85  495
#define SIN_86  496
#define SIN_87  497
#define SIN_88  498
#define SIN_89  499
#define SIN_90  500



static const uint16_t asin_value[SIN_INTERVAL_NUM] =
{
    SIN_0, SIN_1, SIN_2, SIN_3, SIN_4, SIN_5, SIN_6, SIN_7, SIN_8, SIN_9,
    SIN_10, SIN_11, SIN_12, SIN_13, SIN_14, SIN_15, SIN_16, SIN_17, SIN_18, SIN_19,
    SIN_20, SIN_21, SIN_22, SIN_23, SIN_24, SIN_25, SIN_26, SIN_27, SIN_28, SIN_29,
    SIN_30, SIN_31, SIN_32, SIN_33, SIN_34, SIN_35, SIN_36, SIN_37, SIN_38, SIN_39,
    SIN_40, SIN_41, SIN_42, SIN_43, SIN_44, SIN_45, SIN_46, SIN_47, SIN_48, SIN_49,
    SIN_50, SIN_51, SIN_52, SIN_53, SIN_54, SIN_55, SIN_56, SIN_57, SIN_58, SIN_59,
    SIN_60, SIN_61, SIN_62, SIN_63, SIN_64, SIN_65, SIN_66, SIN_67, SIN_68, SIN_69,
    SIN_70, SIN_71, SIN_72, SIN_73, SIN_74, SIN_75, SIN_76, SIN_77, SIN_78, SIN_79,
    SIN_80, SIN_81, SIN_82, SIN_83, SIN_84, SIN_85, SIN_86, SIN_87, SIN_88, SIN_89,
    SIN_90
};


static uint16_t motion_sensor_get_angle(uint16_t value)
{
    int16_t i;

    for (i = (SIN_INTERVAL_NUM - 1); i >= 0; --i)
    {
        if (value >= asin_value[i])
        {
            return  (i);
        }
    }
    return  0;
}


static void motion_sensor_get_tilt(const vector3_t *input, vector3_t *ouput)
{
    if (input->X >= 0)
    {
        ouput->X = motion_sensor_get_angle(input->X);
    }
    else
    {
        ouput->X = -motion_sensor_get_angle(-input->X);
    }
    if (input->Y >= 0)
    {
        ouput->Y = motion_sensor_get_angle(input->Y);
    }
    else
    {
        ouput->Y = -motion_sensor_get_angle(-input->Y);
    }
    if (input->Z >= 0)
    {
        ouput->Z = motion_sensor_get_angle(input->Z);
    }
    else
    {
        ouput->Z = -motion_sensor_get_angle(-input->Z);
    }

    //Nprintf(1,"[TILT]X, Y, Z tilt = %d,%d,%d\r\n", m_vector_angle.X, m_vector_angle.Y, m_vector_angle.Z);
}


bool tilt_module_config_angle_alarm(tilt_user_config_t *input)
{

    if (NULL != input &&
        input->user_alarm_angle_value >= 30 &&
        input->user_alarm_angle_value <= 90 &&
        input->user_alarm_time_value  >= USER_ALARM_TIME_VALUE_SET_MIN &&
        input->user_alarm_time_value  <= USER_ALARM_TIME_VALUE_SET_MAX)
    {
        m_user_config_alarm_value.user_alarm_angle_value = 90 - input->user_alarm_angle_value;
        m_user_config_alarm_value.user_alarm_time_value = input->user_alarm_time_value;
        return true;
    }
    return false;
}


//角度定义: 设备与水平面的夹角 X， sin(X) = y_offset / 512.
//设备角度检测主要关注 z轴方向的量.
//用户设置角度小于X报警，那么z轴就是大于 (90 - X)报警.
void tilt_moudle_gsensor_data_handler(vector3_t *input, bool temp_device_is_sleep_flag)
{
    vector3_t 	vector_angle;
    motion_sensor_get_tilt(input, &vector_angle); //get the angle.

//    uint16_t temp_device_current_angle = SHORT_ABS(vector_angle.Z);
#if (TILT_VERTICAL_ACCEL_S == TILT_VERTICAL_ACCEL_POINT_X)
    uint16_t temp_device_current_angle = SHORT_ABS(vector_angle.X);

#elif (TILT_VERTICAL_ACCEL_S == TILT_VERTICAL_ACCEL_POINT_Y)
    uint16_t temp_device_current_angle = SHORT_ABS(vector_angle.Y);

#elif (TILT_VERTICAL_ACCEL_S == TILT_VERTICAL_ACCEL_POINT_Z)
    uint16_t temp_device_current_angle = SHORT_ABS(vector_angle.Z);

#else
    uint16_t temp_device_current_angle = SHORT_ABS(vector_angle.X);
#endif

    //Nprintf("[TILT] angle to y :%d \r\n", temp_device_current_angle);
//    Nprintf("[TILT]X, Y, Z tilt = %d,%d,%d\r\n", vector_angle.X, vector_angle.Y, vector_angle.Z);
    
    switch (m_device_current_state)
    {
    default:
        {
            m_device_current_state = TILT_PREPARE;
            m_device_have_first_angle = false;
            m_user_war_prepare_counter = 0;
            m_user_tilt_static_counter = 0;
            m_tilt_vaild_state_counter = 0;
            m_user_war_prepare_counter = 0;
            m_state_timeout_counter = 0;
            m_device_shake_threshold_counter = 0;
            break;
        }

    case TILT_PREPARE:
        {
            if (true == m_device_have_first_angle)
            {
                if (temp_device_current_angle <= m_user_config_alarm_value.user_alarm_angle_value)
                {
                    Nprintf( "[TILT] TILT_PREPARE to TILT_COUNT \r\n");
                    m_device_current_state				= TILT_COUNT;
                    m_state_timeout_counter				= 0;
                    m_device_shake_threshold_counter	= 0;
                    m_tilt_vaild_state_counter			= 0;
                    m_device_have_first_angle			= false;
                }
            }
            else
            {
                if (temp_device_current_angle > USER_ALARM_ANGLE_VALUE_MAX)
                {
                    Nprintf("[TILT] m_user_war_prepare_counter = %d\r\n", m_user_war_prepare_counter);
                    if (++m_user_war_prepare_counter > TILT_TIME_MS_REQ(8000))
                    {
                        Nprintf("[TILT] m_device_have_first_angle = true  \r\n");
                        //        loc_gsm_task_setup_musicplay(MUSIC_PRE_TILT);
                        m_user_war_prepare_counter	= 0;
                        m_device_have_first_angle	= true;
                    }
                }
                else
                {
                    m_user_war_prepare_counter = 0;
                }
            }
            break;
        }

    case TILT_COUNT:
        {
            if (++m_state_timeout_counter >= TILT_TIME_MS_REQ(5000))
            {
                Nprintf("[TILT] m_device_current_state = TILT_REBACK_FIRST  \r\n");
                m_device_current_state = TILT_REBACK_FIRST;
                break;
            }
            if (false == temp_device_is_sleep_flag)
            {
                if (++m_device_shake_threshold_counter >= TILT_TIME_MS_REQ(3000)) // 3秒的防抖阈值.
                {
//                    Nprintf(1, "[TILT] 防抖超时，回到默认\r\n");
                    m_device_current_state = TILT_REBACK_FIRST;
                    break;
                }
                m_tilt_vaild_state_counter = 0;
            }

            if (temp_device_current_angle <= m_user_config_alarm_value.user_alarm_angle_value)
            {
                if (++m_tilt_vaild_state_counter >= TILT_TIME_MS_REQ(1500))
                {
                    Nprintf( "[TILT] count to static! temp_device_current_angle:%d \r\n", temp_device_current_angle);
                    m_device_current_state				= TILT_STATIC;
                    m_user_tilt_static_counter			= 0;
                    m_device_shake_threshold_counter	= 0;
                    m_state_timeout_counter				= 0;
                }
            }
            else
            {
                m_tilt_vaild_state_counter = 0;
            }
            break;
        }

    case TILT_STATIC:
        {
            if (++m_state_timeout_counter >= TILT_TIME_MS_REQ((1 + USER_ALARM_TIME_VALUE_SET_MAX) * 1000))
            {
                Nprintf( "[TILT] static to reback_fist\r\n");
                m_device_current_state = TILT_REBACK_FIRST;
                break;
            }
            if (false == temp_device_is_sleep_flag || temp_device_current_angle > m_user_config_alarm_value.user_alarm_angle_value)
            {
                if (++m_device_shake_threshold_counter >= TILT_TIME_MS_REQ(5000))
                {
//                    Nprintf(1, "[TILT] 静态下防抖超时，回到默认\r\n");
                    m_device_current_state = TILT_REBACK_FIRST;
                    break;
                }
                m_user_tilt_static_counter = 0;
            }
            else
            {
                m_device_shake_threshold_counter = 0;
            }
            
            if (++m_user_tilt_static_counter >= TILT_TIME_MS_REQ(m_user_config_alarm_value.user_alarm_time_value * 1000))
            {
                if (NULL != m_tilt_module.tilt_state_handler)
                {
                    m_tilt_module.tilt_state_handler(TILE_MODULE_STATUE_OCCUR);
                }
                Nprintf( "[TILT] tilt detect! \r\n");
//                Nprintf( "[TILT] 设置的倾斜角度：%d 倾斜持续时间：%d.\r\n", m_user_config_alarm_value.user_alarm_angle_value, m_user_config_alarm_value.user_alarm_time_value);
                m_device_current_state = TILT_REBACK_FIRST;
            }
            break;
        }
    }
}


bool tilt_get_switch(void)
{
    return m_flag;
}


void tilt_set_switch(bool input)
{
    m_flag = input;
}


void tilt_module_init(TILT_MODULE_INIT *input)
{
    m_tilt_module.tilt_state_handler = input->tilt_state_handler;
    m_user_config_alarm_value.user_alarm_angle_value = 45;
    m_user_config_alarm_value.user_alarm_time_value  = 5;
}


#endif


