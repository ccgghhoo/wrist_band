#include "sport_degree_count.h"
//#include "Datetime.h"
//#include "HalActivityCFS.h"
#if 1
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#else
#define HZH_LOG(...)
#endif
#define APP_LOG(...)

#define SHORT_ABS(x)        				(( ((x) & 0x8000) > 0) ? ((~x) + 1)  : (x) )
#define SAMPLE_RATE

static float 		     m_one_second_value_sum    = 0;
static float    	         m_gsensor_period_sum      = 0;
md_gsensor_degree_t 	     m_new_degree_value;


static float inv_sqrt(float x)  //高效的开方算法
{
    float xhalf = 0.5f * x;
    int i = *(int *)&x;

    i = 0x5f375a86 - (i >> 1); // this is magic
    x = *(float *)&i;
    x = x * (1.5f - xhalf * x * x);     // 1st newton iteration
    x = x * (1.5f - xhalf * x * x);     // repeating increases accuracy,  this can be remove

    return 1 / x;
}


void md_module_input_gsensor_raw_handle(vector3_t *p_input_gsensor_raw)
{
    if (NULL == p_input_gsensor_raw)
    {
        return;
    }

    // scalar used to check motion/static
    uint32_t x = SHORT_ABS(p_input_gsensor_raw->X);
    uint32_t y = SHORT_ABS(p_input_gsensor_raw->Y);
    uint32_t z = SHORT_ABS(p_input_gsensor_raw->Z);

    uint32_t temp = (uint32_t)(x * x + y * y + z * z);
    temp = (uint32_t)inv_sqrt((float)temp);

    float one_point_value = (temp)*1.0 / 512;
    one_point_value -= 0.98;

    if (one_point_value < 0.0)
    {
        one_point_value = -one_point_value;
    }
    m_one_second_value_sum += one_point_value;
}


void md_module_period_one_second(void) /// ON SECONDS
{
    m_one_second_value_sum /= 100.0; //100 point one second time.

    m_gsensor_period_sum += m_one_second_value_sum;

    m_one_second_value_sum 	= 0;

}


//void md_module_period_save(void) ///10 MINTUNES EVENT
//{
//    if (m_gsensor_period_sum < 0.001) //判定数值是否过小
//    {
//        //  return;
//    }
//    //get the period sum
//    float temp = m_gsensor_period_sum * 10000; //
//    m_new_degree_value.gsensor_point_value = (uint32_t)temp; // for module test
//    m_new_degree_value.utc = UTC_GetValue();


//    if (m_new_degree_value.gsensor_point_value  < 10)
//    {
//        m_new_degree_value.gsensor_point_value = 10;
//    }

//    HalActivityCFS_Write((uint8_t *)&m_new_degree_value, sizeof(md_gsensor_degree_t)); 
//    //      bool flag = Tinyfs_push_new_record((uint8_t *)&m_new_degree_value, sizeof(md_gsensor_degree_t));

//    NRF_LOG_HEXDUMP_INFO((uint8_t *)&m_new_degree_value, 8);

//    //clear the sum
//    m_gsensor_period_sum = 0;
//}
