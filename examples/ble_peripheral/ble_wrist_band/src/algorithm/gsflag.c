

#include "nrf.h"
#include "gsflag.h"
//#include "DateTime.h"
#include "m_step.h"
//#include "indicator.h"
//#include "loc_ble.h"
//#include "loc_gsm.task.h"


#if 1
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#define DEBUG       NRF_LOG_INFO
#else
#define DEBUG(...)
#endif

#define GPS_WORK_TIME_IN_NOMOTION_SOS_STATUS (3600)// !<<<


static gsflag_t 				 gs;
static loc_alert_code_t   		 m_alert_code;
static loc_general_data_t 		 m_general_data;


typedef struct
{
    uint32_t steps;
} function_gsensor_data_t;

static function_gsensor_data_t   m_step_count =
{
    .steps = 0,
};

static uint8_t m_system_zone = 0;





uint8_t loc_get_ble_system_zone(void)
{
    return m_system_zone;
}

void loc_set_ble_system_zone(uint8_t input)
{
    m_system_zone = input;
}


bool loc_get_ble_connect_flag(void)
{
    return gs.BLE;
}

void loc_set_ble_connect_flag(bool input)
{
    if (input)
    {
        ble_task_start_connect();
        Indicator_Evt(ALERT_TYPE_BLE_CONNECTED);
        bl_user_data_change(BL_ADV_STATE, BA_BLE_CONNECTED, true);
    }
    else
    {
        ble_task_start_disconnect();
        Indicator_Evt(ALERT_TYPE_BLE_DISCONNECTED);
        bl_user_data_change(BL_ADV_STATE, BA_BLE_CONNECTED, false);
    }
    gs.BLE = input;
}



void loc_clear_history_exist_flag(void)
{
    m_general_data.Status.Historical_data = 0;
}

void loc_set_gsm_signal(uint8_t input)
{
    m_general_data.Status.GSM_signal_strength = input;
}

void loc_set_battery_value(uint8_t input)
{
    m_general_data.Status.Battery_level = input;
}
void loc_set_motion_value(bool motion)
{
    m_general_data.Status.Motion = motion;
}


void loc_add_step(void)
{
    ++m_step_count.steps;
}
void loc_set_step_count(uint32_t input)
{
    m_step_count.steps = input;
    if (0 == input)
    {
        SmartWear_SportHealth_Sport_Step_Reset();
    }
}

uint32_t loc_get_step_count(void)
{
    return m_step_count.steps;
}




void general_data_clear_wifi_tower(void)
{
    m_general_data.Status.wifi_tower = 0;
}

void general_data_set_wifi_tower(void)
{
    m_general_data.Status.wifi_tower = 1;
}

void general_data_set_signal_strength(uint8_t input)
{
    m_general_data.Status.GSM_signal_strength = input;
}

void general_data_set_battery(uint8_t input)
{
    m_general_data.Status.Battery_level = input;
}


__INLINE loc_general_data_t* general_data_get(void)
{
    return &m_general_data;
}


uint32_t GSFlag_GetGeneralDataValue()
{
    return m_general_data.Status.value;
}






__INLINE uint32_t gsflag_get(void)
{
    return gs.value;
}
/**@brief 清除报警标志
 * @param null
 * @param null
 * @return
 * @return
*/
void gsflag_reset(void)
{
    // comment the flag not clear
    uint32_t f =
       0
//        ( 1 << GS_POS_GPS )
       |   (1 << GS_POS_REBOOT)
//    |   ( 1 << GS_POS_BLE )
//    |   ( 1 << GS_POS_CHARGE )
//    |   ( 0x0f << GS_POS_WORK_MODE )

       |   (1 << GS_POS_BAT_LOW)
       |   (1 << GS_POS_OVER_SPEED)
       |   (1 << GS_POS_FALL_DOWN)
       |   (1 << GS_POS_GEO1)
       |   (1 << GS_POS_GEO2)
       |   (1 << GS_POS_GEO3)
       |   (1 << GS_POS_GEO4)
       |   (1 << GS_POS_TILT)

       |   (1 << GS_POS_MOVEMENT)
       |   (1 << GS_POS_POWER_OFF)
       |   (1 << GS_POS_AGPS)
       |   (1 << GS_POS_MOTION)
       |   (1 << GS_POS_NOMOTION)
       |   (1 << GS_POS_SOS_KEY)
       |   (1 << GS_POS_SIDE_L_KEY)
       |   (1 << GS_POS_SIDE_R_KEY)
    ;
    f = ~f;

    gs.value &= f;

    AlertFlag_Clear();

#if ( BLUETOOTH_DEV_ROLE == PERIPHERAL_MODE )
    bl_user_data_change(BL_ADV_STATE, BA_IN_ALERTS_RESET, false);
#endif
    DEBUG("[GSFLAG]: reset \r\n");
}


//__INLINE void gs_reset_alert_flags(void)
//{
//	gs.Reboot = 0 ;
//}

__INLINE void gs_set_gsm_locator(void)
{
    m_general_data.Status.GSM_tower = 1;
}

__INLINE void gs_clear_gsm_locator(void)
{
    m_general_data.Status.GSM_tower = 0;
}

__INLINE void gs_set_GPS_normal(void)
{
    gs.GPS = 1;

    m_general_data.Status.GPS = 1;
}

__INLINE void gs_set_GPS_abnormal(void)
{
    gs.GPS = 0;
    m_general_data.Status.GPS = 0;
}

__INLINE void gs_set_reboot(void)
{
    gs.Reboot = 0;

    m_general_data.Status.Reboot = 1;
}

__INLINE void gs_clear_reboot(void)
{
    gs.Reboot = 0;
    m_general_data.Status.Reboot = 0;
}



__INLINE void gs_set_charge_full(void)
{
    m_general_data.Status.Charging_complete = 1; //
}

__INLINE void gs_clear_charge_full(void)
{
    m_general_data.Status.Charging_complete = 0; //
}

__INLINE void gs_set_charging(void)
{
    gs.Charge = 1;
    gs.BattLow = 0;
    m_general_data.Status.In_Chargeing = 1;

    loc_gsm_task_setup_musicplay(MUSIC_CHARGING);

    Indicator_Evt(ALERT_TYPE_IN_CHARGING);
#if ( BLUETOOTH_DEV_ROLE == PERIPHERAL_MODE )
    bl_user_data_change(BL_ADV_STATE, BA_BATT_IN_CHARGING, true);
#endif
}
__INLINE void gs_set_discharge(void)
{
    gs.Charge = 0;

    m_general_data.Status.In_Chargeing = 0;
#if ( BLUETOOTH_DEV_ROLE == PERIPHERAL_MODE )
    bl_user_data_change(BL_ADV_STATE, BA_BATT_IN_CHARGING, false);
#endif
}

__INLINE void gs_set_mode(uint8_t mode)
{
    gs.WorkMode = mode;

    m_general_data.Status.Work_mode = mode;
}

__INLINE uint32_t AlertFlag_Get(void)
{
    return m_alert_code.value;
}

void AlertFlag_Clear()
{
    m_alert_code.value =  0;
}
uint32_t AlertFlag_GetUTC()
{
    return  m_alert_code.timestamp;
}


void AlertFlag_Set(loc_alert_code_t *code)
{

    if (code->Fall_Down_Alert == true ||
        code->Battery_Low == true ||
        code->Over_speed_Alert == true ||
        code->Tilt_Alert == true ||
        code->GEO1_Alert == true ||
        code->GEO2_Alert == true ||
        code->GEO3_Alert == true ||
        code->GEO4_Alert == true ||
        code->Motion_Alert == true ||
        code->No_Motion_Alert == true ||
        code->SOS_Key == true ||
        code->Side1_Key == true ||
        code->Side2_Key == true

       )
    {
        m_alert_code.timestamp = UTC_GetValue();
    }


    ModeSingleLoc_UpdateAlert();
    m_alert_code.value |= code->value;

    if (code->Fall_Down_Alert)
    {
#if ( BLUETOOTH_DEV_ROLE == PERIPHERAL_MODE )
        bl_user_data_change(BL_ADV_STATE, BA_IN_FALLDOWN_ALERT, true);
#endif
    }
    if (code->Tilt_Alert)
    {
#if ( BLUETOOTH_DEV_ROLE == PERIPHERAL_MODE )
        bl_user_data_change(BL_ADV_STATE, BA_IN_TILT_ALERT, true);
#endif
    }
    if (code->Motion_Alert)
    {
#if ( BLUETOOTH_DEV_ROLE == PERIPHERAL_MODE )
        bl_user_data_change(BL_ADV_STATE, BA_IN_MOTION_ALERT, true);
#endif
    }

    if (code->No_Motion_Alert)
    {
#if ( BLUETOOTH_DEV_ROLE == PERIPHERAL_MODE )
        bl_user_data_change(BL_ADV_STATE, BA_IN_STATIC_ALERT, true);
#endif
    }





}





__INLINE void gs_set_power_low(void)
{
    gs.BattLow = 1;
#if ( BLUETOOTH_DEV_ROLE == PERIPHERAL_MODE )
    bl_user_data_change(BL_ADV_STATE, BA_BATT_LOW, true);
#endif
}




__INLINE void gs_set_power_normal(void)
{
    gs.BattLow = 0;
#if ( BLUETOOTH_DEV_ROLE == PERIPHERAL_MODE )
    bl_user_data_change(BL_ADV_STATE, BA_BATT_LOW, false);
#endif
}



__INLINE void gs_set_AGPSRE(void)
{
    gs.AGPS_RE = 1;
}
__INLINE void gs_reset_AGPSRE(void)
{
    gs.AGPS_RE = 0;
}


void gs_init(void)
{
    gs.value = 0;



    AlertFlag_Clear();
    loc_alert_code_t alert = { 0 };
    alert.Power_on_Alert = true;
    AlertFlag_Set(&alert);


    m_general_data.Status.value = 0;
    m_general_data.Status.Reboot = 1;

    loc_set_step_count(0);



}




