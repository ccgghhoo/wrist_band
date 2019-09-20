

#ifndef LIS3DH_H__
#define LIS3DH_H__

#include <stdint.h>

#define LIS3DH_FIFO_MAX_SIZE    32
#define LIS3DH_FIFO_MAX_BYTES   (32 * 6)

#define LIS3DH_STATUS_REG_AUX 0x07
#define LIS3DH_OUT_ADC1_L 0x08
#define LIS3DH_OUT_ADC1_H 0x09
#define LIS3DH_OUT_ADC2_L 0x0A
#define LIS3DH_OUT_ADC2_H 0x0B
#define LIS3DH_OUT_ADC3_L 0x0C
#define LIS3DH_OUT_ADC3_H 0x0D
#define LIS3DH_INT_COUNTER_REG 0x0E
#define LIS3DH_WHO_AM_I 0x0F
#define LIS3DH_TEMP_CFG_REG 0x1F
#define LIS3DH_CTRL_REG1 0x20
#define LIS3DH_CTRL_REG2 0x21
#define LIS3DH_CTRL_REG3 0x22
#define LIS3DH_CTRL_REG4 0x23
#define LIS3DH_CTRL_REG5 0x24
#define LIS3DH_CTRL_REG6 0x25
#define LIS3DH_REFERENCE 0x26
#define LIS3DH_STATUS_REG 0x27
#define LIS3DH_OUT_X_L 0x28
#define LIS3DH_OUT_X_H 0x29
#define LIS3DH_OUT_Y_L 0x2A
#define LIS3DH_OUT_Y_H 0x2B
#define LIS3DH_OUT_Z_L 0x2C
#define LIS3DH_OUT_Z_H 0x2D
#define LIS3DH_FIFO_CTRL_REG 0x2E
#define LIS3DH_FIFO_SRC_REG 0x2F

#define LIS3DH_INT1_CFG 0x30
#define LIS3DH_INT1_SRC 0x31
#define LIS3DH_INT1_THS 0x32 

#define LIS3DH_INT1_DURATION 0x33 
#define LIS3DH_INT2_CFG 0x34 
#define LIS3DH_INT2_SRC 0x35 
#define LIS3DH_INT2_THS 0x36 
#define LIS3DH_INT2_DURATION 0x37 

#define LIS3DH_CLICK_CFG 0x38 
#define LIS3DH_CLICK_SRC 0x39 
#define LIS3DH_CLICK_THS 0x3A 
#define LIS3DH_TIME_LIMIT 0x3B 
#define LIS3DH_TIME_LATENCY 0x3C 
#define LIS3DH_TIME_WINDOW 0x3D 
#define LIS3DH_ACT_THS 0x3E 
#define LIS3DH_ACT_DUR 0x3F 


typedef void (* lis3dh_int_evt_handle_t)(void); 

typedef struct{
    lis3dh_int_evt_handle_t    evt_handle; 
}lis3dh_init_t; 

typedef struct{
    lis3dh_int_evt_handle_t     cb; 
}lis3dh_t; 

void lis3dh_read_bytes( uint8_t start, uint8_t *pdata, uint8_t len ); 

uint8_t lis3dh_read_int1_src(void); 
void lis3dh_clear_int1_source(void); 
void lis3dh_setup_to_work(void);
void lis3dh_stay_in_sleep_mode(void);
void lis3dh_stop_work(void); 

uint8_t lis3dh_read_fifo_axis_data( uint8_t * pdata, uint8_t max );
void lis3dh_init(lis3dh_init_t * p_init_obj); 

#endif 

