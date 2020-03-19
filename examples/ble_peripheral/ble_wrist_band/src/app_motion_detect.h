

#ifndef MOTION_DECTION_H__
#define MOTION_DECTION_H__

#include <stdint.h> 
#include <stdbool.h> 


typedef struct
{
    bool timer_started; 
    bool timeout;     
    bool wakeup; 
    bool workmode; 
    bool tilt_flag;   //chen    
    bool fall_down_flag;    
    bool motion_alert_flag;    
    bool static_alert_flag;
    
    uint8_t motion_ind_flag;    
    uint8_t static_flag; 
    uint8_t motion_flag;
    
    uint32_t  static_detct_cnt;
    uint32_t  motion_detct_cnt;
    
}md_t; 

typedef struct{
    union{
        struct{	
            uint32_t motion_int_en: 1; 
            uint32_t static_int_en: 1;
            uint32_t tilt_int_en: 1; 
            uint32_t falldown_int_en: 1;   
            uint32_t motion_state_change_int_en: 1; 
            uint32_t step_over_flow_thres: 1; 
            uint32_t batt_lvl_change_thres: 1; 
            uint32_t reserve: 25;          
        }int_en_bits;    
        uint32_t value; 
    }int_en;
    
    uint16_t  step_threshold;
    uint8_t   batt_threshold;
}alert_int_en_t;
 

extern   alert_int_en_t   pin_int_en; 

#define  WRIST_ROLL_DETECT         1

//#define  EINT2_MOTION_ALERT_PIN    9
#define  EINT1_MOTION_ALERT_PIN    9
#define  MD_STATIC_COUNTER         300 //300*0.2S 

#define  MOTION_ALERT_EVENT        1
#define  NOMOTION_ALERT_EVENT      2
#define  TILT_ALERT_EVENT          3
#define  FALLDOWN_ALERT_EVENT      4


 extern  md_t       md;


typedef void (*on_orient_detected_handle_t)( uint8_t orient ); 

void md_conf_change_on_motion( void ); 
void md_conf_change_on_static( void );
void md_conf_orient_enable( on_orient_detected_handle_t cb );
void md_conf_orient_disable(void);
void md_conf_change_on_tilt(void);

void md_timeout_check(void); 
void md_process(void); 
void md_init(void); 

void md_app_tilt_falldown_event_received(uint32_t input_event);  //chen
void md_set_wakeup(void);
bool md_get_wakeup(void);
uint32_t md_app_get_step_counter(void);
void md_app_set_step_counter(uint32_t step_cnt);
md_t * md_app_get_motion_status(void);
void md_motion_or_static_alert_judge(void);
void md_motion_int_en(uint32_t );


#endif

