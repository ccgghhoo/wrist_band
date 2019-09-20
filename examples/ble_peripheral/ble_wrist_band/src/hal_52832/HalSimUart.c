#include "HalSimUart.h"
#include "simUart.h"
#include "nrf_drv_gpiote.h"
//RTC 32768 = 30.5 us
// init
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "DateTime.h"
#include "nrf_drv_timer.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#if 0

  #define UART_LOG			NRF_LOG_INFO
  #define UART_LOG_RAW(...)
#else
  #define UART_LOG(...)
  #define UART_LOG_RAW(...)
#endif
uint8_t sg_rxPin = 32;
void timer_led_event_handler(nrf_timer_event_t event_type, void *p_context);
const nrf_drv_timer_t TIMER_LED = NRF_DRV_TIMER_INSTANCE(1);

#define TEST_PIN 32// 29

void (*ReceiveData)(uint8_t data);
void GpsRxTimer_Start()
{
  nrf_drv_timer_enable(&TIMER_LED);
}

void GpsRxTimer_Stop()
{
  nrf_drv_timer_disable(&TIMER_LED);
}
void myTimerInit()
{
  uint32_t time_ms = 104; //Time(in miliseconds) between consecutive compare events.
  uint32_t time_ticks;


  time_ticks = nrf_drv_timer_us_to_ticks(&TIMER_LED, time_ms);

  nrf_drv_timer_extended_compare(
    &TIMER_LED, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);
  GpsRxTimer_Start();
}
enum
{
  START,
  SDATA,
  STOP,
};

void VM_Uart3_RX(uint8_t dataBit) // 模拟UART接收函数
{
  static uint8_t status = START;
  static uint8_t number = 0;
  static uint8_t rData;

  switch (status)
  {
      case START:
        number = 0;
        status = SDATA;
      case SDATA:
        {

          if (dataBit)
          {
            rData |= 0x80;
          } else
          {
            rData &= 0x7F;
          }
          if (++number < 8)
          {
            rData >>= 1;
          } else
          {
            number = 0;
            status = STOP;
          }
        }
        break;
      case STOP:
        {
          GpsRxTimer_Stop();
          nrf_drv_gpiote_in_event_enable(sg_rxPin, true);
          if (dataBit)
          {
            ReceiveData(rData);
          } else
          {
            __NOP();
          }
          status = START;
        }
        break;
      default:
        {
          status = START;
        }
        break;
  }
}


/**
 * @brief Handler for timer events.
 */
void timer_led_event_handler(nrf_timer_event_t event_type, void *p_context)
{
  // uint32_t led_to_invert = (1 << leds_list[(i++) % LEDS_NUMBER]);
  // printf("6\n");

  switch (event_type)
  {
      case NRF_TIMER_EVENT_COMPARE0:
        //  LEDS_INVERT(led_to_invert);
        nrf_gpio_pin_toggle(TEST_PIN);
        VM_Uart3_RX(nrf_gpio_pin_read(sg_rxPin));

        break;

      default:
        //Do nothing.
        break;
  }
}




int32_t Uart_Stop()
{
  GpsRxTimer_Stop();
  nrf_drv_gpiote_in_event_disable(sg_rxPin);
  return 0;
}


#pragma optimize = s
static void uart_int_1_evt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
  uint32_t isHight = (nrf_gpio_pin_read(pin) != 0);
  if (isHight == 0)
  {
    nrf_gpio_pin_clear(TEST_PIN);
    nrf_delay_us(10);
    nrf_gpio_pin_set(TEST_PIN);
    nrf_delay_us(10);
    if (nrf_gpio_pin_read(pin) == 0)
    {
      nrf_gpio_pin_clear(TEST_PIN);
      myTimerInit();
      nrf_drv_gpiote_in_event_disable(sg_rxPin);
    }
  }
}




int32_t Uart_Init(uint32_t rxPin, uint32_t txPin, void (*cb)(uint8_t data))
{
  // Uart_Stop(rxPin,txPin);
  // GpsRxTimer_Stop();
  ReceiveData = cb;
  nrf_drv_gpiote_in_config_t int_config = {
    .sense 			= NRF_GPIOTE_POLARITY_TOGGLE,
    .pull			  = NRF_GPIO_PIN_NOPULL,
    .is_watcher 	= false,
    .hi_accuracy	= false,
  };
  APP_ERROR_CHECK(nrf_drv_gpiote_in_init(rxPin,
                                         &int_config,
                                         uart_int_1_evt_handler));
  sg_rxPin = rxPin;



  //Configure TIMER_LED for generating simple light effect - leds on board will invert his state one after the other.
  uint32_t err_code = nrf_drv_timer_init(&TIMER_LED, NULL, timer_led_event_handler);
  APP_ERROR_CHECK(err_code);



  //nrf_drv_gpiote_in_event_enable(rxPin, true);
  nrf_gpio_cfg_output(TEST_PIN);
  nrf_gpio_pin_set(TEST_PIN);
  return 0;
}


int32_t Uart_Start()
{
  nrf_drv_gpiote_in_event_enable(sg_rxPin, true);
  GpsRxTimer_Stop(); 
  return false;
}




