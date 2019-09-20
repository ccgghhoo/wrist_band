
#include "nrf.h"
#include "nrf_saadc.h"
#include "nrf_drv_common.h"
#include "HalBattery.h"
#include "boards.h"
#include "nrf_gpio.h"
#include "nrf.h"
#include "nrf_soc.h"
#include "nrf_drv_gpiote.h"
typedef void (*app_ssadc_drv_call_back_t)(uint32_t voltage);
#define ADC_REFERENCE_VOLTAGE			60				// unit: mV
#define ADC_RESOLUATION_BITS			0x400  			// 0x400
#define ADC_PRESCALER					110//78
#define BATTERY_VOLTAGE( adc_result ) 	 ( (ADC_REFERENCE_VOLTAGE * adc_result * ADC_PRESCALER) / ADC_RESOLUATION_BITS  )



#define SET_CHARGE_ENABLE()       nrf_gpio_pin_clear( BATTERY_CHARGE_ENABLE_PIN_NUMBER )
#define SET_CHARGE_DISABLE()      nrf_gpio_pin_set( BATTERY_CHARGE_ENABLE_PIN_NUMBER )
#define DEVICE_IN_CHARGING()		  nrf_gpio_pin_read( USB_DETECTED_PIN_NUMBER ) > 0



static nrf_saadc_value_t 				result;
static bool 							m_in_progress = false;
static app_ssadc_drv_call_back_t		m_cb;
int (*pfnChargePin)(bool isCharge);

void HalBattery_Start(void)
{
    if (nrf_saadc_busy_check()) return;

    nrf_saadc_enable();

    nrf_saadc_buffer_init(&result, 1);

    if (m_in_progress) nrf_saadc_task_trigger(NRF_SAADC_TASK_SAMPLE);
    else nrf_saadc_task_trigger(NRF_SAADC_TASK_START);
}

void HalBattery_Stop(void)
{
    nrf_saadc_event_clear(NRF_SAADC_EVENT_STOPPED);
}


void SAADC_IRQHandler(void)
{
    if (nrf_saadc_event_check(NRF_SAADC_EVENT_STARTED))
    {
        nrf_saadc_event_clear(NRF_SAADC_EVENT_STARTED);

        m_in_progress = true;

        nrf_saadc_task_trigger(NRF_SAADC_TASK_SAMPLE);
    }

    if (nrf_saadc_event_check(NRF_SAADC_EVENT_END))
    {
        nrf_saadc_event_clear(NRF_SAADC_EVENT_END);

        if (nrf_saadc_amount_get() > 0)
        {
            nrf_saadc_task_trigger(NRF_SAADC_TASK_STOP);
            uint32_t voltage = BATTERY_VOLTAGE(result);
            m_cb(voltage);
        }
    }


    if (nrf_saadc_event_check(NRF_SAADC_EVENT_STOPPED))
    {
        nrf_saadc_event_clear(NRF_SAADC_EVENT_STOPPED);

        m_in_progress = false;
        nrf_saadc_disable();
    }
}


static uint8_t sg_USB_DETECTEDRes = 0;
#define PIN_L 0X01
#define PIN_H 0X02

uint8_t GetUSB_DETECTEDStatus()
{
    return sg_USB_DETECTEDRes;
}


static void ChargeState_Exit_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (pin == USB_DETECTED_PIN_NUMBER)
    {
        if (nrf_gpio_pin_read(USB_DETECTED_PIN_NUMBER) > 0)
        {
            sg_USB_DETECTEDRes |= PIN_H;
            pfnChargePin(true);
        }
        else
        {
            sg_USB_DETECTEDRes |= PIN_L;
            pfnChargePin(false);
        }
    }
}


void HalBattery_Init(battery_init_t *p_battery_obj)
{
    m_cb = p_battery_obj->cb;
    pfnChargePin = p_battery_obj->ChargePin;
    nrf_saadc_resolution_set(NRF_SAADC_RESOLUTION_10BIT);
    nrf_saadc_oversample_set(NRF_SAADC_OVERSAMPLE_DISABLED);

    nrf_saadc_channel_config_t	saadc_config;

    saadc_config.resistor_p		= NRF_SAADC_RESISTOR_DISABLED;
    saadc_config.resistor_n		= NRF_SAADC_RESISTOR_DISABLED;
    saadc_config.gain			= NRF_SAADC_GAIN1;
    saadc_config.reference		= NRF_SAADC_REFERENCE_INTERNAL;
    saadc_config.acq_time		= NRF_SAADC_ACQTIME_10US;
    saadc_config.mode			= NRF_SAADC_MODE_SINGLE_ENDED;
    saadc_config.burst			= NRF_SAADC_BURST_DISABLED;
    saadc_config.pin_n			= NRF_SAADC_INPUT_DISABLED;
    saadc_config.pin_p			= (nrf_saadc_input_t)ADC_SELECTED_CHANNEL_NUMBER;

    nrf_saadc_channel_init(0, &saadc_config);

    nrf_saadc_int_enable(NRF_SAADC_INT_END | NRF_SAADC_INT_STARTED | NRF_SAADC_INT_STOPPED);

    nrf_drv_common_irq_enable(SAADC_IRQn, SAADC_CONFIG_IRQ_PRIORITY);

    nrf_saadc_enable();

    nrf_saadc_task_trigger(NRF_SAADC_TASK_CALIBRATEOFFSET);
    while (NRF_SAADC->EVENTS_CALIBRATEDONE == 0);
    nrf_saadc_event_clear(NRF_SAADC_EVENT_CALIBRATEDONE);

    nrf_saadc_disable();




    //
    nrf_drv_gpiote_in_config_t drv_int_config = {
        .sense 			= NRF_GPIOTE_POLARITY_TOGGLE,
        .pull			= NRF_GPIO_PIN_NOPULL,
        .is_watcher 	= false,
        .hi_accuracy	= false,
    };

    uint32_t err_code = (nrf_drv_gpiote_in_init(USB_DETECTED_PIN_NUMBER,
                                                &drv_int_config,
                                                ChargeState_Exit_handler)
                        );
    APP_ERROR_CHECK(err_code);

    if (nrf_gpio_pin_read(USB_DETECTED_PIN_NUMBER) == 0)
    {
        sg_USB_DETECTEDRes |= PIN_L;
    }
    else
    {
        sg_USB_DETECTEDRes |= PIN_H;
    }


    // set charge detect pin interrupt enable
    nrf_drv_gpiote_in_event_enable(USB_DETECTED_PIN_NUMBER, true);

    // Charge Enable while power on
    nrf_gpio_cfg_output(BATTERY_CHARGE_ENABLE_PIN_NUMBER);

}



void HalBattery_SetCharge(bool enable)
{
    if (enable)
    {
        SET_CHARGE_ENABLE();
    }
    else
    {
        SET_CHARGE_DISABLE();
    }
}


bool HalBattery_IsCharge()
{
    if (nrf_gpio_pin_read(USB_DETECTED_PIN_NUMBER) > 0)
    {
        sg_USB_DETECTEDRes |= PIN_H;
    }
    else
    {
        sg_USB_DETECTEDRes |= PIN_L;
    }
    return DEVICE_IN_CHARGING();
}

void HalBattery_Loop()
{

}

