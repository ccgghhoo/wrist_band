#include "HalGPSUart.h"
#include "config.h"
#include "HalUart.h"
#include "HalSimUart.h"
#include "nrf.h"
// MARK: APP_UART_DRV INSTANCE
static const uart_instance_t GPS_UART_INSTANCE =
{
  .piority					= UART_PIORITY_GPS,
  .txd_pin					= GPS_UART_TXD_PIN_NUMBER,
  .rxd_pin					= 32, //不通过硬件串口接收。
  .baudrate 			  = UARTE_BAUDRATE_BAUDRATE_Baud9600,
  .evt_handle			  = NULL,
};
static UART_INSTANCE_SEQ_T					m_gps_uart_seq;


__INLINE bool HalGPSTxUart_Open()
{
  return uart_arbitrate_apply(&GPS_UART_INSTANCE, &m_gps_uart_seq);
}


__INLINE bool HalGPSTxUart_Close()
{
  uart_release_control(m_gps_uart_seq);
    return true;
}


__INLINE bool HalGPSTxUart_Put(uint8_t *buf,uint32_t len)
{
  return uart_data_packet_put(m_gps_uart_seq, buf, len);
}

__INLINE void HalGPS_Init(void (*readcb)(uint8_t data))
{
  //TX 是硬件的。
  Uart_Init(GPS_UART_RXD_PIN_NUMBER, 32, readcb);
}

__INLINE bool HalGPSUart_Open(uint32_t baudRate)
{
  Uart_Start(baudRate);
  return true;
}
__INLINE bool HalGPSUart_Close()
{
  Uart_Stop();
  return true;
}



