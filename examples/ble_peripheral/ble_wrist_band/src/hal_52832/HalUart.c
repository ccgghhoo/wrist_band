
#include <string.h>
#include <stdlib.h>

#include "sdk_config.h"
#include "nrf.h"
#include "nrf_gpio.h"

#include "nrf_drv_common.h"
#include "HalUart.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "boards.h"

#if 0
#define UART_LOG_INFO			NRF_LOG_INFO
#define UART_LOG_DUBUG			NRF_LOG_DEBUG
#define UART_LOG_ERROR			NRF_LOG_ERROR
#define UART_LOG_WARNING			NRF_LOG_WARNING
#else
#define UART_LOG_INFO(...)
#define UART_LOG_DUBUG(...)
#define UART_LOG_ERROR(...)
#define UART_LOG_WARNING(...)
#endif





#define APP_UART_TX_FIFO_SIZE	32 //
#define APP_UART_TX_FIFO_MASK	(APP_UART_TX_FIFO_SIZE - 1)

#define APP_UART_RX_FIFO_SIZE	64
#define APP_UART_RX_FIFO_MASK	(APP_UART_RX_FIFO_SIZE - 1)

#define APP_UART_RX_FIFO_GROUPS 8
#define APP_UART_RX_FIFO_GROUPS_MASK	(APP_UART_RX_FIFO_GROUPS - 1)


static bool						m_is_uart_enable = false;
static bool						m_is_tx_in_progress = false;
static bool						m_is_uart_endrx_shortcut_startrx = false;
volatile UART_State_en sg_enUart = UART_State_CLOSE;
//static uint32_t 				m_uart_received_count = 0;

static uart_instance_t			m_uart_instance;
static uart_evt_t				m_uart_evt;

typedef struct {
    uint8_t *p_data_buff;
    uint16_t 		length;
}app_uart_drv_data_t;

typedef struct {
    uint16_t 		len;
    uint8_t			data_buff[APP_UART_RX_FIFO_SIZE];
}dma_recv_data_t;

// MARK: UART tx queue variable
#if 0
static uint32_t 				gps_m_uart_tx_read_index = 0; //define the buffer for gps module
static uint32_t 				gps_m_uart_tx_write_index = 0;
static app_uart_drv_data_t		gps_m_uart_tx_fifo[ APP_UART_TX_FIFO_SIZE ];
static app_uart_drv_data_t 		gps_m_uart_tx_fifo_read;
#endif

static uint32_t 				m_uart_tx_read_index = 0;
static uint32_t 				m_uart_tx_write_index = 0;
static app_uart_drv_data_t		m_uart_tx_fifo[APP_UART_TX_FIFO_SIZE];
static app_uart_drv_data_t 		m_uart_tx_fifo_read;

static volatile uint32_t 		m_uart_rx_read_index = 0;
static volatile uint32_t 		m_uart_rx_write_index = 0;
static dma_recv_data_t			m_uart_rx_fifo[APP_UART_RX_FIFO_GROUPS];

// MARK: Uart rx queue variable

// MARK: tx fifo message handler
static __INLINE bool is_uart_tx_fifo_empty(void)
{
    return (m_uart_tx_read_index & APP_UART_TX_FIFO_MASK) == (m_uart_tx_write_index & APP_UART_TX_FIFO_MASK);
}

#define UARTE_MAX_CNT	255
static void uart_tx_fifo_queue(const uint8_t *p_buff, uint16_t len)
{
    if (p_buff == NULL) return;

    uint8_t *ptr = (uint8_t *)p_buff;

    do
    {
        uint32_t index = m_uart_tx_write_index & APP_UART_TX_FIFO_MASK;
        m_uart_tx_fifo[index].p_data_buff = (uint8_t *)ptr;
        if (len >= UARTE_MAX_CNT)
        {
            m_uart_tx_fifo[index].length		= UARTE_MAX_CNT;
            len -= UARTE_MAX_CNT;
            ptr += UARTE_MAX_CNT;
            UART_LOG_DUBUG("[UART]: tx fifo index  = %d\r\n", index);
        }
        else
        {
            m_uart_tx_fifo[index].length		= len;
            len = 0;
            UART_LOG_DUBUG("[UART]: tx fifo index  = %d\r\n", index);
        }

        m_uart_tx_write_index++;
    }
    while (len > 0);

}

static app_uart_drv_data_t* uart_tx_fifo_dequeue(void)
{
    if (!is_uart_tx_fifo_empty())
    {
        uint32_t index = m_uart_tx_read_index & APP_UART_TX_FIFO_MASK;

        m_uart_tx_fifo_read.p_data_buff	= m_uart_tx_fifo[index].p_data_buff;
        m_uart_tx_fifo_read.length		= m_uart_tx_fifo[index].length;
        if (sg_enUart == UART_State_GPS)
        {
        }
        m_uart_tx_read_index++;

        return &m_uart_tx_fifo_read;
    }

    return NULL;
}

void uart_get_tx_fifo_index(bool *is_empty, uint32_t *read, uint32_t *write)
{
    *is_empty = false;
    if (is_uart_tx_fifo_empty())
    {
        *is_empty = true;
    }
    *read =  m_uart_tx_read_index & APP_UART_TX_FIFO_MASK;
    *write = m_uart_tx_write_index & APP_UART_TX_FIFO_MASK;

}

void uart_clear_tx_buffer(void)
{
    m_uart_tx_read_index = m_uart_tx_write_index = 0;
}
void uart_clear_rx_buffer(void)
{
    m_uart_rx_read_index = m_uart_rx_write_index = 0;
}

static void uart_rx_fifo_queue(uint16_t len)
{
    m_uart_rx_fifo[m_uart_rx_write_index & APP_UART_RX_FIFO_GROUPS_MASK].len = len;
    m_uart_rx_write_index++;

    //UART_LOG_DUBUG("[UART]: rx write index = %d\r\n", m_uart_rx_write_index);
}

static dma_recv_data_t* uart_rx_fifo_dequeue(void)
{
    uint32_t rx_read_index = m_uart_rx_read_index;
    if ((rx_read_index & APP_UART_RX_FIFO_GROUPS_MASK)
        == (m_uart_rx_write_index & APP_UART_RX_FIFO_GROUPS_MASK)) return NULL;

    dma_recv_data_t *pointer = &m_uart_rx_fifo[m_uart_rx_read_index & APP_UART_RX_FIFO_GROUPS_MASK];

    m_uart_rx_read_index++;

    //UART_LOG_DUBUG("[UART]: rx read index = %d\r\n", m_uart_rx_read_index);

    if (pointer->len == 0) return NULL;

    return pointer;
}

bool is_uart_transmitting(void)
{
    return m_is_tx_in_progress;
}



void uart_disable(void)
{
    //if (m_is_uart_enable)
    {
        m_is_uart_enable 	= false;
        NRF_UARTE0->ENABLE	= UARTE_ENABLE_ENABLE_Disabled << UARTE_ENABLE_ENABLE_Pos;

        nrf_gpio_cfg_input(NRF_UARTE0->PSEL.RXD, NRF_GPIO_PIN_NOPULL);
        nrf_gpio_cfg_input(NRF_UARTE0->PSEL.TXD, NRF_GPIO_PIN_NOPULL);
        nrf_gpio_pin_set(NRF_UARTE0->PSEL.RXD);
        nrf_gpio_pin_set(NRF_UARTE0->PSEL.TXD);
    }
}

static void uart_tx_start(void)
{
    if (m_is_tx_in_progress) return;

    app_uart_drv_data_t *p_rx_drv_data = uart_tx_fifo_dequeue();

    if (p_rx_drv_data != NULL)
    {
        NRF_UARTE0->TXD.PTR = (uint32_t)p_rx_drv_data->p_data_buff;
        NRF_UARTE0->TXD.MAXCNT = p_rx_drv_data->length;
        NRF_UARTE0->TASKS_STARTTX = 1;

        //UART_LOG_DUBUG("[AUD]-tx buffer is valid\r\n");
        if (sg_enUart == UART_State_GPS)
        {
            static uint16_t mLen =  0;
            mLen += p_rx_drv_data->length;
        }
        m_is_tx_in_progress = true;
    }
    else
    {
        m_is_tx_in_progress = false;
    }
}

void uart_transmit_data_abort(UART_INSTANCE_SEQ_T instance_seq)
{

/*
  if (instance_seq != m_uart_instance.seq_id) //deny abort request
    return;
*/
    uart_clear_tx_buffer(); //clear the tx buffer.
    uart_clear_rx_buffer();
    m_is_tx_in_progress = false;
    if (NRF_UARTE0->EVENTS_TXSTARTED > 0)
    {
        NRF_UARTE0->TASKS_STOPTX = 1;
    }
}

bool uart_data_packet_put(UART_INSTANCE_SEQ_T  instance_seq, const uint8_t *p_buff, uint16_t len)
{
    if (sg_enUart == UART_State_GPS)
    {
    }
    if (instance_seq != m_uart_instance.seq_id) return false;

    UART_LOG_INFO("UART len %d\r\n", len);

    uart_tx_fifo_queue(p_buff, len);

    if (!m_is_tx_in_progress)
    {
        uart_tx_start();
    }

    return true;
}

void uart_receive_data_abort(bool restart, UART_INSTANCE_SEQ_T instance_seq)
{
    if (instance_seq != m_uart_instance.seq_id) return;

    NRF_UARTE0->TASKS_STOPRX = 1;
    m_is_uart_endrx_shortcut_startrx = restart;
}

static __INLINE void uart_start_reception(void)
{
    NRF_UARTE0->RXD.PTR			= (uint32_t)m_uart_rx_fifo[\
                                                        m_uart_rx_write_index & APP_UART_RX_FIFO_GROUPS_MASK\
                                                        ].data_buff;

    NRF_UARTE0->RXD.MAXCNT		= APP_UART_RX_FIFO_SIZE;

    NRF_UARTE0->TASKS_STARTRX 	= 1;
}


void uart_required_init(void)
{
    NRF_UARTE0->ENABLE		= UARTE_ENABLE_ENABLE_Disabled << UARTE_ENABLE_ENABLE_Pos;

    NRF_UARTE0->BAUDRATE	= UARTE_BAUDRATE_BAUDRATE_Baud115200;
    NRF_UARTE0->CONFIG		= 0; //HWF disble / parity exclude

    NRF_UARTE0->PSEL.CTS	= UARTE_PSEL_CTS_CONNECT_Disconnected << UARTE_PSEL_CTS_CONNECT_Pos;
    NRF_UARTE0->PSEL.RTS	= UARTE_PSEL_RTS_CONNECT_Disconnected << UARTE_PSEL_RTS_CONNECT_Pos;
    NRF_UARTE0->PSEL.TXD	= UARTE_PSEL_TXD_CONNECT_Disconnected << UARTE_PSEL_TXD_CONNECT_Pos;
    NRF_UARTE0->PSEL.RXD	= UARTE_PSEL_RXD_CONNECT_Disconnected << UARTE_PSEL_RXD_CONNECT_Pos;

    NRF_UARTE0->INTENSET	= 0x00000000UL
//							| ( UARTE_INTENSET_TXSTOPPED_Enabled << UARTE_INTENSET_TXSTOPPED_Pos )	// Transmitter stopped
//							| ( UARTE_INTENSET_TXSTARTED_Enabled << UARTE_INTENSET_TXSTARTED_Pos )	// UART transmitter has started
       | (UARTE_INTENSET_RXSTARTED_Enabled << UARTE_INTENSET_RXSTARTED_Pos)  // UART receiver has started
       | (UARTE_INTENSET_RXTO_Enabled << UARTE_INTENSET_RXTO_Pos)      // Receiver timeout
       | (UARTE_INTENSET_ERROR_Enabled << UARTE_INTENSET_ERROR_Pos)      // Error detected
       | (UARTE_INTENSET_ENDTX_Enabled << UARTE_INTENSET_ENDTX_Pos)      // Last TX byte transmitted
//							| ( UARTE_INTENSET_TXDRDY_Enabled << UARTE_INTENSET_TXDRDY_Pos )		// Data sent from TXD
       | (UARTE_INTENSET_ENDRX_Enabled << UARTE_INTENSET_ENDRX_Pos)      // Receive buffer is filled up
       | (UARTE_INTENSET_RXDRDY_Enabled << UARTE_INTENSET_RXDRDY_Pos)    // Data received in RXD (but potentially not yet transferred to Data RAM)
//							| ( UARTE_INTENSET_NCTS_Enabled << UARTE_INTENSET_NCTS_Pos )			// CTS is deactivated (set high). Not Clear To Send.
//							| ( UARTE_INTENSET_CTS_Enabled << UARTE_INTENSET_CTS_Pos )				// CTS is activated (set low). Clear To Send.
    ;

    nrf_drv_common_irq_enable(nrf_drv_get_IRQn((void *)NRF_UARTE0), UART_DEFAULT_CONFIG_IRQ_PRIORITY);

    m_is_uart_enable = false;

    memset(&m_uart_instance, 0, sizeof(m_uart_instance));
}


UART_State_en UartGet_Instance()
{
    return sg_enUart;
}

bool uart_arbitrate_apply(const uart_instance_t *p_config, UART_INSTANCE_SEQ_T *instance_seq)
{
    if (p_config == NULL) return false;

    // has been use
    if (m_uart_instance.in_used)
    {

        if (p_config->piority < m_uart_instance.piority)
        {
            return false;
        }
        else if (p_config->piority > m_uart_instance.piority)
        {
            if (m_uart_instance.evt_handle != NULL)
            {
                m_uart_evt.evt_id	= UART_EVT_LOSE_CTRL;
                m_uart_instance.evt_handle(&m_uart_evt);
            }
        }
        else // do nothing
        {
            *instance_seq = m_uart_instance.seq_id;
            return true;
        }
    }
    uart_transmit_data_abort(*instance_seq);
    // have a higer piority
    m_uart_instance.seq_id++;
    *instance_seq							      = m_uart_instance.seq_id;
    m_uart_instance.piority					= p_config->piority;

    m_uart_instance.rxd_pin					= p_config->rxd_pin;
    m_uart_instance.txd_pin					= p_config->txd_pin;
    m_uart_instance.evt_handle		  = p_config->evt_handle;

    // Disable UART
    if (m_is_uart_enable)
    {
        NRF_UARTE0->ENABLE		= UARTE_ENABLE_ENABLE_Disabled << UARTE_ENABLE_ENABLE_Pos;
    }

    nrf_gpio_cfg_output(p_config->txd_pin);
    nrf_gpio_cfg_input(p_config->rxd_pin, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_pin_set(p_config->txd_pin);
    nrf_gpio_pin_set(p_config->rxd_pin);

    NRF_UARTE0->BAUDRATE			 = p_config->baudrate;
    NRF_UARTE0->PSEL.TXD			 = p_config->txd_pin;
    NRF_UARTE0->PSEL.RXD			 = p_config->rxd_pin;

    if (p_config->baudrate == UARTE_BAUDRATE_BAUDRATE_Baud9600)
    {
        sg_enUart = UART_State_GPS;
    }
    else
    {
        sg_enUart = UART_State_GSM;
    }
    NRF_UARTE0->ENABLE				 = UARTE_ENABLE_ENABLE_Enabled << UARTE_ENABLE_ENABLE_Pos;

    uart_start_reception();
    m_is_uart_enable 				 = true;
    m_is_uart_endrx_shortcut_startrx = true;

    m_uart_instance.in_used = true;

    return true;
}

void uart_release_control(UART_INSTANCE_SEQ_T instance_seq)
{
    if (instance_seq != m_uart_instance.seq_id) return;

    if (m_uart_instance.in_used)
    {
        UART_INSTANCE_SEQ_T		seq_id = m_uart_instance.seq_id;

        memset(&m_uart_instance, 0, sizeof(m_uart_instance));

        m_uart_instance.in_used 	= false;
        m_uart_instance.seq_id	= seq_id;
        sg_enUart = UART_State_CLOSE;
        uart_disable();
    }
}

void uart_select_baudrate(uint32_t bps)
{
    if (m_is_uart_enable > 0)
    {
        NRF_UARTE0->ENABLE		= UARTE_ENABLE_ENABLE_Disabled << UARTE_ENABLE_ENABLE_Pos;
    }

    NRF_UARTE0->BAUDRATE		= bps;

    NRF_UARTE0->ENABLE 			= UARTE_ENABLE_ENABLE_Enabled << UARTE_ENABLE_ENABLE_Pos;

    NRF_UARTE0->TASKS_STARTRX   = 1;
}


void UARTE0_UART0_IRQHandler(void)
{
    // transmit completed
    if (NRF_UARTE0->EVENTS_ENDTX > 0)
    {
        NRF_UARTE0->EVENTS_ENDTX = 0;

        if (is_uart_tx_fifo_empty() && m_uart_instance.evt_handle != NULL)
        {
            m_uart_evt.evt_id	= UART_EVT_END_TX;
            m_uart_instance.evt_handle(&m_uart_evt);
        }

        //nrf_gpio_pin_toggle( GREEN_LED_PIN_NUMBER );

        m_is_tx_in_progress 	= false;
        uart_tx_start();

        if (!m_is_tx_in_progress)
        {
            NRF_UARTE0->TASKS_STOPTX = 1;
        }
    }

    // rxd line received data bytes
    if (NRF_UARTE0->EVENTS_RXDRDY > 0)
    {
        NRF_UARTE0->EVENTS_RXDRDY = 0;

        if (m_uart_instance.evt_handle != NULL)
        {
            m_uart_evt.evt_id	= UART_EVT_RXRDY;
            m_uart_instance.evt_handle(&m_uart_evt);
        }
    }

    // receive completed, fetch data buffer to handle
    if (NRF_UARTE0->EVENTS_ENDRX > 0)
    {
        NRF_UARTE0->EVENTS_ENDRX = 0;

        uart_rx_fifo_queue(NRF_UARTE0->RXD.AMOUNT); // save rx data length
        if (m_is_uart_endrx_shortcut_startrx)     // restart receive task
            uart_start_reception();

//		nrf_gpio_pin_toggle( RED_LED_PIN_NUMBER );

        if (m_uart_instance.evt_handle != NULL)     // generate event
        {
            dma_recv_data_t *p_rx_fifo = uart_rx_fifo_dequeue();

            if (p_rx_fifo != NULL)
            {
                m_uart_evt.evt_id				= UART_EVT_END_RX;
                m_uart_evt.params.end_rx.p_buff 	= p_rx_fifo->data_buff;
                m_uart_evt.params.end_rx.len	  	= p_rx_fifo->len;


                m_uart_instance.evt_handle(&m_uart_evt);
            }
        }
    }

    if (NRF_UARTE0->EVENTS_RXTO > 0)
    {
        NRF_UARTE0->EVENTS_RXTO = 0;
//		nrf_gpio_pin_toggle( BLUE_LED_PIN_NUMBER );
        if (m_uart_instance.evt_handle != NULL)
        {
            m_uart_evt.evt_id	= UART_EVT_RXTO;
            m_uart_instance.evt_handle(&m_uart_evt);
        }
    }

    if (NRF_UARTE0->EVENTS_ERROR > 0)
    {
        NRF_UARTE0->EVENTS_ERROR = 0;
        //UART_LOG_DUBUG("x");
    }


    if (NRF_UARTE0->EVENTS_RXSTARTED > 0)  //event notify
    {
        NRF_UARTE0->EVENTS_RXSTARTED = 0;

        if (m_uart_instance.evt_handle != NULL)
        {
            m_uart_evt.evt_id	= UART_EVT_RXSTARTED;
            m_uart_instance.evt_handle(&m_uart_evt);
        }
    }

}



