

#ifndef STATE_DETECT_H
#define STATE_DETECT_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <stdint.h>


#define MOTION_THRESHOLD_MAX   200000
#define MOTION_THRESHOLD_MIN    70000

#define STATIC_SETUP_TIMER_MIN                  60
typedef enum {
    MOTION_ALERT_STA_NONE,
    MOTION_ALERT_STA_STATIC, //޲ֹƐ֨
    MOTION_ALERT_STA_WAIT_MOTION, //ֈսՋѐ
    MOTION_ALERT_STA_MOTION_TIMEOUT,
    MOTION_ALERT_STA_ALERT,

}MotionAlertState_en;

typedef struct
{
    MotionAlertState_en state;
    uint32_t static_timestramp;
    uint32_t static_count;
    uint32_t motion_count;
    uint32_t cannelCount;
    uint32_t cannelTick;
} state_motion_t;


typedef enum {
    NOMOTION_ALERT_STA_NONE,
    NOMOTION_ALERT_STA_STATIC,
    //NOMOTION_ALERT_STA_ALERT,
    NOMOTION_ALERT_STA_WAIT_MOTION,
    NOMOTION_ALERT_STA_ALERT_CANNEL,
}NomotionAlertState_en;

typedef struct {
    NomotionAlertState_en state;
    bool isAlert;
    uint32_t count;
    uint32_t timestamp;
    uint32_t motionCount;
}NomotionInfo_t;


extern state_motion_t  m_state_motion;
extern NomotionInfo_t  sg_sNomotionInfo;



void static_restart_update(void);

void state_config_motion_flag_update(void);
void state_config_static_flag_update(void);
void state_input_data(int16_t x, int16_t y, int16_t z);
void MotionAlertLoop(void);

#endif

