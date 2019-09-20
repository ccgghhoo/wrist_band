/**
 *  
 * 这里的所有的包，都是设备主动发出的包。 
 *  
 *  
 *  
 *  
 */
#include <string.h>
#include <stdbool.h>
//#include "sport_degree_count.h"
#include "proto.h"
#include "proto.pack.h"
#include "proto.pb.h"
#include "stdarg.h"

//#include "dev_conf.pb.h"
//#include "dev_config.get.h"
//#include "HalActivityCFS.h"
#include "proto.debug.h"

//#include "ls_api.h"

//#include "record.h"

#include "DateTime.h"
#include "LibHeap.h"
#include "crc16.h"
//#include "battery.h"
//#include "gsflag.h"
#include "app_nus.h"
//#include "storage.h"
#include "halble.h"

#if FEATURE_USB_HID
#include "HalUSB.h"
#endif


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#if 0
#define DEBUG       NRF_LOG_INFO
#else
#define DEBUG(...)
#endif



/**
 * 此为主动发包。
 */


resp_callback_t					m_resp_cb;
msg_packet_t *mp_msg_ptr;
bool 						m_resp_sent;
static uint16_t 				m_send_count = 0;
uint8_t UserID_Append(uint8_t *p_data)
{
    int len = dev_config_user_id_len();
    if (dev_config_user_id_len() == NULL)
    {
        return 0;
    }

    int index = 0;
    p_data[index++] = len + 1;
    p_data[index++] = DATA_KEY_USER_ID;
    //memcpy(p_data + index, dev_config_user_id(), len);
    index += len;
    return index;
}

/**
 * 组包函数
 * 
 * @author hzhmcu (2018/8/28)
 * 
 * @param reqHead 请求包头，返回组包后的包头
 * @param data  内容数据
 * @param len   内容数据的长度
 * @param flag  标志位
 * @param data_buff 组包后的数据
 * 
 * @return uint32_t 组包后的数据长度
 */
uint32_t  ProtoPacking_WithSn(msg_packet_t *reqHead, uint8_t *data, uint16_t len, uint8_t flag, uint8_t *data_buff, uint16_t sn)
{
    msg_packet_t 	respHead;
    respHead.magic	= MAGIC_NUMBER;
    respHead.len	= len;
    respHead.val	= flag;
    respHead.id		= sn;


    if (len > 0)
    {
        uint16_t crc16 = crc16_compute(data, len, 0);
        respHead.crc = crc16;
    }
    else
    {
        respHead.crc = 0;
    }

    memcpy(data_buff, (uint8_t *)&respHead, 8);
    if (reqHead != NULL)
    {
        memcpy((uint8_t *)reqHead, (uint8_t *)&respHead, 8);
    }
    memcpy(data_buff + 8, data, len);
    return len + 8;
}

/**
 * 组包函数
 * 
 * @author hzhmcu (2018/8/28)
 * 
 * @param reqHead 请求包头，返回组包后的包头
 * @param data  内容数据
 * @param len   内容数据的长度
 * @param flag  标志位
 * @param data_buff 组包后的数据
 * 
 * @return uint32_t 组包后的数据长度
 */
uint32_t  ProtoPacking(msg_packet_t *reqHead, uint8_t *data, uint16_t len, uint8_t flag, uint8_t *data_buff)
{


    msg_packet_t 	respHead;
    respHead.magic	= MAGIC_NUMBER;
    respHead.len	= len;
    respHead.val	= flag;
    respHead.id		= ++m_send_count;


    if (len > 0)
    {
        uint16_t crc16 = crc16_compute(data, len, 0);
        respHead.crc = crc16;
    }
    else
    {
        respHead.crc = 0;
    }

    memcpy(data_buff, (uint8_t *)&respHead, 8);
    if (reqHead != NULL)
    {
        memcpy((uint8_t *)reqHead, (uint8_t *)&respHead, 8);
    }
    memcpy(data_buff + 8, data, len);
    return len + 8;
}


uint8_t* __send_pack_msg(msg_packet_t *reqHead, uint8_t *data, uint16_t len, uint8_t flag)
{
    uint8_t *data_buff = (uint8_t *)APP_MALLOC(PROTO_PACK_SEND_MSG_PACK, len + 8);
    if (data_buff == NULL) //no more heap to alloc
    {
        return NULL;
    }
    ProtoPacking(reqHead, data, len, flag, data_buff);
    return data_buff;
}




#if 1

/**
 * 请求ble 基站的数据
 * 
 * @author hzhmcu (2018/7/5)
 * 
 * @param reqHead 
 */
void proto_pack_get_ble_loc(msg_packet_t *reqHead)
{
    uint8_t temp[100];
    uint16_t chunk_len = 0;

    temp[0] = COMMAND_ID_DATA;

    temp[1] = 16;
    temp[2] = DATA_KEY_IMEI;
    memcpy(temp + 3, (uint8_t *)dev_config_get_IMEI(), 15);
    chunk_len += 18;
    chunk_len += UserID_Append(temp + chunk_len);

    temp[chunk_len + 0] = 1;
    temp[chunk_len + 1] = DATA_KEY_BLE_LOC;

    chunk_len += 2;

    uint8_t *data_buff = NULL;
    data_buff = __send_pack_msg(reqHead, temp, chunk_len, FLAG_ACK_RESP);
    if (data_buff == NULL) //no more heap to alloc
    {
        DEBUG("[PACK]: no more buffer \r\n ");
        return;
    }
    app_nus_tx_data_put(data_buff, chunk_len + 8);
}

/**
 * 请求ble底座的信息
 * 
 * @author hzhmcu (2018/7/5)
 * 
 * @param reqHead 
 */
void proto_pack_set_dev_info(msg_packet_t *reqHead)
{
    uint8_t temp[100];
    uint16_t index = 0;

    temp[index++] = COMMAND_ID_BLE_BASE;

    temp[index++] = 5;
    temp[index++] = BLE_BASE_DEV_TYPE;
    int size =  dev_config_get_ble_often_connect_switch();
    temp[index++] = size;
    temp[index++] = size >> 8;
    temp[index++] = size >> 16;
    temp[index++] = size >> 24;
    uint8_t *data_buff = NULL;
    data_buff = __send_pack_msg(reqHead, temp, index, FLAG_ACK_RESP);
    if (data_buff == NULL) //no more heap to alloc
    {
        DEBUG("[PACK]: no more buffer \r\n ");
        return;
    }
    app_nus_tx_data_put(data_buff, index + 8);
}


/**
 * 请求ble底座的信息
 * 
 * @author hzhmcu (2018/7/5)
 * 
 * @param reqHead 
 */
void proto_pack_get_ble_base_info(msg_packet_t *reqHead)
{
    uint8_t temp[100];
    uint16_t index = 0;

    temp[index++] = COMMAND_ID_BLE_BASE;

    temp[index++] = 1;
    temp[index++] = BLE_BASE_BASE_TYPE;

    uint8_t *data_buff = NULL;
    data_buff = __send_pack_msg(reqHead, temp, index, FLAG_ACK_RESP);
    if (data_buff == NULL) //no more heap to alloc
    {
        DEBUG("[PACK]: no more buffer \r\n ");
        return;
    }
    app_nus_tx_data_put(data_buff, index + 8);
}


/**
 * 播放放音乐
 * 
 * @author hzhmcu (2018/7/5)
 * 
 * @param reqHead 
 */
void proto_pack_send_ble_music_play(msg_packet_t *reqHead, int music, uint8_t loop)
{
    uint8_t temp[100];

    int index = 0;
    temp[index++] = COMMAND_ID_BLE_BASE;

    temp[index++] = 6;
    temp[index++] = BLE_BASE_MUSIC_PLAY;
    temp[index++] = music;
    temp[index++] = music >> 8;
    temp[index++] = music >> 16;
    temp[index++] = music >> 24;
    temp[index++] = loop;

    uint8_t *data_buff = NULL;
    data_buff = __send_pack_msg(reqHead, temp, index, FLAG_ACK_RESP);
    if (data_buff == NULL) //no more heap to alloc
    {
        DEBUG("[PACK]: no more buffer \r\n ");
        return;
    }
    app_nus_tx_data_put(data_buff, index + 8);
}


/**
 * 播放放音乐
 * 
 * @author hzhmcu (2018/7/5)
 * 
 * @param reqHead 
 */
void proto_pack_send_ble_music_ctrl(msg_packet_t *reqHead, int control)
{
    uint8_t temp[100];
    int index = 0;
    temp[index++] = COMMAND_ID_BLE_BASE;

    temp[index++] = 5;
    temp[index++] = BLE_BASE_MUSIC_CTRL;
    temp[index++] = control;
    temp[index++] = control >> 8;
    temp[index++] = control >> 16;
    temp[index++] = control >> 24;
    uint8_t *data_buff = NULL;
    data_buff = __send_pack_msg(reqHead, temp, index, FLAG_ACK_RESP);
    if (data_buff == NULL) //no more heap to alloc
    {
        DEBUG("[PACK]: no more buffer \r\n ");
        return;
    }
    app_nus_tx_data_put(data_buff, index + 8);
}

#endif

#if 1 // record data request => response





void proto_pack_get_historical_tx_completed_msg(PacketInfo_t *targ)
{
    int len = 0;
    uint8_t temp[EPB_PAYLOAD_MAX_LEN + 30];
    LibStack_Debug();
    uint16_t chunk_len = 0;

    temp[0] = COMMAND_ID_DATA;

    temp[1] = 16;
    temp[2] = DATA_KEY_IMEI;
    memcpy(temp + 3, (uint8_t *)dev_config_get_IMEI(), 15);
    chunk_len += 18;
    chunk_len += UserID_Append(temp + chunk_len);
    DEBUG("##############################\r\n");




#if USE_SAVE_BUFFER
    //report the buffer data.


    rec_data_t *p_rec_data = record_data_get_buffer();
    rec_data_t *p_rec_tx  = record_data_get_tx_buffer();    
    len = p_rec_data->len;
    p_rec_tx->len = p_rec_data->len;
    if (p_rec_data->len)
    {
        memcpy(p_rec_tx->p_buf, p_rec_data->p_buf, p_rec_data->len);
        memcpy(temp + chunk_len, p_rec_data->p_buf, p_rec_data->len);
       
        chunk_len += p_rec_data->len;
      
    }
    HID_LOG(LOG_TCP,"buf len %d\r\n",p_rec_tx->len );
    //取完数据就清除
    record_clear_buffer_data();
#endif

    if (len > 0)
    {
        temp[chunk_len + 0] = 1;
        temp[chunk_len + 1] = DATA_KEY_TX_COMPLETED;
        chunk_len += 2;
        targ->p_data = __send_pack_msg(&targ->head, temp, chunk_len, FLAG_ACK_RESP);
        targ->size = chunk_len + EPB_HEAD_DATA_SIZE;
    }
    else
    {
        targ->p_data = NULL;
        targ->size = 0;
    }


}





#endif




// GSM send data active
/*
1. heartbeat data 
2. alert data 
3. historical data 
*/

/*
<command> service data 
<imei>	len-key-value
<heartbeat> len-key-value 
*/
// send heart beat keep connect alive
void proto_pack_get_heart_beat_msg(PacketInfo_t *targ)
{
    // imei
    // heartrate beat data
    uint8_t temp[100];
    uint16_t chunk_len = 0;
    // uint8_t append_len = 0;
    temp[0] = COMMAND_ID_SERVICES;
    chunk_len += 1;

    temp[1] = 16;
    temp[2] = SERV_KEY_IMEI;
    memcpy(temp + 3, (uint8_t *)dev_config_get_IMEI(), 15);
    chunk_len += 17;
    chunk_len += UserID_Append(temp + chunk_len);

    temp[chunk_len + 0] = 2;
    temp[chunk_len + 1] = SERV_KEY_KEEP_ALIVE;
    temp[chunk_len + 2] = EPB_HEART_BEAT_MAGIC_CODE;
    chunk_len += 3;

    targ->p_data = 	__send_pack_msg(&targ->head, temp, chunk_len, FLAG_ACK_RESP);
    targ->size = chunk_len + EPB_HEAD_DATA_SIZE;

}


void proto_pack_get_alert_msg(PacketInfo_t *targ)
{
    // imei
    // heartrate beat data
    uint8_t temp[100];
    uint16_t chunk_len = 0;
    // uint8_t append_len = 0;
    temp[0] = COMMAND_ID_DATA;
    chunk_len += 1;

    temp[1] = 16;
    temp[2] = SERV_KEY_IMEI;
    memcpy(temp + 3, (uint8_t *)dev_config_get_IMEI(), 15);
    chunk_len += 17;
    chunk_len += UserID_Append(temp + chunk_len);

//    HID_LOG(LOG_TCP, "alert pend\r\n");

//    set_locator_status(NONE_LOCATOR_CURR);
    //general area
    chunk_len += locate_general_data_append(temp + chunk_len);
    uint16_t  temp_offset = locate_alarm_code_data_append(temp + chunk_len);

    chunk_len += temp_offset;

//    HID_LOG(LOG_TCP, "alert pend end\r\n");
    targ->p_data = 	__send_pack_msg(&targ->head, temp, chunk_len, FLAG_ACK_RESP);
    targ->size = chunk_len + EPB_HEAD_DATA_SIZE;

}
/*
<command> service data 
<imei>	len-key-value
<heartbeat> len-key-value 
*/
// send heart beat keep connect alive
void proto_pack_get_time(PacketInfo_t *targ)
{
    // imei
    // heartrate beat data
    uint8_t temp[100];
    uint16_t chunk_len = 0;
    // uint8_t append_len = 0;
    temp[0] = COMMAND_ID_SERVICES;
    chunk_len += 1;

    temp[1] = 16;
    temp[2] = SERV_KEY_IMEI;
    memcpy(temp + 3, (uint8_t *)dev_config_get_IMEI(), 15);
    chunk_len += 17;
    chunk_len += UserID_Append(temp + chunk_len);

    temp[chunk_len + 0] = 1;
    temp[chunk_len + 1] = SERV_KEY_GET_TIMESTARMP;
    chunk_len += 2;

    targ->p_data = __send_pack_msg(&targ->head, temp, chunk_len, FLAG_ACK_RESP);
    targ->size = chunk_len + EPB_HEAD_DATA_SIZE;
}

// send current gps data & current event
void proto_pack_get_gps_curr_data_msg(PacketInfo_t *targ)
{
    uint8_t temp[100];
    uint16_t chunk_len = 0;

    temp[0] = COMMAND_ID_DATA;

    // IMEI
    temp[1] = 16;
    temp[2] = DATA_KEY_IMEI;
    memcpy(temp + 3, (uint8_t *)dev_config_get_IMEI(), 15);
    chunk_len += 18;
    chunk_len += UserID_Append(temp + chunk_len);

    //final location
    temp[chunk_len + 1] = DATA_KEY_GPS_LATEST_LOC;
    temp[chunk_len + 0] = ls_api_get_final_locate_packet(temp + chunk_len + 2) + 1;
    chunk_len += temp[chunk_len + 0] + 1;

    //general data
    temp[chunk_len + 0] = 7;
    temp[chunk_len + 1] = DATA_KEY_GENERAL_DATA;
    temp[chunk_len + 2] = battery_level();
    temp[chunk_len + 3] = ls_api_get_gsm_csq();
    uint32_t time = RunTime_GetValue();
    temp[chunk_len + 4] = (uint8_t)(time >> 0);
    temp[chunk_len + 5] = (uint8_t)(time >> 8);
    temp[chunk_len + 6] = (uint8_t)(time >> 16);
    temp[chunk_len + 7] = (uint8_t)(time >> 24);
    chunk_len += 8;

    targ->p_data = __send_pack_msg(&targ->head, temp, chunk_len, FLAG_ACK_RESP);
    targ->size = chunk_len + EPB_HEAD_DATA_SIZE;
}



void proto_pack_get_alert_msg1(PacketInfo_t *targ)
{
#if 0
    HZH_LOG("+++++++++++++++++++++++++++++++\r\n tcp Alert\r\n");
    uint8_t temp[100];
    uint16_t chunk_len = 0;
    uint8_t append_len = 0;
    temp[0] = COMMAND_ID_DATA;

    // imei
    temp[1] = 16;
    temp[2] = DATA_KEY_IMEI;
    memcpy(temp + 3, (uint8_t *)dev_config_get_IMEI(), 15);
    chunk_len += 18;

    //general data here.
    append_len=locate_general_data_append(temp + append_len);
    chunk_len += append_len;
    //alarm area
    append_len = locate_alarm_code_data_append(temp + chunk_len);
    chunk_len += append_len;

    // GPS location
    if (ls_api_get_has_valid_gps_for_upload())
    {
        temp[chunk_len + 1] = DATA_KEY_GPS_ALERT_NOTIFY;
        temp[chunk_len + 0] = ls_api_get_final_locate_packet(temp + chunk_len + 2) + 1;
        chunk_len += temp[chunk_len + 0] + 1;
    }
    else
    { //GSM location
        temp[chunk_len + 1] = DATA_KEY_GSM_ALERT_NOTIFY;
        temp[chunk_len + 0] = ls_api_get_cell_locator_data(temp + chunk_len + 2) + 1; //56max
        chunk_len += temp[chunk_len + 0] + 1;
    }


    targ->p_data = __send_pack_msg(&targ->head, temp, chunk_len, FLAG_ACK_RESP);
    targ->size = chunk_len + EPB_HEAD_DATA_SIZE;
#else
    targ->p_data = NULL;
    targ->size = 0;
#endif
}
static int read_offset = 0;
void proto_pack_activity_save_offset(bool isSave)
{
    HalActivityCFS_SaveReadIndex(read_offset, isSave);
}

void proto_pack_activity_clear_offset()
{
    read_offset = 0;
}


void proto_pack_get_activity_data_msg(PacketInfo_t *targ)
{

#ifdef DEGREE
#define MAX_LEN 500
    uint8_t *tx_buff = (uint8_t *)APP_MALLOC(PROTO_PACK_SEND_HISTORICAL_DATA,  MAX_LEN);

    DEBUG("X1\r\n");
    if (tx_buff == NULL)
    {
        targ->p_data = NULL;
        return;
    }
    DEBUG("X2\r\n");
    uint16_t chunk_len = 0;

    tx_buff[0] = COMMAND_ID_DATA;

    //add device IMEI
    tx_buff[1] = 16;
    tx_buff[2] = DATA_KEY_IMEI;
    chunk_len += 3;
    memcpy(tx_buff + chunk_len, (uint8_t *)dev_config_get_IMEI(), 15);
    chunk_len += 15;
    chunk_len += UserID_Append(tx_buff + chunk_len);

    uint8_t *pLen = NULL;


#define TEMP_SIZE 250

    pLen = tx_buff + chunk_len;
    chunk_len++; //len
    tx_buff[chunk_len++] = DATA_KEY_ACTIVITY_DATA;

    uint16_t sum_len = 0;

    int err = HalActivityCFS_Read(tx_buff + chunk_len, read_offset, TEMP_SIZE);


    //err = err / sizeof(md_gsensor_degree_t) * sizeof(md_gsensor_degree_t);

    sum_len = err;
    read_offset += sum_len;
    *pLen = sum_len + 1;
    chunk_len += sum_len;



    if (chunk_len > 17 + 8)
    {
        targ->p_data = __send_pack_msg(&targ->head, tx_buff, chunk_len, FLAG_ACK_RESP);
        targ->size = chunk_len + EPB_HEAD_DATA_SIZE;

    }
    else
    {

        targ->size = 0;
        targ->p_data = NULL;
    }


    APP_FREE(tx_buff);
    tx_buff = NULL;

    HID_SEND(targ->p_data, targ->size);
#else
    targ->size = 0;
    targ->p_data = NULL;
#endif

}

// send historical data
void proto_pack_get_historical_data_msg(PacketInfo_t *targ)
{
    uint8_t *tx_buff = (uint8_t *)APP_MALLOC(PROTO_PACK_SEND_HISTORICAL_DATA,  EPB_PAYLOAD_MAX_LEN);
    if (tx_buff == NULL)
    {
        targ->p_data = NULL;
        return;
    }

    uint16_t chunk_len = 0, temp_offset = 0;

    tx_buff[0] = COMMAND_ID_DATA;

    //add device IMEI
    tx_buff[1] = 16;
    tx_buff[2] = DATA_KEY_IMEI;
    chunk_len += 3;
    memcpy(tx_buff + chunk_len, (uint8_t *)dev_config_get_IMEI(), 15);
    chunk_len += 15;
    chunk_len += UserID_Append(tx_buff + chunk_len);
    //
    DEBUG("X1\r\n");
    temp_offset = record_data_fetch(tx_buff + chunk_len, (NEW_EPB_PAYLOAD_MAX_LEN - chunk_len));
    chunk_len += temp_offset;
    DEBUG("X2 temp_offset%d\r\n", temp_offset);

    LibStack_Debug();
    if (temp_offset > 0)
    {
        targ->p_data = __send_pack_msg(&targ->head, tx_buff, chunk_len, FLAG_ACK_RESP);
        targ->size = chunk_len + EPB_HEAD_DATA_SIZE;

    }
    else
    {
        targ->size = 0;
        targ->p_data = NULL;
    }


    APP_FREE(tx_buff);
    tx_buff = NULL;
}



// parse langtitude & longitude address text
void proto_pack_req_server_parse_lat_long(PacketInfo_t *targ)
{
    int32_t lat, lng;

#if 1
    if (!ls_api_get_final_lat_lng(&lat, &lng))
    {
        targ->p_data = NULL;
        DEBUG("No data\r\n");
        return;
    }
#else
// only for test
    lat = 226177365;
    lng = 1140364104;
#endif
    uint8_t temp[40];
    uint8_t index = 0;
    temp[index++] = COMMAND_ID_SERVICES;
    temp[index++] = 16;
    temp[index++] = DATA_KEY_IMEI;

    memcpy(temp + index, (uint8_t *)dev_config_get_IMEI(), 15);
    index += 15;
    index += UserID_Append(temp + index);
    temp[index++] = 9;
    temp[index++] = SERV_KEY_ADDR_PARSE;
    uint32_encode(lat, temp + index); index += 4;
    uint32_encode(lng, temp + index); index += 4;

    targ->p_data = __send_pack_msg(&targ->head, temp, index, FLAG_ACK_RESP);
    targ->size = index + EPB_HEAD_DATA_SIZE;

    DEBUG("Request parse Address\r\n");
}

// weak reference
// define your own handle somewhere
__WEAK void long_lan_parsed_from_server(const uint8_t *addr, uint16_t len)
{
}



void G_sensor_report_data(uint8_t *p_data, uint16_t length)
{
    uint8_t		temp[300];
    uint16_t	chunk_len = 0;

    temp[0] = COMMAND_ID_DATA;
    temp[1] = length + 1;
    temp[2] = DATA_GSENSOR_RAW_DATA;
    memcpy(temp + 3, p_data, length);
    chunk_len += 3;
    chunk_len += length;

    uint8_t *data = temp; uint16_t len = chunk_len; uint8_t flag = FLAG_ACK_RESP;
    uint8_t *data_buff = (uint8_t *)APP_MALLOC(PROTO_PACK_DATA_PACK, len + 8);
    if (data_buff == NULL) //no more heap to alloc
    {
        DEBUG("[PACK]: no more buffer \r\n ");
        return;
    }
    msg_packet_t 	respHead;
    respHead.magic	= MAGIC_NUMBER;
    respHead.len	= len;
    respHead.val	= flag;
    respHead.id		= mp_msg_ptr->id;

    m_send_count	= mp_msg_ptr->id;
    if (len > 0)
    {
        uint16_t crc16 = crc16_compute(data, len, 0);
        respHead.crc = crc16;
    }
    else
    {
        respHead.crc = 0;
    }
    memcpy(data_buff, (uint8_t *)&respHead, 8);
    memcpy(data_buff + 8, data, len);
    app_nus_tx_data_put(data_buff, len + 8);
}
static void Hid_Send(uint8_t *p_data, uint16_t length)
{

#if FEATURE_USB_HID
    if (HalBattery_IsCharge() == false && BLE_IsConnect() == false)
#else
    if (BLE_IsConnect() == false)
#endif
        {
            return;
        }
    uint8_t *data_buff = (uint8_t *)APP_MALLOC(PROTO_PACK_DATA_PACK, length);
    if (data_buff == NULL) //no more heap to alloc
    {
        DEBUG("[PACK]: no more buffer \r\n ");
        return;
    }
    memcpy(data_buff, p_data, length);
#if FEATURE_USB_HID
    if (HalBattery_IsCharge())
    {
        HalUSB_LogSend(data_buff, length);
    }
    else
#endif
        if (BLE_IsConnect())
        {
            app_nus_tx_data_put(data_buff, length);
        }
}
/*
static void Hid_Log(uint8_t cmd, uint8_t *p_data, uint16_t length)
{

#if FEATURE_USB_HID
    if (HalBattery_IsCharge() == false && BLE_IsConnect() == false)
#else
    if (BLE_IsConnect() == false)
#endif
        {
            return;
        }
    switch (cmd)
    {
    default:
        return;
    case DEBUG_KEY_LOG_GPS_RX:
    case DEBUG_KEY_LOG_GPS_TX:
        if (ProtoDebug_IsGPSLog() == false)
        {
            return;
        }
        break;
    case DEBUG_KEY_DOWN_SIM800_FILE:
        break;
    case DEBUG_KEY_LOG_DEBUG:
        if (ProtoDebug_IsDebugLog(0xffffffff) == false)
        {
            return;
        }
        break;
    case DEBUG_KEY_LOG_GSM_RX:
    case DEBUG_KEY_LOG_GSM_TX:
        if (ProtoDebug_IsGSMLog() == false)
        {
            return;
        }
        break;
    }
    uint8_t		temp[length + 3];
    uint16_t	chunk_len = 0;

    temp[0] = COMMAND_ID_DEBUG;
    temp[1] = length + 1;
    temp[2] = cmd;
    memcpy(temp + 3, p_data, length);
    chunk_len += 3;
    chunk_len += length;

    uint8_t *data_buff = (uint8_t *)APP_MALLOC(PROTO_PACK_DATA_PACK, chunk_len + 8);
    if (data_buff == NULL) //no more heap to alloc
    {
        DEBUG("[PACK]: no more buffer \r\n ");
        return;
    }
    msg_packet_t 	respHead;
    respHead.magic	= MAGIC_NUMBER;
    respHead.len	= chunk_len;
    respHead.val	= FLAG_NORMAL_RESP;
    respHead.id	= mp_msg_ptr->id;

    respHead.crc = crc16_compute(temp, chunk_len, 0);

    memcpy(data_buff, (uint8_t *)&respHead, 8);
    memcpy(data_buff + 8, temp, chunk_len);

#if FEATURE_USB_HID
    if (HalBattery_IsCharge())
    {
        HalUSB_LogSend(data_buff, chunk_len + 8);
    }
    else
#endif
        if (BLE_IsConnect())
        {
            app_nus_tx_data_put(data_buff, chunk_len + 8);
        }



}*/







/**
 * GPS Rx 的数据放在Log 打印
 * 
 * @author hzhmcu (2018/9/4)
 * 
 * @param p_data 接收到的数据，最好是一段数据。
 * @param length 数据的长度
 */
void GPSLog_Rx(uint8_t *p_data, uint16_t length)
{
    Hid_Log(DEBUG_KEY_LOG_GPS_RX, p_data, length);
}




/**
 * GSM Rx 的数据放在Log 打印
 * 
 * @author hzhmcu (2018/9/4)
 * 
 * @param p_data  接收到的数据，最好是一段数据。
 * @param length 数据的长度
 */
void GSMLog_Rx(uint8_t *p_data, uint16_t length)
{
    Hid_Log(DEBUG_KEY_LOG_GSM_RX, p_data, length);
}



/**
 * DUEBUG 的数据放在Log 打印
 * 
 * @author hzhmcu (2018/9/4)
 * 
 * @param p_data  接收到的数据，最好是一段数据。
 * @param length 数据的长度
 */
void GSMLog_DEBUG(uint8_t *p_data, uint16_t length)
{
    Hid_Log(DEBUG_KEY_LOG_DEBUG, p_data, length);
}


void __HID_LOG(uint32_t flag, const char *name, const char  *format, ...)
{
    if (ProtoDebug_IsDebugLog(flag) == false)
    {
        return;
    }

    char	str[256];
    sprintf(str, "[%s]:", name);
    int len = strlen(str);
    char *p = str + len;
    va_list ap;

    va_start(ap, format);
    (void)vsnprintf(p, sizeof(str) - len, format, ap);
    va_end(ap);
    GSMLog_DEBUG((uint8_t *)str, strlen(str));

}


/**
 * 
 * 
 * @author hzhmcu (2018/9/4)
 * 
 * @param p_data  接收到的数据，最好是一段数据。
 * @param length 数据的长度
 */
void GSMDown_Rx(uint8_t *p_data, uint16_t length)
{
    Hid_Log(DEBUG_KEY_DOWN_SIM800_FILE, p_data, length);
}



/**
 * GSM Rx 的数据放在Log 打印
 * 
 * @author hzhmcu (2018/9/4)
 * 
 * @param p_data  接收到的数据，最好是一段数据。
 * @param length 数据的长度
 */
void GSMLog_Tx(uint8_t *p_data, uint16_t length)
{
    Hid_Log(DEBUG_KEY_LOG_GSM_TX, p_data, length);
}

