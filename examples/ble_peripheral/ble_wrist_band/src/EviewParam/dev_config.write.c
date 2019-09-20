

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "app_util.h"

#include "dev_conf_defines.h"
#include "dev_conf.pb.h"
#include "halParam.h"

#include "gsflag.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#if 1
#define DEBUG 		NRF_LOG_INFO
#define SMS_STR_LOG		NRF_LOG_HEXDUMP_INFO
#else
#define DEBUG(...)
#define SMS_STR_LOG(...)
#endif


#define HOUR_VALIDATE(x)	( (x)  < 24 )
#define MINUTE_VALIDATE(x)	( (x) <  60 )


extern void dev_conf_save_profile(void);

#if 1 // property update

void dfw_property_imei_update(char *imei)
{
    uint32_t len = strlen(imei);
    if (len != DEV_IMEI_DATA_SIZE) return;

    strcpy(dev_property.imei, imei);
    DEBUG("[DFW]: imei update!!!\r\n");
}


static bool gsm_module(char *module)
{
    dev_property.module = -1;
    if (strcmp(module, "HL7650\r\n") == 0)
    {
        dev_property.module = 7650;
    }
    else if (strcmp(module, "HL7648\r\n") == 0)
    {
        dev_property.module = 7640;
    }
    else if (strcmp(module, "HL7688\r\n") == 0)
    {
        dev_property.module = 7688;
    }
    else if (strcmp(module, "HL7692\r\n") == 0)
    {
        dev_property.module = 7692;
    }
    else if (strncmp(module, "SIM800", strlen("SIM800")) == 0)
    {
        dev_property.module = 800;
    }

    if (dev_property.module  == -1)
    {
        return  false;
    }
    return true;
}

void dfw_property_gsm_module_update(char *module)
{
    uint32_t len = strlen(module);
    strcpy(dev_property.gsm_modlue, module);
    gsm_module(module);
    DEBUG("[DFW]: module update!!!\r\n");
}

void dfw_property_imsi_update(char *imsi)
{
    uint32_t len = strlen(imsi);
    if (len > DEV_IMSI_DATA_SIZE) return;

    strcpy(dev_property.imsi, imsi);
    DEBUG("[DFW]: imei update!!!\r\n");
}


void dfw_property_iccid_update(char *iccid)
{
    uint32_t len = strlen(iccid);
    if (len > DEV_ICCID_DATA_SIZE)
    {
        return;
    }
    strcpy(dev_property.iccid, iccid);
    DEBUG("[DFW]: iccid update!!!\r\n");
}

#endif

#if 1  // System Settings

bool dfw_system_mode_update(uint8_t mode, uint32_t time)
{
    sys_mode_t sysMode;
    sysMode.mode = mode;
    sysMode.time = time;
    if (DevCfg_Verify_Mode(&sysMode) == false)
    {
        return false;
    }

    dev_settings.system.mode.mode = mode;
    dev_settings.system.mode.time = time;
    m_dev_cb(DEV_CONF_SYSTEM_MODE, NULL);

//	DEBUG("[DFW]: system mode update!!!\r\n");
    return true;
}

bool dfw_system_init_mileage_update(uint32_t value)
{

    if (DevCfg_Verify_InitMileage(&value) == false) return false;

    dev_settings.system.initMileage = value;

    m_dev_cb(DEV_CONF_SYSTEM_MODE, NULL);

    return true;
}

//bool dfw_system_work_time_update( uint8_t start_hour, uint8_t start_minute, uint8_t stop_hour, uint8_t stop_minute )
//{
//	if( !HOUR_VALIDATE( start_hour ) || !HOUR_VALIDATE( stop_hour )
//	||	!MINUTE_VALIDATE(start_minute ) || !MINUTE_VALIDATE( stop_minute )) {
//		return false;
//	}
//
//	dev_settings.system.work_time.start_hour = start_hour;
//	dev_settings.system.work_time.start_minute = start_minute;
//	dev_settings.system.work_time.end_hour = stop_hour;
//	dev_settings.system.work_time.end_minute = stop_minute;
//
//	return true;
//}

bool dfw_system_do_not_disturb_update(uint8_t flag, uint8_t start_hour, uint8_t start_minute,
                                      uint8_t stop_hour, uint8_t stop_minute)
{
    system_no_disturb_t no_dis;
    no_dis.flag = flag;
    no_dis.start_hour = start_hour;
    no_dis.start_minute = start_minute;
    no_dis.end_hour = stop_hour;
    no_dis.end_minute = stop_minute;
    if (DevCfg_Verify_NoDisturb(&no_dis) == false) return false;

    dev_settings.system.no_disturb.flag = (flag & 0x80);
    dev_settings.system.no_disturb.start_hour = start_hour;
    dev_settings.system.no_disturb.start_minute = start_minute;
    dev_settings.system.no_disturb.end_hour = stop_hour;
    dev_settings.system.no_disturb.end_minute = stop_minute;

    m_dev_cb(DEV_CONF_SYSTEM_NO_DISTURB, NULL);

    return true;
}


bool dfw_system_timezone_update(int8_t offset)
{
    sys_time_zone_t zone;
    zone.offset = offset;
    if (DevCfg_Verify_Timezone(&zone) == false) return false;
    if (dev_settings.system.zone.offset != offset)
    {
        dev_settings.system.zone.offset = offset;
        m_dev_cb(DEV_CONF_SYSTEM_TIMEZONE, NULL);
    }

//	DEBUG("[DFW]: timezone update!!!\r\n");
    return true;
}


bool dfw_system_password_update(bool enable, uint32_t value)
{
    sys_password_t password;
    password.enable = enable;
    password.number = value;
    if (DevCfg_Verify_Password(&password) == false) return false;
    dev_settings.system.password.enable = enable;
    dev_settings.system.password.number = value;

    return true;
}

bool dfw_system_password_protect(bool enable)
{
    dev_settings.system.password.enable = enable;
//	DEBUG("[DFW]: password update!!!\r\n");
    return true;
}

bool dfw_system_switch_leds(bool on)
{
    dev_settings.system.switches.leds = on ? 1 : 0;
    m_dev_cb(DEV_CONF_SYSTEM_ENABLE_CONTROL, NULL);
    return true;
}

bool dfw_system_switch_motor(bool on)
{
    dev_settings.system.switches.motor = on ? 1 : 0;
    m_dev_cb(DEV_CONF_SYSTEM_ENABLE_CONTROL, NULL);
    return true;
}


bool dfw_system_switch_buzzer(bool on)
{
    dev_settings.system.switches.buzzer = on ? 1 : 0;
    m_dev_cb(DEV_CONF_SYSTEM_ENABLE_CONTROL, NULL);
    return true;
}

bool dfw_system_switch_wifi_loc(bool on)
{
    dev_settings.system.switches.WifiLoc = on ? 1 : 0;
    m_dev_cb(DEV_CONF_SYSTEM_ENABLE_CONTROL, NULL);
    return true;
}


bool dfw_system_switch_ble_loc(bool on)
{
    dev_settings.system.switches.BLELoc = on ? 1 : 0;
    m_dev_cb(DEV_CONF_SYSTEM_ENABLE_CONTROL, NULL);
    return true;
}





bool dfw_system_switch_sos_speaker(bool on)
{
    dev_settings.system.switches.SosSpeaker = on ? 1 : 0;
    m_dev_cb(DEV_CONF_SYSTEM_ENABLE_CONTROL, NULL);
    return true;
}
bool dfw_system_switch_x_speaker(bool on)
{
    dev_settings.system.switches.XSpeaker = on ? 1 : 0;
    m_dev_cb(DEV_CONF_SYSTEM_ENABLE_CONTROL, NULL);
    return true;
}

bool dfw_system_switch_BLEOftenConnection(bool on)
{
    dev_settings.system.switches.BLEOftenConnection = on ? 1 : 0;
    m_dev_cb(DEV_CONF_SYSTEM_ENABLE_CONTROL, NULL);
    return true;
}


bool dfw_system_switch_gsm_loc(bool on)
{
    dev_settings.system.switches.GSMLoc = on ? 1 : 0;
    m_dev_cb(DEV_CONF_SYSTEM_ENABLE_CONTROL, NULL);
    return true;
}

bool dfw_system_switch_agps(bool on)
{
    dev_settings.system.switches.AGPS = on ? 1 : 0;
    m_dev_cb(DEV_CONF_SYSTEM_ENABLE_CONTROL, NULL);
    return true;
}
bool dfw_system_switch_auto_update(bool on)
{
    dev_settings.system.switches.autoUpdate = on ? 1 : 0;
    m_dev_cb(DEV_CONF_SYSTEM_ENABLE_CONTROL, NULL);
    return true;
}


bool dfw_system_switches_update(uint32_t value)
{
    dev_settings.system.switches.value = value;
    m_dev_cb(DEV_CONF_SYSTEM_ENABLE_CONTROL, NULL);
    return true;
}

bool dfw_system_volume_ringtone_update(uint8_t level)
{
    if (DevCfg_Verify_RingToneVolume(&level) == false) return false;
    uint8_t lstValue = dev_settings.system.volume.ringtone;
    dev_settings.system.volume.ringtone = level;
    if (lstValue != level)
    {
        m_dev_cb(DEV_CONF_SYSTEM_RING_TONE_VOLUME, NULL);
    }

    return true;

}
bool dfw_system_volume_mic_update(uint8_t level)
{
    if (DevCfg_Verify_MicVolume(&level) == false) return false;
    uint8_t lstValue = dev_settings.system.volume.mic;
    dev_settings.system.volume.mic = level;

    if (lstValue != level)
    {
        m_dev_cb(DEV_CONF_SYSTEM_MIC_VOLUME, NULL);
    }


    return true;

}
bool dfw_system_volume_speaker_update(uint8_t level)
{
    if (DevCfg_Verify_SpeakerVolume(&level) == false) return false;
    uint8_t lstValue = dev_settings.system.volume.speaker;
    dev_settings.system.volume.speaker = level;
    if (lstValue != level)
    {
        m_dev_cb(DEV_CONF_SYSTEM_SPEAKER_VOLUME, NULL);
    }

    return true;

}


bool dfw_system_music_onoff_update(uint32_t value)
{

    dev_settings.system.musicswitch.value = value;
    return true;

}

bool dfw_system_gps_url_update(uint8_t const *data, uint8_t len)
{
    if (len > URL_MAX)
    {
        return false;
    }
    memcpy(dev_settings.system.gps_url.url, data, len);
    dev_settings.system.gps_url.url[len] = '\0';
    return true;
}

bool dfw_system_wifi_lbs_url_update(uint8_t const *data, uint8_t len)
{
    if (len > URL_MAX)
    {
        return false;
    }
    memcpy(dev_settings.system.wifi_lsb_url.url, data, len);
    dev_settings.system.wifi_lsb_url.url[len] = '\0';
    return true;
}


bool dfw_system_alarmclock_update(alarm_clock_t *alarm)
{
    if (DevCfg_Verify_Alarm(alarm) == false) return false;
    uint32_t idx = alarm->flag & CLOCKS_NUM_MASK;
    dev_settings.system.alarms[idx] = *alarm;
    return true;
}

bool dfw_system_ble_whitelist_update(uint8_t key, const uint8_t *p_data, uint8_t len)
{
    sys_ble_whitelist_t whitelist;
    memcpy((uint8_t *)&whitelist, p_data, sizeof(sys_ble_whitelist_t));
    memcpy((uint8_t *)&dev_settings.system.whitelist[whitelist.index], p_data, sizeof(sys_ble_whitelist_t));
    uint8_t index = whitelist.index;
    m_dev_cb(key, &index);
    return true;
}


#endif // !System

#if 1 //Buttons

bool dfw_buttons_update(uint8_t key, const uint8_t *p_data, uint8_t size)
{
    if (size != 2) return false;
    button_operation_t button;
    uint16_t value = uint16_decode(p_data);
    button.value = value;
    if (DevCfg_Verify_Button(&button) == false) return false;

    if (key == DEV_CONF_BUTTONS_SOS)
    {
        dev_settings.buttons.sos.value = value;
    }
    else if (key == DEV_CONF_BUTTONS_CALL_1)
    {
        dev_settings.buttons.call_1.value = value;
    }
    /*
    else if (key == DEV_CONF_BUTTONS_CALL_2)
    {
        dev_settings.buttons.call_2.value = value;
    }
    */
    else
    {
        return false;
    }

    m_dev_cb(key, NULL);
    return true;
}
bool dfw_alert_sos_button_mode(uint32_t input)
{
    bool ret = false;
    if (input == 2)
    {
        // 双击
        dev_settings.buttons.sos.enable = true;
        dev_settings.buttons.sos.task = BUTTONS_OP_SOS_ALERT;
        dev_settings.buttons.sos.triggerMode = BUTTONS_TRIGGER_MODE_DOUBLE_CLICK;

        ret = true;
    }
    else if (input == 1)
    {
        //长按
        dev_settings.buttons.sos.enable = true;
        dev_settings.buttons.sos.task = BUTTONS_OP_SOS_ALERT;
        dev_settings.buttons.sos.triggerMode = BUTTONS_TRIGGER_MODE_LONG;
        ret = true;
    }
    m_dev_cb(DEV_CONF_BUTTONS_SOS, NULL);
    return ret;
}


bool dfw_alert_sos_button(uint32_t input, uint16_t time)
{
    bool ret = false;

    button_operation_t button;
    button.enable = true;
    button.time =  time;
    button.feedback = SYS_BUTTON_FEEDBACK_DEF;
    button.task = BUTTONS_OP_SOS_ALERT;

    if (input == 2)
    {
        // 双击
        button.triggerMode = BUTTONS_TRIGGER_MODE_DOUBLE_CLICK;
        ret = true;
    }
    else if (input == 1)
    {
        //长按
        button.triggerMode = BUTTONS_TRIGGER_MODE_LONG;

        ret = true;
    }
    else return false;

    if (DevCfg_Verify_Button(&button) == false) return false;

    dev_settings.buttons.sos.value = button.value;
    m_dev_cb(DEV_CONF_BUTTONS_SOS, NULL);
    return ret;
}




bool dfw_alert_sos_button_time(uint32_t time)
{
    dev_settings.buttons.sos.time = time;
    m_dev_cb(DEV_CONF_BUTTONS_SOS, NULL);
    return true;
}


bool dfw_buttons_side_button_dial_no(uint8_t contact)
{
    return false;
}

bool dfw_buttons_set_side_button_disable()
{
    dev_settings.buttons.call_1.enable = false;
    m_dev_cb(DEV_CONF_BUTTONS_CALL_1, NULL);
    return true;
}

bool dfw_buttons_set_side_button(uint8_t auth, uint8_t feeback, uint8_t time)
{
    if (feeback > 3) return -1;
    if (auth > 10 || auth < 1) return -1;
    if (SYS_BUTTON_TIME_MAX < time) return -1;

    dev_settings.buttons.call_1.enable = true;
    dev_settings.buttons.call_1.feedback = 1; //only motor
    dev_settings.buttons.call_1.task = auth;
    dev_settings.buttons.call_1.time = time;
    m_dev_cb(DEV_CONF_BUTTONS_CALL_1, NULL);
    return true;
}

#endif // !Buttons

#if 1 // GPRS
bool dfw_gprs_apn_enable_set(bool enable)
{
    dev_settings.gprs.apn.enable = enable ? 1 : 0;
    return true;
}

bool DiscardNR(const char *in, char *out, int maxLen)
{
    uint32_t len = strlen(in);
    if (maxLen < len) return false;
    memcpy(out, in, len);
    for (int i = 0; i < len; i++)
    {
        if (out[i] == '\r' || out[i] == '\n')
        {
            out[i] = 0;
            return true;
        }
    }
    return true;
}


bool dfw_gprs_apn_apn_update(const char *apnStr)
{
    memset(dev_settings.gprs.apn.apns, 0, SYS_GPRS_APN_LEN_MAX);
    if (apnStr == NULL)
    {
        dev_settings.gprs.apn.apnLen = 0;
        dev_settings.gprs.apn.apns[0] = '\0';
        dfw_gprs_apn_enable_set(false);
        return true;
    }
    else
    {

        uint32_t len = strlen(apnStr) + 1;
        char out[len];
        memset(out, 0, len);
        if (false == DiscardNR(apnStr, out, len)) return false;
        len = strlen(out);

        if (len > SYS_GPRS_APN_LEN_MAX)
        {
            dev_settings.gprs.apn.apnLen = 0;
            dfw_gprs_apn_enable_set(false);
            return false;
        }
        strcpy(dev_settings.gprs.apn.apns, out);
        dev_settings.gprs.apn.apnLen = len;
        dfw_gprs_apn_enable_set(true);
    }

    return true;
}
bool dfw_gprs_apn_user_name_update(const char *usrStr)
{
    memset(dev_settings.gprs.apn.username, 0, SYS_GPRS_APN_USERNAME_LEN_MAX);
    if (usrStr == NULL)
    {
        dev_settings.gprs.apn.userNameLen = 0;
        dev_settings.gprs.apn.username[0] = '\0';
        return true;
    }
    else
    {
        uint32_t len = strlen(usrStr) + 1;
        if (len > SYS_GPRS_APN_USERNAME_LEN_MAX)
        {
            dev_settings.gprs.apn.userNameLen = 0;
            return false;
        }
        strcpy(dev_settings.gprs.apn.username, usrStr);
        dev_settings.gprs.apn.userNameLen = len;
    }
    return true;
}
bool dfw_gprs_apn_password_update(const char *pwdStr)
{
    memset(dev_settings.gprs.apn.password, 0, SYS_GPRS_APN_PASSWORD_LEN_MAX);
    if (pwdStr == NULL)
    {
        dev_settings.gprs.apn.passwordLen = 0;
        dev_settings.gprs.apn.password[0] = '\0';

        return true;
    }
    else
    {
        uint32_t len = strlen(pwdStr) + 1;
        if (len > SYS_GPRS_APN_PASSWORD_LEN_MAX)
        {
            dev_settings.gprs.apn.passwordLen = 0;
            return false;
        }
        strcpy(dev_settings.gprs.apn.password, pwdStr);
        dev_settings.gprs.apn.passwordLen = len;
    }
    return true;
}

bool dfw_gprs_cf_connect(uint32_t input)
{
    bool ret = false;
    if (input == 0)
    {
        ret = true;
        dfw_gprs_apn_enable_set(false);
    }
    else if (input == 1)
    {
        ret = true;
        dfw_gprs_apn_enable_set(true);
    }
    return ret;
}






bool dfw_gprs_apn_update(const char *apn, const char *usrname, const char *password)
{
    if (!dfw_gprs_apn_apn_update(apn)
        ||  !dfw_gprs_apn_user_name_update(usrname)
        ||  !dfw_gprs_apn_password_update(password))
    {
        return false;
    }

    m_dev_cb(DEV_CONF_GPRS_APN, NULL);
    return true;
}

bool dfw_gprs_apn_write(const uint8_t *p_data, uint8_t len)
{
    memset(dev_settings.gprs.apn.apns, 0, SYS_GPRS_APN_LEN_MAX);
    if (len == 0)
    {
        dev_settings.gprs.apn.apnLen = 0;
        dev_settings.gprs.apn.apns[0] = '\0';
        dfw_gprs_apn_enable_set(false);
        return true;
    }
    if (p_data == NULL) return false;

    //uint32_t offset = 0;
    uint8_t apnLen = len;
    if (apnLen > SYS_GPRS_APN_LEN_MAX) return false;

    if (apnLen > 0) memcpy(dev_settings.gprs.apn.apns, p_data, apnLen);
    else
    {
        dfw_gprs_apn_enable_set(false);
        dev_settings.gprs.apn.apns[0] = 0;
    }
    dfw_gprs_apn_enable_set(true);
    dev_settings.gprs.apn.apnLen = apnLen;

    m_dev_cb(DEV_CONF_GPRS_APN, NULL);
    return true;
}

bool dfw_gprs_apn_name_write(const uint8_t *p_data, uint8_t len)
{
    memset(dev_settings.gprs.apn.username, 0, SYS_GPRS_APN_USERNAME_LEN_MAX);
    if (len == 0)
    {
        dev_settings.gprs.apn.userNameLen = 0;
        dev_settings.gprs.apn.username[0] = '\0';
        return true;
    }
    if (p_data == NULL) return false;


    uint8_t userLen = len;

    if (userLen > SYS_GPRS_APN_USERNAME_LEN_MAX) return false;
    if (userLen > 0) memcpy(dev_settings.gprs.apn.username, p_data, userLen);
    else dev_settings.gprs.apn.username[0] = 0;
    dev_settings.gprs.apn.userNameLen =  userLen;

    m_dev_cb(DEV_CONF_GPRS_APN_USER_NAME, NULL);
    return true;
}

bool dfw_gprs_apn_password_write(const uint8_t *p_data, uint8_t len)
{
    memset(dev_settings.gprs.apn.password, 0, SYS_GPRS_APN_PASSWORD_LEN_MAX);
    if (len == 0)
    {
        dev_settings.gprs.apn.passwordLen = 0;
        dev_settings.gprs.apn.password[0] = '\0';
        return true;
    }
    if (p_data == NULL) return false;
    uint8_t pwLen = len;
    if (pwLen > SYS_GPRS_APN_PASSWORD_LEN_MAX) return false;
    if (pwLen > 0) memcpy(dev_settings.gprs.apn.password, p_data, pwLen);
    else dev_settings.gprs.apn.password[0] = 0;
    dev_settings.gprs.apn.passwordLen = pwLen;
    m_dev_cb(DEV_CONF_GPRS_APN_PASSWORD, NULL);
    return true;

}
bool dfw_gprs_set_connect(bool enable)
{
    dev_settings.gprs.server.enable = enable;
    m_dev_cb(DEV_CONF_GPRS_SERVER_PARAM, NULL);
    return true;
}

bool dfw_gprs_server_update(bool enable, bool isUDP, const char *ip, uint16_t port)
{
    if (ip == NULL) return false;
    uint32_t ipLen = strlen(ip);
    gprs_server_t server;
    server.enable = enable;
    server.isUDP = isUDP;
    server.port = port;
    if (SYS_GPRS_IP_LEN_MAX < ipLen) return false;
    strcpy(server.ip, ip);
    if (false == DevCfg_Verify_GPRS_Server(&server)) return false;

    dev_settings.gprs.server.enable = enable;
    dev_settings.gprs.server.isUDP = isUDP;
    strcpy(dev_settings.gprs.server.ip, ip);
    dev_settings.gprs.server.port = port;
    m_dev_cb(DEV_CONF_GPRS_SERVER_PARAM, NULL);
    return true;
}

bool dfw_gprs_server_write(const uint8_t *p_data, uint8_t len)
{
    if (len < 3 || len > ((SYS_GPRS_IP_LEN_MAX - 1) + 3)) return false;

    if (p_data == NULL) return false;

    uint8_t flag = p_data[0];
    uint16_t port = uint16_decode(p_data + 1);

    uint8_t ipLen = len - 3;


    dev_settings.gprs.server.flag = flag;
    dev_settings.gprs.server.port = port;
    memset(dev_settings.gprs.server.ip, 0, SYS_GPRS_IP_LEN_MAX);
    if (ipLen > 0) memcpy(dev_settings.gprs.server.ip, p_data + 3, ipLen);
    else dev_settings.gprs.server.ip[0] = 0;
    dev_settings.gprs.server.ip[ipLen] = '\0';


    m_dev_cb(DEV_CONF_GPRS_SERVER_PARAM, NULL);
    return true;
}

bool dfw_gprs_heartbeat_update(bool enable, uint32_t interval)
{
    interval &= 0x7FFFFFFF;
    if (interval < SYS_GPRS_HEART_BEAT_MIN
        ||  interval > SYS_GPRS_HEART_BEAT_MAX)
    {
        return false;
    }

    dev_settings.gprs.interval.heartbeat = interval;
    dev_settings.gprs.interval.hb_enable = enable;
    m_dev_cb(DEV_CONF_GPRS_INTERVAL, NULL);
    return true;
}
bool dfw_gprs_upload_interval_update(uint32_t interval)
{
    if (interval < SYS_GPRS_AUTO_UPLOAD_MIN
        ||  interval > SYS_GPRS_AUTO_UPLOAD_MAX)
    {
        return false;
    }

    dev_settings.gprs.interval.upload = interval;
    m_dev_cb(DEV_CONF_GPRS_INTERVAL, NULL);
    return true;
}
bool dfw_gprs_upload_lazy_interval_update(uint32_t interval)
{
    if (interval < SYS_GPRS_AUTO_UPLOAD_LAZY_MIN
        ||  interval > SYS_GPRS_AUTO_UPLOAD_LAZY_MAX)
    {
        return false;
    }

    dev_settings.gprs.interval.lazy_upload = interval;
    m_dev_cb(DEV_CONF_GPRS_INTERVAL, NULL);
    return true;
}

bool dfw_gprs_continue_loc_time_update(uint16_t interval, uint16_t time)
{
    gprs_continue_loc_t loc;
    loc.time = time;
    loc.interval = interval;
    if (DevCfg_Verify_ContinueLoc_Time(&loc) == false)
    {
        return false;
    }
    dev_settings.gprs.continue_loc.interval = interval;
    dev_settings.gprs.continue_loc.time = time;

    m_dev_cb(DEV_CONF_GPRS_CINTINUE_LOC, NULL);
    return true;
}
bool dfw_gprs_user_id_update(uint8_t const *data, uint16_t len)
{
    if (len > SYS_GPRS_GPRS_USER_ID_MAX)
    {
        return false;
    }
    memset(dev_settings.gprs.userid.data, 0, SYS_GPRS_GPRS_USER_ID_MAX);
    if (data == NULL || len == 0)
    {
        dev_settings.gprs.userid.len = 0;
    }
    else
    {
        memcpy(dev_settings.gprs.userid.data, data, len);
        dev_settings.gprs.userid.len  = len;
    }
    return true;
}


#endif // !GPRS

#if 1 // Phone
bool dfw_phone_contact_delete_at(uint8_t no)
{
    phone_contact_t cont;
    cont.idx = no;
    if (no > SYS_PHONE_INDEX_MAX) return false;
    dev_settings.phone.contacts[cont.idx].value = 0;
    dev_settings.phone.contacts[cont.idx].number[0] = '\0';
    return true;
}

bool dfw_phone_contact_update(uint8_t no, bool sms, bool phone, const char *number)
{

    if (
        // no < SYS_PHONE_INDEX_MIN ||
        no > SYS_PHONE_INDEX_MAX) return false;


    if (DevCfg_Verify_PhoneNumber(number) == false) return false;

    uint8_t index = no;
    dev_settings.phone.contacts[index].value = 0;
    dev_settings.phone.contacts[index].enable = 1;
    dev_settings.phone.contacts[index].sms = sms ? 1 : 0;
    dev_settings.phone.contacts[index].phone = phone ? 1 : 0;
    dev_settings.phone.contacts[index].idx = no;
    memset((uint8_t *)(dev_settings.phone.contacts[index].number), 0, SYS_PHONE_NUMBER_LEN_MAX);
    strcpy((char *)(dev_settings.phone.contacts[index].number), number);

    return true;
}

bool dfw_phone_contact_write(const uint8_t *p_data, uint8_t len)
{
    if (p_data == NULL) return false;

    if (len < SYS_PHONE_NUMBER_LEN_MIN || len > SYS_PHONE_NUMBER_LEN_MAX) return false;

    phone_contact_t contact;
    contact.value = p_data[0];
    uint8_t index = contact.idx;
    memset(contact.number, 0, SYS_PHONE_NUMBER_LEN_MAX);
    memcpy(contact.number, p_data + 1, len - 1);
    if (false == DevCfg_Verify_PhoneContact(&contact, index)) return false;

    dev_settings.phone.contacts[index].value = contact.value;
    memset(dev_settings.phone.contacts[index].number, 0, SYS_PHONE_NUMBER_LEN_MAX);
    memcpy(dev_settings.phone.contacts[index].number, p_data + 1, len - 1);

    return true;
}

bool dfw_phone_sms_prefix_update(bool enable, const char *prefix_text)
{
    if (!enable)
    {
        dev_settings.phone.sms_prefix.enable = false;
        dev_settings.phone.sms_prefix.size = 0;
        return true;
    }

    if (prefix_text == NULL) return false;

    uint32_t textLen = strlen(prefix_text);
    if (textLen > SYS_PHONE_SMS_REPLY_LEN_MAX) return false;

    dev_settings.phone.sms_prefix.enable = true;
    dev_settings.phone.sms_prefix.size = (uint8_t)textLen;
    strcpy(dev_settings.phone.sms_prefix.text, prefix_text);
    return true;
}

bool dfw_phone_sms_prefix_write(const uint8_t *p_data, uint8_t len)
{
    if (p_data == NULL) return false;

    uint8_t flag = p_data[0];
    if (len > (PHONE_SMS_PREFIX_LEN_MAX + 1)) return false;
    len -= 1;
    dev_settings.phone.sms_prefix.flag = flag;
    if (len > 0)
    {
        dev_settings.phone.sms_prefix.size = len;
        memcpy(dev_settings.phone.sms_prefix.text, p_data + 1, len);

        if (dev_settings.phone.sms_prefix.text[len - 2] == '\r' ||
            dev_settings.phone.sms_prefix.text[len - 2] == '\n'
           )
        {
            dev_settings.phone.sms_prefix.text[len - 2] = '\0';
            dev_settings.phone.sms_prefix.size = len - 2;
        }
        else if (dev_settings.phone.sms_prefix.text[len - 1] == '\r' ||
                 dev_settings.phone.sms_prefix.text[len - 1] == '\n'
                )
        {
            dev_settings.phone.sms_prefix.text[len - 1] = '\0';
            dev_settings.phone.sms_prefix.size = len - 1;
        }

    }
    else
    {
        dev_settings.phone.sms_prefix.size = 0;
        memset(dev_settings.phone.sms_prefix.text, '\0', SYS_PHONE_SMS_REPLY_LEN_MAX);
    }

    return true;
}

bool dfw_phone_sos_holding_time_update(uint16_t holding_time)
{

    //if (holding_time < SYS_PHONE_HOND_TIME_MIN || holding_time > SYS_PHONE_HOND_TIME_MAX) return false;

    dev_settings.phone.SOS.holding_time = holding_time;
    return true;
}
bool dfw_phone_sos_ring_time_update(uint8_t ring_time)
{
    if (ring_time < SYS_PHONE_RING_TIME_MIN || ring_time > SYS_PHONE_RING_TIME_MAX) return false;
    dev_settings.phone.SOS.rings_time = ring_time;
    return true;
}
bool dfw_phone_sos_loops_update(uint8_t loops)
{
    if (loops > SYS_PHONE_CYCLE_MAX) return false;
    dev_settings.phone.SOS.loops = loops;
    return true;
}

bool dfw_phone_sos_write(const uint8_t *p_data, uint8_t len)
{
    if (p_data == NULL) return false;

    if (len != PHONE_SOS_DATA_SIZE) return false;

    uint16_t holding_time = uint16_decode(p_data);
    uint8_t ring_time = p_data[2];
    uint8_t loops = p_data[3];
    phone_sos_t sos;
    sos.holding_time = holding_time;
    sos.loops = loops;
    sos.rings_time = ring_time;
    if (false == DevCfg_Verify_PhoneSOSOption(&sos)) return false;

    dev_settings.phone.SOS.holding_time = holding_time;
    dev_settings.phone.SOS.rings_time = ring_time;
    dev_settings.phone.SOS.loops = loops;
    return true;
}

bool dfw_phone_switch_auto_answer_update(bool enable, uint8_t rings)
{
    if (rings > SYS_PHONE_SWITCH_ANSWER_AFTER_MAX) return false;

    dev_settings.phone.switches.auto_answer_enable = enable;
    dev_settings.phone.switches.auto_answer_rings_delay = rings;
    return true;
}
bool dfw_phone_switch_intercept_update(uint8_t enable)
{
    dev_settings.phone.switches.call_set_only_by_auth_number = enable;
    return true;
}
bool dfw_phone_switch_hangup_update(uint8_t enable)
{
    dev_settings.phone.switches.hangup_manual_not_allow = enable;
    return true;
}
bool dfw_phone_switch_monitor_update(uint8_t enable)
{
    dev_settings.phone.switches.monitor_environment_enable = enable;
    return true;
}
bool dfw_phone_switch_set_sms_update(uint8_t enable)
{
    dev_settings.phone.switches.sms_set_only_by_auth_number = enable;
    return true;
}

bool dfw_phone_switches_update(const uint8_t *p_data, uint8_t len)
{
    if (len != PHONE_SWITCHES_DATA_SIZE) return false;

    uint32_t value = uint32_decode(p_data);
    phone_swithes_t *pswitch = (phone_swithes_t *)&value;
    if (pswitch->auto_answer_rings_delay > SYS_PHONE_SWITCH_ANSWER_AFTER_MAX) return false;

    dev_settings.phone.switches.value = value;
    return true;
}

#endif // !Phone

#if 1 // Alert
bool dfw_alert_power_low_update(uint8_t enable, uint8_t threshold)
{
    if (enable == true)
    {
        if (threshold > SYS_ALERT_POWER_THRES_MAX) return false;
        if (threshold < SYS_ALERT_POWER_THRES_MIN)  return false;
        dev_settings.alert.power.threshold = threshold;
    }
    dev_settings.alert.power.low = enable;
    return true;
}
bool dfw_alert_power_on_update(uint8_t enable)
{
    dev_settings.alert.power.on = enable;
    return true;
}

bool dfw_alert_power_off_update(uint8_t enable)
{
    dev_settings.alert.power.off = enable;
    return true;
}

bool dfw_alert_power_update(const uint8_t *p_data, uint8_t len)
{
    if (len != ALERT_POWER_DATA_SIZE) return false;

    alert_power_t power;
    power.value = uint32_decode(p_data);
    if (DevCfg_Verify_AlertPower(&power) == false) return false;
    dev_settings.alert.power = power;
    return true;
}

bool dfw_alert_geo_delete_at(uint8_t no)
{
    if (no > SYS_ALERT_GEO_INDEX_MAX) return false;

    dev_settings.alert.geos[no].enable = 0;
    uint32_t param = no;
    m_dev_cb(DEV_CONF_ALERT_GEO, &param);
    return true;
}
bool dfw_alert_geo_update(uint8_t no, uint8_t enable, uint8_t dir, uint16_t radius, int32_t longitude, int32_t latitude)
{
    if (no > SYS_ALERT_GEO_INDEX_MAX) return false;
    alert_geo_t GEO;
    GEO.idx = no;
    GEO.enable = enable;
    GEO.dir = dir;
    GEO.radius = radius;
    GEO.type = 0;   // use radius
    GEO.count = 1;

    GEO.data[0] = latitude;
    GEO.data[1] = longitude;

    if (false == DevCfg_Verify_AlertGEO(&GEO)) return false;

    dev_settings.alert.geos[no].value = GEO.value;
    memset(dev_settings.alert.geos[no].data, 0xff, (ALERT_GEO_FENCE_POINTS * 2 * 4));
    dev_settings.alert.geos[no].data[0] = GEO.data[0];
    dev_settings.alert.geos[no].data[1] = GEO.data[1];
    uint32_t param = no;
    m_dev_cb(DEV_CONF_ALERT_GEO, &param);
    return true;
}


bool dfw_alert_geo_write(const uint8_t *p_data, uint8_t len)
{
    if (len < ALERT_GEO_DATA_SIZE_MIN ||
        len > ALERT_GEO_DATA_SIZE_MAX) return false;

    if (p_data == NULL) return false;

    alert_geo_t Geo;
    memset(Geo.data, 0xff, (ALERT_GEO_FENCE_POINTS << 1));

    Geo.value = uint32_decode(p_data);

    if (Geo.idx >= ALERT_GEO_NUMBERS) return false;

    if ((Geo.count * 8) != (len - ALERT_GEO_DATA_SIZE_MIN)) return false;

    if (Geo.type == 0) //Radius Mode
    {
        if (Geo.count != 1) return false;
        if (Geo.radius <  ALERT_GEO_RADIUS_MIN || Geo.radius > ALERT_GEO_RADIUS_MAX) return false;
    }
    else // Fence Mode
    {
        if (Geo.count < 3) return false;
    }

    // parse lat&long point
    uint32_t index = 0;
    uint32_t offset = 4;
    for (uint32_t i = 0; i < Geo.count; i++)
    {
        Geo.data[index++] = uint32_decode(p_data + offset);
        Geo.data[index++] = uint32_decode(p_data + offset + 4);
        offset += 8;
    }

    // dev_settings.alert.geos[Geo.idx] = Geo;
    memcpy((uint8_t *)&dev_settings.alert.geos[Geo.idx],  (uint8_t *)&Geo, sizeof(alert_geo_t));
    uint32_t param = Geo.idx;
    m_dev_cb(DEV_CONF_ALERT_GEO, &param);
    return true;
}

bool dfw_alert_motion_update(bool enable, bool isDial, uint16_t setup_time, uint16_t action_time)
{
    if (enable)
    {
        if (setup_time > 0)
        {
            if (setup_time < ALERT_MOTION_SETUP_TIME_MIN
                ||	setup_time > ALERT_MOTION_SETUP_TIME_MAX)
            {
                return false;
            }
        }
        if (action_time > 0)
        {
            if (action_time < ALERT_MOTION_ACTION_TIME_MIN
                || action_time > ALERT_MOTION_ACTION_TIME_MAX)
            {
                return false;
            }
        }
        dev_settings.alert.motion.dial = isDial;
        dev_settings.alert.motion.enable = 1;
        if (setup_time > 0) dev_settings.alert.motion.setup_time = setup_time;
        if (action_time > 0) dev_settings.alert.motion.action_time = action_time;
    }
    else
    {
        dev_settings.alert.motion.enable = 0;
    }

    m_dev_cb(DEV_CONF_ALERT_MOTION, NULL);
    return true;
}

bool dfw_alert_motion_write(const uint8_t *p_data, uint8_t len)
{
    if (len != ALERT_MOTION_DATA_SIZE) return false;

    alert_motion_t motion;
    motion.value = uint32_decode(p_data);

    if (motion.enable > 0)
    {
        if (motion.setup_time < ALERT_MOTION_SETUP_TIME_MIN
            ||	motion.setup_time > ALERT_MOTION_SETUP_TIME_MAX)
        {
            return false;
        }

        if (motion.action_time < ALERT_MOTION_ACTION_TIME_MIN
            || motion.action_time > ALERT_MOTION_ACTION_TIME_MAX)
        {
            return false;
        }
    }

    dev_settings.alert.motion = motion;
    m_dev_cb(DEV_CONF_ALERT_MOTION, NULL);
    return true;
}
bool dfw_alert_actionless_update(bool enable, bool isDial, uint32_t threshold)
{
    if (enable)
    {
        if (threshold > ALERT_STATIC_THRESHOLD_MAX
            || threshold < ALERT_STATIC_THRESHOLD_MIN) return false;
        dev_settings.alert.actionless.dial = isDial;
        dev_settings.alert.actionless.enable = 1;
        dev_settings.alert.actionless.threshold = threshold;

    }
    else
    {
        dev_settings.alert.actionless.enable = 0;
    }
    m_dev_cb(DEV_CONF_ALERT_NO_MOTION, NULL);
    return true;
}

bool dfw_alert_actionless_write(const uint8_t *p_data, uint8_t len)
{
    if (len != ALERT_STATIC_DATA_SIZE) return false;

    alert_static_t actionless;
    actionless.value = uint32_decode(p_data);
    if (actionless.enable > 0)
    {
        if (actionless.threshold > ALERT_STATIC_THRESHOLD_MAX
            || actionless.threshold < ALERT_STATIC_THRESHOLD_MIN) return false;
    }

    dev_settings.alert.actionless = actionless;
    m_dev_cb(DEV_CONF_ALERT_NO_MOTION, NULL);
    return true;
}
bool dfw_alert_speed_update(bool enable, uint32_t threshold)
{
    if (enable)
    {
        if (threshold > ALERT_SPEED_THRESHOLD_MAX
            ||  threshold < ALERT_SPEED_THRESHOLD_MIN) return false;

        dev_settings.alert.speed.enable = 1;
        dev_settings.alert.speed.threshold = threshold;
    }
    else
    {
        dev_settings.alert.speed.enable = 0;
    }
    m_dev_cb(DEV_CONF_ALERT_OVER_SPEED, NULL);
    return true;
}

bool dfw_alert_speed_write(const uint8_t *p_data, uint8_t len)
{
    if (len != ALERT_SPEED_DATA_SIZE) return false;

    alert_speed_t speed;
    speed.value = uint16_decode(p_data);
    if (speed.enable > 0)
    {
        if (speed.threshold > ALERT_SPEED_THRESHOLD_MAX
            ||  speed.threshold < ALERT_SPEED_THRESHOLD_MIN) return false;
    }

    dev_settings.alert.speed = speed;
    m_dev_cb(DEV_CONF_ALERT_OVER_SPEED, NULL);
    return true;

}
bool dfw_alert_tilt_update(bool enable, bool isDial, uint8_t angle, uint16_t time)
{
    if (enable)
    {
        if (angle > 0)
        {
            if (angle > ALERT_TILT_ANGLE_MAX //|| angle < ALERT_TILT_ANGLE_MIN
               ) return false;
        }
        if (time > 0)
        {
            if (time > ALERT_TILT_TIME_MAX || time < ALERT_TILT_TIME_MIN) return false;
        }

        dev_settings.alert.tilt.enable = 1;
        dev_settings.alert.tilt.dial = isDial;
        if (angle > 0) dev_settings.alert.tilt.angle = angle;
        if (time > 0) dev_settings.alert.tilt.time = time;
    }
    else
    {
        dev_settings.alert.tilt.enable = 0;
    }
    m_dev_cb(DEV_CONF_ALERT_TILT, NULL);
    return true;
}
bool dfw_alert_tilt_write(const uint8_t *p_data, uint8_t len)
{
    if (len != ALERT_TILT_DATA_SIZE) return false;

    alert_tilt_t tilt;
    tilt.value = uint32_decode(p_data);
    if (tilt.enable > 0)
    {
        if (tilt.angle > ALERT_TILT_ANGLE_MAX /*|| tilt.angle < ALERT_TILT_ANGLE_MIN*/) return false;
        if (tilt.time > ALERT_TILT_TIME_MAX || tilt.time < ALERT_TILT_TIME_MIN) return false;
    }

    dev_settings.alert.tilt = tilt;
    m_dev_cb(DEV_CONF_ALERT_TILT, NULL);
    return true;
}

bool dfw_alert_falldown_update(bool enable, uint8_t level, bool dial)
{
    alert_falldown_t falldown;
    falldown.enable = enable;

    falldown.dial = dial;

    falldown.level = level;
    if (falldown.level > ALERT_FALLDOWN_LEVEL_MAX || falldown.level < ALERT_FALLDOWN_LEVEL_MIN) return false;


    dev_settings.alert.falldown = falldown;
    m_dev_cb(DEV_CONF_ALERT_FALLDOWN, NULL);
    return true;
}

bool dfw_alert_falldown_write(const uint8_t *p_data, uint8_t len)
{


    if (len != ALERT_FALLDOWN_DATA_SIZE) return false;

    alert_falldown_t falldown;
    falldown.value = p_data[0]; // uint32_decode(p_data);

    if (falldown.level > ALERT_FALLDOWN_LEVEL_MAX || falldown.level < ALERT_FALLDOWN_LEVEL_MIN) return false;


    dev_settings.alert.falldown = falldown;


    m_dev_cb(DEV_CONF_ALERT_FALLDOWN, NULL);
    return true;
}
#endif // !Alert
#if 1//ble
bool dfw_ble_scan_time_write(const uint8_t *p_data, uint8_t len)
{
    ble_scan_time_t scan_time;
    scan_time.scan_time = uint16_decode(p_data);
    scan_time.sleep_time = uint16_decode(p_data + 2);
    dev_settings.ble.scan_time = scan_time;
    m_dev_cb(DEV_CONF_BLE_SCAN_TIME, NULL);
    return true;
}
bool dfw_ble_scan_beacon_time_write(const uint8_t *p_data, uint8_t len)
{
    ble_scan_beacon_time_t scan_time;
    scan_time.time = uint16_decode(p_data);
    dev_settings.ble.scan_beacon_time = scan_time;
    m_dev_cb(DEV_CONF_BLE_SCAN_BEACON_TIME, NULL);
    return true;
}
bool dfw_ble_beacon_uuid_write(const uint8_t *p_data, uint8_t len)
{
    if (len != 19)
    {
        return false;
    }
    dev_settings.ble.beacon.flag = p_data[0];
    dev_settings.ble.beacon.company_id =  uint16_decode(p_data+1);
    memcpy(dev_settings.ble.beacon.uuid, p_data + 3, len);
    m_dev_cb(DEV_CONF_BLE_BEACON_UUID, NULL);
    return true;
}


#endif

bool dfw_settings_update(const uint8_t *p_data, uint16_t size, bool isSave)
{
    uint16_t index = 0;
    uint8_t field_type, field_len;
    bool success = true;
//	uint32_t value;
    DEBUG("Update dfw Setting\r\n");
    while (index < size)
    {
        field_len = p_data[index + 0];
        field_type = p_data[index + 1];
        DEBUG("field_type 0x%d\r\n", field_type);
        if ((field_len + index) > size) break;

        switch (field_type)
        {
        default:
            // success = false;  // not allow to write
            break;

#if 1	// System Update
        case DEV_CONF_SYSTEM_MODE:
            {
                if (field_len == 5)
                {
                    sys_mode_t sys_mode;
                    sys_mode.value = uint32_decode(p_data + index + 2);
                    if (!dfw_system_mode_update(sys_mode.mode, sys_mode.time))
                    {
                        success = false;
                        DEBUG("[CONF WRITE]: fail in systemo mode\r\n");
                    }
                }
                else
                {
                    success = false;
                }
                break; }


        case DEV_CONF_SYSTEM_INIT_MILEAGE:
            {
                if (field_len == 5)
                {
                    uint32_t value;
                    value = uint32_decode(p_data + index + 2);
                    if (!dfw_system_init_mileage_update(value))
                    {
                        success = false;
                        DEBUG("[CONF WRITE]: fail in systemo mode\r\n");
                    }
                }
                else
                {
                    success = false;
                }
                break; }


        case DEV_CONF_SYSTEM_NO_DISTURB:
            {
                if (field_len == 6)
                {
                    system_no_disturb_t no_disturb;
                    no_disturb.flag = p_data[index + 2];
                    no_disturb.start_hour = p_data[index + 3];
                    no_disturb.start_minute = p_data[index + 4];
                    no_disturb.end_hour = p_data[index + 5];
                    no_disturb.end_minute = p_data[index + 6];
                    if (!dfw_system_do_not_disturb_update(no_disturb.flag, no_disturb.start_hour,  no_disturb.start_minute,
                                                          no_disturb.end_hour, no_disturb.end_minute)
                       )
                    {
                        success = false;

                        DEBUG("[CONF WRITE]: fail in no disturb\r\n");
                    }
                }
                else
                {
                    success = false;
                }
                break;}
        case DEV_CONF_SYSTEM_PASSWORD:
            {
                if (field_len == 5)
                {
                    sys_password_t password;
                    password.value = uint32_decode(p_data + index + 2);
                    if (!dfw_system_password_update(password.enable, password.number))
                    {
                        success = false;

                        DEBUG("[CONF WRITE]: fail in password\r\n");
                    }
                }
                else
                {
                    success = false;
                }
                break; }
        case DEV_CONF_SYSTEM_TIMEZONE:
            {
                if (field_len == 2)
                {
                    uint8_t offset = p_data[index + 2];
                    if (!dfw_system_timezone_update((int8_t)offset))
                    {
                        success = false;

                        DEBUG("[CONF WRITE]: fail in timezone\r\n");
                    }
                }
                else
                {
                    success = false;
                }
                break; }
        case DEV_CONF_SYSTEM_ENABLE_CONTROL:
            {
                if (field_len == 5)
                {
                    uint32_t value = uint32_decode(p_data + index + 2);
                    if (!dfw_system_switches_update(value))
                    {
                        success = false;

                        DEBUG("[CONF WRITE]: fail in system switches\r\n");
                    }
                }
                else
                {
                    success = false;
                }
                break; }
        case DEV_CONF_SYSTEM_RING_TONE_VOLUME:
            {
                DEBUG("[CONF]: field_len = %d\r\n", field_len);
                if (field_len == 2)
                {


                    if (!dfw_system_volume_ringtone_update(p_data[index + 2])
                       )
                    {
                        success = false;

                        DEBUG("[CONF WRITE]: fail in volume \r\n");
                    }
                    DEBUG("[CONF]: rt = %d\r\n", p_data[index + 2]);
                }
                else
                {
                    success = false;
                }
                break; }

        case DEV_CONF_SYSTEM_MIC_VOLUME:
            {
                DEBUG("[CONF]: field_len = %d\r\n", field_len);
                if (field_len == 2)
                {


                    if (!dfw_system_volume_mic_update(p_data[index + 2])
                       )
                    {
                        success = false;

                        DEBUG("[CONF WRITE]: fail in volume \r\n");
                    }
                    DEBUG("[CONF]: mic = %d\r\n",  p_data[index + 2]);
                }
                else
                {
                    success = false;
                }
                break; }
        case DEV_CONF_SYSTEM_SPEAKER_VOLUME:
            {
                DEBUG("[CONF]: field_len = %d\r\n", field_len);
                if (field_len == 2)
                {
                    if (!dfw_system_volume_speaker_update(p_data[index + 2])
                       )
                    {
                        success = false;

                        DEBUG("[CONF WRITE]: fail in volume \r\n");
                    }
                    DEBUG("[CONF]: speaker = %d\r\n",  p_data[index + 2]);
                }
                else
                {
                    success = false;
                }
                break; }

        case DEV_CONF_SYSTEM_GPS_URL:
            {

                if (!dfw_system_gps_url_update(p_data + index + 2, field_len - 1))
                {
                    success = false;
                }
                break;
            }

        case DEV_CONF_SYSTEM_WIFILBS_URL:
            {

                if (!dfw_system_wifi_lbs_url_update(p_data + index + 2, field_len - 1))
                {
                    success = false;
                }
                break;
            }



        case DEV_CONF_SYSTEM_MUSIC_ONOFF:
            {
                DEBUG("[CONF]: field_len = %d\r\n", field_len);
                if (field_len == 5)
                {
                    uint32_t MusicOnoff = uint32_decode(p_data + index + 2);
                    if (!dfw_system_music_onoff_update(MusicOnoff)
                       )
                    {
                        success = false;

                        DEBUG("[CONF WRITE]: fail in volume \r\n");
                    }

                }
                else
                {
                    success = false;
                }
                break; }


        case DEV_CONF_SYSTEM_ALARMCLOCK:
            {
                if (field_len == 7)
                {
                    alarm_clock_t alarm;
                    alarm.flag = p_data[index + 2];
                    alarm.hour = p_data[index + 3];
                    alarm.minute = p_data[index + 4];
                    alarm.weekday = p_data[index + 5];
                    alarm.ring_time = p_data[index + 6];
                    alarm.ring_type = p_data[index + 7];
                    if (!dfw_system_alarmclock_update(&alarm))
                    {
                        success = false;
                        DEBUG("[CONF WRITE]: fail in alarmclock\r\n");
                    }
                }
                else
                {
                    success = false;
                }
                break; }

        case DEV_CONF_SYSTEM_BLE_WHITELIST:
            {
                success = dfw_system_ble_whitelist_update(field_type, p_data + index + 2, field_len - 1);
                if (!success)
                {
                    DEBUG("[CONF WRITE]: ble loc  \r\n");
                }
                break;
            }


#endif

#if 1 	// Buttons
        case DEV_CONF_BUTTONS_SOS:
        case DEV_CONF_BUTTONS_CALL_1:
        case DEV_CONF_BUTTONS_CALL_2:
            {
                success = dfw_buttons_update(field_type, p_data + index + 2, field_len - 1);
                if (!success)
                {
                    DEBUG("[CONF WRITE]: fail in button sense \r\n");
                }
                break;
            }

#endif 	//!Buttons

#if 1 	// GPRS
        case DEV_CONF_GPRS_APN:
            success = dfw_gprs_apn_write(p_data + index + 2, field_len - 1);
            if (!success)
            {
                DEBUG("[CONF WRITE]: fail in APN\r\n");
            }
            break;

        case DEV_CONF_GPRS_APN_USER_NAME:
            success = dfw_gprs_apn_name_write(p_data + index + 2, field_len - 1);
            if (!success)
            {
                DEBUG("[CONF WRITE]: fail in APN\r\n");
            }
            break;
        case DEV_CONF_GPRS_APN_PASSWORD:
            success = dfw_gprs_apn_password_write(p_data + index + 2, field_len - 1);
            if (!success)
            {
                DEBUG("[CONF WRITE]: fail in APN\r\n");
            }
            break;

        case DEV_CONF_GPRS_SERVER_PARAM:
            success = dfw_gprs_server_write(p_data + index + 2, field_len - 1);
            if (!success)
            {
                DEBUG("[CONF WRITE]: fail in server \r\n");
            }
            break;
        case DEV_CONF_GPRS_INTERVAL:
            if (field_len == 13)
            {
                uint32_t heartbeat = uint32_decode(p_data + index + 2);
                bool isEnable = heartbeat > 0x80000000;
                heartbeat &= 0x7FFFFFFF;
                uint32_t upload = uint32_decode(p_data + index + 6);
                uint32_t lazy_upload = uint32_decode(p_data + index + 10);

                success = 	dfw_gprs_heartbeat_update(isEnable, heartbeat)
                   && dfw_gprs_upload_interval_update(upload)
                   && dfw_gprs_upload_lazy_interval_update(lazy_upload);
                if (!success)
                {
                    DEBUG("[CONF WRITE]: fail in gprs time \r\n");
                }
            }
            else
            {
                success = false;
            }
            break;
        case DEV_CONF_GPRS_USER_ID:
            if (field_len > 1)
            {
                success =  dfw_gprs_user_id_update(p_data + index + 2, field_len - 1);
                if (!success)
                {
                    DEBUG("[CONF WRITE]: fail in gprs time \r\n");
                }
            }
            else
            {
                dfw_gprs_user_id_update(NULL, 0);
                success = true;
            }
            break;
        case DEV_CONF_GPRS_CINTINUE_LOC:
            if (field_len == 5)
            {

                uint16_t interval = uint16_decode(p_data + index + 2);
                uint16_t time = uint16_decode(p_data + index + 4);

                success =  dfw_gprs_continue_loc_time_update(interval, time);
                if (!success)
                {
                    DEBUG("[CONF WRITE]: fail in gprs time \r\n");
                }
            }
            else
            {
                success = false;
            }
            break;
#endif 	//!GPRS

#if 1 	// Phone
        case DEV_CONF_PHONE_AUTH_NUM:
            success = dfw_phone_contact_write(p_data + index + 2, field_len - 1);
            if (!success)
            {
                DEBUG("[CONF WRITE]: fail in phone contacts\r\n");
            }
            break;
        case DEV_CONF_PHONE_SMS_PREFIX:
            success = dfw_phone_sms_prefix_write(p_data + index + 2, field_len - 1);
            if (!success)
            {
                DEBUG("[CONF WRITE]: fail in SMS prefix\r\n");
            }
            break;
        case DEV_CONF_PHONE_SOS_OPTION:
            success = dfw_phone_sos_write(p_data + index + 2, field_len - 1);
            if (!success)
            {
                DEBUG("[CONF WRITE]: fail in SOS option\r\n");
            }
            break;
        case DEV_CONF_PHONE_SWITCHES:
            success = dfw_phone_switches_update(p_data + index + 2, field_len - 1);
            if (!success)
            {
                DEBUG("[CONF WRITE]: fail in phone switches \r\n");
            }
            break;
#endif 	//!Phone

#if 1 	// Alert
        case DEV_CONF_ALERT_POWER:
            success = dfw_alert_power_update(p_data + index +  2, field_len - 1);
            if (!success)
            {
                DEBUG("[CONF WRITE]: fail in alert power\r\n");
            }
            break;
        case DEV_CONF_ALERT_GEO:
            success = dfw_alert_geo_write(p_data + index + 2, field_len - 1);
            if (!success)
            {
                DEBUG("[CONF WRITE]: fail in alert GEO\r\n");
            }
            break;
        case DEV_CONF_ALERT_MOTION:
            success = dfw_alert_motion_write(p_data + index + 2, field_len - 1);
            if (!success)
            {
                DEBUG("[CONF WRITE]: fail in alert motion\r\n");
            }
            break;
        case DEV_CONF_ALERT_NO_MOTION:
            success = dfw_alert_actionless_write(p_data + index + 2, field_len - 1);
            if (!success)
            {
                DEBUG("[CONF WRITE]: fail in alert static\r\n");
            }
            break;
        case DEV_CONF_ALERT_OVER_SPEED:
            success = dfw_alert_speed_write(p_data + index + 2, field_len - 1);
            if (!success)
            {
                DEBUG("[CONF WRITE]: fail in alert speed\r\n");
            }
            break;
        case DEV_CONF_ALERT_TILT:
            success = dfw_alert_tilt_write(p_data + index + 2, field_len - 1);
            if (!success)
            {
                DEBUG("[CONF WRITE]: fail in alert tilt\r\n");
            }
            break;
        case DEV_CONF_ALERT_FALLDOWN:
            success = dfw_alert_falldown_write(p_data + index + 2, field_len - 1);
            if (!success)
            {
                DEBUG("[CONF WRITE]: fail in alert falldown\r\n");
            }
            break;
#endif 	//!Alert
#if 1  //BLE
        case DEV_CONF_BLE_SCAN_TIME:
            success = dfw_ble_scan_time_write(p_data + index + 2, field_len - 1);
            if (!success)
            {
                DEBUG("[CONF WRITE]: fail in ble scan time\r\n");
            }
            break;
        case DEV_CONF_BLE_SCAN_BEACON_TIME:
            success = dfw_ble_scan_beacon_time_write(p_data + index + 2, field_len - 1);
            if (!success)
            {
                DEBUG("[CONF WRITE]: fail in ble scan beacon time\r\n");
            }
            break;
        case DEV_CONF_BLE_BEACON_UUID:
            success = dfw_ble_beacon_uuid_write(p_data + index + 2, field_len - 1);
            if (!success)
            {
                DEBUG("[CONF WRITE]: fail in ble scan beacon time\r\n");
            }
            break;
#endif //! BLE

        }


        if (success == false)
        {
            //错误也不要break;
            success = success;
            // break;
        }
        index += field_len + 1;
    }

    if (isSave)
    {
        //设置完参数，统一保存。
        dev_conf_save_profile();
    }

    return success;
}

