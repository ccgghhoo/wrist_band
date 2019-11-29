
/******************** (C) COPYRIGHT  ********************
* File Name          : LIS3DH_driver.c
* Author             : MSH Application Team
* Version            : $Revision:$
* Date               : $Date:$
* Description        : LIS3DH driver file
*                      
* HISTORY:
* Date               |  Modification                    |   Author

********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* THIS SOFTWARE IS SPECIFICALLY DESIGNED FOR EXCLUSIVE USE WITH ST PARTS.
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LIS3DH_drv.h"

#include "nrf_drv_spi.h"
#include "HalSpi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include "app_motion_detect.h"

#include <string.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


#define SPI_INSTANCE  0 /**< SPI instance index. */
#define LIS3DH_CHIP_ID  0X33
#define _25_hz    0x37
#define _50_hz    0x47
#define _100_hz  0x57 

#define LIS3DH_CS_ENABLE        nrf_gpio_pin_clear(SPIM0_SS_PIN)
#define LIS3DH_CS_DISABLE       nrf_gpio_pin_set(SPIM0_SS_PIN)

static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /**< SPI instance. */
static volatile bool spi_xfer_done;  /**< Flag used to indicate that SPI instance completed the transfer. */

static uint8_t       m_tx_buf[8];      /**< TX buffer. */
static uint8_t       m_rx_buf[8];      /**< RX buffer. */
//static uint8_t       m_length ;        /**< Transfer length. */
AxesRaw_t            m_lis3dh_acc_data;


#define   MY_ACC_MODE     0
#define   SPI_NRF_API     1
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
    //LIS3DH_INT1_CFG, 0x2A,      // 3-axis enable
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

//////////////////////////////////////////////////////////////////////////////////

void LIS3DH_spi_uninit(void)
{
    LIS3DH_SetMode(LIS3DH_POWER_DOWN); 
    
    nrfx_spi_uninit((nrfx_spi_t const *)&spi);
}

void lis3dh_work_mode(void)
{
#if MY_ACC_MODE	
    for (uint32_t i = 0; i < sizeof(lis3dh_work_cfg) / 2; i++)
    {
        LIS3DH_WriteReg(lis3dh_work_cfg[i][0], lis3dh_work_cfg[i][1]);
    }
#else
  
    LIS3DH_WriteReg(0x3e,0x00); //act threshold 2*32mg=64mg   
    LIS3DH_WriteReg(0x3f,0x00); //act duration
    LIS3DH_WriteReg(LIS3DH_CTRL_REG3,0x00);
    LIS3DH_WriteReg(LIS3DH_CTRL_REG6,0x00); 
    //nrf_drv_gpiote_in_event_disable(LIS3DH_INT2_PIN);
#endif    
 
}

void lis3dh_sleep_mode(void)
{
#if MY_ACC_MODE	
    for (uint32_t i = 0; i < sizeof(lis3dh_sleep_cfg) / 2; i++)
    {
        LIS3DH_WriteReg(lis3dh_sleep_cfg[i][0], lis3dh_sleep_cfg[i][1]);
    }
#else
    uint8_t int2src;
    LIS3DH_ReadReg(0X35, &int2src); //clear INT2
    
    LIS3DH_WriteReg(0x3e,0x8); //act threshold 8*16mg=128mg   
    LIS3DH_WriteReg(0x3f,0x02); //act duration
    LIS3DH_WriteReg(LIS3DH_CTRL_REG3,0x04);
    LIS3DH_WriteReg(LIS3DH_CTRL_REG6,0x08); 
    //nrf_drv_gpiote_in_event_enable(LIS3DH_INT2_PIN, true);
#endif   
}



static void lis3dh_int_1_evt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    
    //nrf_delay_ms(50);
    if (nrf_gpio_pin_read(pin) == 0)
    {       
		md_set_wakeup(); 
    }
}

void LIS3DH_INT_PIN_init(void)
{


#if MY_ACC_MODE
    nrf_drv_gpiote_in_config_t int_config = {
        .sense 			= NRF_GPIOTE_POLARITY_TOGGLE,
        .pull			= NRF_GPIO_PIN_NOPULL,
        .is_watcher 	= false,
        .hi_accuracy	= false,
    };

    APP_ERROR_CHECK(nrf_drv_gpiote_in_init(LIS3DH_INT1_PIN,
                                           &int_config,
                                           lis3dh_int_1_evt_handler)
                   );
    nrf_drv_gpiote_in_event_enable(LIS3DH_INT1_PIN, true);
    
#else
    
    nrf_drv_gpiote_in_config_t int_config = {
        .sense 			= NRF_GPIOTE_POLARITY_TOGGLE,
        .pull			= NRF_GPIO_PIN_NOPULL,
        .is_watcher 	= false,
        .hi_accuracy	= false, 
    };

    APP_ERROR_CHECK(nrf_drv_gpiote_in_init(LIS3DH_INT2_PIN,
                                           &int_config,
                                           lis3dh_int_1_evt_handler)
                   );   
    nrf_drv_gpiote_in_event_enable(LIS3DH_INT2_PIN, true);
    
#endif 
    
    //nrf_gpio_cfg_input(LIS3DH_INT1_PIN, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_default(LIS3DH_INT1_PIN);
    
  }
/*******************************************************************************
* Function Name     : LIS3DH_SpiInit
* Description       : Generic Reading function. It must be fullfilled with either
*           : I2C or SPI reading functions                  
* Input         : Register Address
* Output        : Data Read
* Return        : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
void LIS3DH_SpiInit(void) {
#if SPI_NRF_API  
    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   = SPIM0_SS_PIN;
    spi_config.miso_pin = SPIM0_MISO_PIN;
    spi_config.mosi_pin = SPIM0_MOSI_PIN;
    spi_config.sck_pin  = SPIM0_SCK_PIN;
//    spi_config.frequency    = NRF_DRV_SPI_FREQ_125K;
//    spi_config.mode = NRF_DRV_SPI_MODE_2;
    
    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, /*spi_event_handler*/NULL, NULL));
#else    
    HalSpi0_Init();
#endif        
    nrf_delay_ms(1);
    
    LIS3DH_GetWHO_AM_I(m_rx_buf);
    if(m_rx_buf[0]!=LIS3DH_CHIP_ID)
    {
        NRF_LOG_INFO("ERROR CHIP_ID: %X" ,m_rx_buf[0]);
        //return;
    }
    else
    {
        NRF_LOG_INFO("Gsensor is OK!!");
    }
    
    
#if MY_ACC_MODE    
    
    lis3dh_work_mode();
    
    
    return;
#else    
    
//    if(LIS3DH_SetSelfTest(LIS3DH_SELF_TEST_1)==MEMS_SUCCESS) 
//    {
//        NRF_LOG_INFO("self test OK!!!");
//    } 
//    else
//    {
//        NRF_LOG_INFO("self test faillll!!!");
//    }
    
    
    LIS3DH_WriteReg(LIS3DH_INT1_CFG, 0x00); 
      
    LIS3DH_WriteReg(LIS3DH_CTRL_REG1,_100_hz); //47=50HZ NORMAL MODE  57= 100HZ   

    LIS3DH_WriteReg(LIS3DH_CTRL_REG2,0x00); //cannot enable high pass filter, data 
//
    //LIS3DH_SetInt1Pin(LIS3DH_INT1_OVERRUN_ENABLE | LIS3DH_WTM_ON_INT1_ENABLE);
    LIS3DH_WriteReg(LIS3DH_CTRL_REG3,0x00); //06=overrun+watermark  10=XYZdata interrupt on INT1

    LIS3DH_WriteReg(LIS3DH_CTRL_REG4,0x18);  //00 +-2G 4mg/LSB  18=+-4G 32mg/LSB High-resolution 12bits

    //LIS3DH_FIFOModeEnable(LIS3DH_FIFO_STREAM_MODE);
    LIS3DH_WriteReg(LIS3DH_CTRL_REG5,0x40);  //FIFOEN  INT1,2 UNLATCH
    LIS3DH_WriteReg(LIS3DH_FIFO_CTRL_REG,0x00);
    LIS3DH_WriteReg(LIS3DH_FIFO_CTRL_REG,0x80/*|0x1d*/);  //FIFO stream mode ,water mark=0X1E
//    
    LIS3DH_WriteReg(LIS3DH_CTRL_REG6,0x00);  //active int out to INT2 ,polarity active low
    LIS3DH_WriteReg(0x26,0x00);
    LIS3DH_WriteReg(0x3e,0x00); //act threshold 2*32mg=64mg   
    LIS3DH_WriteReg(0x3f,0x00); //act duration   
    
//       LIS3DH_spi_uninit(); 
    
#endif
}


/*******************************************************************************
* Function Name     : LIS3DH_ReadReg
* Description       : Generic Reading function. It must be fullfilled with either
*           : I2C or SPI reading functions                  
* Input         : Register Address
* Output        : Data REad
* Return        : None
*******************************************************************************/
uint8_t LIS3DH_ReadReg(uint8_t Reg, uint8_t* recData) {
 
#if SPI_NRF_API    
  uint32_t error_code;
  m_tx_buf[0]=Reg; 
  m_tx_buf[0]|= 0x80;

  //APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, 1, recData, 1));
  error_code=nrf_drv_spi_transfer(&spi, m_tx_buf, 1, m_rx_buf, 2); //rx_number=2, recv the second byte is valid
   
  if(error_code==0)
  {
     // NRF_LOG_INFO("read gsesor reg OK!!!");
      *recData=m_rx_buf[1]; 
      return MEMS_SUCCESS;
  }
  else
  {
      //NRF_LOG_INFO("read gsesor reg faillll!!!");
      *recData=0;
      return MEMS_ERROR; 
  }
#else
 
    LIS3DH_CS_ENABLE;
    HalSpi0_Xfer(Reg | 0x80);
    *recData =  HalSpi0_Xfer(0xff);
    LIS3DH_CS_DISABLE;
    return MEMS_SUCCESS;
#endif    
  
  
}


/*******************************************************************************
* Function Name     : LIS3DH_WriteReg
* Description       : Generic Writing function. It must be fullfilled with either
*           : I2C or SPI writing function
* Input         : Register Address, Data to be written
* Output        : None
* Return        : None
*******************************************************************************/
uint8_t LIS3DH_WriteReg(uint8_t WriteAddr, uint8_t Data) {
#if SPI_NRF_API	
  uint32_t error_code;    
  m_tx_buf[0]=WriteAddr;
  m_tx_buf[1]=Data;  

  error_code=nrf_drv_spi_transfer(&spi, m_tx_buf, 2, 0, 0);
   //  APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, 2, 0, 0)); 
  if(error_code==0)
  {
      //NRF_LOG_INFO("write gsesor reg OK!!!");
      return MEMS_SUCCESS;
  }
  else
  {
      //NRF_LOG_INFO("write gsesor reg faillll!!!");
      return MEMS_ERROR; 
  } 
#else
    LIS3DH_CS_ENABLE;
    HalSpi0_Xfer(WriteAddr);
    HalSpi0_Xfer(Data);
    LIS3DH_CS_DISABLE;  

    return MEMS_SUCCESS;
#endif    
}


/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : LIS3DH_GetWHO_AM_I
* Description    : Read identification code by WHO_AM_I register
* Input          : Char to empty by Device identification Value
* Output         : None
* Return         : Status [value of FSS]
*******************************************************************************/
status_t LIS3DH_GetWHO_AM_I(uint8_t* val){
  
  if( !LIS3DH_ReadReg(LIS3DH_WHO_AM_I, val) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_GetStatusAUX
* Description    : Read the AUX status register
* Input          : Char to empty by status register buffer
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_GetStatusAUX(uint8_t* val) {
  
  if( !LIS3DH_ReadReg(LIS3DH_STATUS_AUX, val) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;  
}



/*******************************************************************************
* Function Name  : LIS3DH_GetStatusAUXBIT
* Description    : Read the AUX status register BIT
* Input          : LIS3DH_STATUS_AUX_321OR, LIS3DH_STATUS_AUX_3OR, LIS3DH_STATUS_AUX_2OR, LIS3DH_STATUS_AUX_1OR,
                   LIS3DH_STATUS_AUX_321DA, LIS3DH_STATUS_AUX_3DA, LIS3DH_STATUS_AUX_2DA, LIS3DH_STATUS_AUX_1DA
* Output         : None
* Return         : Status of BIT [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_GetStatusAUXBit(uint8_t statusBIT, uint8_t* val) {
  uint8_t value;  
  
  if( !LIS3DH_ReadReg(LIS3DH_STATUS_AUX, &value) )
    return MEMS_ERROR;
  
  if(statusBIT == LIS3DH_STATUS_AUX_321OR){
    if(value &= LIS3DH_STATUS_AUX_321OR){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }   
  }
  
  if(statusBIT == LIS3DH_STATUS_AUX_3OR){
    if(value &= LIS3DH_STATUS_AUX_3OR){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }     
  }
  
  if(statusBIT == LIS3DH_STATUS_AUX_2OR){
    if(value &= LIS3DH_STATUS_AUX_2OR){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }    
  }
  
  if(statusBIT == LIS3DH_STATUS_AUX_1OR){
    if(value &= LIS3DH_STATUS_AUX_1OR){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }   
  }
  
  if(statusBIT == LIS3DH_STATUS_AUX_321DA){
    if(value &= LIS3DH_STATUS_AUX_321DA) {     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }   
  }
  
  if(statusBIT == LIS3DH_STATUS_AUX_3DA){
    if(value &= LIS3DH_STATUS_AUX_3DA){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }   
  }
  
  if(statusBIT == LIS3DH_STATUS_AUX_2DA){
    if(value &= LIS3DH_STATUS_AUX_2DA){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  
  if(statusBIT == LIS3DH_STATUS_AUX_1DA){
    if(value &= LIS3DH_STATUS_AUX_1DA){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }  
  return MEMS_ERROR;
}


/*******************************************************************************
* Function Name  : LIS3DH_SetODR
* Description    : Sets LIS3DH Output Data Rate
* Input          : Output Data Rate
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetODR(LIS3DH_ODR_t ov){
  uint8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG1, &value) )
    return MEMS_ERROR;
  
  value &= 0x0f;
  value |= ov<<LIS3DH_ODR_BIT;
  
  if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG1, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_SetTemperature
* Description    : Sets LIS3DH Output Temperature
* Input          : MEMS_ENABLE, MEMS_DISABLE
* Output         : None
* Note           : For Read Temperature by LIS3DH_OUT_AUX_3, LIS3DH_SetADCAux and LIS3DH_SetBDU 
                   functions must be ENABLE
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetTemperature(State_t state){
  uint8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_TEMP_CFG_REG, &value) )
    return MEMS_ERROR;
  
  value &= 0xBF;
  value |= state<<LIS3DH_TEMP_EN;
  
  if( !LIS3DH_WriteReg(LIS3DH_TEMP_CFG_REG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_SetADCAux
* Description    : Sets LIS3DH Output ADC
* Input          : MEMS_ENABLE, MEMS_DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetADCAux(State_t state){
  uint8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_TEMP_CFG_REG, &value) )
    return MEMS_ERROR;
  
  value &= 0x7F;
  value |= state<<LIS3DH_ADC_PD;
  
  if( !LIS3DH_WriteReg(LIS3DH_TEMP_CFG_REG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_GetAuxRaw
* Description    : Read the Aux Values Output Registers
* Input          : Buffer to empty
* Output         : Aux Values Registers buffer
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_GetAuxRaw(LIS3DH_Aux123Raw_t* buff) {
  uint8_t valueL;
  uint8_t valueH;
  
  if( !LIS3DH_ReadReg(LIS3DH_OUT_1_L, &valueL) )
    return MEMS_ERROR;
  
  if( !LIS3DH_ReadReg(LIS3DH_OUT_1_H, &valueH) )
    return MEMS_ERROR;
  
  buff->AUX_1 = (u16_t)( (valueH << 8) | valueL )/16;
  
  if( !LIS3DH_ReadReg(LIS3DH_OUT_2_L, &valueL) )
    return MEMS_ERROR;
  
  if( !LIS3DH_ReadReg(LIS3DH_OUT_2_H, &valueH) )
    return MEMS_ERROR;
  
  buff->AUX_2 = (u16_t)( (valueH << 8) | valueL )/16;
  
  if( !LIS3DH_ReadReg(LIS3DH_OUT_3_L, &valueL) )
    return MEMS_ERROR;
  
  if( !LIS3DH_ReadReg(LIS3DH_OUT_3_H, &valueH) )
    return MEMS_ERROR;
  
  buff->AUX_3 = (u16_t)( (valueH << 8) | valueL )/16;
  
  return MEMS_SUCCESS;  
}


/*******************************************************************************
* Function Name  : LIS3DH_GetTempRaw
* Description    : Read the Temperature Values by AUX Output Registers OUT_3_H
* Input          : Buffer to empty
* Output         : Temperature Values Registers buffer
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_GetTempRaw(i8_t* buff) {
  uint8_t valueL;
  uint8_t valueH;
  
  if( !LIS3DH_ReadReg(LIS3DH_OUT_3_L, &valueL) )
    return MEMS_ERROR;
  
  if( !LIS3DH_ReadReg(LIS3DH_OUT_3_H, &valueH) )
    return MEMS_ERROR;
  
  *buff = (i8_t)( valueH );
  
  return MEMS_SUCCESS;  
}


/*******************************************************************************
* Function Name  : LIS3DH_SetMode
* Description    : Sets LIS3DH Operating Mode
* Input          : Modality (LIS3DH_NORMAL, LIS3DH_LOW_POWER, LIS3DH_POWER_DOWN)
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetMode(LIS3DH_Mode_t md) {
  uint8_t value;
  uint8_t value2;
  static   uint8_t ODR_old_value;
  
  if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG1, &value) )
    return MEMS_ERROR;
  
  if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG4, &value2) )
    return MEMS_ERROR;
  
  if((value & 0xF0)==0) 
    value = value | (ODR_old_value & 0xF0); //if it comes from POWERDOWN  
  
  switch(md) {
    
  case LIS3DH_POWER_DOWN:
    ODR_old_value = value;
    value &= 0x0F;
    break;
    
  case LIS3DH_NORMAL:
    value &= 0xF7;
    value |= (MEMS_RESET<<LIS3DH_LPEN);
    value2 &= 0xF7;
    value2 |= (MEMS_SET<<LIS3DH_HR);   //set HighResolution_BIT
    break;
    
  case LIS3DH_LOW_POWER:        
    value &= 0xF7;
    value |=  (MEMS_SET<<LIS3DH_LPEN);
    value2 &= 0xF7;
    value2 |= (MEMS_RESET<<LIS3DH_HR); //reset HighResolution_BIT
    break;
    
  default:
    return MEMS_ERROR;
  }
  
  if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG1, value) )
    return MEMS_ERROR;
  
  if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG4, value2) )
    return MEMS_ERROR;  
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_SetAxis
* Description    : Enable/Disable LIS3DH Axis
* Input          : LIS3DH_X_ENABLE/DISABLE | LIS3DH_Y_ENABLE/DISABLE | LIS3DH_Z_ENABLE/DISABLE
* Output         : None
* Note           : You MUST use all input variable in the argument, as example
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetAxis(LIS3DH_Axis_t axis) {
  uint8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG1, &value) )
    return MEMS_ERROR;
  value &= 0xF8;
  value |= (0x07 & axis);
  
  if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG1, value) )
    return MEMS_ERROR;   
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_SetFullScale
* Description    : Sets the LIS3DH FullScale
* Input          : LIS3DH_FULLSCALE_2/LIS3DH_FULLSCALE_4/LIS3DH_FULLSCALE_8/LIS3DH_FULLSCALE_16
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetFullScale(LIS3DH_Fullscale_t fs) {
  uint8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG4, &value) )
    return MEMS_ERROR;
  
  value &= 0xCF;    
  value |= (fs<<LIS3DH_FS);
  
  if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG4, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_SetBDU
* Description    : Enable/Disable Block Data Update Functionality
* Input          : ENABLE/DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetBDU(State_t bdu) {
  uint8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG4, &value) )
    return MEMS_ERROR;
  
  value &= 0x7F;
  value |= (bdu<<LIS3DH_BDU);
  
  if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG4, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_SetBLE
* Description    : Set Endianess (MSB/LSB)
* Input          : BLE_LSB / BLE_MSB
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetBLE(LIS3DH_Endianess_t ble) {
  uint8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG4, &value) )
    return MEMS_ERROR;
  
  value &= 0xBF;    
  value |= (ble<<LIS3DH_BLE);
  
  if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG4, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_SetSelfTest
* Description    : Set Self Test Modality
* Input          : LIS3DH_SELF_TEST_DISABLE/ST_0/ST_1
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetSelfTest(LIS3DH_SelfTest_t st) {
  uint8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG4, &value) )
    return MEMS_ERROR;
  
  value &= 0xF9;
  value |= (st<<LIS3DH_ST);
  
  if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG4, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_HPFClick
* Description    : Enable/Disable High Pass Filter for click
* Input          : MEMS_ENABLE/MEMS_DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_HPFClickEnable(State_t hpfe) {
  uint8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG2, &value) )
    return MEMS_ERROR;
  
  value &= 0xFB;
  value |= (hpfe<<LIS3DH_HPCLICK);
  
  if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG2, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_HPFAOI1
* Description    : Enable/Disable High Pass Filter for AOI on INT_1
* Input          : MEMS_ENABLE/MEMS_DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_HPFAOI1Enable(State_t hpfe) {
  uint8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG2, &value) )
    return MEMS_ERROR;
  
  value &= 0xFE;
  value |= (hpfe<<LIS3DH_HPIS1);
  
  if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG2, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_HPFAOI2
* Description    : Enable/Disable High Pass Filter for AOI on INT_2
* Input          : MEMS_ENABLE/MEMS_DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_HPFAOI2Enable(State_t hpfe) {
  uint8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG2, &value) )
    return MEMS_ERROR;
  
  value &= 0xFD;
  value |= (hpfe<<LIS3DH_HPIS2);
  
  if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG2, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_SetHPFMode
* Description    : Set High Pass Filter Modality
* Input          : LIS3DH_HPM_NORMAL_MODE_RES/LIS3DH_HPM_REF_SIGNAL/
                   LIS3DH_HPM_NORMAL_MODE/LIS3DH_HPM_AUTORESET_INT
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetHPFMode(LIS3DH_HPFMode_t hpm) {
  uint8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG2, &value) )
    return MEMS_ERROR;
  
  value &= 0x3F;
  value |= (hpm<<LIS3DH_HPM);
  
  if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG2, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_SetHPFCutOFF
* Description    : Set High Pass CUT OFF Freq
* Input          : HPFCF [0,3]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetHPFCutOFF(LIS3DH_HPFCutOffFreq_t hpf) {
  uint8_t value;
  
  if (hpf > 3)
    return MEMS_ERROR;
  
  if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG2, &value) )
    return MEMS_ERROR;
  
  value &= 0xCF;
  value |= (hpf<<LIS3DH_HPCF);
  
  if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG2, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
  
}


/*******************************************************************************
* Function Name  : LIS3DH_SetFilterDataSel
* Description    : Set Filter Data Selection bypassed or sent to FIFO OUT register
* Input          : MEMS_SET, MEMS_RESET
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetFilterDataSel(State_t state) {
  uint8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG2, &value) )
    return MEMS_ERROR;
  
  value &= 0xF7;
  value |= (state<<LIS3DH_FDS);
  
  if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG2, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
  
}


/*******************************************************************************
* Function Name  : LIS3DH_SetInt1Pin
* Description    : Set Interrupt1 pin Function
* Input          :  LIS3DH_CLICK_ON_PIN_INT1_ENABLE/DISABLE    | LIS3DH_I1_INT1_ON_PIN_INT1_ENABLE/DISABLE |              
                    LIS3DH_I1_INT2_ON_PIN_INT1_ENABLE/DISABLE  | LIS3DH_I1_DRDY1_ON_INT1_ENABLE/DISABLE    |              
                    LIS3DH_I1_DRDY2_ON_INT1_ENABLE/DISABLE     | LIS3DH_WTM_ON_INT1_ENABLE/DISABLE         |           
                    LIS3DH_INT1_OVERRUN_ENABLE/DISABLE  
* example        : SetInt1Pin(LIS3DH_CLICK_ON_PIN_INT1_ENABLE | LIS3DH_I1_INT1_ON_PIN_INT1_ENABLE |              
                    LIS3DH_I1_INT2_ON_PIN_INT1_DISABLE | LIS3DH_I1_DRDY1_ON_INT1_ENABLE | LIS3DH_I1_DRDY2_ON_INT1_ENABLE |
                    LIS3DH_WTM_ON_INT1_DISABLE | LIS3DH_INT1_OVERRUN_DISABLE   ) 
* Note           : To enable Interrupt signals on INT1 Pad (You MUST use all input variable in the argument, as example)
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetInt1Pin(LIS3DH_IntPinConf_t pinConf) {
  uint8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG3, &value) )
    return MEMS_ERROR;
  
  value &= 0x00;
  value |= pinConf;
  
  if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG3, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_SetInt2Pin
* Description    : Set Interrupt2 pin Function
* Input          : LIS3DH_CLICK_ON_PIN_INT2_ENABLE/DISABLE   | LIS3DH_I2_INT1_ON_PIN_INT2_ENABLE/DISABLE |               
                   LIS3DH_I2_INT2_ON_PIN_INT2_ENABLE/DISABLE | LIS3DH_I2_BOOT_ON_INT2_ENABLE/DISABLE |                   
                   LIS3DH_INT_ACTIVE_HIGH/LOW
* example        : LIS3DH_SetInt2Pin(LIS3DH_CLICK_ON_PIN_INT2_ENABLE/DISABLE | LIS3DH_I2_INT1_ON_PIN_INT2_ENABLE/DISABLE |               
                   LIS3DH_I2_INT2_ON_PIN_INT2_ENABLE/DISABLE | LIS3DH_I2_BOOT_ON_INT2_ENABLE/DISABLE |                   
                   LIS3DH_INT_ACTIVE_HIGH/LOW)
* Note           : To enable Interrupt signals on INT2 Pad (You MUST use all input variable in the argument, as example)
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetInt2Pin(LIS3DH_IntPinConf_t pinConf) {
  uint8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG6, &value) )
    return MEMS_ERROR;
  
  value &= 0x00;
  value |= pinConf;
  
  if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG6, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}                       


/*******************************************************************************
* Function Name  : LIS3DH_SetClickCFG
* Description    : Set Click Interrupt config Function
* Input          : LIS3DH_ZD_ENABLE/DISABLE | LIS3DH_ZS_ENABLE/DISABLE  | LIS3DH_YD_ENABLE/DISABLE  | 
                   LIS3DH_YS_ENABLE/DISABLE | LIS3DH_XD_ENABLE/DISABLE  | LIS3DH_XS_ENABLE/DISABLE 
* example        : LIS3DH_SetClickCFG( LIS3DH_ZD_ENABLE | LIS3DH_ZS_DISABLE | LIS3DH_YD_ENABLE | 
                               LIS3DH_YS_DISABLE | LIS3DH_XD_ENABLE | LIS3DH_XS_ENABLE)
* Note           : You MUST use all input variable in the argument, as example
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetClickCFG(uint8_t status) {
  uint8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_CLICK_CFG, &value) )
    return MEMS_ERROR;
  
  value &= 0xC0;
  value |= status;
  
  if( !LIS3DH_WriteReg(LIS3DH_CLICK_CFG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}  


/*******************************************************************************
* Function Name  : LIS3DH_SetClickTHS
* Description    : Set Click Interrupt threshold
* Input          : Click-click Threshold value [0-127]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetClickTHS(uint8_t ths) {
  
  if(ths>127)     
    return MEMS_ERROR;
  
  if( !LIS3DH_WriteReg(LIS3DH_CLICK_THS, ths) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
} 


/*******************************************************************************
* Function Name  : LIS3DH_SetClickLIMIT
* Description    : Set Click Interrupt Time Limit
* Input          : Click-click Time Limit value [0-127]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetClickLIMIT(uint8_t t_limit) {
  
  if(t_limit>127)     
    return MEMS_ERROR;
  
  if( !LIS3DH_WriteReg(LIS3DH_TIME_LIMIT, t_limit) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
} 


/*******************************************************************************
* Function Name  : LIS3DH_SetClickLATENCY
* Description    : Set Click Interrupt Time Latency
* Input          : Click-click Time Latency value [0-255]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetClickLATENCY(uint8_t t_latency) {
  
  if( !LIS3DH_WriteReg(LIS3DH_TIME_LATENCY, t_latency) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
} 


/*******************************************************************************
* Function Name  : LIS3DH_SetClickWINDOW
* Description    : Set Click Interrupt Time Window
* Input          : Click-click Time Window value [0-255]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetClickWINDOW(uint8_t t_window) {
  
  if( !LIS3DH_WriteReg(LIS3DH_TIME_WINDOW, t_window) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_GetClickResponse
* Description    : Get Click Interrupt Response by CLICK_SRC REGISTER
* Input          : char to empty by Click Response Typedef
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_GetClickResponse(uint8_t* res) {
  uint8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_CLICK_SRC, &value) ) 
    return MEMS_ERROR;
  
  value &= 0x7F;
  
  if((value & LIS3DH_IA)==0) {        
    *res = LIS3DH_NO_CLICK;     
    return MEMS_SUCCESS;
  }
  else {
    if (value & LIS3DH_DCLICK){
      if (value & LIS3DH_CLICK_SIGN){
        if (value & LIS3DH_CLICK_Z) {
          *res = LIS3DH_DCLICK_Z_N;   
          return MEMS_SUCCESS;
        }
        if (value & LIS3DH_CLICK_Y) {
          *res = LIS3DH_DCLICK_Y_N;   
          return MEMS_SUCCESS;
        }
        if (value & LIS3DH_CLICK_X) {
          *res = LIS3DH_DCLICK_X_N;   
          return MEMS_SUCCESS;
        }
      }
      else{
        if (value & LIS3DH_CLICK_Z) {
          *res = LIS3DH_DCLICK_Z_P;   
          return MEMS_SUCCESS;
        }
        if (value & LIS3DH_CLICK_Y) {
          *res = LIS3DH_DCLICK_Y_P;   
          return MEMS_SUCCESS;
        }
        if (value & LIS3DH_CLICK_X) {
          *res = LIS3DH_DCLICK_X_P;   
          return MEMS_SUCCESS;
        }
      }       
    }
    else{
      if (value & LIS3DH_CLICK_SIGN){
        if (value & LIS3DH_CLICK_Z) {
          *res = LIS3DH_SCLICK_Z_N;   
          return MEMS_SUCCESS;
        }
        if (value & LIS3DH_CLICK_Y) {
          *res = LIS3DH_SCLICK_Y_N;   
          return MEMS_SUCCESS;
        }
        if (value & LIS3DH_CLICK_X) {
          *res = LIS3DH_SCLICK_X_N;   
          return MEMS_SUCCESS;
        }
      }
      else{
        if (value & LIS3DH_CLICK_Z) {
          *res = LIS3DH_SCLICK_Z_P;   
          return MEMS_SUCCESS;
        }
        if (value & LIS3DH_CLICK_Y) {
          *res = LIS3DH_SCLICK_Y_P;   
          return MEMS_SUCCESS;
        }
        if (value & LIS3DH_CLICK_X) {
          *res = LIS3DH_SCLICK_X_P;   
          return MEMS_SUCCESS;
        }
      }
    }
  }
  return MEMS_ERROR;
} 


/*******************************************************************************
* Function Name  : LIS3DH_Int1LatchEnable
* Description    : Enable Interrupt 1 Latching function
* Input          : ENABLE/DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_Int1LatchEnable(State_t latch) {
  uint8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG5, &value) )
    return MEMS_ERROR;
  
  value &= 0xF7;
  value |= latch<<LIS3DH_LIR_INT1;
  
  if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG5, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_ResetInt1Latch
* Description    : Reset Interrupt 1 Latching function
* Input          : None
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_ResetInt1Latch(void) {
  uint8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_INT1_SRC, &value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_SetIntConfiguration
* Description    : Interrupt 1 Configuration (without LIS3DH_6D_INT)
* Input          : LIS3DH_INT1_AND/OR | LIS3DH_INT1_ZHIE_ENABLE/DISABLE | LIS3DH_INT1_ZLIE_ENABLE/DISABLE...
* Output         : None
* Note           : You MUST use all input variable in the argument, as example
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetIntConfiguration(LIS3DH_Int1Conf_t ic) {
  uint8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_INT1_CFG, &value) )
    return MEMS_ERROR;
  
  value &= 0x40; 
  value |= ic;
  
  if( !LIS3DH_WriteReg(LIS3DH_INT1_CFG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
} 

     
/*******************************************************************************
* Function Name  : LIS3DH_SetIntMode
* Description    : Interrupt 1 Configuration mode (OR, 6D Movement, AND, 6D Position)
* Input          : LIS3DH_INT_MODE_OR, LIS3DH_INT_MODE_6D_MOVEMENT, LIS3DH_INT_MODE_AND, 
                   LIS3DH_INT_MODE_6D_POSITION
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetIntMode(LIS3DH_Int1Mode_t int_mode) {
  uint8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_INT1_CFG, &value) )
    return MEMS_ERROR;
  
  value &= 0x3F; 
  value |= (int_mode<<LIS3DH_INT_6D);
  
  if( !LIS3DH_WriteReg(LIS3DH_INT1_CFG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}

    
/*******************************************************************************
* Function Name  : LIS3DH_SetInt6D4DConfiguration
* Description    : 6D, 4D Interrupt Configuration
* Input          : LIS3DH_INT1_6D_ENABLE, LIS3DH_INT1_4D_ENABLE, LIS3DH_INT1_6D_4D_DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetInt6D4DConfiguration(LIS3DH_INT_6D_4D_t ic) {
  uint8_t value;
  uint8_t value2;
  
  if( !LIS3DH_ReadReg(LIS3DH_INT1_CFG, &value) )
    return MEMS_ERROR;
  if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG5, &value2) )
    return MEMS_ERROR;
  
  if(ic == LIS3DH_INT1_6D_ENABLE){
    value &= 0xBF; 
    value |= (MEMS_ENABLE<<LIS3DH_INT_6D);
    value2 &= 0xFB; 
    value2 |= (MEMS_DISABLE<<LIS3DH_D4D_INT1);
  }
  
  if(ic == LIS3DH_INT1_4D_ENABLE){
    value &= 0xBF; 
    value |= (MEMS_ENABLE<<LIS3DH_INT_6D);
    value2 &= 0xFB; 
    value2 |= (MEMS_ENABLE<<LIS3DH_D4D_INT1);
  }
  
  if(ic == LIS3DH_INT1_6D_4D_DISABLE){
    value &= 0xBF; 
    value |= (MEMS_DISABLE<<LIS3DH_INT_6D);
    value2 &= 0xFB; 
    value2 |= (MEMS_DISABLE<<LIS3DH_D4D_INT1);
  }
  
  if( !LIS3DH_WriteReg(LIS3DH_INT1_CFG, value) )
    return MEMS_ERROR;
  if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG5, value2) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_Get6DPosition
* Description    : 6D, 4D Interrupt Position Detect
* Input          : Byte to empty by POSITION_6D_t Typedef
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_Get6DPosition(uint8_t* val){
  uint8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_INT1_SRC, &value) )
    return MEMS_ERROR;
  
  value &= 0x7F;
  
  switch (value){
  case LIS3DH_UP_SX:   
    *val = LIS3DH_UP_SX;    
    break;
  case LIS3DH_UP_DX:   
    *val = LIS3DH_UP_DX;    
    break;
  case LIS3DH_DW_SX:   
    *val = LIS3DH_DW_SX;    
    break;
  case LIS3DH_DW_DX:   
    *val = LIS3DH_DW_DX;    
    break;
  case LIS3DH_TOP:     
    *val = LIS3DH_TOP;      
    break;
  case LIS3DH_BOTTOM:  
    *val = LIS3DH_BOTTOM;   
    break;
  }
  
  return MEMS_SUCCESS;  
}


/*******************************************************************************
* Function Name  : LIS3DH_SetInt1Threshold
* Description    : Sets Interrupt 1 Threshold
* Input          : Threshold = [0,31]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetInt1Threshold(uint8_t ths) {
  if (ths > 127)
    return MEMS_ERROR;
  
  if( !LIS3DH_WriteReg(LIS3DH_INT1_THS, ths) )
    return MEMS_ERROR;    
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_SetInt1Duration
* Description    : Sets Interrupt 1 Duration
* Input          : Duration value
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetInt1Duration(LIS3DH_Int1Conf_t id) {
  
  if (id > 127)
    return MEMS_ERROR;
  
  if( !LIS3DH_WriteReg(LIS3DH_INT1_DURATION, id) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_FIFOModeEnable
* Description    : Sets Fifo Modality
* Input          : LIS3DH_FIFO_DISABLE, LIS3DH_FIFO_BYPASS_MODE, LIS3DH_FIFO_MODE, 
                   LIS3DH_FIFO_STREAM_MODE, LIS3DH_FIFO_TRIGGER_MODE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_FIFOModeEnable(LIS3DH_FifoMode_t fm) {
  uint8_t value;  
  
  if(fm == LIS3DH_FIFO_DISABLE) { 
    if( !LIS3DH_ReadReg(LIS3DH_FIFO_CTRL_REG, &value) )
      return MEMS_ERROR;
    
    value &= 0x1F;
    value |= (LIS3DH_FIFO_BYPASS_MODE<<LIS3DH_FM);                     
    
    if( !LIS3DH_WriteReg(LIS3DH_FIFO_CTRL_REG, value) )           //fifo mode bypass
      return MEMS_ERROR;   
    if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG5, &value) )
      return MEMS_ERROR;
    
    value &= 0xBF;    
    
    if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG5, value) )               //fifo disable
      return MEMS_ERROR;   
  }
  
  if(fm == LIS3DH_FIFO_BYPASS_MODE)   {  
    if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG5, &value) )
      return MEMS_ERROR;
    
    value &= 0xBF;
    value |= MEMS_SET<<LIS3DH_FIFO_EN;
    
    if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG5, value) )               //fifo enable
      return MEMS_ERROR;  
    if( !LIS3DH_ReadReg(LIS3DH_FIFO_CTRL_REG, &value) )
      return MEMS_ERROR;
    
    value &= 0x1f;
    value |= (fm<<LIS3DH_FM);                     //fifo mode configuration
    
    if( !LIS3DH_WriteReg(LIS3DH_FIFO_CTRL_REG, value) )
      return MEMS_ERROR;
  }
  
  if(fm == LIS3DH_FIFO_MODE)   {
    if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG5, &value) )
      return MEMS_ERROR;
    
    value &= 0xBF;
    value |= MEMS_SET<<LIS3DH_FIFO_EN;
    
    if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG5, value) )               //fifo enable
      return MEMS_ERROR;  
    if( !LIS3DH_ReadReg(LIS3DH_FIFO_CTRL_REG, &value) )
      return MEMS_ERROR;
    
    value &= 0x1f;
    value |= (fm<<LIS3DH_FM);                      //fifo mode configuration
    
    if( !LIS3DH_WriteReg(LIS3DH_FIFO_CTRL_REG, value) )
      return MEMS_ERROR;
  }
  
  if(fm == LIS3DH_FIFO_STREAM_MODE)   {  
    if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG5, &value) )
      return MEMS_ERROR;
    
    value &= 0xBF;
    value |= MEMS_SET<<LIS3DH_FIFO_EN;
    
    if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG5, value) )               //fifo enable
      return MEMS_ERROR;   
    if( !LIS3DH_ReadReg(LIS3DH_FIFO_CTRL_REG, &value) )
      return MEMS_ERROR;
    
    value &= 0x1f;
    value |= (fm<<LIS3DH_FM);                      //fifo mode configuration
    
    if( !LIS3DH_WriteReg(LIS3DH_FIFO_CTRL_REG, value) )
      return MEMS_ERROR;
  }
  
  if(fm == LIS3DH_FIFO_TRIGGER_MODE)   {  
    if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG5, &value) )
      return MEMS_ERROR;
    
    value &= 0xBF;
    value |= MEMS_SET<<LIS3DH_FIFO_EN;
    
    if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG5, value) )               //fifo enable
      return MEMS_ERROR;    
    if( !LIS3DH_ReadReg(LIS3DH_FIFO_CTRL_REG, &value) )
      return MEMS_ERROR;
    
    value &= 0x1f;
    value |= (fm<<LIS3DH_FM);                      //fifo mode configuration
    
    if( !LIS3DH_WriteReg(LIS3DH_FIFO_CTRL_REG, value) )
      return MEMS_ERROR;
  }
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_SetTriggerInt
* Description    : Trigger event liked to trigger signal INT1/INT2
* Input          : LIS3DH_TRIG_INT1/LIS3DH_TRIG_INT2
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetTriggerInt(LIS3DH_TrigInt_t tr) {
  uint8_t value;  
  
  if( !LIS3DH_ReadReg(LIS3DH_FIFO_CTRL_REG, &value) )
    return MEMS_ERROR;
  
  value &= 0xDF;
  value |= (tr<<LIS3DH_TR); 
  
  if( !LIS3DH_WriteReg(LIS3DH_FIFO_CTRL_REG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_SetWaterMark
* Description    : Sets Watermark Value
* Input          : Watermark = [0,31]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetWaterMark(uint8_t wtm) {
  uint8_t value;
  
  if(wtm > 31)
    return MEMS_ERROR;  
  
  if( !LIS3DH_ReadReg(LIS3DH_FIFO_CTRL_REG, &value) )
    return MEMS_ERROR;
  
  value &= 0xE0;
  value |= wtm; 
  
  if( !LIS3DH_WriteReg(LIS3DH_FIFO_CTRL_REG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}

  
/*******************************************************************************
* Function Name  : LIS3DH_GetStatusReg
* Description    : Read the status register
* Input          : char to empty by Status Reg Value
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_GetStatusReg(uint8_t* val) {
  if( !LIS3DH_ReadReg(LIS3DH_STATUS_REG, val) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;  
}


/*******************************************************************************
* Function Name  : LIS3DH_GetStatusBIT
* Description    : Read the status register BIT
* Input          : LIS3DH_STATUS_REG_ZYXOR, LIS3DH_STATUS_REG_ZOR, LIS3DH_STATUS_REG_YOR, LIS3DH_STATUS_REG_XOR,
                   LIS3DH_STATUS_REG_ZYXDA, LIS3DH_STATUS_REG_ZDA, LIS3DH_STATUS_REG_YDA, LIS3DH_STATUS_REG_XDA, 
                   LIS3DH_DATAREADY_BIT
                   val: Byte to be filled with the status bit   
* Output         : status register BIT
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_GetStatusBit(uint8_t statusBIT, uint8_t* val) {
  uint8_t value;  
  
  if( !LIS3DH_ReadReg(LIS3DH_STATUS_REG, &value) )
    return MEMS_ERROR;
  
  switch (statusBIT){
  case LIS3DH_STATUS_REG_ZYXOR:     
    if(value &= LIS3DH_STATUS_REG_ZYXOR){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  case LIS3DH_STATUS_REG_ZOR:       
    if(value &= LIS3DH_STATUS_REG_ZOR){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  case LIS3DH_STATUS_REG_YOR:       
    if(value &= LIS3DH_STATUS_REG_YOR){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }                                 
  case LIS3DH_STATUS_REG_XOR:       
    if(value &= LIS3DH_STATUS_REG_XOR){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }     
  case LIS3DH_STATUS_REG_ZYXDA:     
    if(value &= LIS3DH_STATUS_REG_ZYXDA){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }   
  case LIS3DH_STATUS_REG_ZDA:       
    if(value &= LIS3DH_STATUS_REG_ZDA){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }   
  case LIS3DH_STATUS_REG_YDA:       
    if(value &= LIS3DH_STATUS_REG_YDA){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }   
  case LIS3DH_STATUS_REG_XDA:       
    if(value &= LIS3DH_STATUS_REG_XDA){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }                                  
    
  }
  return MEMS_ERROR;
}


/*******************************************************************************
* Function Name  : LIS3DH_GetAccAxesRaw
* Description    : Read the Acceleration Values Output Registers
* Input          : buffer to empity by AxesRaw_t Typedef
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_GetAccAxesRaw(AxesRaw_t* buff) {
  i16_t value;
  uint8_t *valueL = (uint8_t *)(&value);
  uint8_t *valueH = ((uint8_t *)(&value)+1);
  
  if( !LIS3DH_ReadReg(LIS3DH_OUT_X_L, valueL) )
    return MEMS_ERROR;
  
  if( !LIS3DH_ReadReg(LIS3DH_OUT_X_H, valueH) )
    return MEMS_ERROR;
  
  buff->X = value;
  
  if( !LIS3DH_ReadReg(LIS3DH_OUT_Y_L, valueL) )
    return MEMS_ERROR;
  
  if( !LIS3DH_ReadReg(LIS3DH_OUT_Y_H, valueH) )
    return MEMS_ERROR;
  
  buff->Y = value;
  
  if( !LIS3DH_ReadReg(LIS3DH_OUT_Z_L, valueL) )
    return MEMS_ERROR;
  
  if( !LIS3DH_ReadReg(LIS3DH_OUT_Z_H, valueH) )
    return MEMS_ERROR;
  
  buff->Z = value;
  
  return MEMS_SUCCESS; 
}

/*******************************************************************************
* Function Name  : LIS3DH_ConvAccValue
* Description    : Convert the raw acceleration data to value in mg units
* Input          : raw data to be converted
* Output         : converted data with value in mg units
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_ConvAccValue(AxesRaw_t* data)
{
  
  uint8_t crtl4;
  uint8_t sensitivity;
  
  if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG4, &crtl4) )
    return MEMS_ERROR;
  
  /* switch the sensitivity value set in the CRTL4*/
  switch(crtl4 & 0x30)
  {
  case 0x00:
    sensitivity = LIS3DH_Acc_Sensitivity_2g;
    break;
  case 0x10:
    sensitivity = LIS3DH_Acc_Sensitivity_4g;
    break;
  case 0x20:
    sensitivity = LIS3DH_Acc_Sensitivity_8g;
    break;
  case 0x30:
    sensitivity = LIS3DH_Acc_Sensitivity_16g;
    break;
  }    
  
  data->X = (data->X >> 4)*sensitivity;
  data->Y = (data->Y >> 4)*sensitivity;
  data->Z = (data->Z >> 4)*sensitivity;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_GetInt1Src
* Description    : Reset Interrupt 1 Latching function
* Input          : Char to empty by Int1 source value
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_GetInt1Src(uint8_t* val) {
  
  if( !LIS3DH_ReadReg(LIS3DH_INT1_SRC, val) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_GetInt1SrcBit
* Description    : Reset Interrupt 1 Latching function
* Input          : statusBIT: LIS3DH_INT_SRC_IA, LIS3DH_INT_SRC_ZH, LIS3DH_INT_SRC_ZL.....
*                  val: Byte to be filled with the status bit
* Output         : None
* Return         : Status of BIT [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_GetInt1SrcBit(uint8_t statusBIT, uint8_t* val) {
  uint8_t value;  
   
  if( !LIS3DH_ReadReg(LIS3DH_INT1_SRC, &value) )
      return MEMS_ERROR;
   
  if(statusBIT == LIS3DH_INT1_SRC_IA){
    if(value &= LIS3DH_INT1_SRC_IA){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  
  if(statusBIT == LIS3DH_INT1_SRC_ZH){
    if(value &= LIS3DH_INT1_SRC_ZH){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  
  if(statusBIT == LIS3DH_INT1_SRC_ZL){
    if(value &= LIS3DH_INT1_SRC_ZL){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  
  if(statusBIT == LIS3DH_INT1_SRC_YH){
    if(value &= LIS3DH_INT1_SRC_YH){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  
  if(statusBIT == LIS3DH_INT1_SRC_YL){
    if(value &= LIS3DH_INT1_SRC_YL){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  if(statusBIT == LIS3DH_INT1_SRC_XH){
    if(value &= LIS3DH_INT1_SRC_XH){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  
  if(statusBIT == LIS3DH_INT1_SRC_XL){
    if(value &= LIS3DH_INT1_SRC_XL){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  return MEMS_ERROR;
}


/*******************************************************************************
* Function Name  : LIS3DH_GetFifoSourceReg
* Description    : Read Fifo source Register
* Input          : Byte to empty by FIFO source register value
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_GetFifoSourceReg(uint8_t* val) {
  
  if( !LIS3DH_ReadReg(LIS3DH_FIFO_SRC_REG, val) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : LIS3DH_GetFifoSourceBit
* Description    : Read Fifo WaterMark source bit
* Input          : statusBIT: LIS3DH_FIFO_SRC_WTM, LIS3DH_FIFO_SRC_OVRUN, LIS3DH_FIFO_SRC_EMPTY
*                  val: Byte to fill  with the bit value
* Output         : None
* Return         : Status of BIT [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_GetFifoSourceBit(uint8_t statusBIT,  uint8_t* val){
  uint8_t value;  
  
  if( !LIS3DH_ReadReg(LIS3DH_FIFO_SRC_REG, &value) )
    return MEMS_ERROR;
  
  
  if(statusBIT == LIS3DH_FIFO_SRC_WTM){
    if(value &= LIS3DH_FIFO_SRC_WTM){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  
  if(statusBIT == LIS3DH_FIFO_SRC_OVRUN){
    if(value &= LIS3DH_FIFO_SRC_OVRUN){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  if(statusBIT == LIS3DH_FIFO_SRC_EMPTY){
    if(value &= statusBIT == LIS3DH_FIFO_SRC_EMPTY){     
      *val = MEMS_SET;
      return MEMS_SUCCESS;
    }
    else{  
      *val = MEMS_RESET;
      return MEMS_SUCCESS;
    }  
  }
  return MEMS_ERROR;
}


/*******************************************************************************
* Function Name  : LIS3DH_GetFifoSourceFSS
* Description    : Read current number of unread samples stored in FIFO
* Input          : Byte to empty by FIFO unread sample value
* Output         : None
* Return         : Status [value of FSS]
*******************************************************************************/
status_t LIS3DH_GetFifoSourceFSS(uint8_t* val){
  uint8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_FIFO_SRC_REG, &value) )
    return MEMS_ERROR;
  
  value &= 0x1F;
  
  *val = value;
  
  return MEMS_SUCCESS;
}

      
/*******************************************************************************
* Function Name  : LIS3DH_SetSPIInterface
* Description    : Set SPI mode: 3 Wire Interface OR 4 Wire Interface
* Input          : LIS3DH_SPI_3_WIRE, LIS3DH_SPI_4_WIRE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t LIS3DH_SetSPIInterface(LIS3DH_SPIMode_t spi) {
  uint8_t value;
  
  if( !LIS3DH_ReadReg(LIS3DH_CTRL_REG4, &value) )
    return MEMS_ERROR;
  
  value &= 0xFE;
  value |= spi<<LIS3DH_SIM;
  
  if( !LIS3DH_WriteReg(LIS3DH_CTRL_REG4, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}
/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/


//----------------------------------------------------------------------------------------
