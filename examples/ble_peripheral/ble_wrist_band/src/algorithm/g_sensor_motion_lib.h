

#ifndef ACCEL_ALGORITHM_LIB_H__
#define ACCEL_ALGORITHM_LIB_H__

#include <stdint.h>
#include <stdbool.h>

#define SAMPLE_FREQ                     ( 100 )      //user set sample rate
#define ONE_POINT_TIME_MS_SIZE		( 1000 / SAMPLE_FREQ )
#define SECONDS_OF_FREQ(x)              ( (x) * (1000 / (ONE_POINT_TIME_MS_SIZE)) )

#define AGRAVITY_OFFSET     12 

#define UNIT_IN_FS          2  // mg 

#define LOST_GRAVITY        0
#define LOST_GRAVITY_OFFSET ( 50 / UNIT_IN_FS ) 
#define IMPACT_GRAVITY      ( 2000 / UNIT_IN_FS) 
#define STATIC_GRAVITY      ( 1000 / UNIT_IN_FS)

#define SCALE_100_MG        ( 100 / UNIT_IN_FS ) 
#define SCALE_200_MG        ( 200 / UNIT_IN_FS ) 
#define SCALE_300_MG        ( 300 / UNIT_IN_FS ) 
#define SCALE_MG(X)         ( X / UNIT_IN_FS ) 

#define MOTION_THRESHOLD    ( 150 / UNIT_IN_FS ) // mg 
#define ALERT_THRESHOLD    ( 200 / UNIT_IN_FS ) // mg 

#define STATIC_THRESHOLD    ( 100 / UNIT_IN_FS ) 

#define MOTION_EXIT_TIME    ( 3000 / SAMPLE_FREQ ) //3second 
#define STATIC_ENTER_TIME   ( 3000 / SAMPLE_FREQ ) 

#define ORIENT_POS_GRAVITY  ( 800 / UNIT_IN_FS ) 
#define ORIENT_NEGA_GRAVITY  ( -800 / UNIT_IN_FS ) 


typedef struct{
    int16_t X; 
    int16_t Y;
    int16_t Z;
}vector3_t; 

typedef enum{
    AL_EVT_STATIC = 0, 
    AL_EVT_MOTION, 
    AL_EVT_ORIENT, 
    AL_EVT_FALLDOWN,
    AL_EVT_TILT, 
}al_cfg_evt_t; 

typedef enum{
    ORIENT_INVALID = 0, 
    ORIENT_POS_X,
    ORIENT_NEG_X,
    ORIENT_POS_Y,
    ORIENT_NEG_Y, 
    ORIENT_POS_Z, 
    ORIENT_NEG_Z, 
}orient_dir_t; 

typedef struct{
    bool            enable; 
}orient_cfg_t; 

typedef struct{
    bool            enable; 
    uint16_t        level;
}falldown_cfg_t; 

typedef struct{
    bool            enable; 
    int16_t         threshold; 
}tilt_cfg_t; 



#if defined(__CC_ARM)
    #pragma push
    #pragma anon_unions
#elif defined(__ICCARM__)
    #pragma language=extended
#elif defined(__GNUC__)
    // anonymous unions are enabled by default
#endif

typedef struct{
    al_cfg_evt_t   evt; 
    union{
        orient_cfg_t     orient; 
        falldown_cfg_t   falldown;
        tilt_cfg_t       tilt; 
    };
}al_cfg_t; 


#if defined(__CC_ARM)
  #pragma pop
#elif defined(__ICCARM__)
  /* leave anonymous unions enabled */
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#endif



typedef void (*evt_handler_t)(al_cfg_evt_t evt_type); 
typedef void (*orient_identied_handle_t)( orient_dir_t dir ); 




typedef struct{
    orient_cfg_t     orient; 
    falldown_cfg_t   falldown;
    tilt_cfg_t       tilt; 
}algo_cfg_t; 

typedef struct{
    evt_handler_t               cb; 
    orient_identied_handle_t    orient;
}algo_evt_t; 

typedef struct{
    algo_cfg_t          cfg; 
    algo_evt_t          evt; 
}algo_init_t; 

typedef struct{
    algo_cfg_t          cfg; 
    algo_evt_t          evt; 
}algo_lib_t; 

void algo_lib_cfg_falldown( bool enable, uint8_t level ); 

void algo_lib_cfg_orient( bool enable ); 

void algo_lib_cfg_tilt( bool enable, int16_t thres ); 

uint16_t algo_lib_version(void); 

bool algo_lib_stay_in_quiet(void); 

void algo_lib_reset(void); 

void algo_lib_runtime( vector3_t *vector, bool temp_device_is_sleep_flag );

void algo_lib_init(algo_init_t *algo_init_obj);
void algo_lib_param_update(void);

#endif 

