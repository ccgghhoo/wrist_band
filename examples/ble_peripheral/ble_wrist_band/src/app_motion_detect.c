#if 1

#include <stdint.h>
#include "app_timer.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include "tilt.h"
#include "app_motion_detect.h"
#include "dev_conf.h"
#include "dev_config.get.h"
#include "LIS3DH_drv.h"
#include "g_sensor_motion_lib.h"
#include "Step_count.h"
#include "m_interface.h"
#include "fall_detect.h"
#include "sport_degree_count.h"
#include "state_detect.h"
#include "m_step.h"
//#include "protocol.h"
#include "nrf_delay.h"
#include "batt_adc_detect.h"

#define SHORT_ABS(x)                            (( ((x) & 0x8000) > 0) ? ((~x) + 1)  : (x) )

#if 1
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#define MD_LOG				        NRF_LOG_INFO
#else
#define MD_LOG(...)
#endif

APP_TIMER_DEF(md_timer);


#define MD_TIMER_INTERVAL_25Hz                  APP_TIMER_TICKS(1000)
#define MD_TIMER_INTERVAL_50Hz                  APP_TIMER_TICKS(400) 
#define MD_TIMER_INTERVAL_100Hz                 APP_TIMER_TICKS(200) 

   md_t       md;  
alert_int_en_t   pin_int_en; 
static  uint32_t    md_step_counter_curr ;

static on_orient_detected_handle_t on_orient_callback = NULL;


void md_app_tilt_falldown_event_received(uint32_t input_event)
{
    if(input_event==TILT_ALERT_EVENT)
    {
        md.tilt_flag = true;
        MD_LOG("[MD]: TILT DETECTED!!!! \r\n");
        
    }
    else if(input_event==FALLDOWN_ALERT_EVENT)
    {
        md.fall_down_flag = true;
        MD_LOG("[MD]: FALLDOWN DETECTED!!!! \r\n");
    }
    else if(MOTION_ALERT_EVENT==input_event)
    {
        md.motion_alert_flag = 1;
        MD_LOG("[MD]: motion alert generated !!!! \r\n");
    }
    else if(NOMOTION_ALERT_EVENT==input_event)
    {
        md.static_alert_flag = 1;
        MD_LOG("[MD]: STATIC  alert generated !!!! \r\n");
    }
    
}

void md_app_set_step_counter(uint32_t step_cnt)
{
    if (step_cnt==0)
    {
        SmartWear_SportHealth_Sport_Step_Reset();
    }
    md_step_counter_curr = step_cnt; 
    //MD_LOG("[MD]: md_app_set_step_counter: %x", md_step_counter_curr);    
}

uint32_t md_app_get_step_counter(void)
{
    return md_step_counter_curr ;
}
md_t * md_app_get_motion_status(void)
{
    return &md;
}



void md_conf_orient_enable(on_orient_detected_handle_t cb)
{
    algo_lib_cfg_orient(true);
    on_orient_callback = cb;
}

void md_conf_orient_disable(void)
{
    algo_lib_cfg_orient(false);
    on_orient_callback = NULL;
}



static void md_timer_start(void)
{
    if (!md.timer_started)
    {
#if     ( SAMPLE_FREQ == 100 )
        uint32_t err_code = app_timer_start(md_timer, MD_TIMER_INTERVAL_100Hz, NULL);
#elif   ( SAMPLE_FREQ == 50 )
        uint32_t err_code = app_timer_start(md_timer, MD_TIMER_INTERVAL_50Hz, NULL);
#elif   ( SAMPLE_FREQ == 25 )
        uint32_t err_code = app_timer_start(md_timer, MD_TIMER_INTERVAL_25Hz, NULL);
#endif      
        APP_ERROR_CHECK(err_code);
        
        md.timer_started = true;
        MD_LOG("[MD]: md.timer_started!!!! \r\n");
    }
}

static void md_timer_stop(void)
{
    if (md.timer_started)
    {
        uint32_t err_code = app_timer_stop(md_timer);
        APP_ERROR_CHECK(err_code);
        md.timer_started = false;
         MD_LOG("[MD]: md.timer_stoped!!!! \r\n");
    }
}

static void md_evt_handler(al_cfg_evt_t evt)
{
    switch (evt)
    {
    case AL_EVT_STATIC:
        //MD_LOG("[MD]: sleep time reached because of static !!!! \r\n");      //3mins sleep time
        //ls_api_event_input(LS_EVT_STATIC_DETECTED);         
        //md.static_alert_flag = true;
        break;

    case AL_EVT_MOTION:
        //MD_LOG("[MD]:   from static to motion  !!!! \r\n");  //acc > acc_threshold about 3cycles
        //ls_api_event_input(LS_EVT_MOTION_DETECTED);
        //md.motion_alert_flag = true;
        break;

    case AL_EVT_ORIENT: // do nothing.
        //MD_LOG("[MD]:   AL_EVT_ORIENT!!!! \r\n");
        break;
    default:
        break;


    }
}

static void md_orient_change_handler(orient_dir_t dir)
{
    if (on_orient_callback != NULL)
    {
        on_orient_callback(dir);
    }
    switch (dir)
    {
    case ORIENT_INVALID:
        //MD_LOG("[MD]: On Changing\r\n");
        break;
    case ORIENT_POS_X:
        //MD_LOG("[MD]: X - POS\r\n");
        break;
    case ORIENT_NEG_X:
        //MD_LOG("[MD]: X - NEG\r\n");
        break;
    case ORIENT_POS_Y:
        //MD_LOG("[MD]: Y - POS\r\n");
        break;
    case ORIENT_NEG_Y:
        //MD_LOG("[MD]: Y - NEG\r\n");
        break;
    case ORIENT_POS_Z:
        //MD_LOG("[MD]: Z - POS\r\n");
        break;
    case ORIENT_NEG_Z:
        //MD_LOG("[MD]: Z - NEG\r\n");
        break;
    }
}

void md_motion_int_en(uint32_t  pin)
{    
    nrf_gpio_pin_set(pin);
    nrf_delay_us(1000);
    nrf_gpio_pin_clear(pin);      
}

void md__int_pin_init(void)
{
    nrf_gpio_cfg_output(EINT1_MOTION_ALERT_PIN);   
    nrf_gpio_pin_clear(EINT1_MOTION_ALERT_PIN);
}



void md_set_wakeup(void)
{
    md.wakeup = true;
    md.static_detct_cnt = 0;
    //md_motion_int_en(EINT1_MOTION_ALERT_PIN);  // chen for test 2019/07/09 
}

bool md_get_wakeup(void)
{
    return md.wakeup;
}


static void md_start_work(void)
{
    LIS3DH_WriteReg(0x3e,0x00); //act threshold 2*32mg=64mg   
    LIS3DH_WriteReg(0x3f,0x00); //act duration
    LIS3DH_WriteReg(LIS3DH_CTRL_REG3,0x00);
    LIS3DH_WriteReg(LIS3DH_CTRL_REG6,0x00);    
    
    md_timer_start();
    md.timeout = true;
    md.static_detct_cnt = 0; //chen 2019/06/22
    algo_lib_reset();    
    static_restart_update();
    
    md.static_flag = 0;
    md.motion_flag = 1;
    md.motion_ind_flag = 1;
    md.motion_detct_cnt = 0;
    md.static_detct_cnt = 0;
    
    MD_LOG("[MD]: work mode\r\n");
}


static void md_start_sleep(void)
{
//    lis3dh_stay_in_sleep_mode();
    LIS3DH_WriteReg(0x3e,0x8); //act threshold 8*16mg=128mg   
    LIS3DH_WriteReg(0x3f,0x02); //act duration
    LIS3DH_WriteReg(LIS3DH_CTRL_REG3,0x04);
    LIS3DH_WriteReg(LIS3DH_CTRL_REG6,0x0A);
    
    md_timer_stop();
    md.workmode = false;
    md.timeout = false;
    md.wakeup=false; //chen
    md.motion_flag = 0;
    md.static_flag = 1;
    md.motion_ind_flag = 0; //2019/08/30
    
    md.motion_detct_cnt=0;
    md.motion_alert_flag = 0;
    md.static_detct_cnt = 0;
    MD_LOG("[MD]: sleep mode\r\n");
}


void md_motion_or_static_alert_judge(void)
{
 /*   
    if(md.static_flag==1)
    {
        if(md.static_alert_flag && md.motion_flag==3)
        {
            md.motion_flag =0;
//            md_motion_int_en(EINT1_MOTION_ALERT_PIN);
//            MD_LOG("[MD]: static alert generated \r\n");
        }
        
        if(md.motion_flag ==0)
        {
            if((md.motion_detct_cnt++)>dev_config_get_alert_motion_setup_time())
            {
                md.motion_flag =1;
                md.motion_detct_cnt=0;
            }
        }        
        
    }
    else if(md.motion_flag==2 )
    {
        if((md.motion_detct_cnt++)>dev_config_get_alert_motion_action_time())
        {
            md.motion_flag =0;
            md.motion_detct_cnt=0;
            md.motion_alert_flag = 3;  //motion confirmed
//            md_motion_int_en(EINT1_MOTION_ALERT_PIN);
//            MD_LOG("[MD]: motion alert generated \r\n");
        }
        else
        {
            if(md.static_detct_cnt>=5*30)//static 30s
            {
                md.motion_flag = 0;
                md.motion_detct_cnt=0;
                //md.motion_alert_flag = 0;
            }
        }
        
    }
*/ 
//        static uint8_t last_tmp;
//        uint8_t tmp = m_state_motion.state;//sg_sNomotionInfo.state;
//        
//        if(last_tmp!=tmp)
//        {
//            MD_LOG("[MD]: motion state :%d", tmp);
//            last_tmp=tmp;
//        }

        uint8_t int_en_satus=0;
      
        if(md.fall_down_flag && pin_int_en.int_en.int_en_bits.falldown_int_en)
        { 
            MD_LOG("[MD]: fall_down_flag alert INT generated \r\n");
            int_en_satus = 1;
            //md_motion_int_en(EINT1_MOTION_ALERT_PIN);
        }                   
        if(md.tilt_flag && pin_int_en.int_en.int_en_bits.tilt_int_en)
        {   
            MD_LOG("[MD]: tilt_flag alert INT generated \r\n");
            //md_motion_int_en(EINT1_MOTION_ALERT_PIN);
            int_en_satus = 1;
        }
        if(md.static_alert_flag && pin_int_en.int_en.int_en_bits.static_int_en)
        {
            MD_LOG("[MD]: static_alert_flag alert INT generated \r\n");
            //md_motion_int_en(EINT1_MOTION_ALERT_PIN);
            int_en_satus = 1;
        }
        if(md.motion_alert_flag && pin_int_en.int_en.int_en_bits.motion_int_en)
        {
            MD_LOG("[MD]: motion_alert_flag alert INT generated \r\n");
            //md_motion_int_en(EINT1_MOTION_ALERT_PIN);
            int_en_satus = 1;
        }
        if(md.motion_ind_flag==1 && pin_int_en.int_en.int_en_bits.motion_state_change_int_en)//08/30
        {
            md.motion_ind_flag |= 0x80;               //alert once
            //md_motion_int_en(EINT1_MOTION_ALERT_PIN);
            int_en_satus = 1;
        }
        
        if(batt_level_get()<pin_int_en.batt_threshold && pin_int_en.int_en.int_en_bits.batt_lvl_change_thres)
        {
            MD_LOG("[MD]: batt change_thres alert INT generated \r\n");
            //md_motion_int_en(EINT1_MOTION_ALERT_PIN);
            int_en_satus = 1;
        }
        
        if(int_en_satus)
        {
            md_motion_int_en(EINT1_MOTION_ALERT_PIN);
        }
    
 
    
    
    
    
}



static void md_mode_change(void)
{
    if (md.wakeup)
    {
        md.wakeup = false; //chen
//        (void)lis3dh_read_int1_src();
//        LIS3DH_GetFifoSourceReg(&fifo_src);

        if (!md.workmode)
        {
            md_start_work();
            md.workmode = true;
        }
    }
}






static float inv_sqrt(float x)
{
    float xhalf = 0.5f * x;
    int i = *(int *)&x;

    i = 0x5f375a86 - (i >> 1); // this is magic
    x = *(float *)&i;
    x = x * (1.5f - xhalf * x * x);     // 1st newton iteration
    x = x * (1.5f - xhalf * x * x);     // repeating increases accuracy,  this can be remove

    return 1 / x;
}


static bool IsKeeprelative(short *data)
{
    static    short   accMpssbuf[3];
    static    uint8_t times = 0;
    if ((((accMpssbuf[0] - 60) < data[0]) && (data[0] < (accMpssbuf[0] + 60))) &&
        (((accMpssbuf[1] - 60) < data[1]) && (data[1] < (accMpssbuf[1] + 60))) &&
        (((accMpssbuf[2] - 60) < data[2]) && (data[2] < (accMpssbuf[2] + 60))))
    {
        times++;
        if (times >= 30)
        {
            times = 30;

            for (uint8_t i = 0; i < 3; i++)
            {
                accMpssbuf[i] = data[i];
            }
            return 1;
        }
    }
    else
    {
        times = 0;
        for (uint8_t i = 0; i < 3; i++)
        {
            accMpssbuf[i] = data[i];
        }
    }
    return 0;
}

static void md_algorithm_run(void)
{    
    if (!md.timeout) return;
    md.timeout = false;
    
    uint8_t bytes_in_fifo ;
    LIS3DH_GetFifoSourceFSS(&bytes_in_fifo);
    //LIS3DH_GetAccAxesRaw(raw_data, LIS3DH_FIFO_MAX_BYTES);
    if (bytes_in_fifo == 0) return;
    //NRF_LOG_INFO("[MD]: bytes_in_fifo= %d\r\n", bytes_in_fifo);
    AxesRaw_t raw_data[32];     //2019/07/29
    for(uint32_t i=0; i<bytes_in_fifo; i++)
    {
      LIS3DH_GetAccAxesRaw(&raw_data[i]); 
//      NRF_LOG_INFO("x=%d y=%d z=%d", raw_data[i].X/16, raw_data[i].Y/16, raw_data[i].Z/16);      
    }
    vector3_t r3;
    
//#ifdef __SIMPLE_ACC
//#warning "only test"
//    G_sensor_report_data(raw_data, bytes_in_fifo);
//#endif

    for (uint8_t i = 0; i < bytes_in_fifo; i ++) //+6
    {
//        r3.X = (int16_t)((raw_data[i + 1] << 8) | raw_data[i + 0]);
//        r3.Y = (int16_t)((raw_data[i + 3] << 8) | raw_data[i + 2]);
//        r3.Z = (int16_t)((raw_data[i + 5] << 8) | raw_data[i + 4]); 
        
//        LIS3DH_GetAccAxesRaw((AxesRaw_t*)&r3);//2019/07/29
        
        r3.X = raw_data[i].X;
        r3.Y = raw_data[i].Y;
        r3.Z = raw_data[i].Z;
        
        r3.X = r3.X / (1 << 4);
        r3.Y = r3.Y / (1 << 4);
        r3.Z = r3.Z / (1 << 4); 
#ifdef DEGREE
        md_module_input_gsensor_raw_handle(&r3);
#endif
        //input the step run.
        static uint32_t rawcount = 0;
        if (++rawcount % 3 == 0)
        {
            AxesRaw_t rawcurrent;
            rawcurrent.X = r3.X;
            rawcurrent.Y = r3.Y;
            rawcurrent.Z = r3.Z;
            m_interface_input_gsensor_source_data(rawcurrent);  //step cnt 
        }
        
        state_input_data(r3.X, r3.Y, r3.Z);
        //falldown and tilt check
        bool device_is_activated = false;
        short active_value[3];
        active_value[0] = r3.X;
        active_value[1] = r3.Y;
        active_value[2] = r3.Z;
        
        device_is_activated = IsKeeprelative(active_value);  //chen --for static or motion detect
        algo_lib_runtime(&r3, device_is_activated);


#if 0        
        if (true == tilt_get_switch())
        {
            tilt_moudle_gsensor_data_handler(&r3, true); //
        }


        
        if (true == fall_get_switch())
        {
            uint16_t xx = SHORT_ABS(r3.X); //
            uint16_t yy = SHORT_ABS(r3.Y);
            uint16_t zz = SHORT_ABS(r3.Z);
            uint32_t Raw_input_data = (uint32_t)(xx * xx + yy * yy + zz * zz);
            Raw_input_data = (uint32_t)inv_sqrt((float)Raw_input_data);
            float Fall_current_accel = Raw_input_data * 1.0 / 512;           
            fall_detector_state_handler(Fall_current_accel, device_is_activated);
        }
#endif
        
    }
    
          
    
    if (algo_lib_stay_in_quiet() && md.static_flag==0)
    {             
        md_start_sleep();
    }
    
}



// to check motion-detect timeout threshold
void md_timeout_check(void)
{ 
    MotionAlertLoop();         
}


// sensor read data & algorithm
void md_process(void)
{
    md_mode_change();
    md_algorithm_run();
    md_timeout_check();
}


// app-timer timeout handle
static void md_timer_timeout_handle(void *p_context)
{
    md.timeout = true;
}
 

void md_conf_change_on_motion(void)
{
    state_config_motion_flag_update();
    md.motion_alert_flag=0;
}

void md_conf_change_on_static(void)
{
    state_config_static_flag_update();
    md.static_alert_flag = 0;
}
void md_conf_change_on_tilt(void)
{
    if (dev_config_get_alert_tilt_enable() == false)
    {
        tilt_set_switch(false);
        return;
    }
    tilt_set_switch(true);
    tilt_user_config_t input;
    input.user_alarm_angle_value = dev_config_get_alert_tilt_angle();
    input.user_alarm_time_value = dev_config_get_alert_tilt_threshold();
    tilt_module_config_angle_alarm(&input);
    
    md.tilt_flag=0;
}

void md_conf_change_on_falldown(void)
{
       algo_lib_param_update();
       md.fall_down_flag = 0;
}

void dev_config_value_change(uint32_t conf_update)
{
   

    switch (conf_update)
    {

//    case DEV_UPDATE_ALERT_MOTION:
//        md_conf_change_on_motion();        
//        break;
//    case DEV_UPDATE_ALERT_STATIC:
//        md_conf_change_on_static();       
//        break;
//    case DEV_UPDATE_ALERT_TILT:
//        md_conf_change_on_tilt();
//        break;
//    case DEV_UPDATE_ALERT_FALLDOWN:
//       md_conf_change_on_falldown(); 
//        break;
    default:
        break;

    }
    
}



void md_init(void)
{
    memset(&md, 0, sizeof(md));
    
    
    LIS3DH_INT_PIN_init();
    LIS3DH_SpiInit();
//    dev_conf_init_t  dev_conf_obj;
//    dev_conf_obj.cb = dev_config_value_change;    
//    dev_conf_init(&dev_conf_obj);

    // init mems
//    lis3dh_init_t   lis3dh_init_obj;
//    lis3dh_init_obj.evt_handle = md_wakeup;
//    lis3dh_init(&lis3dh_init_obj);
    
//    md__int_pin_init();  // 2019/07/11
    
    // register algorithm evt callback
    algo_init_t     algo_init_obj;

    algo_init_obj.cfg.orient.enable = false;
    algo_init_obj.cfg.falldown.enable = false;

    algo_init_obj.evt.cb = md_evt_handler;
    algo_init_obj.evt.orient = md_orient_change_handler;
    algo_lib_init(&algo_init_obj);

    uint32_t err_code = app_timer_create(&md_timer, APP_TIMER_MODE_REPEATED, md_timer_timeout_handle);
    if (err_code != NRF_SUCCESS)
    {
        APP_ERROR_CHECK(err_code);
    }

    // force work mode while power on.
    md.wakeup = true;
    md.workmode = false;
 
    
//    dev_config_value_change(DEV_UPDATE_ALERT_MOTION);
//    dev_config_value_change(DEV_UPDATE_ALERT_STATIC);
//    dev_config_value_change(DEV_UPDATE_ALERT_TILT);
//    dev_config_value_change(DEV_UPDATE_ALERT_FALLDOWN);
//    algo_lib_param_update();   //set default falldown param

}

#endif

