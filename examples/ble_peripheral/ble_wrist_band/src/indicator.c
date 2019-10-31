/**
 *  
 * 指示所用的驱动 
 */
#include "config.h"
//#include "Hal.h"
#include "app_timer.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
//#include "battery.h"
#include "indicator.h"
#include "app_init.h"


#define ALERT_TIME_BASE		100
#define TIME_MS(ms)                    (ms/ALERT_TIME_BASE)


static bool m_indicator_timer_is_active = false;
typedef union {
    struct {
        uint32_t led:1;
        uint32_t motor:1;
        uint32_t rvs:30;
    };
    uint32_t value;
}IndEnable_t;

typedef union {
    struct {
        uint32_t red:1;
        uint32_t green:1;
        uint32_t blue:1;
        uint32_t motor:1;
        uint32_t rvs:28;
    };
    uint32_t value;
}IndState_t;

// MARK: Definition
#define APP_TIMER_INTERVAL				APP_TIMER_TICKS(100)
APP_TIMER_DEF(INDICATOR_TIMER);
static void timer_stop(void);
//使能标志位
static IndEnable_t sg_sIndEnable;

typedef struct {
    uint32_t value:30;
    uint32_t on:1;
}IndEvt_t;
typedef struct {
    uint32_t evtMark;
    const IndEvt_t *evts;
    uint8_t sizeOfEvt;
    uint32_t cycle;
}IndEvtList_t;


static IndState_t sg_sIndState = { .value = 0 };


bool Indicator_GetMotor()
{
    return sg_sIndState.motor > 0;
}





#define EVT_TEST              0X10000000

const IndEvt_t csg_sTestEvt[] = {
    { .on = 1, .value = TIME_MS(500), },
    { .on = 0, .value = TIME_MS(500), },
};
#define TEST_LOOP 5


#define LED_RED

#define LED_RED_EVT_KEY_DOWN  0X00080000
#define LED_RED_EVT_KEY_UP    0X00040000
#define LED_RED_EVT_KEY_MARK  0X000C0000


#define LED_RED_EVT_BAT_100   0X00008000
#define LED_RED_EVT_BAT_80    0X00004000
#define LED_RED_EVT_BAT_60    0X00002000
#define LED_RED_EVT_BAT_40    0X00001000
#define LED_RED_EVT_BAT_20    0X00000800

#define LED_RED_EVT_BAT_MARK  0X0000F800
#define LED_RED_EVT_BLE       0X00000200
#define LED_RED_EVT_BLE_ADV   0X00000100  //chen added

#define LED_RED_EVT_CHRG_END   0X00000080
#define LED_RED_EVT_CHRG_ING   0X00000040
#define LED_RED_EVT_CHRG_MARK  (LED_RED_EVT_CHRG_END |LED_RED_EVT_CHRG_ING)

#define LED_RED_EVT_BAT_LOW    0X00000020






/**
 * 充电的状态表
 * 
 * @author hzhmcu (2018/9/21)
 */
const IndEvt_t csg_sKeyDownEvt[] = {
    { .on = 1, .value = TIME_MS(10000), },
};
const IndEvt_t csg_sKeyUpEvt[] = {
    { .on = 0, .value = TIME_MS(500), },
};
const IndEvt_t csg_sChargEvt[] = {
    { .on = 1, .value = TIME_MS(1000), },
    { .on = 0, .value = TIME_MS(5000), },
};
const IndEvt_t csg_sPower20Evt[] = {
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(2900), },
};

const IndEvt_t csg_sPower40Evt[] = {
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(300), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(2500), },
};
const IndEvt_t csg_sPower60Evt[] = {
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(300), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(300), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(2100), },
};

const IndEvt_t csg_sPower80Evt[] = {
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(300), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(300), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(300), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(1700), },
};
const IndEvt_t csg_sPower100Evt[] = {
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(300), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(300), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(300), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(300), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(1300), },
};

const IndEvt_t csg_sBatLowEvt[] = {
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(2900), },
};
const IndEvt_t csg_sChargeEndEvt[] = {
    { .on = 1, .value = TIME_MS(10000), },
};
const IndEvt_t csg_sBLEConnectEvt[] = {
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(300), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(2500), },
};

const IndEvt_t csg_sBLEAdvEvt[] = {     //chen
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(500), },
};


const IndEvtList_t  csg_sIndEvtList_LEDRed[] = {

    {
        .evtMark   = EVT_TEST,
        .evts      = &csg_sTestEvt[0],
        .sizeOfEvt = sizeof(csg_sTestEvt) / sizeof(csg_sTestEvt[0]),
        .cycle     = TEST_LOOP,
    },
    {
        .evtMark   = LED_RED_EVT_KEY_DOWN,
        .evts      = &csg_sKeyDownEvt[0],
        .sizeOfEvt = sizeof(csg_sKeyDownEvt) / sizeof(csg_sKeyDownEvt[0]),
        .cycle     = 0,
    },


    {
        .evtMark   = LED_RED_EVT_KEY_UP,
        .evts      = &csg_sKeyUpEvt[0],
        .sizeOfEvt = sizeof(csg_sKeyUpEvt) / sizeof(csg_sKeyUpEvt[0]),
        .cycle     = 1,
    },

    {
        .evtMark   = LED_RED_EVT_BAT_100,
        .evts      = &csg_sPower100Evt[0],
        .sizeOfEvt = sizeof(csg_sPower100Evt) / sizeof(csg_sPower100Evt[0]),
        .cycle     = 1,
    },


    {
        .evtMark   = LED_RED_EVT_BAT_80,
        .evts      = &csg_sPower80Evt[0],
        .sizeOfEvt = sizeof(csg_sPower80Evt) / sizeof(csg_sPower80Evt[0]),
        .cycle     = 1,
    },
    {
        .evtMark   = LED_RED_EVT_BAT_60,
        .evts      = &csg_sPower60Evt[0],
        .sizeOfEvt = sizeof(csg_sPower60Evt) / sizeof(csg_sPower60Evt[0]),
        .cycle     = 1,
    },
    {
        .evtMark   = LED_RED_EVT_BAT_40,
        .evts      = &csg_sPower40Evt[0],
        .sizeOfEvt = sizeof(csg_sPower40Evt) / sizeof(csg_sPower40Evt[0]),
        .cycle     = 1,
    },
    {
        .evtMark   = LED_RED_EVT_BAT_20,
        .evts      = &csg_sPower20Evt[0],
        .sizeOfEvt = sizeof(csg_sPower20Evt) / sizeof(csg_sPower20Evt[0]),
        .cycle     = 1,
    },
    //
    {
        .evtMark   = LED_RED_EVT_CHRG_ING,
        .evts      = &csg_sChargEvt[0],
        .sizeOfEvt = sizeof(csg_sChargEvt) / sizeof(csg_sChargEvt[0]),
        .cycle     = 0, //无限次
    },
    {
        .evtMark   = LED_RED_EVT_CHRG_END,
        .evts      = &csg_sChargeEndEvt[0],
        .sizeOfEvt = sizeof(csg_sChargeEndEvt) / sizeof(csg_sChargeEndEvt[0]),
        .cycle     = 0, //无限次
    },
    {
        .evtMark   = LED_RED_EVT_BAT_LOW,
        .evts      = &csg_sBatLowEvt[0],
        .sizeOfEvt = sizeof(csg_sBatLowEvt) / sizeof(csg_sBatLowEvt[0]),
        .cycle     = 0, //无限次
    },
    {
        .evtMark   = LED_RED_EVT_BLE,
        .evts      = &csg_sBLEConnectEvt[0],
        .sizeOfEvt = sizeof(csg_sBLEConnectEvt) / sizeof(csg_sBLEConnectEvt[0]),
        .cycle     = 0, //无限次
    },
    
    {
        .evtMark   = LED_RED_EVT_BLE_ADV,
        .evts      = &csg_sBLEAdvEvt[0],
        .sizeOfEvt = sizeof(csg_sBLEAdvEvt) / sizeof(csg_sBLEAdvEvt[0]),
        .cycle     = 0, //无限次
    },

};

#define LED_GREEN

#define LED_GREEN_AT_ERROR         0X00100000

#define LED_GREEN_CALL               0X00100000

#define LED_GREEN_NO_SIM_CARD       0X00080000
#define LED_GREEN_NO_SIM_CARD_MARK  (LED_GREEN_NO_SIM_CARD|LED_GREEN_GPRS|LED_GREEN_GSM|LED_GREEN_AT_ERROR|LED_GREEN_NO_CREG)
#define LED_GREEN_GPRS              0X00040000
#define LED_GREEN_GSM               0X00020000
#define LED_GREEN_NO_CREG           0x00010000

#define LED_GREEN_GSM_MARK          (LED_GREEN_NO_SIM_CARD|LED_GREEN_AT_ERROR|LED_GREEN_NO_CREG)




/**
 * 充电的状态表
 * 
 * @author hzhmcu (2018/9/21)
 */

const IndEvt_t csg_sCALLEvt[] = {
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(300), },
};


const IndEvt_t csg_sATERREvt[] = {
    { .on = 1, .value = TIME_MS(10000), },
};

const IndEvt_t csg_sNoSimCardEvt[] = {
    { .on = 1, .value = TIME_MS(1000), },
    { .on = 0, .value = TIME_MS(2000), },
};

const IndEvt_t csg_sNoCreg[] = {
    { .on = 1, .value = TIME_MS(500), },
    { .on = 0, .value = TIME_MS(2500), },
};

const IndEvt_t csg_sGSMEvt[] = {
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(2900), },
};
const IndEvt_t csg_sGPRSEvt[] = {
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(300), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(2500), },
};

const IndEvtList_t  csg_sIndEvtList_LEDGreen[] = {
    {
        .evtMark   = EVT_TEST,
        .evts      = &csg_sTestEvt[0],
        .sizeOfEvt = sizeof(csg_sTestEvt) / sizeof(csg_sTestEvt[0]),
        .cycle     = TEST_LOOP,
    },

    {
        .evtMark   = LED_GREEN_CALL,
        .evts      = &csg_sCALLEvt[0],
        .sizeOfEvt = sizeof(csg_sCALLEvt) / sizeof(csg_sCALLEvt[0]),
        .cycle     = 0,
    },

    {
        .evtMark   = LED_GREEN_AT_ERROR,
        .evts      = &csg_sATERREvt[0],
        .sizeOfEvt = sizeof(csg_sATERREvt) / sizeof(csg_sATERREvt[0]),
        .cycle     = 0,
    },
    {
        .evtMark   = LED_GREEN_NO_SIM_CARD,
        .evts      = &csg_sNoSimCardEvt[0],
        .sizeOfEvt = sizeof(csg_sNoSimCardEvt) / sizeof(csg_sNoSimCardEvt[0]),
        .cycle     = 0,
    },

    {
        .evtMark   = LED_GREEN_NO_CREG,
        .evts      = &csg_sNoCreg[0],
        .sizeOfEvt = sizeof(csg_sNoCreg) / sizeof(csg_sNoCreg[0]),
        .cycle     = 0,
    },
    {
        .evtMark   = LED_GREEN_GPRS,
        .evts      = &csg_sGPRSEvt[0],
        .sizeOfEvt = sizeof(csg_sGPRSEvt) / sizeof(csg_sGPRSEvt[0]),
        .cycle     = 0,
    },
    {
        .evtMark   = LED_GREEN_GSM,
        .evts      = &csg_sGSMEvt[0],
        .sizeOfEvt = sizeof(csg_sGSMEvt) / sizeof(csg_sGSMEvt[0]),
        .cycle     = 0,
    },

};
#define LED_BLUE


#define LED_BLUE_BLE_LOC      0X00200000
#define LED_BLUE_WIFI_LOC     0X00100000
#define LED_BLUE_NO_NMEA_DATA 0X00080000
#define LED_BLUE_NO_GPS       0X00040000
#define LED_BLUE_GPS          0X00020000
#define LED_BLUE_AGPS         0X00010000
#define LED_BLUE_GPS_AGPS     0X00030000


#define LED_BLUE_GPS_MARK     (LED_BLUE_NO_NMEA_DATA|LED_BLUE_NO_GPS|LED_BLUE_GPS|LED_BLUE_AGPS)
/**
 * 充电的状态表
 * 
 * @author hzhmcu (2018/9/21)
 */
const IndEvt_t csg_sBLELocEvt[] = {
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(300), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(300), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(3000), },
};

/**
 * 充电的状态表
 * 
 * @author hzhmcu (2018/9/21)
 */
const IndEvt_t csg_sWIFILocEvt[] = {
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(300), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(300), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(300), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(3000), },
};



const IndEvt_t csg_sNoNMEAEvt[] = {
    { .on = 1, .value = TIME_MS(10000), },
};

const IndEvt_t csg_sNoGPSEvt[] = {
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(2900), },
};


const IndEvt_t csg_sGPSEvt[] = {
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(300), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(2500), },
};

const IndEvt_t csg_sGPSWithAGPSEvt[] = {
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(300), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(300), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(2100), },
};



const IndEvtList_t  csg_sIndEvtList_LEDBlue[] = {
    {
        .evtMark   = EVT_TEST,
        .evts      = &csg_sTestEvt[0],
        .sizeOfEvt = sizeof(csg_sTestEvt) / sizeof(csg_sTestEvt[0]),
        .cycle     = TEST_LOOP,
    },

    {
        .evtMark   = LED_BLUE_BLE_LOC,
        .evts      = &csg_sBLELocEvt[0],
        .sizeOfEvt = sizeof(csg_sBLELocEvt) / sizeof(csg_sBLELocEvt[0]),
        .cycle     = 2,
    },

    {
        .evtMark   = LED_BLUE_WIFI_LOC,
        .evts      = &csg_sWIFILocEvt[0],
        .sizeOfEvt = sizeof(csg_sWIFILocEvt) / sizeof(csg_sWIFILocEvt[0]),
        .cycle     = 3,
    },
    {
        .evtMark   = LED_BLUE_NO_NMEA_DATA,
        .evts      = &csg_sNoNMEAEvt[0],
        .sizeOfEvt = sizeof(csg_sNoNMEAEvt) / sizeof(csg_sNoNMEAEvt[0]),
        .cycle     = 0,
    },


    {
        .evtMark   = LED_BLUE_NO_GPS,
        .evts      = &csg_sNoGPSEvt[0],
        .sizeOfEvt = sizeof(csg_sNoGPSEvt) / sizeof(csg_sNoGPSEvt[0]),
        .cycle     = 0,
    },
    {
        .evtMark   = LED_BLUE_GPS_AGPS,
        .evts      = &csg_sGPSWithAGPSEvt[0],
        .sizeOfEvt = sizeof(csg_sGPSWithAGPSEvt) / sizeof(csg_sGPSWithAGPSEvt[0]),
        .cycle     = 0,
    },
    {
        .evtMark   = LED_BLUE_GPS,
        .evts      = &csg_sGPSEvt[0],
        .sizeOfEvt = sizeof(csg_sGPSEvt) / sizeof(csg_sGPSEvt[0]),
        .cycle     = 0,
    },

};
#define MOTOR



#define MOTOR_SHUTDOWN_KEY    0X00100000
#define MOTOR_PHONE           0X00080000
#define MOTOR_SMS             0X00040000
#define MOTOR_KEY             0X00020000
#define MOTOR_SHORT_KEY       0X00010000

/**
 * 充电的状态表
 * 
 * @author hzhmcu (2018/9/21)
 */

const IndEvt_t csg_sNoKEYEvt[] = {
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(1000), },
};


const IndEvt_t csg_sShortKEYEvt[] = {
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(100), },
};

const IndEvt_t csg_sSMSvt[] = {
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(1000), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(1000), },
};

const IndEvt_t csg_sPHONEvt[] = {
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(1000), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(1000), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(1000), },
};

const IndEvt_t csg_sShutDownEvt[] = {
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(500), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(500), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(500), },
    { .on = 1, .value = TIME_MS(100), },
    { .on = 0, .value = TIME_MS(500), },
};

const IndEvtList_t  csg_sIndEvtList_Motor[] = {
    {
        .evtMark   = EVT_TEST,
        .evts      = &csg_sTestEvt[0],
        .sizeOfEvt = sizeof(csg_sTestEvt) / sizeof(csg_sTestEvt[0]),
        .cycle     = TEST_LOOP,
    },
    {
        .evtMark   = MOTOR_PHONE,
        .evts      = &csg_sPHONEvt[0],
        .sizeOfEvt = sizeof(csg_sPHONEvt) / sizeof(csg_sPHONEvt[0]),
        .cycle     = 1,
    },
    {
        .evtMark   = MOTOR_SMS,
        .evts      = &csg_sSMSvt[0],
        .sizeOfEvt = sizeof(csg_sSMSvt) / sizeof(csg_sSMSvt[0]),
        .cycle     = 1,
    },
    {
        .evtMark   = MOTOR_KEY,
        .evts      = &csg_sNoKEYEvt[0],
        .sizeOfEvt = sizeof(csg_sNoKEYEvt) / sizeof(csg_sNoKEYEvt[0]),
        .cycle     = 1,
    },
    {
        .evtMark   = MOTOR_SHORT_KEY,
        .evts      = &csg_sNoKEYEvt[0],
        .sizeOfEvt = sizeof(csg_sShortKEYEvt) / sizeof(csg_sShortKEYEvt[0]),
        .cycle     = 1,
    },
    {
        .evtMark   = MOTOR_SHUTDOWN_KEY,
        .evts      = &csg_sShutDownEvt[0],
        .sizeOfEvt = sizeof(csg_sShutDownEvt) / sizeof(csg_sShutDownEvt[0]),
        .cycle     = 1,
    },
};



typedef struct {

    bool isNewEvt;
    uint32_t curEvt; //当前的所有事件
    uint32_t lstEvt; //上次运行的事件。
    uint32_t curEvtIdex; //事件表循环多少次
    uint32_t curCycle; //循环多少次
    uint32_t tick;
    const IndEvtList_t *curEvtList; //当前运行的事件表
    const IndEvtList_t *evtTab;
    const uint8_t sizeOfTab; //tab的大小
    void (*action)(bool on); //动作

    IndEnable_t property;

}IndicatorState_t;

void Alert_AllOn()
{
    nrf_gpio_cfg_output(RED_LED_PIN_NUMBER);
    nrf_gpio_pin_clear(RED_LED_PIN_NUMBER);


    nrf_gpio_cfg_output(GREEN_LED_PIN_NUMBER);
    nrf_gpio_pin_clear(GREEN_LED_PIN_NUMBER);


    nrf_gpio_cfg_output(BLUE_LED_PIN_NUMBER);
    nrf_gpio_pin_clear(BLUE_LED_PIN_NUMBER);

    nrf_gpio_pin_set(MOTOR_PIN_NUMBER);
}
void Alert_PowerOffIndicator()
{
    sg_sIndEnable.value = 0;
    timer_stop();
    nrf_gpio_pin_set(MOTOR_PIN_NUMBER);
    nrf_delay_ms(200);
    nrf_gpio_pin_clear(MOTOR_PIN_NUMBER);

    nrf_gpio_cfg_default(RED_LED_PIN_NUMBER);
    nrf_gpio_pin_set(RED_LED_PIN_NUMBER);
    nrf_gpio_cfg_output(BLUE_LED_PIN_NUMBER);
    nrf_gpio_pin_clear(BLUE_LED_PIN_NUMBER);
    nrf_gpio_cfg_output(GREEN_LED_PIN_NUMBER);
    nrf_gpio_pin_clear(GREEN_LED_PIN_NUMBER);
}
void Alert_AllOff()
{
    nrf_gpio_cfg_default(RED_LED_PIN_NUMBER);
    nrf_gpio_pin_set(RED_LED_PIN_NUMBER);

    nrf_gpio_cfg_default(GREEN_LED_PIN_NUMBER);
    nrf_gpio_pin_set(GREEN_LED_PIN_NUMBER);


    nrf_gpio_cfg_default(BLUE_LED_PIN_NUMBER);
    nrf_gpio_pin_set(BLUE_LED_PIN_NUMBER);


    nrf_gpio_pin_clear(MOTOR_PIN_NUMBER);
}


static __INLINE void alert_red_led_action(bool on)
{
    sg_sIndState.red = on;
    if (on)
    {
        nrf_gpio_cfg_output(RED_LED_PIN_NUMBER);
        nrf_gpio_pin_clear(RED_LED_PIN_NUMBER);
    }
    else
    {
        nrf_gpio_cfg_default(RED_LED_PIN_NUMBER);
        nrf_gpio_pin_set(RED_LED_PIN_NUMBER);
    }
}



static __INLINE void alert_green_led_action(bool on)
{

    sg_sIndState.green = on;
    if (on)
    {
        nrf_gpio_cfg_output(GREEN_LED_PIN_NUMBER);
        nrf_gpio_pin_clear(GREEN_LED_PIN_NUMBER);
    }
    else
    {
        nrf_gpio_cfg_default(GREEN_LED_PIN_NUMBER);
        nrf_gpio_pin_set(GREEN_LED_PIN_NUMBER);
    }
}

static __INLINE void alert_blue_led_action(bool on)
{

    sg_sIndState.blue = on;
    if (on)
    {
        nrf_gpio_cfg_output(BLUE_LED_PIN_NUMBER);
        nrf_gpio_pin_clear(BLUE_LED_PIN_NUMBER);
    }
    else
    {
        nrf_gpio_cfg_default(BLUE_LED_PIN_NUMBER);
        nrf_gpio_pin_set(BLUE_LED_PIN_NUMBER);
    }
}

static __INLINE void alert_motor_action(bool on)
{

    sg_sIndState.motor = on;
    if (on)
    {
        nrf_gpio_pin_set(MOTOR_PIN_NUMBER);
    }
    else
    {
        nrf_gpio_pin_clear(MOTOR_PIN_NUMBER);
    }
}
enum
{
    INDIC_LED_RED = 0,
    INDIC_LED_GREEN,
    INDIC_LED_BLUE,
    INDIC_MOTOR,
};

IndicatorState_t sg_sIndStates[] = {
    [INDIC_LED_RED] = {
        .property.led = true,
        .isNewEvt = true,
        .evtTab = &csg_sIndEvtList_LEDRed[0],
        .sizeOfTab = sizeof(csg_sIndEvtList_LEDRed) / sizeof(csg_sIndEvtList_LEDRed[0]),
        .curEvtList = NULL,
        .curEvt = 0,
        .action = alert_red_led_action,
    },

    [INDIC_LED_GREEN] = {
        .property.led = true,
        .isNewEvt = true,
        .evtTab = &csg_sIndEvtList_LEDGreen[0],
        .sizeOfTab = sizeof(csg_sIndEvtList_LEDGreen) / sizeof(csg_sIndEvtList_LEDGreen[0]),
        .curEvtList = NULL,
        .curEvt = 0,
        .action = alert_green_led_action,
    },

    [INDIC_LED_BLUE] = {
        .property.led = true,
        .isNewEvt = true,
        .evtTab = &csg_sIndEvtList_LEDBlue[0],
        .sizeOfTab = sizeof(csg_sIndEvtList_LEDBlue) / sizeof(csg_sIndEvtList_LEDBlue[0]),
        .curEvtList = NULL,
        .curEvt = 0,
        .action = alert_blue_led_action,
    },



    [INDIC_MOTOR] = {
        .property.motor = true,
        .isNewEvt = true,
        .evtTab = &csg_sIndEvtList_Motor[0],
        .sizeOfTab = sizeof(csg_sIndEvtList_Motor) / sizeof(csg_sIndEvtList_Motor[0]),
        .curEvtList = NULL,
        .curEvt = 0,
        .action = alert_motor_action,
    },
};


/**
 * 查找最高优先级的事件
 * 
 * @author hzhmcu (2018/9/21)
 * 
 * @param ind 
 * 
 * @return int32_t evtTab的索引 
 */
static int32_t FindEvt(IndicatorState_t *ind)
{
    //没有事件
    if (ind->curEvt == 0) return -1;

    for (int i = 0; i < ind->sizeOfTab; i++)//寻找和当前事件匹配的on tab表
    {
        //ind->curEvt改变会重新匹配，能保存一个上一次的提示事件
        if (/*ind->evtTab[i].evtMark ==*/ (ind->evtTab[i].evtMark & ind->curEvt))//chen comment
        {

            return i;
        }
    }
    return -1;
}

static int Indicator_Loop(IndicatorState_t *ind)
{
    if (ind->curEvt == 0)
    {
        if (ind->lstEvt != 0)
        {
            ind->lstEvt = 0;
            ind->action(0);
        }
        return 0;
    }
    if (ind->isNewEvt == true)
    {
        int i = FindEvt(ind);

        if ((i >= 0)
            && (ind->evtTab[i].evtMark != ind->lstEvt))
        {
            //有新的事件
            ind->lstEvt = ind->evtTab[i].evtMark;
            ind->isNewEvt = false;
            ind->curEvtIdex = 0;
            ind->curCycle = 0;
            ind->curEvtList = &ind->evtTab[i];
            ind->tick = 0;
        }
    }
    if (ind->curEvtList == NULL)
    {
        //处理异常
        ind->curEvt = 0;
        return -1;
    }
    //int _curEvtIdex = ind->curEvtIdex;
    int _tick = ind->curEvtList->evts[ind->curEvtIdex].value;

    if (ind->tick == 0 || ind->tick > _tick)
    {

        //有新的任务加载
        if (ind->tick != 0)
        {
            ind->curEvtIdex++;
        }
        ind->tick = 0;
        //代表完成一个周期
        if (ind->curEvtIdex >= ind->curEvtList->sizeOfEvt)
        {

            ind->curEvtIdex = 0;
            ind->tick = 0;
            //查cycle
            //0 为永远运行
            if (0 != ind->curEvtList->cycle)
            {
                ind->curCycle++;
                if (ind->curCycle >= ind->curEvtList->cycle)
                {
                    ind->curEvt &= ~(ind->curEvtList->evtMark);
                    ind->action(0);
                    ind->isNewEvt = true;
                    //关闭此任务
                    return 0;
                }
            }

        }

        ind->action(ind->curEvtList->evts[ind->curEvtIdex].on);

    }

    ind->tick++;
    return 0;
}

static void timer_setup(void)
{
    if ((!m_indicator_timer_is_active) && (sg_sIndEnable.value != 0))
    {
        m_indicator_timer_is_active = true;
        uint32_t err_code = app_timer_start(INDICATOR_TIMER, APP_TIMER_INTERVAL, NULL);
        if (err_code != NRF_SUCCESS)
        {
            APP_ERROR_CHECK(err_code);
        }
    }
}
static void timer_stop(void)
{
    if (m_indicator_timer_is_active == true)
    {
        m_indicator_timer_is_active = false;
        uint32_t err_code = app_timer_stop(INDICATOR_TIMER);
        if (err_code != NRF_SUCCESS)
        {
            APP_ERROR_CHECK(err_code);
        }
    }

    nrf_gpio_pin_set(BLUE_LED_PIN_NUMBER);
    nrf_gpio_pin_set(GREEN_LED_PIN_NUMBER);
    nrf_gpio_pin_set(RED_LED_PIN_NUMBER);
    nrf_gpio_pin_clear(MOTOR_PIN_NUMBER);
}
/**
 * 通知的事件
 * 
 * @author hzhmcu (2018/9/22)
 * 
 * @param evt 
 */
void Indicator_Evt(indicator_type_enum_t evt)
{


#if 1
    switch (evt)
    {

    case ALERT_TYPE_TEST:
        {
            sg_sIndStates[INDIC_LED_RED].curEvt |= EVT_TEST;
            sg_sIndStates[INDIC_LED_RED].isNewEvt = true;

            sg_sIndStates[INDIC_LED_BLUE].curEvt |= EVT_TEST;
            sg_sIndStates[INDIC_LED_BLUE].isNewEvt = true;

            sg_sIndStates[INDIC_LED_GREEN].curEvt |= EVT_TEST;
            sg_sIndStates[INDIC_LED_GREEN].isNewEvt = true;


            sg_sIndStates[INDIC_MOTOR].curEvt |= EVT_TEST;
            sg_sIndStates[INDIC_MOTOR].isNewEvt = true;


            break;
        }


#if 1//RED LED

    case ALERT_TYPE_IN_CHARGING:
        {
            sg_sIndStates[INDIC_LED_RED].curEvt &= ~LED_RED_EVT_CHRG_MARK;
            sg_sIndStates[INDIC_LED_RED].curEvt |= LED_RED_EVT_CHRG_ING;
            sg_sIndStates[INDIC_LED_RED].isNewEvt = true;


            sg_sIndStates[INDIC_MOTOR].curEvt |= MOTOR_KEY;
            sg_sIndStates[INDIC_MOTOR].isNewEvt = true;
            break;
        }

    case ALERT_TYPE_CHARGE_FULL:
        {
            sg_sIndStates[INDIC_LED_RED].curEvt &= ~LED_RED_EVT_CHRG_MARK;
            sg_sIndStates[INDIC_LED_RED].curEvt &= ~LED_RED_EVT_BAT_LOW;
            sg_sIndStates[INDIC_LED_RED].curEvt |= LED_RED_EVT_CHRG_END;
            sg_sIndStates[INDIC_LED_RED].isNewEvt = true;
            break;
        }

    case ALERT_TYPE_DISCHARGE:
        {
            sg_sIndStates[INDIC_LED_RED].curEvt &= ~LED_RED_EVT_CHRG_MARK;
            sg_sIndStates[INDIC_LED_RED].isNewEvt = true;
            break;
        }

    case ALERT_TYPE_BATTERY_POWER_LOW:
        {
            sg_sIndStates[INDIC_LED_RED].curEvt |= LED_RED_EVT_BAT_LOW;
            sg_sIndStates[INDIC_LED_RED].isNewEvt = true;
            break;
        }
    case ALERT_TYPE_BATTERY_POWER_NORMAL:
        {
            sg_sIndStates[INDIC_LED_RED].curEvt &= ~LED_RED_EVT_BAT_LOW;
            break;
        }
        //key

    case ALERT_TYPE_KEY_CLICK:
        {
            sg_sIndStates[INDIC_LED_RED].curEvt &= ~LED_RED_EVT_BAT_MARK;
            sg_sIndStates[INDIC_LED_RED].isNewEvt = true;
//            uint8_t value = battery_level();
            uint8_t value = battery_level_cal();
            if (value < 20)
            {
                sg_sIndStates[INDIC_LED_RED].curEvt |= LED_RED_EVT_BAT_20;
            }
            else if (value < 40)
            {
                sg_sIndStates[INDIC_LED_RED].curEvt |= LED_RED_EVT_BAT_40;
            }
            else if (value < 60)
            {
                sg_sIndStates[INDIC_LED_RED].curEvt |= LED_RED_EVT_BAT_60;
            }
            else if (value < 80)
            {
                sg_sIndStates[INDIC_LED_RED].curEvt |= LED_RED_EVT_BAT_80;
            }
            else if (value >= 80 && value <= 100)
            {
                sg_sIndStates[INDIC_LED_RED].curEvt |= LED_RED_EVT_BAT_100;
            }
            break;
        }

    case ALERT_TYPE_KEY_PRESS:
        {
            sg_sIndStates[INDIC_LED_RED].curEvt &= ~LED_RED_EVT_KEY_MARK;
            sg_sIndStates[INDIC_LED_RED].curEvt |= LED_RED_EVT_KEY_DOWN;
            sg_sIndStates[INDIC_LED_RED].isNewEvt = true;
            break;
        }

    case ALERT_TYPE_KEY_RELEASE:
        {
            sg_sIndStates[INDIC_LED_RED].curEvt &= ~LED_RED_EVT_KEY_MARK;
            sg_sIndStates[INDIC_LED_RED].curEvt |= LED_RED_EVT_KEY_UP;
            sg_sIndStates[INDIC_LED_RED].isNewEvt = true;
            break;
        }

    case ALERT_TYPE_BLE_DATA_XFER:
    case ALERT_TYPE_BLE_CONNECTED:
        {
            sg_sIndStates[INDIC_LED_RED].curEvt |= LED_RED_EVT_BLE;
            sg_sIndStates[INDIC_LED_RED].isNewEvt = true;
            break;
        }

    case ALERT_TYPE_BLE_DISCONNECTED:
        {
            sg_sIndStates[INDIC_LED_RED].curEvt &= ~LED_RED_EVT_BLE;
            sg_sIndStates[INDIC_LED_RED].isNewEvt = true;
            break;
        }
        
    case ALERT_TYPE_BLE_ADV:
        {
            sg_sIndStates[INDIC_LED_RED].curEvt |= LED_RED_EVT_BLE_ADV;
            sg_sIndStates[INDIC_LED_RED].isNewEvt = true;
            break;
        }
     case ALERT_TYPE_BLE_ADV_END:  
        {
            sg_sIndStates[INDIC_LED_RED].curEvt &= ~LED_RED_EVT_BLE_ADV;
            sg_sIndStates[INDIC_LED_RED].isNewEvt = true;
            break;
        }
        
#endif



#if 1//BLUE LED



    case ALERT_TYPE_GPS_STOP:
        {
            sg_sIndStates[INDIC_LED_BLUE].curEvt &= ~LED_BLUE_GPS_MARK;
            sg_sIndStates[INDIC_LED_BLUE].isNewEvt = true;
            break;
        }
    case ALERT_TYPE_GPS_NO_NMEA_DATA:
        {
            sg_sIndStates[INDIC_LED_BLUE].curEvt &= ~LED_BLUE_GPS_MARK;
            sg_sIndStates[INDIC_LED_BLUE].curEvt |= LED_BLUE_NO_NMEA_DATA;
            sg_sIndStates[INDIC_LED_BLUE].isNewEvt = true;
            break;
        }
    case ALERT_TYPE_GPS_NO_LOCATION:
        {
            sg_sIndStates[INDIC_LED_BLUE].curEvt &= ~LED_BLUE_GPS_MARK;
            sg_sIndStates[INDIC_LED_BLUE].curEvt |= LED_BLUE_NO_GPS;
            sg_sIndStates[INDIC_LED_BLUE].isNewEvt = true;
            break;
        }

    case ALERT_TYPE_GPS_HAS_LOCATION:
        {
            sg_sIndStates[INDIC_LED_BLUE].curEvt &= ~LED_BLUE_GPS_MARK;
            sg_sIndStates[INDIC_LED_BLUE].curEvt |= LED_BLUE_GPS;
            sg_sIndStates[INDIC_LED_BLUE].isNewEvt = true;
            break;
        }

#if 0
    case ALERT_TYPE_GPS_APGS_WORK:
        {
            sg_sIndStates[INDIC_LED_BLUE].curEvt |= LED_BLUE_AGPS;
            sg_sIndStates[INDIC_LED_BLUE].isNewEvt = true;
            break;
        }
#endif

    case ALERT_TYPE_BLE_LOC:
        sg_sIndStates[INDIC_LED_BLUE].curEvt |= LED_BLUE_BLE_LOC;
        sg_sIndStates[INDIC_LED_BLUE].isNewEvt = true;
        break;
    case ALERT_TYPE_WIFI_LOC:
        sg_sIndStates[INDIC_LED_BLUE].curEvt |= LED_BLUE_WIFI_LOC;
        sg_sIndStates[INDIC_LED_BLUE].isNewEvt = true;
        break;
#endif


#if 1//GREEN LED

    case ALERT_TYPE_GSM_STOP:
        {
            sg_sIndStates[INDIC_LED_GREEN].curEvt = 0;
            sg_sIndStates[INDIC_LED_GREEN].isNewEvt = true;
            break;
        }
    case ALERT_TYPE_AT_NO_ANSWER:
        {
            sg_sIndStates[INDIC_LED_GREEN].curEvt |= LED_GREEN_AT_ERROR;
            sg_sIndStates[INDIC_LED_GREEN].isNewEvt = true;
            break;
        }
    case ALERT_TYPE_SIM_CARD_NOT_INSERT:
        {
            sg_sIndStates[INDIC_LED_GREEN].curEvt &= ~LED_GREEN_NO_SIM_CARD_MARK;
            sg_sIndStates[INDIC_LED_GREEN].curEvt |= LED_GREEN_NO_SIM_CARD;
            sg_sIndStates[INDIC_LED_GREEN].isNewEvt = true;
            break;
        }
    case ALERT_TYPE_GSM_NOT_CREG:
        {
            sg_sIndStates[INDIC_LED_GREEN].curEvt &= ~LED_GREEN_NO_SIM_CARD_MARK;
            sg_sIndStates[INDIC_LED_GREEN].curEvt |= LED_GREEN_NO_CREG;
            sg_sIndStates[INDIC_LED_GREEN].isNewEvt = true;
            break;
        }
    case ALERT_TYPE_GSM_NORMAL_WORK:
        {
            sg_sIndStates[INDIC_LED_GREEN].curEvt &= ~LED_GREEN_GSM_MARK;
            sg_sIndStates[INDIC_LED_GREEN].curEvt |= LED_GREEN_GSM;
            sg_sIndStates[INDIC_LED_GREEN].isNewEvt = true;
            break;
        }

    case ALERT_TYPE_GPRS_CONNECTED:
        {
            sg_sIndStates[INDIC_LED_GREEN].curEvt &= ~LED_GREEN_GSM_MARK;
            sg_sIndStates[INDIC_LED_GREEN].curEvt |= LED_GREEN_GPRS;
            sg_sIndStates[INDIC_LED_GREEN].isNewEvt = true;
            break;
        }

    case ALERT_TYPE_GPRS_DISCONNECTED:
        {
            sg_sIndStates[INDIC_LED_GREEN].curEvt &= ~LED_GREEN_GPRS;
            sg_sIndStates[INDIC_LED_GREEN].isNewEvt = true;
            break;
        }
    case ALERT_TYPE_CALL_START:
        {
            sg_sIndStates[INDIC_LED_GREEN].curEvt |= LED_GREEN_CALL;
            sg_sIndStates[INDIC_LED_GREEN].isNewEvt = true;
            break;
        }
    case ALERT_TYPE_CALL_STOP:
        {
            sg_sIndStates[INDIC_LED_GREEN].curEvt &= ~LED_GREEN_CALL;
            sg_sIndStates[INDIC_LED_GREEN].isNewEvt = true;
            break;
        }
#endif

#if 1// motor
    case ALERT_TYPE_SMS_GET:
        {
            sg_sIndStates[INDIC_MOTOR].curEvt |= MOTOR_SMS;
            sg_sIndStates[INDIC_MOTOR].isNewEvt = true;
            break;
        }
    case ALERT_TYPE_PHONE_CALL:
        // case ALERT_TYPE_PHONE_DIAL:
        // case ALERT_TYPE_PHONE_ACTIVE:
        {
            sg_sIndStates[INDIC_MOTOR].curEvt |= MOTOR_PHONE;
            sg_sIndStates[INDIC_MOTOR].isNewEvt = true;
            break;
        }

    case ALERT_TYPE_KEY_CALL:
        {
            sg_sIndStates[INDIC_MOTOR].curEvt |= MOTOR_SHORT_KEY;
            sg_sIndStates[INDIC_MOTOR].isNewEvt = true;
            break;
        }
        break;

    case ALERT_BUTTON_LONG_HOLD:
    case ALERT_TYPE_POWER_ON:
    case ALERT_TYPE_POWER_OFF:
        {
            sg_sIndStates[INDIC_MOTOR].curEvt |= MOTOR_KEY;
            sg_sIndStates[INDIC_MOTOR].isNewEvt = true;
            break;
        }

    case ALERT_SHUTDOWN_LONG_HOLD:
        {
            sg_sIndStates[INDIC_MOTOR].curEvt |= MOTOR_SHUTDOWN_KEY;
            sg_sIndStates[INDIC_MOTOR].isNewEvt = true;
            break;
        }
#endif

    default:
        break;
    }
#endif
    timer_setup();
}



/**
 * 100ms回调函数
 * 
 * @author hzhmcu (2018/9/21)
 * 
 * @param pcontext 
 */
static void Indicator_timeout_handler(void *pcontext)
{
    uint32_t index = 0;
    for (int i = 0; i < sizeof(sg_sIndStates) / sizeof(sg_sIndStates[0]); i++)
    {
        //如果相应的开关打开了，才运行。如：LED关了，所有LED属性相关的函数都不运行。

        if (sg_sIndStates[i].property.value & sg_sIndEnable.value)
        {
            Indicator_Loop(&sg_sIndStates[i]);
            index |= sg_sIndStates[i].curEvt;
        }
    }

    //没有事件了。就把它定时器关了
    if (index == 0)
    {
        timer_stop();
    }

}



/**
 * 初始化 
 * 把所有的IO进行初始化 
 * 并用初始化一个100ms的定时器 
 * 
 * @author hzhmcu (2018/9/21)
 */
void Indicator_Init(void)
{

    //sg_sIndEnable.value = 0;
    sg_sIndEnable.led = 1;
    sg_sIndEnable.motor = 1;
    // Initialize GPIO output & state
    nrf_gpio_cfg_output(BLUE_LED_PIN_NUMBER);
    nrf_gpio_cfg_output(GREEN_LED_PIN_NUMBER);
    nrf_gpio_cfg_output(RED_LED_PIN_NUMBER);
    nrf_gpio_cfg_output(MOTOR_PIN_NUMBER);

    nrf_gpio_cfg_default(BLUE_LED_PIN_NUMBER);
    nrf_gpio_cfg_default(GREEN_LED_PIN_NUMBER);
    nrf_gpio_cfg_default(RED_LED_PIN_NUMBER);

    nrf_gpio_pin_set(BLUE_LED_PIN_NUMBER);
    nrf_gpio_pin_set(GREEN_LED_PIN_NUMBER);
    nrf_gpio_pin_set(RED_LED_PIN_NUMBER);
    nrf_gpio_pin_clear(MOTOR_PIN_NUMBER);



    // Create App Alert Timer
    uint32_t err_code = app_timer_create(&INDICATOR_TIMER,
                                         APP_TIMER_MODE_REPEATED,
                                         Indicator_timeout_handler);
    if (err_code != NRF_SUCCESS)
    {
        APP_ERROR_CHECK(err_code);
    }



}


/**
 * 总开关。
 * 
 * @author hzhmcu (2018/9/22)
 * 
 * @param ind bit 1 LED bit2 motor  可以与运算
 * @param enable true代表打开
 */
void Indicator_Enable(uint32_t ind, bool enable)
{
    if (enable == false)
    {
        //1.把所有的IO关断。

        for (int i = 0; i < sizeof(sg_sIndStates) / sizeof(sg_sIndStates[0]); i++)
        {
            //如果相应的开关打开了，才运行。如：LED关了，所有LED属性相关的函数都不运行。

            if (sg_sIndStates[i].property.value & ind)
            {
                sg_sIndStates[i].action(false);
            }
        }
        sg_sIndEnable.value &= ~ind;
    }
    else
    {
        sg_sIndEnable.value |= ind;
        timer_setup();
    }
    sg_sIndEnable.rvs = 0;
}








