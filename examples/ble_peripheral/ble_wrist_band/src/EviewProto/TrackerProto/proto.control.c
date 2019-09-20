
#include "proto.control.h"
#include "proto.pb.h"
//#include "mode.h"
//#include "record.h"
//#include "dev_conf.h"

//#include "HalActivityCFS.h"

#if (FEATURE_BLEC)
#include "AppBLEC.h"
#endif

static int ProtoControl_ResetRecode(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoControl_FactoryRecovery(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoControl_DeviceReset(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoControl_EnterDFU(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoControl_PowerOff(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoControl_FindMe(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoControl_BLEScanMode(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);





const EpbProtoKey_t scg_sProtoControlKeys[] =
{
    {
        .key = SYS_KEY_RESET_RECORD,
        .handle = ProtoControl_ResetRecode,
    },

    {
        .key = SYS_KEY_FACTORY_RECOVERY,
        .handle = ProtoControl_FactoryRecovery,
    },

    {
        .key = SYS_KEY_DEVICE_RESET,
        .handle = ProtoControl_DeviceReset,
    },

    {
        .key = SYS_KEY_FIND_ME,
        .handle = ProtoControl_FindMe,
    },
    {
        .key = SYS_KEY_BLE_SCAN_MODE,
        .handle = ProtoControl_BLEScanMode,
    },



};
const uint8_t s_ucProtoControlKeySize = sizeof(scg_sProtoControlKeys) / sizeof(scg_sProtoControlKeys[0]);


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
static int ProtoControl_ResetRecode(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    PB_LOG("Recode Reset\r\n");
    record_data_flush();
    HalActivityCFS_Format();
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
static int ProtoControl_FactoryRecovery(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    dev_conf_reset_to_factory();
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
static int ProtoControl_DeviceReset(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    //device_mode_set(RESET_AS_DIRECTLY);
    //Proto_RespNegative(obj, EPB_SUCCESS);
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
static int ProtoControl_BLEScanMode(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
#if (FEATURE_BLEC)
    AppBLEC_ToggleBLEScanMode();
#endif
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
static int ProtoControl_PowerOff(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    //device_mode_set(ENTER_POWER_OFF_MODE);
    //Proto_RespNegative(obj, EPB_SUCCESS);
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
static int ProtoControl_ResetDirectly(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    //device_mode_set(RESET_AS_DIRECTLY);
    //Proto_RespNegative(obj, EPB_SUCCESS);
    return EPB_SUCCESS;
}

/**
 * Find me
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
static int ProtoControl_FindMe(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    alert_on_findme();
    Proto_RespNegative(obj, EPB_SUCCESS);
    return EPB_SUCCESS;
}
