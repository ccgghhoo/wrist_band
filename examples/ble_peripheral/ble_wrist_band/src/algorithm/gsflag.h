

#ifndef GSFLAG_H__
#define GSFLAG_H__

#include <stdint.h>
#include <stdbool.h>
#if defined(__CC_ARM)
#pragma push
#pragma anon_unions
#elif defined(__ICCARM__)
#pragma language=extended
#elif defined(__GNUC__)
// anonymous unions are enabled by default
#endif

typedef enum {
    GS_POS_GPS          = 0,
    GS_POS_REBOOT       = 1,
    GS_POS_BLE          = 2,
    GS_POS_CHARGE       = 3,
    GS_POS_WORK_MODE    = 4, //4bit

    GS_POS_BAT_LOW      = 8,
    GS_POS_OVER_SPEED,
    GS_POS_FALL_DOWN,
    GS_POS_GEO1,
    GS_POS_GEO2,
    GS_POS_GEO3,
    GS_POS_GEO4,
    GS_POS_TILT,
    GS_POS_MOVEMENT,
    GS_POS_POWER_OFF,
    GS_POS_AGPS,
    GS_POS_MOTION,
    GS_POS_NOMOTION,
    GS_POS_SOS_KEY,
    GS_POS_SIDE_L_KEY,
    GS_POS_SIDE_R_KEY,
}gs_pos_t;

typedef struct {
    union {
        struct {
            uint32_t GPS: 1;    //1: means GPS has location
            uint32_t Reboot: 1;   //1: means System Reboot
            uint32_t BLE: 1;    //1: means BLE in connection
            uint32_t Charge: 1;   //1: means Device in charge
            uint32_t WorkMode: 4;   //1: work mode


            uint32_t BattLow: 1;  //1: battery power low
            uint32_t OverSpeed: 1;  //1: over speed detected
            uint32_t FallDown: 1;   //1: fall down detected
            uint32_t GEO1: 1;     //1: GEO1 detected
            uint32_t GEO2: 1;     //1: GEO2 detected
            uint32_t GEO3: 1;   //1: GEO3 detected
            uint32_t GEO4: 1;     //1: GEO4 detected
            uint32_t Tilt: 1;       //1: Tilt detected
            uint32_t Movement: 1;   //1: movement detected

            uint32_t PowerOff: 1;   //1: indicate device is ready to power off
            uint32_t AGPS_RE: 1;  //1: indicate AGPS work
            uint32_t Motion: 1;   //1: device under motion
            uint32_t NoMotion: 1;   //1: device no motion
            uint32_t SosKey: 1;   //1: Soskey alert
            uint32_t SideRKey: 1;   //1: Side R key event
            uint32_t SideLKey: 1;   //1: Side L Key event
        };
        uint32_t value;
    };
}gsflag_t;



//define the status of the device.
typedef struct {
    union {
        struct
        {
            uint32_t 	Battery_Low:  		1;
            uint32_t 	Over_speed_Alert: 1;
            uint32_t 	Fall_Down_Alert:	1;
            uint32_t 	Tilt_Alert:			  1;
            uint32_t 	GEO1_Alert:			  1;
            uint32_t 	GEO2_Alert:			  1;
            uint32_t 	GEO3_Alert:			  1;
            uint32_t 	GEO4_Alert:			  1;
            uint32_t 	Power_off_Alert:	1;
            uint32_t 	Power_on_Alert:		1;
            uint32_t 	Motion_Alert:		  1;
            uint32_t 	No_Motion_Alert:	1;
            uint32_t 	SOS_Key:			    1;
            uint32_t 	Side1_Key:			  1;
            uint32_t 	Side2_Key:			  1;
            uint32_t 	reserve_area:     15;
        };
        uint32_t value;
    };
    uint32_t timestamp;
}loc_alert_code_t;





//define the status of the device.
typedef struct
{
    union
    {
        struct
        {
            uint32_t  GPS:1;
            uint32_t  wifi_tower: 	1;
            uint32_t  GSM_tower:		1;
            uint32_t  BLE_Connected: 1;
            uint32_t  In_Chargeing:	1;
            uint32_t  Charging_complete:1;
            uint32_t  Reboot:			1;
            uint32_t Historical_data:1;
            uint32_t AGPSIsValid:1;
            uint32_t Motion:1;
            uint32_t Reserve:6;
            uint32_t Work_mode:		3;  //1:refer to @LOC_MODE_ENUM
            uint32_t GSM_signal_strength:5;
            uint32_t Battery_level:8;
        };
        uint32_t	 		value;
    };
} loc_status_of_device_t;


//define the struct of the general data.
typedef struct
{
    loc_status_of_device_t 	Status;
} loc_general_data_t;





#if defined(__CC_ARM)
#pragma pop
#elif defined(__ICCARM__)
// leave anonymous unions enabled
#elif defined(__GNUC__)
// anonymous unions are enabled by default
#endif



void loc_add_step(void);
void loc_set_agps_work_flag(bool input);
uint8_t loc_get_ble_system_zone(void);
void loc_set_ble_system_zone(uint8_t input);
bool loc_get_ble_connect_flag(void);
void loc_set_ble_connect_flag(bool input);
void loc_clear_history_exist_flag(void);
void loc_set_gsm_signal(uint8_t input);
void loc_set_battery_value(uint8_t input);
void loc_set_motion_value(bool motion);



uint32_t loc_get_step_count(void);

void loc_set_step_count(uint32_t input);


loc_general_data_t* general_data_get(void);

uint32_t gsflag_get(void);
void gsflag_reset(void);

void gs_set_gsm_locator(void);
void gs_clear_gsm_locator(void);
void gs_set_GPS_normal(void);
void gs_set_GPS_abnormal(void);
void gs_set_reboot(void);
void gs_clear_reboot(void);

void gs_set_charge_full(void);
void gs_clear_charge_full(void);
void gs_set_charging(void);
void gs_set_discharge(void);
void gs_set_mode(uint8_t mode);
void gs_set_power_low(void);
void gs_set_power_normal(void);


void gs_set_AGPSRE(void);
void gs_reset_AGPSRE(void);

void gs_init(void);

void AlertFlag_Set(loc_alert_code_t *code);
uint32_t AlertFlag_Get(void);
void AlertFlag_Clear();
uint32_t AlertFlag_GetUTC();
#endif


