#include "HalArch.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "config.h"
#include "nordic_common.h"
#include "nrf.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_soc.h"

#include "app_timer.h"
#include "app_error.h"
#include "ble_advertising.h"
#include "app_scheduler.h"
#include "softdevice_handler_appsh.h"

#include "nrf_drv_gpiote.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#if ( BLUETOOTH_DEV_ROLE == PERIPHERAL_MODE )
#include "peripheral_role.h"
#elif ( BLUETOOTH_DEV_ROLE == CENTRAL_PERIPHERAL_MODE )
#include "central_peripheral_role.h"

#endif

#define SCHED_MAX_EVENT_DATA_SIZE        MAX(APP_TIMER_SCHED_EVENT_DATA_SIZE, \
BLE_STACK_HANDLER_SCHED_EVT_SIZE)       /**< Maximum size of scheduler events. */
#ifdef SVCALL_AS_NORMAL_FUNCTION
#define SCHED_QUEUE_SIZE                 20                                          /**< Maximum number of events in the scheduler queue. More is needed in case of Serialization. */
#else
#define SCHED_QUEUE_SIZE                 10                                          /**< Maximum number of events in the scheduler queue. */
#endif

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */



#if ( BLUETOOTH_DEV_ROLE == PERIPHERAL_MODE )
/**@brief Function for the Event Scheduler initialization.
*/
static void scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}
#endif


static void Mac_Show()
{
    ble_gap_addr_t      addr;
    uint32_t err_code;
#if (NRF_SD_BLE_API_VERSION < 3)
    err_code = sd_ble_gap_address_get(&addr);
#else
    err_code = sd_ble_gap_addr_get(&addr);
#endif
    err_code = err_code;
    NRF_LOG_INFO("Mac %2x:%2x:%2x:%2x:%2x:%2x\r\n", addr.addr[5],
                 addr.addr[4], addr.addr[3], addr.addr[2],
                 addr.addr[1], addr.addr[0]);
}
void HalArch_GetBLEMac(char *mac)
{
    ble_gap_addr_t      addr;
    uint32_t err_code;
    err_code = sd_ble_gap_addr_get(&addr);

    for (int i = 0; i < 6; i++)
    {
        mac[i] = addr.addr[i];

    }
}


static void log_init(void)
{
    // Initialize.
    uint32_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for the Power manager.
*/
void HalArch_Sleep(void)
{
    uint32_t err_code = sd_app_evt_wait();

    APP_ERROR_CHECK(err_code);
}

void GPIO_Default(void)
{
    for (uint32_t i = 0; i < 31; i++)
    {
        NRF_GPIO->PIN_CNF[i] = 0x02;
    }



    nrf_gpio_cfg_output(GPS_POWER_PIN_NUMBER);
    nrf_gpio_pin_clear(GPS_POWER_PIN_NUMBER);
#if GPS_VDDIO_NUMBER
    nrf_gpio_cfg_output(GPS_VDDIO_NUMBER);
    nrf_gpio_pin_clear(GPS_VDDIO_NUMBER);
#endif

    nrf_gpio_cfg_output(GSM_POWER_PIN_NUMBER);
    nrf_gpio_pin_clear(GSM_POWER_PIN_NUMBER);


}

static void timers_init(void)
{
    uint32_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

void HalArch_Init()
{
    //LOG
    log_init();
    HZH_LOG("Start \r\n");
    timers_init();
    HZH_LOG("Start2 \r\n");
    Mac_Show();
    app_task_drv_init();
    nrf_drv_gpiote_init();
#if ( BLUETOOTH_DEV_ROLE == PERIPHERAL_MODE )
    scheduler_init();
    ble_stack_init();
#endif

    ble_role_init();
}


void HalArch_Loop()
{
    app_sched_execute();
}


int HalArch_Start()
{
#if ( BLUETOOTH_DEV_ROLE == PERIPHERAL_MODE )
    APP_ERROR_CHECK(ble_advertising_start(BLE_ADV_MODE_FAST));
#endif
    return 0;
}

int HalArch_Stop()
{
#if ( BLUETOOTH_DEV_ROLE == PERIPHERAL_MODE )
    ble_stop();
#endif
    return 0;
}
