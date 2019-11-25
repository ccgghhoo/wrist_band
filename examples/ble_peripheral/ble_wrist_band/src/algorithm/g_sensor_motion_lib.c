

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "g_sensor_motion_lib.h"
#include "fall_detect.h"
//#include "ls.code.h"
//#include "ls_api.h"
#include "dev_config.get.h"
//#include "log.h"
#include "tilt.h"
#include "state_detect.h"
#include "app_motion_detect.h"


#if 1
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#define ALGO_LOG				        NRF_LOG_DEBUG
#else
#define ALGO_LOG(...)
#endif

typedef enum {
    MOTION_STATE_NONE,
    MOTION_STATE_MOTION,
    MOTION_STATE_STATIC,
}MotionState_en;

typedef struct {
    MotionState_en state;
    uint32_t staticCount;
    uint32_t motionCount;
}MotionState_t;

#define SHORT_ABS(x)            (( ((x) & 0x8000) > 0) ? ((~x) + 1)  : (x) )
#define DIFF(x, y)              ( x > y ? (x - y ) : (y - x ) )

#define FILTER_ORDER            4
#define WINDOWS_SIZE            16 //10*16 160ms
#define SLEEP_SECOND            ( 4 * 60 )


static const vector3_t           zero_vector = { 0 };

static vector3_t                 raw_vectors[FILTER_ORDER];
static vector3_t                 average_vector;
static uint32_t                  index_average;
static vector3_t                 filter_vector;

static uint32_t                  lastest_scalar;
static uint32_t                  scalars[WINDOWS_SIZE];
static uint32_t                  index_of_scalars;

static uint8_t                   md_state;



static uint32_t                  flag_orient_sample_counter;



static uint32_t                  flag_stay_in_quiet_counter;


static MotionState_t sg_sMotionState;




static bool                      flag_stay_in_quiet;


static orient_dir_t              m_orient_dir;

static algo_lib_t                m_algo;


typedef enum
{
    FD_AGRAVITY_DETECT = 0,
    FD_IMPACT_DETECT,
    FD_STATIC_DETECT,
    FD_STATE_CHECK,
} falldown_state_t;


typedef struct
{
    uint32_t scalar;
    uint16_t sample_cnt;
    uint8_t active_cnt;
    uint8_t inactive_cnt;

    uint8_t state;
} falldown_t;
static falldown_t           mfall;

uint16_t algo_lib_version(void)
{
    return 0x0100;
}

bool algo_lib_stay_in_quiet(void)
{
    return flag_stay_in_quiet;
}


void algo_lib_cfg_orient(bool enable)
{
    m_algo.cfg.orient.enable = enable;
    flag_orient_sample_counter = 0;
}




static float inv_sqrt(float x)
{
    float xhalf = 0.5f * x;
    int i = *(int *)&x;

    i = 0x5f375a86 - (i >> 1);          // this is magic.
    x = *(float *)&i;
    x = x * (1.5f - xhalf * x * x);     // 1st newton iteration.
    x = x * (1.5f - xhalf * x * x);     // repeating increases accuracy, this can be remove.

    return 1 / x;
}


void algo_lib_reset(void)
{
    uint32_t i;
    index_average = 0;
    lastest_scalar = 0;
    index_of_scalars = 0;

    average_vector = zero_vector;
    filter_vector = zero_vector;

    sg_sMotionState.staticCount = 0;
    sg_sMotionState.motionCount = 0;
    flag_stay_in_quiet_counter = 0;

    flag_stay_in_quiet = false;
    md_state = 0;

    for (i = 0; i < FILTER_ORDER; i++)
    {
        raw_vectors[i] = zero_vector;
    }
    for (i = 0; i < WINDOWS_SIZE; i++)
    {
        scalars[i] = 0;
    }
}

// Axis-Y
static void algo_lib_vector_filt(vector3_t *vector)
{
    average_vector.X = average_vector.X - raw_vectors[index_average].X + vector->X;
    filter_vector.X = average_vector.X / FILTER_ORDER;
    raw_vectors[index_average].X = filter_vector.X;

    average_vector.Y = average_vector.Y - raw_vectors[index_average].Y + vector->Y;
    filter_vector.Y = average_vector.Y / FILTER_ORDER;
    raw_vectors[index_average].Y = filter_vector.Y;

    average_vector.Z = average_vector.Z - raw_vectors[index_average].Z + vector->Z;
    filter_vector.Z = average_vector.Z / FILTER_ORDER;
    raw_vectors[index_average].Z = filter_vector.Z;

    // FILTER X/Y/Z used to check direction/fall down.
    // scalar used to check motion/static.
    uint32_t x = SHORT_ABS(filter_vector.X);
    uint32_t y = SHORT_ABS(filter_vector.Y);
    uint32_t z = SHORT_ABS(filter_vector.Z);

    lastest_scalar = (uint32_t)(x * x + y * y + z * z);
    lastest_scalar = (uint32_t)inv_sqrt((float)lastest_scalar);

    scalars[index_of_scalars] = lastest_scalar; // lack sqrt process.
}


static void algo_lib_orient_identy(uint32_t diff)
{
    if (diff > SCALE_MG(100))
    {
        flag_orient_sample_counter = 0;
        return;
    }
    flag_orient_sample_counter++;
    if (flag_orient_sample_counter > 5)
    {
        orient_dir_t  orient = ORIENT_INVALID;
        uint16_t x, y, z;
        x = SHORT_ABS(filter_vector.X);
        y = SHORT_ABS(filter_vector.Y);
        z = SHORT_ABS(filter_vector.Z);
        if (x > ORIENT_POS_GRAVITY)
        {
            if (filter_vector.X < 0)
            {
                orient = ORIENT_NEG_X;
            }
            else
            {
                orient = ORIENT_POS_X;
            }
        }
        else if (y > ORIENT_POS_GRAVITY)
        {
            if (filter_vector.Y < 0)
            {
                orient = ORIENT_NEG_Y;
            }
            else
            {
                orient = ORIENT_POS_Y;
            }
        }
        else if (z > ORIENT_POS_GRAVITY)
        {
            if (filter_vector.Z < 0)
            {
                orient = ORIENT_NEG_Z;
            }
            else
            {
                orient = ORIENT_POS_Z;
            }
        }
        else
        {
            // do nothing.
        }
        if (orient != m_orient_dir) // callback orientation change.
        {
            m_orient_dir = orient;
            m_algo.evt.orient(orient);
        }
    }
}




/**
 * 
 * 
 * @author hzhmcu (2019/5/9)
 * 
 * @param diff 
 */
static void algo_lib_motion_identy(uint32_t diff) // 3 cycle as a motion.
{
 
    if (diff > MOTION_THRESHOLD)
    {
        sg_sMotionState.staticCount = 0;

        if (sg_sMotionState.state != MOTION_STATE_MOTION)
        {
            sg_sMotionState.motionCount++;
            if (sg_sMotionState.motionCount > 2)
            {
//                HID_LOG(LOG_MOTION, "Motion\r\n");
                m_algo.evt.cb(AL_EVT_MOTION);
                sg_sMotionState.state = MOTION_STATE_MOTION;
            }
        }
    }
}


static void algo_lib_nomotion_detetion()
{
//¾²Ö¹¼ì²â¡£
    sg_sMotionState.staticCount++;
    if (sg_sMotionState.staticCount >= SECONDS_OF_FREQ(SLEEP_SECOND))
    {
        sg_sMotionState.staticCount = 0;
        if (sg_sMotionState.state != MOTION_STATE_STATIC)
        {
//            HID_LOG(LOG_MOTION, "Static\r\n");
            m_algo.evt.cb(AL_EVT_STATIC);
        }
        sg_sMotionState.state = MOTION_STATE_STATIC;

    }
}

static void algo_lib_motion_detetion(uint32_t index_min, uint32_t index_max, uint32_t diff)
{


    // state machine to find peak & valley value.
    switch (md_state)
    {
    case 0: // valley.
        if (index_min == (index_of_scalars + WINDOWS_SIZE / 2) % WINDOWS_SIZE)
        {
            md_state = 1;
        }
        break;

    case 1: // peak.
        if (index_max == (index_of_scalars + WINDOWS_SIZE / 2) % WINDOWS_SIZE)
        {
            md_state = 0;
            //ALGO_LOG("[Max&Min] : %d, %d, %d\r\n", max_scalar, min_scalar, diff);
            algo_lib_motion_identy(diff);
        }
        break;

    default:
        md_state = 0;
        break;
    }

}


static void algo_lib_detection(void)
{
#if 1// find scalar Maximunu & Minimun.
    uint32_t index_max, index_min;
    uint32_t max_scalar, min_scalar;
    uint32_t i;

    max_scalar = scalars[0];
    index_max = 0;
    for (i = 1; i < WINDOWS_SIZE; i++)
    {
        if (scalars[i] > max_scalar)
        {
            max_scalar = scalars[i];
            index_max = i;
        }
    }

    min_scalar = scalars[0];
    index_min = 0;
    for (i = 1; i < WINDOWS_SIZE; i++)
    {
        if (scalars[i] < min_scalar)
        {
            min_scalar = scalars[i];
            index_min = i;
        }
    }

#endif

    uint32_t diff = max_scalar - min_scalar;
    algo_lib_motion_detetion(index_min, index_max, diff);

    if (m_algo.cfg.orient.enable)
    {
        algo_lib_orient_identy(diff);
    }

    if (max_scalar - min_scalar < 100)
    {
        if (++flag_stay_in_quiet_counter >= SECONDS_OF_FREQ(SLEEP_SECOND))
        {
            ALGO_LOG("  flag_stay_in_quiet_counter = %d \r\n", flag_stay_in_quiet_counter);
            flag_stay_in_quiet = true;
        }
    }
    else
    {
        flag_stay_in_quiet_counter = 0;
        flag_stay_in_quiet = false;
    }
}


static void algo_lib_timer_check(void)
{
    if (++index_average >= FILTER_ORDER)
    {
        index_average = 0;
    }
    if (++index_of_scalars  >= WINDOWS_SIZE)
    {
        index_of_scalars = 0;
    }

    
    algo_lib_nomotion_detetion();

}


void algo_lib_runtime(vector3_t *vector, bool temp_device_is_sleep_flag)
{
    algo_lib_vector_filt(vector);
    algo_lib_detection();         //motion check
    algo_lib_timer_check();       //static check 
}


void fall_callback(uint32_t input_state) //fall down cb.
{
    if (input_state == FALL_DOWN_OCCUR)
    {
//        ls_api_event_input(LS_EVT_ALERT_PRE_FALLDOWN);
        md_app_tilt_falldown_event_received(FALLDOWN_ALERT_EVENT);  //chen 2019/06/21
    }
}


void tilt_callback(uint32_t input_state)
{
    if (input_state == TILE_MODULE_STATUE_OCCUR)
    {
//        ls_api_event_input(LS_EVT_ALERT_PRE_TILT);
        md_app_tilt_falldown_event_received(TILT_ALERT_EVENT);  //chen 2019/06/21
               
    }
}


void algo_lib_init(algo_init_t *algo_init_obj)
{
    if (algo_init_obj == NULL) return;

    m_algo.cfg = algo_init_obj->cfg;
    m_algo.evt = algo_init_obj->evt;

    //
    sg_sMotionState.state = MOTION_STATE_NONE;

    sg_sMotionState.motionCount = 0;
    sg_sMotionState.staticCount = 0;

    //init fall down check.
    {
        FALL_MODULE_INIT fall_instance;
        fall_instance.state_handler = fall_callback;
        fall_module_init(&fall_instance, dev_config_get_alert_falldown_level());
    }
    {
        TILT_MODULE_INIT tilt_instance;
        tilt_instance.tilt_state_handler = tilt_callback;
        tilt_module_init(&tilt_instance);
    }
}


void algo_lib_param_update(void)
{
    if (false == dev_config_get_alert_falldown_enable())
    {
        fall_set_switch(false);
        return;
    }

    fall_set_switch(true);
    FALL_MODULE_INIT fall_instance;
    fall_instance.state_handler = fall_callback;
    fall_module_init(&fall_instance, dev_config_get_alert_falldown_level());
}

