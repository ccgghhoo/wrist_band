
#include "proto.blebase.h"
#include "proto.pb.h"
//#include "mode.h"
//#include "record.h"
//#include "dev_conf.h"
//#include "loc_ble.evt.h"
#include "config.h"
#include "app_init.h"

#define BLEBASE_LOG HZH_LOG


static int ProtoBLEwb_sosAlarm(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoBLEwb_infoData(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoBLEwb_sportData(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoBLEwb_updateTime(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoBLEwb_FwInfo(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);

//static int ProtoBLEBase_BaseType(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
//static int ProtoBLEBase_MusicPlay(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
//static int ProtoBLEBase_MusicControl(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
//static int ProtoBLEBase_DevType(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);


const EpbProtoKey_t scg_sProtoBLEBaseKeys[] =
{
#if 0    
    {
        .key = BLE_BASE_BASE_TYPE,
        .handle = ProtoBLEBase_BaseType,
    },

    {
        .key = BLE_BASE_MUSIC_PLAY,
        .handle = ProtoBLEBase_MusicPlay,
    },
    {
        .key = BLE_BASE_MUSIC_CTRL,
        .handle = ProtoBLEBase_MusicControl,
    },
    {
        .key = BLE_BASE_DEV_TYPE,
        .handle = ProtoBLEBase_DevType,
    },
#endif
    

    {
        .key = BLE_WB_SOS_KEY_ALARM,
        .handle = ProtoBLEwb_sosAlarm,
    },

    {
        .key = BLE_WB_READ_INFO_DATA,
        .handle = ProtoBLEwb_infoData,
    },
    {
        .key = BLE_WB_READ_SPORT_DATA,
        .handle = ProtoBLEwb_sportData,
    },
    {
        .key = BLE_WB_UPDATE_UTC_SECONDS,
        .handle = ProtoBLEwb_updateTime,
    }, 
    {
        .key = BLE_WB_READ_FW_INFO,
        .handle = ProtoBLEwb_FwInfo,
    },
    
    
    

};
const uint8_t s_ucProtoBLEBaseKeySize = sizeof(scg_sProtoBLEBaseKeys) / sizeof(scg_sProtoBLEBaseKeys[0]);


#if 0

/**
 * 地址解释
 * 
 * @author hzhmcu (2018/5/10)
 * 
 * @param obj 
 * @param key 
 * @param p_data 
 * @param len 
 * 
 * @return int 
 */
static int ProtoBLEBase_BaseType(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    if (len < 8)
    {
        Proto_RespNegative(obj, EPB_ERR_LENGTH);
        return EPB_SUCCESS;
    }

//    g_sLocBLEEvt.type  = uint32_decode(p_data);
//    g_sLocBLEEvt.FunctionFlag.value = uint32_decode(p_data + 4);

    Proto_RespNegative(obj, EPB_SUCCESS);
    return EPB_SUCCESS;
}

/**
 * 地址解释
 * 
 * @author hzhmcu (2018/5/10)
 * 
 * @param obj 
 * @param key 
 * @param p_data 
 * @param len 
 * 
 * @return int 
 */
static int ProtoBLEBase_MusicPlay(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    //g_sLocBLEEvt.musicplay  = uint32_decode(p_data);
//    HID_LOG(LOG_MUSIC, "ble return  music 0x%x\r\n", g_sLocBLEEvt.musicplay);
    Proto_RespNegative(obj, EPB_SUCCESS);
    return EPB_SUCCESS;
}

/**
 * 地址解释
 * 
 * @author hzhmcu (2018/5/10)
 * 
 * @param obj 
 * @param key 
 * @param p_data 
 * @param len 
 * 
 * @return int 
 */
static int ProtoBLEBase_MusicControl(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    int32_t ctrl  = uint32_decode(p_data);
//    BLEBASE_LOG("Control");
//   // HID_LOG(LOG_MUSIC, "ble return 0x%x\r\n", ctrl);
//    if (ctrl <= BLE_MUSIC_CTRL_FINISH)
//    {
//        g_sLocBLEEvt.musiccontrol  = ctrl;
//    }
//    else
//    {
//        switch (ctrl)
//        {
//        case BLE_MUSIC_CTRL_SOS:
//            button_evt_run(0);
//            break;
//        case BLE_MUSIC_CTRL_CALL1:
//            button_evt_run(1);
//            break;
//        case BLE_MUSIC_CTRL_CALL2:
//            button_evt_run(2);
//            break;
//        }
//
//    }
    if (ctrl != 0)
    {


        uint8_t buff[100] = { 0 };
        int index = 0;
        buff[index++] = COMMAND_ID_BLE_BASE;
        buff[index++] = 5;
        buff[index++] = key;

        int size = 0;
        buff[index++] = size;
        buff[index++] = size >> 8;
        buff[index++] = size >> 16;
        buff[index++] = size >> 24;
//        HID_LOG(LOG_BLE, "ble 2 return 0x%x\r\n", ctrl); 
        Proto_Resp(obj, FLAG_NORMAL_RESP, buff, index);
    }
    return EPB_SUCCESS;
}
/**
 * 地址解释
 * 
 * @author hzhmcu (2018/5/10)
 * 
 * @param obj 
 * @param key 
 * @param p_data 
 * @param len 
 * 
 * @return int 
 */
static int ProtoBLEBase_DevType(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    PB_LOG("OfpenConn\r\n");

//    HID_LOG(LOG_MUSIC, "ble Get Type \r\n");
    uint8_t buff[100] = { 0 };
    int index = 0;
    buff[index++] = COMMAND_ID_BLE_BASE;
    buff[index++] = 5;
    buff[index++] = key;

    int size =  dev_config_get_ble_often_connect_switch();
    buff[index++] = size;
    buff[index++] = size >> 8;
    buff[index++] = size >> 16;
    buff[index++] = size >> 24;

    Proto_Resp(obj, FLAG_NORMAL_RESP, buff, index);

    return EPB_SUCCESS;
}
#endif





static int ProtoBLEwb_sosAlarm(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    if(*p_data == 1 && *(p_data+1) == 1 )
    {
         clear_app_evt(APP_EVT_SOS_ALARM);//表示主机已收到SOS报警信号       
    }
    return EPB_SUCCESS;
}

static int ProtoBLEwb_infoData(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    uint8_t temp[16]={0};
    uint16_t chunk_len = 0;

    temp[0] = COMMAND_ID_BLE_BASE;

    temp[1] = 10;
    temp[2] = BLE_WB_READ_INFO_DATA;
    
    uint32_t temp32 = UTC_GetValue();
    
    memcpy(temp + 3, (uint8_t *)&temp32, 4);
    chunk_len += 7;
    
    temp32 = md_app_get_step_counter();  //get step cnt //chen
    memcpy(&temp[chunk_len], (uint8_t *)&temp32, 4);    
    chunk_len += 4;
    
    temp[chunk_len++] = batt_level_get();
                 
    Proto_Resp(obj, FLAG_NORMAL_RESP, temp, chunk_len);
    
    return EPB_SUCCESS;
}
static int ProtoBLEwb_sportData(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
 /*   
    uint8_t temp[128] = {0};
    uint16_t chunk_len = 0;
    uint16_t read_len =0;

    temp[0] = COMMAND_ID_BLE_BASE;
    //temp[1] = 0;
    temp[2] = BLE_WB_READ_SPORT_DATA;
    
    chunk_len += 3;            
    sport_level_data_read((uint32_t*)&temp[3],  &read_len, *p_data);  //read_len = 96bytes/hour
    chunk_len += read_len;
    temp[1] = chunk_len-2;   //data len
    
    Proto_Resp(obj, FLAG_NORMAL_RESP, temp, chunk_len);
*/  
    if(*p_data == 0)
    {       
        del_current_sport_record(); 
        clear_app_evt(APP_EVT_SPORT_SEND_WAIT);
        return EPB_SUCCESS;
    }
    else
    {
        return -1;
    }
    

}

static int ProtoBLEwb_updateTime(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
       
    uint32_t utc = p_data[0] + (p_data[1]<<8) + (p_data[2]<<16) + (p_data[3]<<24);
    
    UTC_SetValue(utc) ;  
    DateTime_UTC2DateTime(utc);   
    
    //Proto_RespPositive(objEpbProto_t *obj, uint8_t cmd, uint8_t key)
    Proto_RespNegative(obj, EPB_SUCCESS);   
    
    return EPB_SUCCESS;
}

static int ProtoBLEwb_FwInfo(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    uint8_t temp[44] = {0};
     
    temp[0] = 41;     //len
	temp[1] = BLE_WB_READ_FW_INFO;   //key
    
    memcpy(&temp[2], (uint8_t *)&appl_info.fw_version, 40/*sizeof(app_info_t)*/);
        
    Proto_Resp(obj, FLAG_NORMAL_RESP, temp, 41+1);
    
    return EPB_SUCCESS;
}





