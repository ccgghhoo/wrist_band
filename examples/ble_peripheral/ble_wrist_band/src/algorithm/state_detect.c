//#include "config.h"
#include "state_detect.h"
#include "g_sensor_motion_lib.h"
//#include "log.h"
#include "dev_config.get.h"
#include "DateTime.h"
//#include "ls.code.h"
#include "app_motion_detect.h"


//static
    state_motion_t m_state_motion =
{
    .state = MOTION_ALERT_STA_NONE,

};


//static 
    NomotionInfo_t sg_sNomotionInfo =
{
    .state = NOMOTION_ALERT_STA_NONE,
    .isAlert = false,
};




static volatile bool isMotor = false;

bool IsStateInvaild()
{
    //if (Indicator_GetMotor() == true || GetSpeaker()) return true;
    return false;
}
#define INVALUEDATA 0X7FFFFFFF


void NomotionAlert_Update(int32_t diff);



#define FIFO_MAX 30

typedef struct
{
    int32_t fifo[FIFO_MAX];
    int index;
    int count;
    int32_t Max;
    int32_t Min;
    int32_t diff;
}FifoInfo_t;

FifoInfo_t sg_sFifoInfo = { .count = 0 };

static bool PutData(FifoInfo_t *pFifo, int fifoMax, int32_t value)
{
    pFifo->fifo[pFifo->index++] = value;
    if (pFifo->index >= fifoMax)
    {
        pFifo->index  = 0;
    }
    if (pFifo->count < fifoMax)
    {
        pFifo->count++;
        return false;
    }

    pFifo->Max = pFifo->fifo[0];
    pFifo->Min = pFifo->fifo[0];

    for (int i = 1; i < fifoMax; i++)
    {
        if (pFifo->fifo[i] >  pFifo->Max) pFifo->Max  = pFifo->fifo[i];
        if (pFifo->fifo[i] <  pFifo->Min) pFifo->Min  = pFifo->fifo[i];
    }
    pFifo->diff = pFifo->Max - pFifo->Min;
    return true;
}


void MotionAlert_Update(int32_t diff)
{
    if (dev_config_get_alert_motion_enable() == false) return;
    switch (m_state_motion.state)
    {

    default:
    case MOTION_ALERT_STA_NONE:
        {
//            HID_LOG(LOG_MOALERT, "MotionClear.\r\n");
            m_state_motion.motion_count = 0;
            m_state_motion.static_count = 0;
            m_state_motion.static_timestramp = RunTime_GetValue();
            m_state_motion.state = MOTION_ALERT_STA_STATIC;
            break;
        }
    case MOTION_ALERT_STA_STATIC:
        {
            static uint32_t tick = 0;
            ++m_state_motion.static_count;
            if (diff > MOTION_THRESHOLD_MAX)
            {
                tick = 0;
                m_state_motion.static_timestramp = RunTime_GetValue();
                m_state_motion.static_count = 0;
            }

/*
            else if (m_state_motion.static_count >= SECONDS_OF_FREQ(dev_config_get_alert_motion_setup_time())) //
            {

                m_state_motion.state = MOTION_ALERT_STA_WAIT_MOTION;
                HID_LOG(LOG_MOALERT, "Motion Wait Motion.\r\n");
            }
*/

            else
            {
                if (m_state_motion.static_count - tick > 100)
                {
                    tick = m_state_motion.static_count;
//                    HID_LOG(LOG_MOALERT, "Motion Static %d.\r\n", tick / 100);
                }
            }

        }
        break;
    case MOTION_ALERT_STA_WAIT_MOTION:
        {
            if (diff > MOTION_THRESHOLD_MAX)
            {
                m_state_motion.motion_count = 0;
                m_state_motion.state = MOTION_ALERT_STA_MOTION_TIMEOUT;
//                HID_LOG(LOG_MOALERT, "Motion  Motion Check.\r\n");
            }
            break;
        }

    case MOTION_ALERT_STA_MOTION_TIMEOUT:
        {
            m_state_motion.motion_count++;

            if (diff > MOTION_THRESHOLD_MAX) m_state_motion.static_count = 0;
            else if (diff < MOTION_THRESHOLD_MIN) m_state_motion.static_count++;

            if (m_state_motion.static_count >= SECONDS_OF_FREQ(10))//1
            {
                m_state_motion.state = MOTION_ALERT_STA_NONE;
                m_state_motion.cannelCount = m_state_motion.static_count;
                m_state_motion.cannelTick = m_state_motion.motion_count;

//                HID_LOG(LOG_MOALERT, "Motion  Static %d %d .\r\n", m_state_motion.cannelCount, m_state_motion.motion_count);
                break;
            }

            if (m_state_motion.motion_count >= SECONDS_OF_FREQ(dev_config_get_alert_motion_action_time()))
            {
//                HID_LOG(LOG_MOALERT, "Motion  Alert %d %d .\r\n");
                m_state_motion.state = MOTION_ALERT_STA_ALERT;
                
                md_app_tilt_falldown_event_received(MOTION_ALERT_EVENT); //chen 
                break;
            }
            break;
        }

    case MOTION_ALERT_STA_ALERT:
        {
            m_state_motion.state = MOTION_ALERT_STA_NONE;
//            ls_api_event_input(LS_EVT_ALERT_PRE_MOTION);
            break;
        }

    }
}


void NomotionAlert_Update(int32_t diff)
{
    if (dev_config_get_alert_actionless_enable() == false) return;

    switch (sg_sNomotionInfo.state)
    {
    default:
    case NOMOTION_ALERT_STA_NONE:
        {

//            HID_LOG(LOG_MOALERT, "Static  None .\r\n");
            sg_sNomotionInfo.count = 0;
            if (sg_sNomotionInfo.isAlert == false)
            {
                sg_sNomotionInfo.timestamp = RunTime_GetValue();
                sg_sNomotionInfo.state = NOMOTION_ALERT_STA_STATIC;
            }
            else sg_sNomotionInfo.state = NOMOTION_ALERT_STA_WAIT_MOTION;

            break;
        }
    case NOMOTION_ALERT_STA_STATIC:
        {
            if (diff > MOTION_THRESHOLD_MAX)
            {
                sg_sNomotionInfo.count = 0;
                sg_sNomotionInfo.timestamp = RunTime_GetValue();
            }

            /*
            if (++sg_sNomotionInfo.count >= SECONDS_OF_FREQ(dev_config_get_alert_actionless_threshold()))
            {
                HID_LOG(LOG_MOALERT, "Static  Alert .\r\n");
                sg_sNomotionInfo.state = NOMOTION_ALERT_STA_ALERT;
                break;
            }
            */
            break;
        }
        /*
        
        case NOMOTION_ALERT_STA_ALERT:
        {


            break;
        }
        */
    case NOMOTION_ALERT_STA_WAIT_MOTION:
        {
            if (diff > MOTION_THRESHOLD_MAX)
            {
                sg_sNomotionInfo.state = NOMOTION_ALERT_STA_ALERT_CANNEL;
                sg_sNomotionInfo.count = 0;
                sg_sNomotionInfo.motionCount = 0;
            }
            
            

            break;
        }

    case NOMOTION_ALERT_STA_ALERT_CANNEL:
        {

            sg_sNomotionInfo.motionCount++;

            if (diff > MOTION_THRESHOLD_MAX) sg_sNomotionInfo.count = 0; //  m_state_motion.static_count = 0;
            else if (diff < MOTION_THRESHOLD_MIN) sg_sNomotionInfo.count++;

            if (sg_sNomotionInfo.count >= SECONDS_OF_FREQ(1))
            {
                sg_sNomotionInfo.state = NOMOTION_ALERT_STA_WAIT_MOTION;
                break;
            }

            if (sg_sNomotionInfo.motionCount >= SECONDS_OF_FREQ(3))
            {
                sg_sNomotionInfo.isAlert = false;
                sg_sNomotionInfo.state = NOMOTION_ALERT_STA_NONE;
                break;
            }
            break;
        }
    }


}


void state_input_data(int16_t x, int16_t y, int16_t z)
{
    static int32_t lst_Value = INVALUEDATA;
    if (IsStateInvaild() == true)
    {
        m_state_motion.state = MOTION_ALERT_STA_NONE;

        sg_sNomotionInfo.state = NOMOTION_ALERT_STA_NONE;

        lst_Value = INVALUEDATA;
        sg_sFifoInfo.count = 0;
        return;
    }

    int32_t value = x * x + y * y + z * z;
    if (PutData(&sg_sFifoInfo, FIFO_MAX, value) == false) return;

    lst_Value = (lst_Value + sg_sFifoInfo.diff) / 2;

    MotionAlert_Update(lst_Value);

    NomotionAlert_Update(lst_Value);

}





void state_config_motion_flag_update(void)
{
    m_state_motion.state = MOTION_ALERT_STA_NONE;

}


void state_config_static_flag_update(void)
{
    sg_sNomotionInfo.state  = NOMOTION_ALERT_STA_NONE;
    sg_sNomotionInfo.isAlert = false;
}


void static_restart_update(void)
{
//    HID_LOG(LOG_MOALERT, "[S]: start work.\r\n");
    sg_sNomotionInfo.state  = NOMOTION_ALERT_STA_NONE;
    //  m_state_motion.state = MOTION_ALERT_STA_NONE;
}



void MotionAlertLoop()
{

    //¾²Ö¹¼ì²â¡£
    if (dev_config_get_alert_actionless_enable() == true)
    {
               
        if (sg_sNomotionInfo.state == NOMOTION_ALERT_STA_STATIC
            && false == RunTime_InRange(sg_sNomotionInfo.timestamp, dev_config_get_alert_actionless_threshold()))
        {
                sg_sNomotionInfo.state = NOMOTION_ALERT_STA_WAIT_MOTION;
            
        }
        else if(sg_sNomotionInfo.state == NOMOTION_ALERT_STA_WAIT_MOTION) //chen added 2019/07/11
        {                  
            {
                if(sg_sNomotionInfo.isAlert == false)
                {
                    sg_sNomotionInfo.isAlert = true;
                    md_app_tilt_falldown_event_received(NOMOTION_ALERT_EVENT);
                } 
            }
        }

    }

    //¶¯¼ì²â¡£
    if(dev_config_get_alert_motion_enable() == true)
    {
        if (MOTION_ALERT_STA_STATIC == m_state_motion.state)
        {
            if(false == RunTime_InRange(m_state_motion.static_timestramp, dev_config_get_alert_motion_setup_time()))
            {
                m_state_motion.state = MOTION_ALERT_STA_WAIT_MOTION;
//                HID_LOG(LOG_MOALERT, "Motion Wait Motion.\r\n");
            }
        }


    }

}






