#include "config.h"
#include "HalExit.h"
#include "nrf_drv_gpiote.h"
#include "nrf_gpio.h"
#include "nrf_error.h"
typedef struct HalButton
{
    uint32_t pin;
}HalButton_t;


/**
 * 读Pin
 * 
 * @author hzhmcu (2019/3/16)
 * 
 * @param pin 
 * 
 * @return bool 
 */
bool HalExit_Read(uint32_t pin)
{
    return (nrf_gpio_pin_read(pin) > 0);
}
typedef void (*button_cb_t)(uint32_t pin);
static button_cb_t  pfnButtonCb = NULL;


/**
 * 初始华Button。 
 * 只可以初始化一次 
 * 
 * @author hzhmcu (2019/3/16)
 * 
 * @param cb 
 * @param pin 
 * 
 * @return bool 
 */
bool HalExit_Init(void (*cb)(uint32_t pin))
{
    pfnButtonCb = cb;
    return true;
}




static void button_drv_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (pfnButtonCb)
    {
        pfnButtonCb(pin);
    }
}

/**
 * 注册Button中断
 * 
 * @author hzhmcu (2019/3/16)
 * 
 * @param pin 
 * @param push 
 * 
 * @return int 
 */
int HalExit_Register(uint32_t pin, ExitPush_en push)
{
    nrf_drv_gpiote_in_config_t int_config =
    {
        .sense 			= NRF_GPIOTE_POLARITY_TOGGLE,
        .pull			= NRF_GPIO_PIN_PULLUP,
        .is_watcher 	= false, //不启动PPI.
        .hi_accuracy	= true, //不启动PPI.
    };


    switch (push)
    {
    default:
    case EXIT_PUSH_UP:
        int_config.pull = NRF_GPIO_PIN_PULLUP;
        break;
    case EXIT_PUSH_NO:
        int_config.pull = NRF_GPIO_PIN_NOPULL;
        break;
    case EXIT_PUSH_DOWN:
        int_config.pull = NRF_GPIO_PIN_PULLDOWN;
        break;
    }

    uint32_t err_code = NRF_SUCCESS;
    err_code = nrf_drv_gpiote_in_init(pin,
                                      &int_config,
                                      button_drv_handler
                                     );
    

    return err_code;
}


/**
 * 使能Button中断
 * 
 * @author hzhmcu (2019/3/16)
 * 
 * @param pin 
 * @param isEnable 
 * 
 * @return int 
 */
int HalExit_Enable(uint32_t pin, bool isEnable)
{
    nrf_drv_gpiote_in_event_enable(pin, isEnable); 
    return 0;
}



