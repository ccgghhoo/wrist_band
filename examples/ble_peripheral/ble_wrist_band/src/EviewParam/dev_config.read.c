

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdbool.h>
#include <string.h>
#include "ble_gap.h"
#include "app_util.h"
#include "DateTime.h"


#include "battery.h"

#include "dev_conf_defines.h"
#include "dev_conf.pb.h"
#include "dev_conf.h"
#include "dev_config.read.h"
#include "dev_config.factory.h"

#include "app_fw_info.h"
#include "LibConfig.h"
#include "DateTime.h"
#if 1
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#define CONFIG_LOG				NRF_LOG_INFO
#else
#define CONFIG_LOG(...)
#endif

#if 1 // DEV_CONF PB READ ALL KEYS
static const uint8_t all_dev_read[] =
{

    DEV_CONF_SYSTEM_MODULE, //0X01
    DEV_CONF_SYSTEM_FIRMWARE, //0x02
    DEV_CONF_SYSTEM_IMEI, //0x03
    DEV_CONF_SYSTEM_ICCID, //0x04
    DEV_CONF_SYSTEM_MAC, //0x05
    DEV_CONF_SYSTEM_SETTINGTIME, //0x06
    DEV_CONF_SYSTEM_RUNTIME, //0x07
    DEV_CONF_SYSTEM_FIRMWARE_INFO, //0x08
    DEV_CONF_SYSTEM_GSM_MODLUE,

    DEV_CONF_SYSTEM_INIT_MILEAGE, //09
    DEV_CONF_SYSTEM_MODE, //0xa
    DEV_CONF_SYSTEM_ALARMCLOCK, //0x0b
    DEV_CONF_SYSTEM_NO_DISTURB, //0x0c
    DEV_CONF_SYSTEM_PASSWORD, //0x0d
    DEV_CONF_SYSTEM_TIMEZONE, //0x0e
    DEV_CONF_SYSTEM_ENABLE_CONTROL, //0x0F
    DEV_CONF_SYSTEM_RING_TONE_VOLUME, //0x10
    DEV_CONF_SYSTEM_MIC_VOLUME, //0X11
    DEV_CONF_SYSTEM_SPEAKER_VOLUME, //0X12
    DEV_CONF_SYSTEM_DEVICE_NAME, //0X13
    DEV_CONF_SYSTEM_BATTERY, //0X14
    DEV_CONF_SYSTEM_BLE_WHITELIST, //0X16
    DEV_CONF_SYSTEM_GPS_URL, //0X17
    DEV_CONF_SYSTEM_WIFILBS_URL, //0X18
    DEV_CONF_SYSTEM_MUSIC_ONOFF,
    DEV_CONF_SYSTEM_FW, //1a


    // Button
    DEV_CONF_BUTTONS_SOS,
    DEV_CONF_BUTTONS_CALL_1,
    //DEV_CONF_BUTTONS_CALL_2,
    // Phone
    DEV_CONF_PHONE_AUTH_NUM,
    DEV_CONF_PHONE_SMS_PREFIX,
    DEV_CONF_PHONE_SOS_OPTION,
    DEV_CONF_PHONE_SWITCHES,

    // GPRS 0x40
    DEV_CONF_GPRS_APN,
    DEV_CONF_GPRS_APN_USER_NAME,
    DEV_CONF_GPRS_APN_PASSWORD,
    DEV_CONF_GPRS_SERVER_PARAM,
    DEV_CONF_GPRS_INTERVAL,
    DEV_CONF_GPRS_CINTINUE_LOC,
    DEV_CONF_GPRS_USER_ID,

    // ALERT 0x29
    DEV_CONF_ALERT_POWER,
    DEV_CONF_ALERT_GEO, //0x51
    DEV_CONF_ALERT_MOTION,  //0x52
    DEV_CONF_ALERT_NO_MOTION,  //0x53
    DEV_CONF_ALERT_OVER_SPEED,  //0x54
    DEV_CONF_ALERT_TILT,   //0x55
    DEV_CONF_ALERT_FALLDOWN, //0x56

    DEV_CONF_BLE_SCAN_TIME,
    DEV_CONF_BLE_SCAN_BEACON_TIME,
    DEV_CONF_BLE_BEACON_UUID, 
};


static const uint8_t all_dev_save_key[] =
{
    DEV_CONF_SYSTEM_INIT_MILEAGE, //09
    DEV_CONF_SYSTEM_MODE, //0xa
    DEV_CONF_SYSTEM_ALARMCLOCK, //0x0b
    DEV_CONF_SYSTEM_NO_DISTURB, //0x0c
    DEV_CONF_SYSTEM_PASSWORD, //0x0d
    DEV_CONF_SYSTEM_TIMEZONE, //0x0e
    DEV_CONF_SYSTEM_ENABLE_CONTROL, //0x0F
    DEV_CONF_SYSTEM_RING_TONE_VOLUME, //0x10
    DEV_CONF_SYSTEM_MIC_VOLUME, //0X11
    DEV_CONF_SYSTEM_SPEAKER_VOLUME, //0X12
    DEV_CONF_SYSTEM_BATTERY, //0X14
    DEV_CONF_SYSTEM_BLE_WHITELIST, //0x16
    DEV_CONF_SYSTEM_GPS_URL, //0X17
    DEV_CONF_SYSTEM_WIFILBS_URL, //0X18
    DEV_CONF_SYSTEM_MUSIC_ONOFF,


    // Button
    DEV_CONF_BUTTONS_SOS,
    DEV_CONF_BUTTONS_CALL_1,
    //DEV_CONF_BUTTONS_CALL_2,
    // Phone
    DEV_CONF_PHONE_AUTH_NUM,
    DEV_CONF_PHONE_SMS_PREFIX,
    DEV_CONF_PHONE_SOS_OPTION,
    DEV_CONF_PHONE_SWITCHES,

    // GPRS 0x40
    DEV_CONF_GPRS_APN,
    DEV_CONF_GPRS_APN_USER_NAME,
    DEV_CONF_GPRS_APN_PASSWORD,
    DEV_CONF_GPRS_SERVER_PARAM,
    DEV_CONF_GPRS_INTERVAL,
    DEV_CONF_GPRS_CINTINUE_LOC,
    DEV_CONF_GPRS_USER_ID,

    // ALERT 0x29
    DEV_CONF_ALERT_POWER,
    DEV_CONF_ALERT_GEO, //0x51
    DEV_CONF_ALERT_MOTION,  //0x52
    DEV_CONF_ALERT_NO_MOTION,  //0x53
    DEV_CONF_ALERT_OVER_SPEED,  //0x54
    DEV_CONF_ALERT_TILT,   //0x55
    DEV_CONF_ALERT_FALLDOWN, //0x56

    DEV_CONF_BLE_SCAN_TIME,
    DEV_CONF_BLE_SCAN_BEACON_TIME,
    DEV_CONF_BLE_BEACON_UUID,
};
#endif

uint16_t dev_config_read_max_size(void)
{
    return DEV_CONFIG_DATA_SIZE_MAX;
}


/**
 * System
 */

static int16_t Read_SysModule(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_SysFirmwareVersion(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_SysIMEI(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_SysGSMModule(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_SysICCID(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_SysBLEMac(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_SysSettingTime(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_SysRunTime(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_SysFirmwareInfo(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_SysInitMileage(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_SysMode(uint8_t *poutData, uint16_t maxLen);
static int16_t ReadPacketAll_SysAlarmClock(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_SysNoDisturb(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_SysInitMileage(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_SysPassword(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_SysTimeZone(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_SysEnableControl(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_SysRingToneVolume(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_SysMicVolume(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_SysSpeakerVolume(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_DeviceName(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_SysBattery(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_SysGPSURL(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_SysWIFILBSURL(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_SysMusicOnOff(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_SysInternalVersion(uint8_t *poutData, uint16_t maxLen);
static int16_t ReadPacketAll_SysBLEWhiteList(uint8_t *poutData, uint16_t maxLen);
/**
 * Button
 */
static int16_t Read_ButtonSOS(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_ButtonCall_1(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_ButtonCall_2(uint8_t *poutData, uint16_t maxLen);


/**
 * Phone
 */

static int16_t ReadPacketAll_PhoneAuthNumber(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_PhoneSMSPrefix(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_PhoneSOSOption(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_PhoneSwitch(uint8_t *poutData, uint16_t maxLen);



/**
 * GPRS
 */

static int16_t Read_GPRSApn(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_GPRSApnUserName(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_GPRSApnPassword(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_GPRSServiceParam(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_GPRSTimeInterval(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_GPRSContinueLocTime(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_GPRSUserID(uint8_t *poutData, uint16_t maxLen);

/**
 * Alert
 */
static int16_t Read_AlertPower(uint8_t *poutData, uint16_t maxLen);
static int16_t ReadPacketAll_AlertGEO(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_AlertMotion(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_AlertNoMotion(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_AlertSpeed(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_AlertTilt(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_AlertFallDown(uint8_t *poutData, uint16_t maxLen);

/**
 * ble 
 *   DEV_CONF_BLE_SCAN_TIME ,
    DEV_CONF_BLE_SCAN_BEACON_TIME,  
 */
static int16_t Read_BLEScanTime(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_BLEScanBeaconTime(uint8_t *poutData, uint16_t maxLen);
static int16_t Read_BLEBeaconUUID(uint8_t *poutData, uint16_t maxLen);


typedef struct {
    uint8_t key;
    /**
     * 以上是由函数外部处理fieldLen fieldType
     * 
     * @author hzhmcu (2018/5/11)
     */
    int16_t (*ReadHandle)(uint8_t *poutData, uint16_t maxLen);
    /**
     * 以上是由函数内部处理fieldLen fieldType
     *  
     * 主要是 Read_SysAlarmClock 一个命令需要发多个包。 
     *  
     * @author hzhmcu (2018/5/11)
     */
    int16_t (*ReadPacketAllDataHandle)(uint8_t *poutData, uint16_t maxLen);

}ConfigCmd_t;

const ConfigCmd_t scg_sConfigCmds[] = {
    /**
     * system
     */
#if 1
    {
        .key = DEV_CONF_SYSTEM_MODULE,
        .ReadHandle = Read_SysModule,
    },
    {
        .key = DEV_CONF_SYSTEM_FIRMWARE,
        .ReadHandle = Read_SysFirmwareVersion,
    },
    {
        .key = DEV_CONF_SYSTEM_FW,
        .ReadHandle = Read_SysInternalVersion,
    },
    {
        .key = DEV_CONF_SYSTEM_IMEI,
        .ReadHandle = Read_SysIMEI,
    },

    {
        .key = DEV_CONF_SYSTEM_GSM_MODLUE,
        .ReadHandle = Read_SysGSMModule,
    },
    {
        .key = DEV_CONF_SYSTEM_ICCID,
        .ReadHandle = Read_SysICCID,
    },
    {
        .key = DEV_CONF_SYSTEM_DEVICE_NAME,
        .ReadHandle = Read_DeviceName,
    },
    {
        .key = DEV_CONF_SYSTEM_MAC,
        .ReadHandle = Read_SysBLEMac,
    },
    {
        .key = DEV_CONF_SYSTEM_SETTINGTIME,
        .ReadHandle = Read_SysSettingTime,
    },
    {
        .key = DEV_CONF_SYSTEM_RUNTIME,
        .ReadHandle = Read_SysRunTime,
    },
    {
        .key = DEV_CONF_SYSTEM_FIRMWARE_INFO,
        .ReadHandle = Read_SysFirmwareInfo,
    },
    {
        .key = DEV_CONF_SYSTEM_INIT_MILEAGE,
        .ReadHandle = Read_SysInitMileage,
    },
    {
        .key = DEV_CONF_SYSTEM_MODE,
        .ReadHandle = Read_SysMode,
    },

    {
        .key = DEV_CONF_SYSTEM_ALARMCLOCK,
        .ReadHandle = NULL,
        .ReadPacketAllDataHandle = ReadPacketAll_SysAlarmClock,
    },
    {
        .key = DEV_CONF_SYSTEM_NO_DISTURB,
        .ReadHandle = Read_SysNoDisturb,
    },
    {
        .key = DEV_CONF_SYSTEM_PASSWORD,
        .ReadHandle = Read_SysPassword,
    },
    {
        .key = DEV_CONF_SYSTEM_TIMEZONE,
        .ReadHandle = Read_SysTimeZone,
    },

    {
        .key = DEV_CONF_SYSTEM_ENABLE_CONTROL,
        .ReadHandle = Read_SysEnableControl,
    },
    {
        .key = DEV_CONF_SYSTEM_RING_TONE_VOLUME,
        .ReadHandle = Read_SysRingToneVolume,
    },
    {
        .key = DEV_CONF_SYSTEM_MIC_VOLUME,
        .ReadHandle = Read_SysMicVolume,
    },
    {
        .key = DEV_CONF_SYSTEM_SPEAKER_VOLUME,
        .ReadHandle = Read_SysSpeakerVolume,
    },

    {
        .key = DEV_CONF_SYSTEM_BATTERY,
        .ReadHandle = Read_SysBattery,
    },
    {
        .key = DEV_CONF_SYSTEM_BLE_WHITELIST,
        .ReadPacketAllDataHandle = ReadPacketAll_SysBLEWhiteList,
        .ReadHandle = NULL,
    },
    {
        .key = DEV_CONF_SYSTEM_GPS_URL,
        .ReadHandle = Read_SysGPSURL,
    },

    {
        .key = DEV_CONF_SYSTEM_WIFILBS_URL,
        .ReadHandle = Read_SysWIFILBSURL,
    },

    {
        .key = DEV_CONF_SYSTEM_MUSIC_ONOFF,
        .ReadHandle = Read_SysMusicOnOff,
    },

#endif
/**
 * Button 
 */
#if 1
    {
        .key = DEV_CONF_BUTTONS_SOS,
        .ReadHandle = Read_ButtonSOS,
    },


    {
        .key = DEV_CONF_BUTTONS_CALL_1,
        .ReadHandle = Read_ButtonCall_1,
    },
    {
        .key = DEV_CONF_BUTTONS_CALL_2,
        .ReadHandle = Read_ButtonCall_2,
    },


#endif

/**
 * Phone 
 */
#if 1
    {
        .key = DEV_CONF_PHONE_AUTH_NUM,
        .ReadHandle = NULL,
        .ReadPacketAllDataHandle = ReadPacketAll_PhoneAuthNumber,
    },
    {
        .key = DEV_CONF_PHONE_SMS_PREFIX,
        .ReadHandle = Read_PhoneSMSPrefix,
    },
    {
        .key = DEV_CONF_PHONE_SOS_OPTION,
        .ReadHandle = Read_PhoneSOSOption,
    },
    {
        .key = DEV_CONF_PHONE_SWITCHES,
        .ReadHandle = Read_PhoneSwitch,
    },
#endif
/**
 * GPRS
 */
#if 1
    {
        .key = DEV_CONF_GPRS_APN,
        .ReadHandle = Read_GPRSApn,

    },
    {
        .key = DEV_CONF_GPRS_APN_USER_NAME,
        .ReadHandle = Read_GPRSApnUserName,
    },
    {
        .key = DEV_CONF_GPRS_APN_PASSWORD,
        .ReadHandle = Read_GPRSApnPassword,
    },
    {
        .key = DEV_CONF_GPRS_SERVER_PARAM,
        .ReadHandle = Read_GPRSServiceParam,
    },
    {
        .key = DEV_CONF_GPRS_INTERVAL,
        .ReadHandle = Read_GPRSTimeInterval,
    },
    {
        .key = DEV_CONF_GPRS_CINTINUE_LOC,
        .ReadHandle = Read_GPRSContinueLocTime,
    },
    {
        .key = DEV_CONF_GPRS_USER_ID,
        .ReadHandle = Read_GPRSUserID,
    },


#endif


/**
 * ALERT
 */
#if 1
    {
        .key = DEV_CONF_ALERT_POWER,
        .ReadHandle = Read_AlertPower,
    },
    {
        .key = DEV_CONF_ALERT_GEO,
        .ReadHandle = NULL,
        .ReadPacketAllDataHandle = ReadPacketAll_AlertGEO,

    },
    {
        .key = DEV_CONF_ALERT_MOTION,
        .ReadHandle = Read_AlertMotion,
    },
    {
        .key = DEV_CONF_ALERT_NO_MOTION,
        .ReadHandle = Read_AlertNoMotion,
    },
    {
        .key = DEV_CONF_ALERT_OVER_SPEED,
        .ReadHandle = Read_AlertSpeed,
    },
    {
        .key = DEV_CONF_ALERT_TILT,
        .ReadHandle = Read_AlertTilt,
    },
    {
        .key = DEV_CONF_ALERT_FALLDOWN,
        .ReadHandle = Read_AlertFallDown,
    },
#endif
/**
 * BLE
 */
#if 1
    {
        .key = DEV_CONF_BLE_SCAN_TIME,
        .ReadHandle = Read_BLEScanTime,
    },
    {
        .key = DEV_CONF_BLE_SCAN_BEACON_TIME,
        .ReadHandle = Read_BLEScanBeaconTime,
    },
    {
        .key = DEV_CONF_BLE_BEACON_UUID,
        .ReadHandle = Read_BLEBeaconUUID,
    },


#endif


};
const uint8_t cg_ucConfigCmdSize = sizeof(scg_sConfigCmds) / sizeof(scg_sConfigCmds[0]);




enum
{
    ERR_NO_CONTEXT  = -1,
    ERR_MAX_LEN = -2,
    ERR_NO_SUPPORT = -3,
};
/**
 * System
 */
#if 1

/**
 * 读模块号
 *  
 * 4B 
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_SysModule(uint8_t *poutData, uint16_t maxLen)
{
    if (maxLen < 4)
    {
        return ERR_MAX_LEN;
    }
    int16_t ret = 4;
    uint32_encode(PROPERTY_MODULE_NAME, poutData);
    return ret;
}


/**
 * 读版本号
 *  
 * 4B 
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_SysFirmwareVersion(uint8_t *poutData, uint16_t maxLen)
{
    if (maxLen < 4)
    {
        return ERR_MAX_LEN;
    }
    int16_t ret = 4;
    uint32_encode(PROPERTY_FIRMWARE_VER, poutData);
    return ret;
}

/**
 * 读版本号
 *  
 * 4B 
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_SysInternalVersion(uint8_t *poutData, uint16_t maxLen)
{

    int len = strlen(DEV_VERSION);
    if ((len + 1) > maxLen)
    {
        return ERR_MAX_LEN;
    }
    strncpy((char *)poutData, DEV_VERSION, len);
    poutData[len] = '\0';
    return len + 1;
}

/**
 * 读IMEI号
 *  
 * DEV_IMEI_DATA_SIZE 
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_SysIMEI(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = -1;

    if (dev_property.imei[0] == 0)
    {
        if (maxLen < 1)
        {
            return ERR_MAX_LEN;
        }
        ret = 0;
    }
    else
    {
        if (maxLen < DEV_IMEI_DATA_SIZE + 1)
        {
            return ERR_MAX_LEN;
        }
        memcpy(poutData, dev_property.imei, DEV_IMEI_DATA_SIZE);
        ret = DEV_IMEI_DATA_SIZE;
    }
    return ret;
}

/**
 * 读IMEI号
 *  
 * DEV_IMEI_DATA_SIZE 
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_SysGSMModule(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = -1;

    if (dev_property.gsm_modlue[0] == 0)
    {
        if (maxLen < 1)
        {
            return ERR_MAX_LEN;
        }
        ret = 0;
    }
    else
    {
        ret = strlen(dev_property.gsm_modlue);
        if (maxLen < ret + 1)
        {
            return ERR_MAX_LEN;
        }
        strcpy((char *)poutData, dev_property.gsm_modlue);
    }
    return ret;
}

/**
 * 读ICCID
 *  
 * DEV_ICCID_DATA_SIZE 
 *  
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_SysICCID(uint8_t *poutData, uint16_t maxLen)
{
    if (dev_property.iccid[0] == 0)
    {
        return 0;
    }

    memcpy(poutData, dev_property.iccid, DEV_ICCID_DATA_SIZE);
    int16_t ret = DEV_ICCID_DATA_SIZE;

    return ret;
}

/**
 * 读设备名称
 *  
 * DEV_ICCID_DATA_SIZE 
 *  
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_DeviceName(uint8_t *poutData, uint16_t maxLen)
{
    if (maxLen < strlen(DEVICE_NAME))
    {
        return ERR_MAX_LEN;
    }
    memcpy(poutData, DEVICE_NAME,  strlen(DEVICE_NAME));
    int16_t ret =  strlen(DEVICE_NAME);

    return ret;
}


/**
 * 读设备名称
 *  
 * DEV_ICCID_DATA_SIZE 
 *  
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_SysBattery(uint8_t *poutData, uint16_t maxLen)
{
    if (maxLen < 3)
    {
        return ERR_MAX_LEN;
    }
    poutData[0] = battery_level();
    poutData[1] = battery_voltage();
    poutData[2] = battery_voltage() >> 8;
    return 3;
}

/**
 * GPS URL
 *  
 * 
 *  
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_SysGPSURL(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = strlen(dev_settings.system.gps_url.url);
    if (maxLen < ret)
    {
        return ERR_MAX_LEN;
    }

    memcpy(poutData, dev_settings.system.gps_url.url, ret);
    poutData[++ret] = '\0';
    return ret;
}

/**
 * GPS URL
 *  
 * 
 *  
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_SysWIFILBSURL(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = strlen(dev_settings.system.wifi_lsb_url.url);
    if (maxLen < ret)
    {
        return ERR_MAX_LEN;
    }

    memcpy(poutData, dev_settings.system.wifi_lsb_url.url, ret);
    poutData[++ret] = '\0';
    return ret;
}

/**
 * 读音乐开关。
 *  
 * 
 *  
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_SysMusicOnOff(uint8_t *poutData, uint16_t maxLen)
{
    if (maxLen < 4)
    {
        return ERR_MAX_LEN;
    }
    int16_t ret = 4;
    uint32_encode(dev_settings.system.musicswitch.value, poutData);
    return ret;
}
/**
 * 读BLE MAC address
 *  
 * 6B 
 *  
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_SysBLEMac(uint8_t *poutData, uint16_t maxLen)
{
    if (maxLen <  6)
    {
        return ERR_MAX_LEN;
    }
    int16_t ret = 6;
    ble_gap_addr_t gap_addr;
    sd_ble_gap_addr_get(&gap_addr);
    memcpy(poutData, gap_addr.addr, 6);
    return ret;
}



/**
 * 读 系统当前时间
 *  
 * 4B  
 *  
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_SysSettingTime(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 4;
    if (maxLen <  4)
    {
        return ERR_MAX_LEN;
    }
    uint32_encode(UTC_GetValue(), poutData);
    return ret;
}

/**
 * 读 系统运行 tick
 *  
 * 4B  
 *  
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */

static int16_t Read_SysRunTime(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 4;
    if (maxLen <  4)
    {
        return ERR_MAX_LEN;
    }
    uint32_encode(RunTime_GetValue(), poutData);
    return ret;
}


/**
 * 读 版本信息。
 *  
 * 40  
 *  
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_SysFirmwareInfo(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = sizeof(app_infor_t) - 4;
    extern const app_infor_t appl_info;
    uint8_t *p = (uint8_t *)&appl_info;
    p += 4;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }

    memcpy(poutData, p, ret);
    return ret;
}

/**
 * 读初始化 英里数
 *  
 * 4B  
 *  
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_SysInitMileage(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 4;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    uint32_encode(dev_settings.system.initMileage, poutData);
    return ret;
}

/**
 * 读系统工作模式。
 *  
 * 4B  
 *  
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_SysMode(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 4;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    uint32_encode(dev_settings.system.mode.value, poutData);
    return ret;
}

/**
 * 这个是一个特例。 
 * 打包所有的数据。 
 * 一次性打包多个包。 
 * 
 * @author hzhmcu (2018/5/11)
 * 
 * @param poutData 
 * @param maxLen 
 * 
 * @return int16_t 
 */
static int16_t ReadPacketAll_SysAlarmClock(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret =  8 * (CLOCKS_NUM);

    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    int index = 0;
    for (int i = 0; i < ((CLOCKS_NUM)); i++)
    {
        poutData[index++] = 7; //长度一定不能写错。
        poutData[index++] = DEV_CONF_SYSTEM_ALARMCLOCK;
        poutData[index++] = dev_settings.system.alarms[i].flag;
        poutData[index++] = dev_settings.system.alarms[i].hour;
        poutData[index++] = dev_settings.system.alarms[i].minute;
        poutData[index++] = dev_settings.system.alarms[i].weekday;
        poutData[index++] = dev_settings.system.alarms[i].ring_time;
        poutData[index++] = dev_settings.system.alarms[i].ring_type;
//#warning "ring_type 还没有实现功能"
    }
    return index;
}


/**
 * 读 免打扰时间。
 *  
 * 4B  
 *  
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_SysNoDisturb(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 5;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    int index = 0;
    poutData[index++] = (uint8_t)(dev_settings.system.no_disturb.flag);
    poutData[index++] = (uint8_t)(dev_settings.system.no_disturb.start_hm >> 8);
    poutData[index++] = (uint8_t)(dev_settings.system.no_disturb.start_hm >> 0);
    poutData[index++] = (uint8_t)(dev_settings.system.no_disturb.stop_hm >> 8);
    poutData[index++] = (uint8_t)(dev_settings.system.no_disturb.stop_hm >> 0);

    return index;
}



/**
 * 读 Password 
 *  
 * 4B  
 *  
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_SysPassword(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 4;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    uint32_encode(dev_settings.system.password.value, poutData);

    return ret;
}

/**
 * 读 时区。 
 * 1byte,signed char,unit:15minutes
 *  
 * 4B  
 *  
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_SysTimeZone(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 1;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    poutData[0] = (uint8_t)(dev_settings.system.zone.offset);

    return ret;
}


/**
 * 读 开关控制bit 
 *  
 * 4B  
 *  
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_SysEnableControl(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 4;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    uint32_encode(dev_settings.system.switches.value, poutData);
    return ret;
}



/**
 * 读 响铃音量 1-100
 *  
 * 1B  
 *  
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_SysRingToneVolume(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 1;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    poutData[0] = dev_settings.system.volume.ringtone;
    return ret;
}



/**
 * 读 MIC音量 0-15
 *  
 * 1B  
 *  
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_SysMicVolume(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 1;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    poutData[0] = dev_settings.system.volume.mic;
    return ret;
}
/**
 * 读 Speaker音量 0-100
 *  
 * 1B  
 *  
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_SysSpeakerVolume(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 1;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    poutData[0] = dev_settings.system.volume.speaker;
    return ret;
}

static int16_t ReadPacketAll_SysBLEWhiteList(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret =  (2 + 1 + sizeof(sys_ble_whitelist_t)) * (WHITE_LIST_MAX);

    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    int index = 0;
    for (int i = 0; i < ((WHITE_LIST_MAX)); i++)
    {
        if (dev_settings.system.whitelist[i].index != i || dev_settings.system.whitelist[i].enable == false)
        {
            //continue;
        }

        poutData[index++] = 7 + 1;
        poutData[index++] = DEV_CONF_SYSTEM_BLE_WHITELIST;
        poutData[index++] =  dev_settings.system.whitelist[i].flag & 0xff;
        poutData[index++] =  (dev_settings.system.whitelist[i].mac[0]) & 0xff;
        poutData[index++] =  (dev_settings.system.whitelist[i].mac[1]) & 0xff;
        poutData[index++] =  (dev_settings.system.whitelist[i].mac[2]) & 0xff;
        poutData[index++] =  (dev_settings.system.whitelist[i].mac[3]) & 0xff;
        poutData[index++] =  (dev_settings.system.whitelist[i].mac[4]) & 0xff;
        poutData[index++] =  (dev_settings.system.whitelist[i].mac[5]) & 0xff;

    }
    return index;
}
#endif
#if 1
#define BUTTONS
/**
 * 读 SOS操作码
 *  
 * 4B  
 *  
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_ButtonSOS(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 2;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    uint16_encode(dev_settings.buttons.sos.value, poutData);
    return ret;
}
/**
 * 读  Call 1操作码
 *  
 * 4B  
 *  
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_ButtonCall_1(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 2;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    uint16_encode(dev_settings.buttons.call_1.value, poutData);
    return ret;
}

/**
 * 读  Call 2操作码
 *  
 * 4B  
 *  
 * @author hzhmcu (2018/5/15)
 * 
 * @param poutData 传出值
 * @param maxLen 传出最大长度
 * 
 * @return int16_t 返回长度。负数为不正确值。
 */
static int16_t Read_ButtonCall_2(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 2;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    uint16_encode(dev_settings.buttons.call_2.value, poutData);
    return ret;
}
#endif

#if 0 // Buttons Config  dev_settings.buttons

#warning "与协议不符"
case DEV_CONF_BUTTONS_SENSE:
{
    if (remains < (1 + 1 + 8))
    {
        goto DEV_CONFIG_READ_COMPLETED_LABEL;
    }

    read_data[chunk_len + 0] = 9;
    read_data[chunk_len + 1] = DEV_CONF_BUTTONS_SENSE;
    uint16_encode(dev_settings.buttons.sense.click,  read_data + chunk_len + 2);
    uint16_encode(dev_settings.buttons.sense.double_click, read_data + chunk_len + 4);
    uint16_encode(dev_settings.buttons.sense.short_hold, read_data + chunk_len + 6);
    uint16_encode(dev_settings.buttons.sense.long_hold, read_data + chunk_len + 8);

    chunk_len += 2 + 8;
    remains -= 2 + 8;
    break; }

case DEV_CONF_BUTTONS_OP:
{
    if (remains < (1 + 1 + 28))
    {
        goto DEV_CONFIG_READ_COMPLETED_LABEL;
    }

    read_data[chunk_len + 0] = 28 + 1;
    read_data[chunk_len + 1] = DEV_CONF_BUTTONS_OP;

    // add 4 bytes offset
    memcpy(read_data + chunk_len + 2, &dev_settings.buttons.opcode.task[1][0], 28);

    chunk_len += 2 + 28;
    remains -= 2 + 28;
    break; }
#endif // !Buttons Config



/**
 * Phone Config
 */

#if 1
static int16_t ReadPacketAll_PhoneAuthNumber(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret =  (2 + 1 + 20) * (PHONE_CONTACTS_NUM);

    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    int index = 0;
    for (int i = 0; i < ((PHONE_CONTACTS_NUM)); i++)
    {
        uint32_t tLen = strlen(dev_settings.phone.contacts[i].number);
        poutData[index++] = tLen + 2;
        poutData[index++] = DEV_CONF_PHONE_AUTH_NUM;
        poutData[index++] = dev_settings.phone.contacts[i].value;
        memcpy(poutData + index, dev_settings.phone.contacts[i].number, tLen);
        index += tLen;
    }
    return index;
}

static int16_t Read_PhoneSMSPrefix(uint8_t *poutData, uint16_t maxLen)
{
    if (dev_settings.phone.sms_prefix.size > PHONE_SMS_PREFIX_LEN_MAX) dev_settings.phone.sms_prefix.size = PHONE_SMS_PREFIX_LEN_MAX;
    int16_t ret = dev_settings.phone.sms_prefix.size;
    poutData[0] = dev_settings.phone.sms_prefix.flag;
    ret += 1;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    if (dev_settings.phone.sms_prefix.size > 0)
    {
        memcpy(poutData + 1, dev_settings.phone.sms_prefix.text, dev_settings.phone.sms_prefix.size);
    }
    return ret;
}

static int16_t Read_PhoneSOSOption(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 4;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    uint32_encode(dev_settings.phone.SOS.value, poutData);
    return ret;
}

static int16_t Read_PhoneSwitch(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 4;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    uint32_encode(dev_settings.phone.switches.value, poutData);
    return ret;
}


#endif
#define Read_GPRS
#if 1
static int16_t Read_GPRSApn(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = GPRS_APN_APN_LEN_MAX;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }

    if (dev_settings.gprs.apn.apnLen > GPRS_APN_APN_LEN_MAX)
    {
        dev_settings.gprs.apn.apnLen = GPRS_APN_APN_LEN_MAX;
    }

    ret =  dev_settings.gprs.apn.apnLen;
    if (dev_settings.gprs.apn.apnLen > 0)
    {
        memcpy(poutData, dev_settings.gprs.apn.apns, dev_settings.gprs.apn.apnLen);
    }
    return ret;
}
static int16_t Read_GPRSApnUserName(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = GPRS_APN_USERNAME_LEN_MAX;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }


    if (dev_settings.gprs.apn.userNameLen > GPRS_APN_USERNAME_LEN_MAX) dev_settings.gprs.apn.userNameLen = GPRS_APN_USERNAME_LEN_MAX;
    ret = dev_settings.gprs.apn.userNameLen;
    if (dev_settings.gprs.apn.userNameLen > 0)
    {
        memcpy(poutData, dev_settings.gprs.apn.username, dev_settings.gprs.apn.userNameLen);
    }
    return ret;
}


static int16_t Read_GPRSApnPassword(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = GPRS_APN_PASSWORD_LEN_MAX;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }


    if (dev_settings.gprs.apn.passwordLen > GPRS_APN_PASSWORD_LEN_MAX) dev_settings.gprs.apn.passwordLen = GPRS_APN_PASSWORD_LEN_MAX;

    ret = dev_settings.gprs.apn.passwordLen;
    if (dev_settings.gprs.apn.passwordLen > 0)
    {
        memcpy(poutData, dev_settings.gprs.apn.password, dev_settings.gprs.apn.passwordLen);
    }
    return ret;
}

static int16_t Read_GPRSServiceParam(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 32;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }

    uint32_t ipLen = strlen(dev_settings.gprs.server.ip);
    if (ipLen > 30) ipLen = 30;
    ret = ipLen + 3;
    poutData[0] = dev_settings.gprs.server.flag;
    // uint16_encode(dev_settings.gprs.server.flag, poutData);
    uint16_encode(dev_settings.gprs.server.port, poutData + 1);
    if (ipLen > 0)
    {
        memcpy(poutData + 3, dev_settings.gprs.server.ip, ipLen);
    }
    return ret;
}
static int16_t Read_GPRSTimeInterval(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 12;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }

    uint32_encode(dev_settings.gprs.interval.heartbeatValue, poutData);
    uint32_encode(dev_settings.gprs.interval.upload, poutData + 4);
    uint32_encode(dev_settings.gprs.interval.lazy_upload, poutData + 8);

    return ret;
}

static int16_t Read_GPRSContinueLocTime(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 4;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }

    uint16_encode(dev_settings.gprs.continue_loc.interval, poutData);
    uint16_encode(dev_settings.gprs.continue_loc.time, poutData + 2);

    return ret;
}

static int16_t Read_GPRSUserID(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = dev_settings.gprs.userid.len;
    if (maxLen <  dev_settings.gprs.userid.len)
    {
        return ERR_MAX_LEN;
    }
    memcpy(poutData, dev_settings.gprs.userid.data, dev_settings.gprs.userid.len);
    return ret;
}
#endif


#define Read_Alert
#if 1

static int16_t Read_AlertPower(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 4;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    uint32_encode(dev_settings.alert.power.value, poutData);
    return ret;
}

static int16_t ReadPacketAll_AlertGEO(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = (2 + ALERT_GEO_DATA_SIZE_MAX) * ALERT_GEO_NUMBERS;
    int index = 0;
    uint8_t *pLen = 0;
    uint8_t offset = 0;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    for (int i = 0; i < ALERT_GEO_NUMBERS; i++)
    {
        if (dev_settings.alert.geos[i].count > ALERT_GEO_FENCE_POINTS)
        {
            dev_settings.alert.geos[i].count = 0;
        }
        pLen = poutData + index; //len;
        index++;
        poutData[index++] = DEV_CONF_ALERT_GEO;
        uint32_encode(dev_settings.alert.geos[i].value, poutData + index);
        index += 4;
        offset = 1 + 4;

        for (uint32_t j = 0; j < dev_settings.alert.geos[i].count; j++)
        {
            uint32_encode(dev_settings.alert.geos[i].data[2 * j],  poutData + index);
            index += 4;
            offset += 4;
            uint32_encode(dev_settings.alert.geos[i].data[2 * j + 1], poutData + index);
            index += 4;
            offset += 4;
        }
        *pLen = offset;
    }
    return index;
}

static int16_t Read_AlertMotion(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 4;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    uint32_encode(dev_settings.alert.motion.value, poutData);
    return ret;
}


static int16_t Read_AlertNoMotion(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 4;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    uint32_encode(dev_settings.alert.actionless.value, poutData);
    return ret;
}

static int16_t Read_AlertSpeed(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 2;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    poutData[0] = dev_settings.alert.speed.value;
    poutData[1] = dev_settings.alert.speed.value >> 8;
    return ret;
}


static int16_t Read_AlertTilt(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 4;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    uint32_encode(dev_settings.alert.tilt.value, poutData);
    return ret;
}
static int16_t Read_AlertFallDown(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 1;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    poutData[0] = dev_settings.alert.falldown.value;
    // uint32_encode(dev_settings.alert.falldown.value, poutData);
    return ret;
}
#endif
#define Read_BLE
#if 1
static int16_t Read_BLEScanTime(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 4;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    uint16_encode(dev_settings.ble.scan_time.scan_time, poutData);
    uint16_encode(dev_settings.ble.scan_time.sleep_time, poutData + 2);
    return ret;
}
static int16_t Read_BLEScanBeaconTime(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 2;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
    uint16_encode(dev_settings.ble.scan_beacon_time.time, poutData);
    return ret;
}
static int16_t Read_BLEBeaconUUID(uint8_t *poutData, uint16_t maxLen)
{
    int16_t ret = 19;
    if (maxLen <  ret)
    {
        return ERR_MAX_LEN;
    }
   poutData[0] = dev_settings.ble.beacon.flag ;
    uint16_encode(dev_settings.ble.beacon.company_id, poutData+1);
    memcpy(poutData+3,dev_settings.ble.beacon.uuid,16);
    return ret;
}


#endif


/**
 * 读所Config（cmd == 0x02）的所有信息。
 *  
 * 几乎所有的信息都保存在  dev_settings
 *  
 *  
 * @author hzhmcu (2018/5/7)
 * 
 * @param p_data 传来的参数。
 * @param len 参数的长度。
 * @param p_read_out 输出读数据的内容
 * @param buff_max_size 输出最大读数据的内容。
 * 
 * @return uint16_t 
 */
uint16_t dev_config_read(const uint8_t *p_data, uint8_t plen,
                         uint8_t *p_read_out, uint16_t buff_max_size)
{
    if (p_read_out == NULL) return 0;

    const uint8_t *pb_keys;
    uint8_t size = 0;

    if (plen > 0)
    {
        pb_keys = p_data;
        size = plen;
    }
    else
    {
        pb_keys = all_dev_read;
        size = sizeof(all_dev_read);
    }

    uint16_t chunk_len = 0;

    uint16_t remains = buff_max_size;
    uint8_t *read_data = p_read_out;
    uint8_t key = 0;
    int16_t len;
    CONFIG_LOG("Maxsize %d\r\n", remains);

    //需要读多少个key
    for (int i = 0; i < size; i++)
    {
        key = pb_keys[i];

        //通过scg_sConfigCmds查找
        for (int j = 0; j < cg_ucConfigCmdSize; j++)
        {
            //已经找到 key值
            if (key == scg_sConfigCmds[j].key)
            {
                //查看标准处理函数有没有回调函数
                if (scg_sConfigCmds[j].ReadHandle)
                {
                    CONFIG_LOG("key 0x%X\r\n", key);
                    len = scg_sConfigCmds[j].ReadHandle(read_data + 2, remains - 2);
                    if (len >= 0)
                    {
                        read_data[0] = len + 1;
                        read_data[1] = key;
                        remains -= (len + 2);
                        read_data += (len + 2);
                        chunk_len += (len + 2);

                    }
                    else if (len == ERR_MAX_LEN)
                    {

                        CONFIG_LOG("ERR_MAX_LEN key 0x%X\r\n", key);

                        goto DEV_CONFIG_READ_COMPLETED_LABEL;
                    }
                    // CONFIG_LOG("key 0x%X remains:%d curLen:%d\r\n", key, remains, chunk_len);
                    //next key
                    break;
                }
                //再查读所有拼包处理函数有没有回调函数。
                else if (scg_sConfigCmds[j].ReadPacketAllDataHandle)
                {
                    CONFIG_LOG("key 0x%X\r\n", key);
                    len = scg_sConfigCmds[j].ReadPacketAllDataHandle(read_data, remains);
                    if (len > 0)
                    {
                        remains -= (len);
                        read_data += (len);
                        chunk_len += (len);

                    }
                    else if (len == ERR_MAX_LEN)
                    {

                        CONFIG_LOG("ERR_MAX_LEN key 0x%X\r\n", key);

                        goto DEV_CONFIG_READ_COMPLETED_LABEL;
                    }
                    //CONFIG_LOG("key 0x%X remains:%d curLen:%d\r\n", key, remains, chunk_len);
                    //next key
                    break;

                } //end if (scg_sConfigCmds[j].ReadPacketAllDataHandle)
            } //end if (key == scg_sConfigCmds[j].key)

        } //end for  (int j = 0; j < cg_ucConfigCmdSize; j++)
    } //end for size

DEV_CONFIG_READ_COMPLETED_LABEL:
    return chunk_len;
}
/**
 * 读所Config（cmd == 0x02）的所有信息。
 *  
 * 所有读的key
 *  
 *  
 * @author hzhmcu (2018/5/7)
 * 
 * @param p_data 传来的参数。
 * @param len 参数的长度。
 * @param p_read_out 输出读数据的内容
 * @param buff_max_size 输出最大读数据的内容。
 * 
 * @return uint16_t 
 */
uint16_t dev_config_read_all_save_key(uint8_t *p_read_out, uint16_t buff_max_size)
{
    return dev_config_read(all_dev_save_key, sizeof(all_dev_save_key), p_read_out, buff_max_size);
}
