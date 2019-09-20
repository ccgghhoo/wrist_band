
// ASD		app spi driver

#include "nrf.h"
#include "nrf_gpio.h"

#include "boards.h"
#include "HalSpi.h"

static bool				m_is_spi_enable = false;

static bool 			m_is_spi_initialized = false;

//static uint32_t 		m_spi_flags = 0;


void HalSpi0_Enable(void)
{
    if (m_is_spi_enable) return;
    NRF_SPI0->ENABLE	= SPI_ENABLE_ENABLE_Enabled;

    m_is_spi_enable = true;
}

void HalSpi0_Disable(void)
{
    if (!m_is_spi_enable) return;

    NRF_SPI0->ENABLE	= SPI_ENABLE_ENABLE_Disabled;

    m_is_spi_enable = false;
}

uint8_t HalSpi0_Xfer(uint8_t byte)
{
    NRF_SPI0->TXD = byte;

    while (NRF_SPI0->EVENTS_READY == 0);
    NRF_SPI0->EVENTS_READY = 0;

    return (uint8_t)NRF_SPI0->RXD;
}


void HalSpi0_Init(void)
{
    if (m_is_spi_initialized) return;

    NRF_SPI0->PSEL.MISO = SPI0_MISO_PIN_NUMBER;
    NRF_SPI0->PSEL.MOSI = SPI0_MOSI_PIN_NUMBER;
    NRF_SPI0->PSEL.SCK	= SPI0_CLK_PIN_NUMBER;

    NRF_SPI0->FREQUENCY	= SPI_FREQUENCY_FREQUENCY_M4;
    NRF_SPI0->CONFIG	= 0; //MSB first, CPHA = 0/ CPOL = 0

    NRF_SPI0->ENABLE	= SPI_ENABLE_ENABLE_Disabled << SPI_ENABLE_ENABLE_Pos;

#if HAS_GYRO_MODULE
    nrf_gpio_cfg_output(ICM20648_CHIPSELECT_PIN_NUMBER);
    nrf_gpio_pin_set(ICM20648_CHIPSELECT_PIN_NUMBER);
#endif

#if HAS_ACCEL_MODULE
    nrf_gpio_cfg_output(ACCELERATOR_CHIPSELECT_PIN_NUMBER);
    nrf_gpio_pin_set(ACCELERATOR_CHIPSELECT_PIN_NUMBER);
#endif

    nrf_gpio_cfg_output(FLASH_CHIPSELECT_PIN_NUMBER);
    nrf_gpio_pin_set(FLASH_CHIPSELECT_PIN_NUMBER);


    m_is_spi_initialized = true;
}



