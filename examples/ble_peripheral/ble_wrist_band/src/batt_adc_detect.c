/*
*
*
*/
#include "nrf_saadc.h"
#include "nrfx_saadc.h"
#include "nrf_drv_saadc.h"

#include "app_init.h"


#define  BATT_SAMPLE_NUM        8
#define  BATT_AN_PIN            NRF_SAADC_INPUT_AIN3
#define  ADVALUE_TO_MV_RATIO    600*1117/(100*4096)//600*11/4096 //根据电路修正值比例为11.17
#define MIN_BATTER_ADC  			2013  	//3.3v
#define MIN_BATTER_ADC_CHARGING     (MIN_BATTER_ADC+50)//3.3+0.05
#define MAX_BATTER_ADC  			2503  	//4.1v
#define MAX_BATTER_ADC_CHARGING     (MAX_BATTER_ADC+50)//4.1+0.08
#define BATT_LOW_POWER_LEVEL        20

#define BIT_MASK_CHARGING           0x01
#define BIT_MASK_CHARGING_FULL      0x02
#define BIT_MASK_BATT_LOW           0x08
#define BIT_MASK_BATT_LVL_UPDATE    0x04

typedef struct
{
    uint16_t adc_value;
    uint8_t  batt_level;
    uint8_t  batt_level_curr;
    union{
        struct{    
            uint8_t  charging_flag:1;
            uint8_t  charg_full_flag:1;
            uint8_t  batt_level_update_flag:1;
            uint8_t  batt_low_flag:1;
            uint8_t  noused:4;
        };
        uint8_t batt_status_byte;
    };
    
}batt_state_t ;

batt_state_t  m_battery;

static nrf_saadc_value_t     m_batt_adc_buffer[BATT_SAMPLE_NUM];
static uint32_t              m_batt_mv_av;
//static bool                 batt_av_saadc_done_flag;


void batt_charg_ind_pin_init(void);


void saadc_callback(nrf_drv_saadc_evt_t const * p_event)
{
    //NRF_LOG_INFO("p_event->type: %d n\r", p_event->type);
    
    if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
    {
        ret_code_t err_code;
        
        err_code = nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, BATT_SAMPLE_NUM);// init easyDMA
        APP_ERROR_CHECK(err_code);
        
        int i, batt_sum=0;
        
        for (i = 0; i < BATT_SAMPLE_NUM; i++)
        {
            batt_sum+=p_event->data.done.p_buffer[i];
            //NRF_LOG_INFO("%d", p_event->data.done.p_buffer[i]);
        }
        batt_sum /= BATT_SAMPLE_NUM;
        m_batt_mv_av = batt_sum*ADVALUE_TO_MV_RATIO;     //Vref=600mv  resolution=12bit  PCB resistor ratio=1/11
        //batt_av_saadc_done_flag = true;
        NRF_LOG_INFO("batt adc value: %d \n\r", batt_sum);
        NRF_LOG_INFO("batt voltage : %d mv \n\r", m_batt_mv_av);
    }
}



void batt_adc_init(void)
{
    ret_code_t err_code;
    nrf_saadc_channel_config_t channel_config =
        NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(BATT_AN_PIN);
    
    channel_config.gain = NRF_SAADC_GAIN1;  //gain=1
    nrfx_saadc_config_t  saadc_config;
    saadc_config.low_power_mode=1;
    saadc_config.resolution = NRF_SAADC_RESOLUTION_12BIT;
    saadc_config.oversample = NRF_SAADC_OVERSAMPLE_DISABLED;
    saadc_config.interrupt_priority = 6;
    err_code = nrf_drv_saadc_init(&saadc_config, NULL/*saadc_callback*/);
    //err_code = nrf_drv_saadc_init(NULL, saadc_callback);//用默认的竟然不行 不知道为什么
    APP_ERROR_CHECK(err_code);
    
    err_code = nrfx_saadc_channel_init(0, &channel_config);
    APP_ERROR_CHECK(err_code);
    
    //    err_code = nrfx_saadc_buffer_convert(m_batt_adc_buffer, BATT_SAMPLE_NUM);  
    //    APP_ERROR_CHECK(err_code);
    
    nrfx_saadc_sample();
    
    battery_level_cal();
    
    batt_charg_ind_pin_init();
    
}

void batt_adc_uninit(void)
{    
    nrfx_saadc_uninit();
}

void batt_start_measure(void)
{    
    nrfx_saadc_sample(); // only start adc sample once
}

int16_t batt_adc_value_get(void)
{
    int16_t bat_value;
    ret_code_t err_code;
    err_code = nrfx_saadc_sample_convert(BATT_AN_PIN, &bat_value); //cpu is blocked until ad convert ended
    //    NRF_LOG_INFO("adc sample error code : %x \n\r", err_code);   
    if(err_code!=NRF_SUCCESS)
        return 0;
    return bat_value;
}

int16_t batt_adc_value_av_get(void)
{
    uint32_t batt_value_sum =0;
    for(uint32_t i =0; i<BATT_SAMPLE_NUM; i++)
    {
        m_batt_adc_buffer[i] = batt_adc_value_get();
        batt_value_sum += m_batt_adc_buffer[i];
    }
    batt_value_sum /= BATT_SAMPLE_NUM;
    
    return batt_value_sum;
}

uint32_t batt_voltage_av_get(void)
{    
    return m_batt_mv_av;
}

int16_t batt_voltage_get(void)
{
    int16_t bat_value;
    uint32_t batt_mv;
    bat_value=batt_adc_value_av_get();
    batt_mv = bat_value*ADVALUE_TO_MV_RATIO;
    //NRF_LOG_INFO("batt advalue: %d  ", bat_value);
    NRF_LOG_INFO("batt mv: %d  ", batt_mv); 
    return batt_mv;
}



uint8_t battery_level_cal(void)
{
    
    uint32_t  batt_level;
    uint16_t  min_batt_adc ,max_batt_adc;
    static uint8_t   low_power_cnt=0;
    
    int16_t batt_adc_value = batt_adc_value_av_get();
    
    uint32_t batt_mv = batt_adc_value*ADVALUE_TO_MV_RATIO;
    NRF_LOG_INFO("batt advalue: %d  ", batt_adc_value);
    NRF_LOG_INFO("batt mv: %d  ", batt_mv); 
    
    if(m_battery.charging_flag)
    {
        min_batt_adc =  MIN_BATTER_ADC_CHARGING;
        max_batt_adc =  MAX_BATTER_ADC_CHARGING;
    }
    else
    {             
        min_batt_adc =  MIN_BATTER_ADC; 
        max_batt_adc =  MAX_BATTER_ADC;
    }
    
    if(batt_adc_value>=max_batt_adc) 
    {
        batt_level = 100;
        if(m_battery.charging_flag)
        {
            m_battery.charg_full_flag = 1;
        }
    }
    else if(batt_adc_value<=min_batt_adc)
    {
        batt_level = 0;
    }
    else
    {     
        batt_level = (batt_adc_value-min_batt_adc)*100/(max_batt_adc-min_batt_adc);
    }
    
    if((m_battery.batt_level-batt_level)>2)
    //if(m_battery.batt_level!=batt_level)
    {
        m_battery.batt_level = batt_level;
        m_battery.batt_level_update_flag = 1;
    }
    else
    {
        m_battery.batt_level_update_flag = 0;
    }
    
    if(batt_level<BATT_LOW_POWER_LEVEL)
    {
        if(++low_power_cnt>=2)
        {
            low_power_cnt=2;
            m_battery.batt_low_flag = 1;
        }
    }
    else
    {
        low_power_cnt=0;
        m_battery.batt_low_flag = 0;
    }
           
    m_battery.batt_level_curr = batt_level;
    
    NRF_LOG_INFO("batt level: %d  ", m_battery.batt_level_curr); 
    
    return batt_level;
}


uint8_t batt_level_get(void)
{       
    return  m_battery.batt_level_curr;
}

bool batt_low_alert_get(void)
{
    return  m_battery.batt_low_flag;
}

bool batt_state_changed(void)
{       
    return  (m_battery.batt_level_update_flag == 1)||(m_battery.charging_flag==1)||(m_battery.batt_low_flag==1);
}

void batt_clear_adv_update_flag(void)
{
    m_battery.batt_level_update_flag = 0;
}


//------------------  batt  charge  ------------------
void  batt_charg_enable(bool enable)
{
    if(enable)
    {
		m_battery.charging_flag = 1;     //usb plug in
        nrf_gpio_pin_clear(CHARG_EN_PIN);      
    }
    else
    {
        m_battery.charging_flag = 0;     //out
        nrf_gpio_pin_set(CHARG_EN_PIN);     
    }
    
}


void batt_charg_ind_pin_init(void)
{ 
    
    nrf_gpio_cfg_output(CHARG_EN_PIN);
    nrf_gpio_pin_set(CHARG_EN_PIN);  
    
    nrf_gpio_cfg_input(USB_CHARG_IND_PIN, NRF_GPIO_PIN_NOPULL);//NRF_GPIO_PIN_PULLDOWN
    
}

bool batt_charging_check(void)
{
    if(nrf_gpio_pin_read(USB_CHARG_IND_PIN))
    {
        if(m_battery.charging_flag==0)
        {
            batt_charg_enable(true);
            //Indicator_Evt(ALERT_TYPE_IN_CHARGING);
        }
    }
    else
    {
        if(m_battery.charging_flag)
        {
            batt_charg_enable(false);
            //Indicator_Evt(ALERT_TYPE_DISCHARGE);
        }
    }
    
    return m_battery.charging_flag==1;
}

bool batt_charging_flag_get(void)
{
    return m_battery.charging_flag==1;
}


batt_status_t batt_status_get(void)
{
    static uint8_t  pre_batt_state=0;
    batt_status_t  ret=BATT_STATUS_NORMAL;
    
    if(m_battery.charging_flag)
    {
        if(m_battery.charg_full_flag)
        {
            //if(!(pre_batt_state&BIT_MASK_CHARGING_FULL))
            {   
                Indicator_Evt(ALERT_TYPE_CHARGE_FULL);
            }
            ret=  BATT_STATUS_CHARG_FULL;
        }
        else
        {
            if(!(pre_batt_state&BIT_MASK_CHARGING))
            {
                Indicator_Evt(ALERT_TYPE_IN_CHARGING);  
            }
            //NRF_LOG_INFO("batt status charging "); 
            ret=  BATT_STATUS_CHARGING;
        }
    }
    else
    {
        if(pre_batt_state&BIT_MASK_CHARGING)
        {
            Indicator_Evt(ALERT_TYPE_DISCHARGE);       
            m_battery.charg_full_flag = 0;
        }
        
        if(m_battery.batt_low_flag)
        {
            Indicator_Evt(ALERT_TYPE_BATTERY_POWER_LOW);
            //NRF_LOG_INFO("batt status low power ");
            ret=  BATT_STATUS_LOW_POWER;
        }
        else
        {
            if(pre_batt_state&ALERT_TYPE_BATTERY_POWER_LOW)
            {
                Indicator_Evt(ALERT_TYPE_BATTERY_POWER_NORMAL);
            }            
            //NRF_LOG_INFO("batt status normal ");
            ret=  BATT_STATUS_NORMAL;
        }
        
    }
    
    pre_batt_state = m_battery.batt_status_byte;
    
    //NRF_LOG_INFO("batt status = %x" ,m_battery.batt_status_byte);
    
    return ret;
}








