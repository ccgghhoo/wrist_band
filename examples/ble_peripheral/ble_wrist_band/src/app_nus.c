
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
//#include "LibConfig.h"
#include "app_util.h"
#include "app_util_platform.h"
#include "proto.debug.h"
#include "ble_nus.h"

#include "app_error.h"
#include "app_nus.h"
#include "crc16.h"

//#include "DateTime.h"
#include "LibHeap.h"

//#include "role_config.h"

#include "peripheral_role.h"
#include "Proto.h"
#include "dfu.h"
#include "app_flash_drv.h"
#include "nrf_delay.h"
#include "app_init.h"

#if 0
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#define NUS_LOG			NRF_LOG_INFO
#define NUS_HEX_LOG		NRF_LOG_HEXDUMP_INFO
#else
#define NUS_LOG(...)
#define NUS_HEX_LOG(...)
#endif

//static ble_nus_t 		m_ble_nus;

static msg_packet_t	m_data_req;
static uint32_t 		m_rx_next_step = EPB_STEP_MAGIC;
static uint16_t 		msg_bytes_received;

static uint8_array_t 	m_tx_in_used;
static uint8_array_t 	m_tx_in_bak;

static bool 			m_has_data_to_send = false;
static uint32_t 		m_tx_offset = 0;

static uint32_t         m_nus_timeout = 0;


BLE_NUS_DEF(m_ble_nus, NRF_SDH_BLE_TOTAL_LINK_COUNT);  


/** 
 *transmit 发送数据。 
 *response 回调函数，这并不是马上发送数据，而是把数据放入 
 *m_tx_in_used 
 *或 m_tx_in_bak
 * 
 * @author hzhmcu (2018/5/4)
 * 
 * @param data 
 * @param len 
 */
void app_nus_tx_data_put(uint8_t *data, uint16_t len)
{
    if (BLE_IsConnect() == false)
    {
        APP_FREE(data);
        NUS_LOG("[NUS]:Send Busy free\r\n");
        return;
    }
    if (m_tx_in_used.p_data != NULL)
    {
        if (m_tx_in_bak.p_data  != NULL)
        {
            APP_FREE(data);
            NUS_LOG("[NUS]:Send Busy free\r\n");
        }
        else
        {
            m_tx_in_bak.p_data = data;
            m_tx_in_bak.size = len;
            m_has_data_to_send = true;
            NUS_LOG("[NUS]: save to bakup\r\n");
        }
    }
    else
    {
        m_tx_in_used.p_data = data;
        m_tx_in_used.size = len;
        m_has_data_to_send = true;
        m_tx_offset = 0;
        //NUS_LOG("[NUS]: send response\r\n");
    }
}

uint32_t app_nus_send_data(uint8_t * data, uint16_t * len)
{
    uint32_t err_code = 0;       
    err_code=ble_nus_data_send(&m_ble_nus, data , len ,ble_get_conn_handle());
    return err_code;
    
}


void app_nus_res_handler(void)
{
    uint32_t err_code = 0;

    if (m_has_data_to_send)
    {
        //ble_fast_connection(); 
        while (m_tx_offset  < m_tx_in_used.size)
        {
            // get bytes_to_send
            uint16_t bytes_to_send = m_tx_in_used.size - m_tx_offset;
            if (bytes_to_send > m_ble_nus_max_data_len) bytes_to_send = m_ble_nus_max_data_len;

            // transfer to BLE stack
            err_code = ble_nus_data_send(&m_ble_nus, m_tx_in_used.p_data + m_tx_offset, &bytes_to_send, ble_get_conn_handle());
            if (err_code == NRF_SUCCESS) // send success
            {              
                m_tx_offset += bytes_to_send; // send OK
                if (m_tx_offset >= m_tx_in_used.size) // completed
                {
                    APP_FREE(m_tx_in_used.p_data);  // free
                    m_tx_in_used.p_data = NULL;
                    m_tx_in_used.size = 0;

                    m_tx_offset = 0;
                    if (m_tx_in_bak.p_data != NULL) // has bak data to send
                    {

                        m_tx_in_used.p_data = m_tx_in_bak.p_data;
                        m_tx_in_used.size = m_tx_in_bak.size;

                        m_tx_in_bak.p_data = NULL;
                        m_tx_in_bak.size = 0;
                        NUS_LOG("[NUS]: send bak data\r\n");
                    }
                    else
                    {
                        m_has_data_to_send = false;
                    }
                }
            }
            else if (err_code == NRF_ERROR_RESOURCES) //queue is full, try next time
            {
                break;
            }
            else // invalid state/parameters
            {
                APP_FREE(m_tx_in_used.p_data);
                m_tx_in_used.p_data = NULL;
                m_tx_in_used.size = 0;

                if (m_tx_in_bak.p_data != NULL)
                {
                    APP_FREE(m_tx_in_bak.p_data);
                    m_tx_in_bak.p_data = NULL;
                    m_tx_in_bak.size = 0;
                }
                m_tx_offset = 0;
                m_has_data_to_send = false;
            }
        }
        return;
    }
    else
    {
    }

}

static void app_nus_receive_data_handler(ble_nus_evt_t * p_evt)
{
    if (p_evt->type != BLE_NUS_EVT_RX_DATA)return;
  
    
    uint16_t index = 0;
    m_nus_timeout = RunTime_GetValue();
    ble_fast_connection();
    
    uint8_t length=p_evt->params.rx_data.length;        //
    uint8_t const *p_data = p_evt->params.rx_data.p_data;     //2019/09/15
    
    //NUS_LOG("[NUS]: bytes in %d \r\n", length);
    
    while (index < length)
    {
        switch (m_rx_next_step)
        {
        default:
            m_rx_next_step = EPB_STEP_MAGIC;
            break;

        case EPB_STEP_MAGIC:
            m_data_req.magic =  p_data[index++];
            if (m_data_req.magic == MAGIC_NUMBER)
            {
                m_rx_next_step = EPB_STEP_FLAG;
            }
            break;
        case EPB_STEP_FLAG:
            m_data_req.val =  p_data[index++];
            if (m_data_req.flag.version != EPB_VER)
            {
                m_rx_next_step = EPB_STEP_MAGIC;
            }
            else
            {
                m_rx_next_step = EPB_STEP_LEN_LSB;
            }
            break;
        case EPB_STEP_LEN_LSB:
            m_data_req.len = p_data[index++];
            m_rx_next_step = EPB_STEP_LEN_MSB;
            break;
        case EPB_STEP_LEN_MSB:
            m_data_req.len |= (uint16_t)(p_data[index++] << 8);
            if (m_data_req.len == 0 || m_data_req.len > EPB_PAYLOAD_MAX_LEN)
            {
                m_rx_next_step = EPB_STEP_MAGIC;
            }
            else
            {
                m_rx_next_step = EPB_STEP_CRC_LSB;
            }
            break;
        case EPB_STEP_CRC_LSB:
            m_data_req.crc = p_data[index++];
            m_rx_next_step = EPB_STEP_CRC_MSB;
            break;
        case EPB_STEP_CRC_MSB:
            m_data_req.crc |= (uint16_t)(p_data[index++] << 8);
            m_rx_next_step = EPB_STEP_TID_LSB;
            break;
        case EPB_STEP_TID_LSB:
            m_data_req.id = p_data[index++];
            m_rx_next_step = EPB_STEP_TID_MSB;
            break;
        case EPB_STEP_TID_MSB:
            m_data_req.id |= (uint16_t)(p_data[index++] << 8);
            m_rx_next_step = EPB_STEP_PAYLOAD;

            m_data_req.payload = (uint8_t *)APP_MALLOC(NUS_EPB_RECV_DATA_BUFF, m_data_req.len);
            if (m_data_req.payload == NULL)
            {
                m_rx_next_step = EPB_STEP_MAGIC;
            }
            else
            {
                msg_bytes_received = 0;
            }
            break;
        case EPB_STEP_PAYLOAD:
            {
                uint16_t remains = length - index;
                if ((remains + msg_bytes_received) < m_data_req.len)
                {
                    memcpy(m_data_req.payload + msg_bytes_received, p_data + index, remains);
                    msg_bytes_received += remains;
                    index += remains;
                }
                else
                {
                    remains = m_data_req.len - msg_bytes_received;
                    memcpy(m_data_req.payload + msg_bytes_received, p_data + index, remains);
                    m_rx_next_step = EPB_STEP_COMPLETED;
                    NUS_LOG("[NUS]:cmd received EPB_STEP_COMPLETED \r\n");
                }
                break; }
        case EPB_STEP_COMPLETED: // do nothing, wait for
            index = length;
            break;
        }
    }

}



void app_nus_task_loop(void)
{
    if (EPB_STEP_COMPLETED == m_rx_next_step)
    {
        NUS_LOG("[NUS]: packet get size=%d , cmd =0x%x,type=0x%x\r\n", m_data_req.len,
                m_data_req.payload[0], m_data_req.payload[2]);
        if (m_data_req.payload != NULL)
        {
            Proto_PutPack(&m_data_req, app_nus_tx_data_put);         //协议解析接口
            APP_FREE(m_data_req.payload);
        }

        m_rx_next_step = EPB_STEP_MAGIC;
    }
    
    if(objDFU->state!= DFU_STA_INIT_COMMAND)
    {
        if(flash_write_completed())
           app_nus_res_handler();
    }
    else
    {
        app_nus_res_handler();
    }

    if (/*ProtoDebug_IsLog() == false &&*/
        RunTime_InRange(m_nus_timeout, 10) == false
       )
    {
        m_nus_timeout = RunTime_GetValue();
        ble_slow_connection();
        //m_nus_in_conn = false;
        m_rx_next_step = EPB_STEP_MAGIC;
    }

}

void app_nus_init(void)
{
    ble_nus_init_t 		nus_init_obj;

    memset(&nus_init_obj, 0, sizeof(nus_init_obj));

    nus_init_obj.data_handler 		 = app_nus_receive_data_handler;

    uint32_t err_code = ble_nus_init(&m_ble_nus,  &nus_init_obj);
    if (err_code != NRF_SUCCESS)
    {
        APP_ERROR_CHECK(err_code);
    }
}

bool BLE_IsConnect()
{
    extern bool ble_is_connected(void);
    return ble_is_connected();
}


//****************************************************************************

bool ble_send_proto_data_pack(uint8_t *data, uint16_t len, uint8_t flag)
{
    if (BLE_IsConnect() == false)
    {        
        NUS_LOG("[NUS]:ble disconnected, send data fail!\r\n");
        return false;
    }
    
    uint8_t *data_buff = (uint8_t *)APP_MALLOC(PROTO_PACK_DATA_PACK, len + 8);

    if (data_buff == NULL)
    {
        NUS_LOG("[NUS]: no more buffer \r\n ");
        return false;
    }
 	        
	msg_packet_t 	respHead; 
	respHead.magic	= MAGIC_NUMBER; 
	respHead.len	= len; 
	respHead.flag.ack	= flag;       
    respHead.id		= 0; 	
               		
	if( len > 0){			
		uint16_t crc16 = crc16_compute( data, len, 0); 
		respHead.crc = crc16; 
	}else{
		respHead.crc = 0; 
	}
	
	memcpy( data_buff, (uint8_t*)&respHead, 8); 
	memcpy( data_buff + 8, data, len); 
    
    
    app_nus_tx_data_put(data_buff, len+8);
    
    return true;

}


void ble_sos_key_send(void)
{
    if (BLE_IsConnect() == false)
    {        
        NUS_LOG("[NUS]:ble disconnected");
        return;
    }
    uint8_t  databuff[8];           
    databuff[0] = COMMAND_ID_BLE_BASE;
    databuff[1] = 3;
    databuff[2] = BLE_WB_SOS_KEY_ALARM;
    databuff[3] = 0;
    databuff[4] = 1;    
    ble_send_proto_data_pack(databuff, 5, 1);     
}

void ble_utc_time_req_send(void)
{
    if (BLE_IsConnect() == false)
    {        
        NUS_LOG("[NUS]:ble disconnected");
        return;
    }
    uint8_t  databuff[8];           
    databuff[0] = COMMAND_ID_BLE_BASE;
    databuff[1] = 1;
    databuff[2] = BLE_WB_UPDATE_UTC_SECONDS;  
    ble_send_proto_data_pack(databuff, 3, 1);     
}

void ble_sport_data_send(void)
{
    if (BLE_IsConnect() == false)
    {        
        NUS_LOG("[NUS]:ble disconnected");
        return;
    }
    
    static uint8_t send_time_out=0;
    if(check_app_evt(APP_EVT_SPORT_SEND_WAIT))
    {
        if(++send_time_out<15)//30=1minute
        {
            return;
        }
        else
        {
            send_time_out = 0;
            clear_app_evt(APP_EVT_SPORT_SEND_WAIT);
        }
    }
    
    uint32_t  p_sport_data;
    uint16_t  len;
    bool      ret;
    
    ret = find_read_sport_record(&p_sport_data, &len, 0);   
    if(!ret)
    {
        NUS_LOG("[NUS]:no sport data ");
        return;
    }
    if(p_sport_data<0x20001c18) //ram_start = 0x20001c18
    {
        NUS_LOG("[NUS]:access sport data error! ram address : %d ", p_sport_data);
        return;
    }
    
//    NRF_LOG_HEXDUMP_INFO((uint8_t*)p_sport_data, 80);
//    NRF_LOG_HEXDUMP_INFO((uint8_t*)(p_sport_data+80), 16);
        
    uint8_t  databuff[104];
    databuff[0] = COMMAND_ID_BLE_BASE;
    databuff[1] = 1+96;
    databuff[2] = BLE_WB_READ_SPORT_DATA;
    
    memcpy(&databuff[3], (uint8_t*)p_sport_data, len);

    if(ble_send_proto_data_pack(databuff, 99, 1)) //3+96
    {
        set_app_evt(APP_EVT_SPORT_SEND_WAIT);
    }
             
}






