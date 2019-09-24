#include "proto.pb.h"
#include "proto.record.h"
#include "LibHeap.h"
#include "dev_conf.pb.h"
#include "dev_config.get.h"
//#include "loc_gsm.data.h"
//#include "record.h"
//#include "ls_api.h"
//#include "location.h" 
//#include "loc_gsm.data.h"
#include "proto.pack.h"
#include "app_motion_detect.h"

#if  0
static int ProtoRecord_SingleLocation(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoRecord_ContinueLocation(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoRecord_HistoricalData(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoRecord_MileageSet(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoRecord_GPSLatestLoc(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoRecord_BleLoc(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
#endif

static int ProtoRecord_StepCnt(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);

const EpbProtoKey_t scg_sProtoRecordKeys[] =
{
#if  0  
    {
        .key = DATA_SINGLE_LOCATION,
        .handle = ProtoRecord_SingleLocation,
    },
    {
        .key = DATA_CONTIMUE_LOCATION,
        .handle = ProtoRecord_ContinueLocation,
    },
    {
        .key = DATA_KEY_GPS_LATEST_LOC,
        .handle = ProtoRecord_GPSLatestLoc,
    },

    {
        .key = DATA_KEY_HISTORICAL_DATA,
        .handle = ProtoRecord_HistoricalData,
    },


    {
        .key = DATA_KEY_MILEAGE_SET,
        .handle = ProtoRecord_MileageSet,
    },

    {
        .key = DATA_KEY_BLE_LOC,
        .handle = ProtoRecord_BleLoc,
    },
#endif
    
    {
        .key = DATA_KEY_STEP_DATA,
        .handle = ProtoRecord_StepCnt,
    },
    
    
    
    
    
};

const uint8_t s_ucProtoRecodeKeySize = sizeof(scg_sProtoRecordKeys) / sizeof(scg_sProtoRecordKeys[0]);



static int ProtoRecord_StepCnt(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    uint8_t temp[16];
    uint16_t chunk_len = 0;

    temp[0] = COMMAND_ID_DATA;

    temp[1] = 10;
    temp[2] = DATA_KEY_STEP_DATA;
    
    uint32_t temp32 = RunTime_GetValue();
    
    memcpy(temp + 3, (uint8_t *)&temp32, 4);
    chunk_len += 7;
 
    temp32 = md_app_get_step_counter();  //get step cnt //chen
    memcpy(&temp[chunk_len], (uint8_t *)&temp32, 4);
    
    chunk_len += 4;

    Proto_Resp(obj, FLAG_NORMAL_RESP, temp, chunk_len);
    return true;
  
}




#if  0
bool ProtoRecord_gps_latest_data(objEpbProto_t *obj)
{
    uint8_t temp[50];
    uint16_t chunk_len = 0;

    temp[0] = COMMAND_ID_DATA;

    temp[1] = 16;
    temp[2] = DATA_KEY_IMEI;
    memcpy(temp + 3, (uint8_t *)dev_config_get_IMEI(), 15);
    chunk_len += 18;
 

    temp[chunk_len + 1] = DATA_KEY_GPS_LATEST_LOC;
    temp[chunk_len + 0] = ls_api_get_final_locate_packet(temp + chunk_len + 2) + 1;
    if (temp[chunk_len + 0] == 0)
    {
        return false;
    }

    chunk_len += temp[chunk_len + 0] + 1;

    Proto_Resp(obj, FLAG_NORMAL_RESP, temp, chunk_len);
    return true;
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
static int ProtoRecord_SingleLocation(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    PB_LOG("ProtoRecord_SingleLocation\r\n");
    ls_api_event_input(LS_EVT_SINGLE_LOCATOR);
    Proto_RespNegative(obj, EPB_SUCCESS);
    return EPB_SUCCESS;
}


static int ProtoRecord_ContinueLocation(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    PB_LOG("ProtoRecord_ContinueLocation\r\n");
    ls_api_event_input(LS_EVT_CONTINUE_LOCATOR);
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
static int ProtoRecord_HistoricalData(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    PB_LOG("ProtoRecord_HistoricalData\r\n");

    //触发发送历史数据
    ls_api_event_input(LS_EVT_UPDATE_HISTORICAL_DATA);

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
static int ProtoRecord_MileageSet(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    PB_LOG("ProtoRecord_MileageSet\r\n");
    if (len == 4)
    {
        uint32_t mileage = uint32_decode(p_data);
        ls_api_set_mileage(mileage);
    }
    Proto_RespNegative(obj, EPB_SUCCESS);
    return 0;
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
static int ProtoRecord_GPSLatestLoc(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    PB_LOG("ProtoRecord_GPSLatestLoc\r\n");

    uint8_t temp[50];
    uint16_t chunk_len = 0;

    temp[0] = COMMAND_ID_DATA;

    temp[1] = 16;
    temp[2] = DATA_KEY_IMEI;
    memcpy(temp + 3, (uint8_t *)dev_config_get_IMEI(), 15);
    chunk_len += 18;
  
    temp[chunk_len + 1] = DATA_KEY_GPS_LATEST_LOC;
    temp[chunk_len + 0] = ls_api_get_final_locate_packet(temp + chunk_len + 2) + 1;
    if (temp[chunk_len + 0] == 0)
    {
        Proto_RespNegative(obj, EPB_ERR_GPS_IS_NOT_READY);
        return 0;
    }

    chunk_len += temp[chunk_len + 0] + 1;

    Proto_Resp(obj, FLAG_NORMAL_RESP, temp, chunk_len);
    return 0;
}


static int ProtoRecord_BleLoc(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    PB_LOG("ProtoRecord_BLE loc\r\n");
    if (len < 14)
    {
        return 0;
    }
    int lat, lon;
    uint32_t temp;
    uint8_t mac[6];
    memcpy(mac, p_data, 6);
    temp = uint32_decode(p_data + 6);
    lat = *((int32_t *)&temp);
    temp = uint32_decode(p_data + 10);
    lon = *((int32_t *)&temp);

    ls_api_set_ble_loc(mac, lat, lon, NULL);

    Proto_RespNegative(obj, EPB_SUCCESS);
    return 0;
}
#endif

