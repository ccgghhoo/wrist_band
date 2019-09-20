

#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_drv_gpiote.h"

#include "HalSpi.h"

#include "boards.h"
#include "lis3dh.h"


#define LIS3DH_CS_ENABLE        nrf_gpio_pin_clear(ACCELERATOR_CHIPSELECT_PIN_NUMBER)
#define LIS3DH_CS_DISABLE       nrf_gpio_pin_set(ACCELERATOR_CHIPSELECT_PIN_NUMBER)

#define LIS3DH_IDENTIFIER       0x33

static lis3dh_t m_lis3dh;


#define _25_hz   0x37
#define _50_hz   0x47
#define _100_hz  0x57  //add at:2018 4 4


static const uint8_t lis3dh_sleep_cfg[][2] = {
    LIS3DH_CTRL_REG1, 0x27,     // ODR=10Hz, LP enable, X/Y/Z enable
    LIS3DH_CTRL_REG2, 0x09,     // Normal Mode
    LIS3DH_CTRL_REG3, 0x40,     // IA1 interrupt on INT1
    LIS3DH_CTRL_REG4, 0x10,     // FS: +/- 4g, high resolution disable
    LIS3DH_CTRL_REG5, 0x08,     // int1 latch
    LIS3DH_CTRL_REG6, 0x02,     // change interrupt porality to lOW

    LIS3DH_FIFO_CTRL_REG, 0x00, // Bypass mode, clear fifo buffer

    LIS3DH_INT1_THS, 3,        // 1 lsb = 32mg, FS = +/-4G
    LIS3DH_INT1_DURATION, 0x01, // 1/ODR
//    LIS3DH_INT1_CFG, 0x2A,      // 3-axis enable
};
static const uint8_t lis3dh_work_cfg[][2] = {
    LIS3DH_INT1_CFG, 0x00,      // Disable INT1 3-axis

    LIS3DH_CTRL_REG1, 0x00,     // Disable
    LIS3DH_CTRL_REG2, 0x00,
    LIS3DH_CTRL_REG3, 0x00,     // Disable Interrupt
    LIS3DH_CTRL_REG4, 0x18,     // FS: +/- 4g, high resolutiohn enable
    LIS3DH_CTRL_REG5, 0x40,     // FIFO enable

    LIS3DH_FIFO_CTRL_REG, 0x00, // byPass Mode, clear fifo bytes
    LIS3DH_FIFO_CTRL_REG, 0x80, //Stream Mode
    LIS3DH_REFERENCE, 0x00,

    LIS3DH_CTRL_REG1, _100_hz,     //_100_hz // ODR=50Hz, LP disable, x/y/z enable
};


static void lis3dh_write_byte(uint8_t reg, uint8_t byte)
{
    LIS3DH_CS_ENABLE;
    HalSpi0_Xfer(reg);
    HalSpi0_Xfer(byte);
    LIS3DH_CS_DISABLE;
}

static uint8_t lis3dh_read_byte(uint8_t reg)
{
    uint8_t byte;

    LIS3DH_CS_ENABLE;
    HalSpi0_Xfer(reg | 0x80);
    byte =  HalSpi0_Xfer(0xff);
    LIS3DH_CS_DISABLE;

    return byte;
}

//static void lis3dh_read_2bytes(uint8_t reg, uint8_t * pdata )
//{
//    LIS3DH_CS_ENABLE;
//    HalSpi0_Xfer( reg | 0x80 );
//	pdata[0] = HalSpi0_Xfer( 0xff );
//    pdata[1] = HalSpi0_Xfer( 0xff );
//    LIS3DH_CS_DISABLE;
//}

static void lis3dh_read_fifo_raw(uint8_t *pdata)
{
    for (uint32_t i = 0; i < 6; i++)
    {
        pdata[i] = lis3dh_read_byte(LIS3DH_OUT_X_L + i);
    }
//    lis3dh_read_2bytes( LIS3DH_OUT_X_L, pdata + 0 );
//    lis3dh_read_2bytes( LIS3DH_OUT_Y_L, pdata + 2 );
//    lis3dh_read_2bytes( LIS3DH_OUT_Y_L, pdata + 4 );
}

uint8_t lis3dh_read_int1_src(void)
{
    uint8_t byte;
    HalSpi0_Enable();
    byte = lis3dh_read_byte(LIS3DH_INT1_SRC);
    HalSpi0_Disable();

    return byte;
}

void lis3dh_read_bytes(uint8_t start, uint8_t *pdata, uint8_t len)
{
    HalSpi0_Enable();

    for (uint32_t i = 0; i < len; i++)
    {
        pdata[i] = lis3dh_read_byte(start + i);
    }

    HalSpi0_Disable();
}

#define LIS3DH_GET_ID  0X01

#define LIS3DH_INT_1_H 0X02
#define LIS3DH_INT_1_L 0X02
static uint8_t sg_LIS3DHRes = 0;
uint8_t LIS3DH_GetStatus()
{
    return sg_LIS3DHRes;
}



void lis3dh_clear_int1_source(void)
{
    HalSpi0_Enable();

    (void)lis3dh_read_byte(LIS3DH_INT1_SRC);

    HalSpi0_Disable();
}

void lis3dh_setup_to_work(void)
{
    HalSpi0_Enable();


    for (uint32_t i = 0; i < sizeof(lis3dh_work_cfg) / 2; i++)
    {
        lis3dh_write_byte(lis3dh_work_cfg[i][0], lis3dh_work_cfg[i][1]);
    }
//    uint8_t byte;
//    byte =  lis3dh_read_byte( LIS3DH_INT1_SRC );
//    byte = byte;

    HalSpi0_Disable();
}

//void lis3dh_read_cfg_reg(void)
//{
//    uint8_t reg[0x20];
//    for( uint32_t i = 0 ; i < 0x20; i ++ ){
//        reg[ i ] = lis3dh_read_byte( 0x20 + i );
//    }
//}

void lis3dh_stay_in_sleep_mode(void)
{
    HalSpi0_Enable();
    for (uint32_t i = 0; i < sizeof(lis3dh_sleep_cfg) / 2; i++)
    {
        lis3dh_write_byte(lis3dh_sleep_cfg[i][0], lis3dh_sleep_cfg[i][1]);
    }

    lis3dh_read_byte(LIS3DH_REFERENCE);

    lis3dh_write_byte(LIS3DH_INT1_CFG, 0x2A);
//    uint8_t byte = lis3dh_read_byte( LIS3DH_INT1_SRC );
//    byte = byte;


//    lis3dh_read_cfg_reg();

    HalSpi0_Disable();
}

void lis3dh_stop_work(void)
{
    HalSpi0_Enable();

    lis3dh_write_byte(LIS3DH_CTRL_REG1, 0); // stop sample

//    lis3dh_read_byte( LIS3DH_REFERENCE ); // dummy read

    HalSpi0_Disable();
}

uint8_t lis3dh_read_fifo_axis_data(uint8_t *pdata, uint8_t max)
{
    uint8_t size;
    uint8_t bytes_to_read = 0;
    uint8_t i = 0;
    uint8_t *fp = pdata;
    HalSpi0_Enable();

    size = lis3dh_read_byte(LIS3DH_FIFO_SRC_REG) & 0x1f;
//    size = 1;
    if (size > 0)
    {
        while (i < size)
        {
            lis3dh_read_fifo_raw(fp);
            fp += 6;
            bytes_to_read += 6;
            i++;

            if (bytes_to_read >= max) break;
        }
    }

    HalSpi0_Disable();
    return bytes_to_read;
}




static void lis3dh_int_1_evt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (nrf_gpio_pin_read(pin) == 0)
    {
        sg_LIS3DHRes |= LIS3DH_INT_1_L;

        if (m_lis3dh.cb != NULL)
        {
            m_lis3dh.cb();
        }
    }
}

static volatile  uint8_t svg_ucLis3dhID = 0;




void lis3dh_init(lis3dh_init_t *p_init_obj)
{
    if (p_init_obj == NULL) return;

    m_lis3dh.cb = p_init_obj->evt_handle;
    HalSpi0_Init();

    HalSpi0_Enable();
    svg_ucLis3dhID = lis3dh_read_byte(LIS3DH_WHO_AM_I); // stop sample
    HalSpi0_Disable();

    if (svg_ucLis3dhID == 0x33)
    {
        sg_LIS3DHRes |= LIS3DH_GET_ID;
    }

    nrf_drv_gpiote_in_config_t int_config = {
        .sense 			= NRF_GPIOTE_POLARITY_TOGGLE,
        .pull			= NRF_GPIO_PIN_NOPULL,
        .is_watcher 	= false,
        .hi_accuracy	= false,
    };


    APP_ERROR_CHECK(nrf_drv_gpiote_in_init(ACCELERATOR_INT1_PIN_NUMBER,
                                           &int_config,
                                           lis3dh_int_1_evt_handler)
                   );
    if (nrf_gpio_pin_read(ACCELERATOR_INT1_PIN_NUMBER) != 0)
    {
        sg_LIS3DHRes |= LIS3DH_INT_1_H;
    }

    nrf_drv_gpiote_in_event_enable(ACCELERATOR_INT1_PIN_NUMBER, true);

}
