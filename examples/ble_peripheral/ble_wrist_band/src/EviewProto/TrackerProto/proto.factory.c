
#include "proto.control.h"
#include "proto.pb.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "indicator.h"
#include "record.h"
#include "dev_config.get.h"
#include "ls_api.h"
#include "loc_gsm.h"
#include "halbattery.h"
#include "loc.mode.h"
#include "halDelay.h"

#if FEATURE_WIFI
#include "WifiATType.h"
#include "loc_wifi.evt.h"
#endif
static int ProtoFactory_OpenGPS(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoFactory_CloseGPS(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoFactory_GetGPSUart(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoFactory_TestLEDMotor(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoFactory_GetGSMInfo(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoFactory_SetGSMCSQ(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoFactory_TestGSMMIC(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoFactory_GetMEMSID(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoFactory_GetFlashID(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoFactory_GetBatValue(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);

static int ProtoFactory_FirstSet(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoFactory_DownMP3(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);


const EpbProtoKey_t scg_sProtoFactroyKeys[] =
{
    {
        .key = FACTORY_KEY_FIRST_SET,
        .handle = ProtoFactory_FirstSet,
    },
    {
        .key = FACTORY_KEY_OPEN_GPS,
        .handle = ProtoFactory_OpenGPS,
    },
    {
        .key = FACTORY_KEY_CLOSE_GPS,
        .handle = ProtoFactory_CloseGPS,
    },
    {
        .key = FACTORY_KEY_GET_GPS_UART,
        .handle = ProtoFactory_GetGPSUart,
    },
    {
        .key = FACTORY_KEY_TEST_LED_MOTO,
        .handle = ProtoFactory_TestLEDMotor,
    },
    {
        .key = FACTORY_KEY_GET_GSM_INFO,
        .handle = ProtoFactory_GetGSMInfo,
    },
    {
        .key = FACTORY_KEY_SET_GSM_CSQ,
        .handle = ProtoFactory_SetGSMCSQ,
    },
    {
        .key = FACTORY_KEY_TEST_GSM_MIC,
        .handle = ProtoFactory_TestGSMMIC,
    },
    {
        .key = FACTORY_KEY_GET_MEMS_ID,
        .handle = ProtoFactory_GetMEMSID,
    },
    {
        .key = FACTORY_KEY_GET_FLASH_ID,
        .handle = ProtoFactory_GetFlashID,
    },
    {
        .key = FACTORY_KEY_GET_BAT_VALUE,
        .handle = ProtoFactory_GetBatValue,
    },
    {
        .key = FACTORY_KEY_DOWN_MP3,
        .handle = ProtoFactory_DownMP3,
    },
};
#define FACTORY_LOG(...)
const uint8_t s_sProtoFactroyKeysSize = sizeof(scg_sProtoFactroyKeys) / sizeof(scg_sProtoFactroyKeys[0]);

static int ProtoFactory_OpenGPS(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{

    FACTORY_LOG("OpenGPS\r\n");
    Proto_RespNegative(obj, EPB_SUCCESS);
    return EPB_SUCCESS;

}
static int ProtoFactory_CloseGPS(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    FACTORY_LOG("CloseGPS\r\n");
    Proto_RespNegative(obj, EPB_SUCCESS);
    return EPB_SUCCESS;
}


static int ProtoFactory_GetGPSUart(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    FACTORY_LOG("GetGPSUart\r\n");
    Proto_RespNegative(obj, EPB_SUCCESS);
    return EPB_SUCCESS;
}
static int ProtoFactory_TestLEDMotor(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    FACTORY_LOG("TestLEDMotor\r\n");
    Proto_RespNegative(obj, EPB_SUCCESS);
    return EPB_SUCCESS;
}
static int ProtoFactory_GetGSMInfo(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    FACTORY_LOG("GetGSMInfo\r\n");
    Proto_RespNegative(obj, EPB_SUCCESS);
    return EPB_SUCCESS;
}
static int ProtoFactory_SetGSMCSQ(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    FACTORY_LOG("SetGSMCSQ\r\n");
    Proto_RespNegative(obj, EPB_SUCCESS);
    return EPB_SUCCESS;
}
static int ProtoFactory_TestGSMMIC(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    FACTORY_LOG("TestGSMMIC\r\n");
    Proto_RespNegative(obj, EPB_SUCCESS);
    return EPB_SUCCESS;
}
static int ProtoFactory_GetMEMSID(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    FACTORY_LOG("GetMEMSID\r\n");
    Proto_RespNegative(obj, EPB_SUCCESS);
    return EPB_SUCCESS;
}
static int ProtoFactory_GetFlashID(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    FACTORY_LOG("GetFlashID\r\n");
    Proto_RespNegative(obj, EPB_SUCCESS);
    return EPB_SUCCESS;
}
static int ProtoFactory_GetBatValue(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    FACTORY_LOG("GetBatValue\r\n");
    Proto_RespNegative(obj, EPB_SUCCESS);
    return EPB_SUCCESS;
}
static int ProtoFactory_DownMP3(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{

    Proto_RespNegative(obj, EPB_SUCCESS);
    return EPB_SUCCESS;
}

static int ProtoFactory_FirstSet(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    HalBattery_IsCharge();

    ls_api_event_input(LS_EVT_GSM_ECHO_TEST);

#if FEATURE_WIFI
    ls_api_event_input(LS_EVT_WIFI_LOC);
#endif

    //1.打开GPS
    loc_mode_test_GPS_start();

#if GPS_GSM_SHARE_UART== 0
    ProtoDebu_SetGPSLog();
#endif

    //2.打开LED MOTO测试
    Indicator_Evt(ALERT_TYPE_TEST);
    //3.打开向GSM发送CSQ

    //4.向GSM发送 MIC
    ls_api_event_input(LS_EVT_GSM_ECHO_TEST);


    uint8_t buff[300];
    int index = 0;
    buff[index++] = COMMAND_ID_FACOTRY_TEST;

    //FLASH ID
    buff[index++] = 2;
    buff[index++] = FACTORY_KEY_GET_FLASH_ID;
    buff[index++] = (DoGetFlashID() == true ? 1 : 0);


    //MEMS ID
    uint8_t LIS3DH_GetStatus();
    buff[index++] = 2;
    buff[index++] = FACTORY_KEY_GET_MEMS_ID;
    buff[index++] = LIS3DH_GetStatus();

    //GPS ID
    uint8_t GegGPSStatus();
    buff[index++] = 2;
    buff[index++] = FACTORY_KEY_GET_GPS_UART;
    buff[index++] = GegGPSStatus();


    //GSM
    buff[index++] = 1 + 15 + 1 + DEV_ICCID_DATA_SIZE + 1;
    buff[index++] = FACTORY_KEY_GET_GSM_INFO;
    memcpy(buff + index, dev_config_get_IMEI(), 15);
    index += 15;
    buff[index++] =  ls_api_get_gsm_csq();
    memcpy(buff + index, dev_config_get_ICCID(), DEV_ICCID_DATA_SIZE);
    index += DEV_ICCID_DATA_SIZE;
    buff[index++] =  gsm_is_registered();

    //BATTERY
    uint32_t battery_voltage(void);
    uint8_t GetUSB_DETECTEDStatus();
    buff[index++] = 4;
    buff[index++] = FACTORY_KEY_GET_BAT_VALUE;
    buff[index++] = battery_voltage();
    buff[index++] = battery_voltage() >> 8;
    buff[index++] =  GetUSB_DETECTEDStatus();



    //GPS ID
    void HalArch_GetBLEMac(char *mac);
    buff[index++] = 7;
    buff[index++] = FACTORY_KEY_GET_BLE_MAC;
    HalArch_GetBLEMac(buff + index);
    index += 6;

#if FEATURE_WIFI
    WifiEvt_t const *pwifi = LocWifiEvt_GetLast();
    buff[index++] = 1 + pwifi->apInfo.count * 7;

    buff[index++] = FACTORY_KEY_GET_WIFI_LOC;
    for (int i = 0; i < pwifi->apInfo.count; i++)
    {
        buff[index++] = pwifi->apInfo.ap[i].rssi;
        memcpy(buff + index, pwifi->apInfo.ap[i].mac, 6);
        index += 6;
    }
#endif

    Proto_Resp(obj, FLAG_NORMAL_RESP, buff, index);
    FACTORY_LOG("FirstSet\r\n");
    return EPB_SUCCESS;
}






