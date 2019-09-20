
#include "proto.blebase.h"
#include "proto.pb.h"
//#include "mode.h"
//#include "record.h"
//#include "dev_conf.h"
//#include "loc_ble.evt.h"
#include "config.h"

#define BLEBASE_LOG HZH_LOG

static int ProtoBLEBase_BaseType(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoBLEBase_MusicPlay(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoBLEBase_MusicControl(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoBLEBase_DevType(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);


const EpbProtoKey_t scg_sProtoBLEBaseKeys[] =
{
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

};
const uint8_t s_ucProtoBLEBaseKeySize = sizeof(scg_sProtoBLEBaseKeys) / sizeof(scg_sProtoBLEBaseKeys[0]);


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

    g_sLocBLEEvt.type  = uint32_decode(p_data);
    g_sLocBLEEvt.FunctionFlag.value = uint32_decode(p_data + 4);

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
    g_sLocBLEEvt.musicplay  = uint32_decode(p_data);
    HID_LOG(LOG_MUSIC, "ble return  music 0x%x\r\n", g_sLocBLEEvt.musicplay);
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
    BLEBASE_LOG("Control");
   // HID_LOG(LOG_MUSIC, "ble return 0x%x\r\n", ctrl);
    if (ctrl <= BLE_MUSIC_CTRL_FINISH)
    {
        g_sLocBLEEvt.musiccontrol  = ctrl;
    }
    else
    {
        switch (ctrl)
        {
        case BLE_MUSIC_CTRL_SOS:
            button_evt_run(0);
            break;
        case BLE_MUSIC_CTRL_CALL1:
            button_evt_run(1);
            break;
        case BLE_MUSIC_CTRL_CALL2:
            button_evt_run(2);
            break;
        }

    }
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
        HID_LOG(LOG_BLE, "ble 2 return 0x%x\r\n", ctrl); 
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

    HID_LOG(LOG_MUSIC, "ble Get Type \r\n");
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
