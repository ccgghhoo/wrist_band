// Header:		Device Configuration
// File Name: 	dev_conf.c
// Author:		Carlton
// Date:		2017.05.25


#include "nrf.h"
#include <string.h>

#include "crc16.h"
#include "app_util.h"
#include "dev_conf.h"
//#include "steptab.h"
#include "halParam.h"
#include "dev_config.factory.h"
#include "DateTime.h"
#include "LibHeap.h"
#include "proto.pb.h"
#include "Proto.pack.h"
#include "config.h"
#include "dev_conf.h"
#if 0
#define DEV_LOG  NRF_LOG_INFO
#else
#define DEV_LOG
#endif



typedef struct {
    uint8_t *pData;
    uint32_t timestamp; //申请的时间
    uint8_t failCnt;
    bool isNewData;
    bool isSaveFail;
    bool isSaveCompleted;
    uint32_t ret;

}DevSaveInfo_t;
void DevSave_Start();

static DevSaveInfo_t sg_sDevSaveInfo = { .pData = NULL, };
dev_property_t dev_property;
dev_config_t dev_settings;
int32_t dev_config_value_change(uint32_t conf_update, void *param);
dev_settings_update_handler_t m_dev_cb = dev_config_value_change; //dev_config_value_change



#define MAX_PARAM (1024 * 2)





static bool DevConf_IsVerify(dev_config_t *pDevConf, bool isSetDef, bool isAllSetDef)
{
    bool ret = true;
    if (isAllSetDef == true)
    {
        isSetDef = true;
    }
    if (isAllSetDef == true || DevCfg_Verify_Mode(&pDevConf->system.mode) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_Mode_DEF(&pDevConf->system.mode);
    }

    int i = 0;
    for (i = 0; i < 4; i++)
    {

        if (isAllSetDef == true || DevCfg_Verify_Alarm(&pDevConf->system.alarms[i]) == false)
        {
            ret = false;
            if (true == isSetDef) DevCfg_Verify_Alarm_DEF(&pDevConf->system.alarms[i], i);
        }
    }

    if (isAllSetDef == true || DevCfg_Verify_NoDisturb(&pDevConf->system.no_disturb) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_NoDisturb_DEF(&pDevConf->system.no_disturb);
    }



    if (isAllSetDef == true || DevCfg_Verify_Password(&pDevConf->system.password) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_Password_DEF(&pDevConf->system.password);
    }


    if (isAllSetDef == true || DevCfg_Verify_Timezone(&pDevConf->system.zone) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_Timezone_DEF(&pDevConf->system.zone);
    }



    if (isAllSetDef == true || DevCfg_Verify_Switchs(&pDevConf->system.switches) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_Switchs_DEF(&pDevConf->system.switches);
    }


    if (isAllSetDef == true || DevCfg_Verify_RingToneVolume(&pDevConf->system.volume.ringtone) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_RingToneVolume_DEF(&pDevConf->system.volume.ringtone);
    }


    if (isAllSetDef == true || DevCfg_Verify_MicVolume(&pDevConf->system.volume.mic) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_MicVolume_DEF(&pDevConf->system.volume.mic);
    }

    if (isAllSetDef == true || DevCfg_Verify_SpeakerVolume(&pDevConf->system.volume.speaker) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_SpeakerVolume_DEF(&pDevConf->system.volume.speaker);
    }

    if (isAllSetDef == true)
    {
        if (true == isSetDef) DevCfg_Verify_MusicSwitch_DEF(&pDevConf->system.musicswitch);

    }
    //button

    if (isAllSetDef == true || DevCfg_Verify_Button(&pDevConf->buttons.sos) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_Button_DEF(&pDevConf->buttons.sos, 0);
    }

    if (isAllSetDef == true || DevCfg_Verify_Button(&pDevConf->buttons.call_1) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_Button_DEF(&pDevConf->buttons.call_1, 1);
    }

    if (isAllSetDef == true || DevCfg_Verify_Button(&pDevConf->buttons.call_2) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_Button_DEF(&pDevConf->buttons.call_2, 2);
    }


    //phone
    for (i = 0; i < 10; i++)
    {
        if (isAllSetDef == true || DevCfg_Verify_PhoneContact(&pDevConf->phone.contacts[i], i) == false)
        {
            ret = false;
            if (true == isSetDef) DevCfg_Verify_PhoneContact_DEF(&pDevConf->phone.contacts[i], i);
        }
    }
    if (isAllSetDef == true || DevCfg_Verify_PhoneSMSRepleyPrefix(&pDevConf->phone.sms_prefix) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_PhoneSMSRepleyPrefix_DEF(&pDevConf->phone.sms_prefix);
    }
    if (isAllSetDef == true || DevCfg_Verify_PhoneSOSOption(&pDevConf->phone.SOS) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_PhoneSOSOption_DEF(&pDevConf->phone.SOS);
    }

    if (isAllSetDef == true || DevCfg_Verify_PhoneSwitch(&pDevConf->phone.switches) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_PhoneSwitch_DEF(&pDevConf->phone.switches);
    }


    //GPRS

    if (isAllSetDef == true || DevCfg_Verify_APN(&pDevConf->gprs.apn) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_APN_DEF(&pDevConf->gprs.apn);
    }


    if (isAllSetDef == true || DevCfg_Verify_GPRS_Server(&pDevConf->gprs.server) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_GPRS_Server_DEF(&pDevConf->gprs.server);
    }

    if (isAllSetDef == true || DevCfg_Verify_GPRS_Time(&pDevConf->gprs.interval) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_GPRS_Time_DEF(&pDevConf->gprs.interval);
    }
    if (isAllSetDef == true || DevCfg_Verify_ContinueLoc_Time(&pDevConf->gprs.continue_loc) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_ContinueLoc_Time_DEF(&pDevConf->gprs.continue_loc);
    }
    if (isAllSetDef == true || DevCfg_Verify_GPRS_UserID(&pDevConf->gprs.userid) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_GPRS_UserID_DEF(&pDevConf->gprs.userid);
    }


    //Alert

    if (isAllSetDef == true || DevCfg_Verify_AlertPower(&pDevConf->alert.power) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_AlertPower_DEF(&pDevConf->alert.power);
    }
    for (i = 0; i < 4; i++)
    {
        if (isAllSetDef == true || DevCfg_Verify_AlertGEO(&(pDevConf->alert.geos[0])) == false)
        {
            ret = false;
            if (true == isSetDef) DevCfg_Verify_AlertGEO_DEF(&pDevConf->alert.geos[i], i);
        }
    }

    if (isAllSetDef == true || DevCfg_Verify_AlertMotion(&pDevConf->alert.motion) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_AlertMotion_DEF(&pDevConf->alert.motion);
    }


    if (isAllSetDef == true || DevCfg_Verify_AlertNoMotion(&pDevConf->alert.actionless) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_AlertNoMotion_DEF(&pDevConf->alert.actionless);
    }

    if (isAllSetDef == true || DevCfg_Verify_AlertSpeed(&pDevConf->alert.speed) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_AlertSpeed_DEF(&pDevConf->alert.speed);
    }

    if (isAllSetDef == true || DevCfg_Verify_AlertTilt(&pDevConf->alert.tilt) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_AlertTilt_DEF(&pDevConf->alert.tilt);
    }

    if (isAllSetDef == true || DevCfg_Verify_AlertFallDown(&pDevConf->alert.falldown) == false)
    {
        ret = false;
        if (true == isSetDef) DevCfg_Verify_AlertFallDown_DEF(&pDevConf->alert.falldown);
    }
    if (isAllSetDef == true)
    {
        return true;
    }
    return ret;
}

static bool DevConf_VaildAndGetHead(CONF_FILE_TYPE_ENUM type, msg_packet_t *pHead)
{
    uint8_t *pdest = HalParam_ReadAddr(type);
    if (pdest == NULL)
    {
        return false;
    }
    memcpy((uint8_t *)pHead, pdest, sizeof(msg_packet_t));
    if (pHead->magic != MAGIC_NUMBER) return false;
    if (pHead->len  > (MAX_PARAM - 9)) return false;
    uint16_t crc = crc16_compute(pdest + 8, pHead->len, 0);
    if (crc != pHead->crc) return false;
    return true;
}


static bool DevConf_GetAndVaildGetHead(CONF_FILE_TYPE_ENUM type, msg_packet_t *pHead)
{
    uint8_t *pdest = HalParam_ReadAddr(type);
    if (pdest == NULL)
    {
        return false;
    }
    memcpy((uint8_t *)pHead, pdest, sizeof(msg_packet_t));
    if (pHead->magic != MAGIC_NUMBER) return false;
    if (pHead->len  > (MAX_PARAM - 9)) return false;
    uint16_t crc = crc16_compute(pdest + 8, pHead->len, 0);
    if (crc != pHead->crc) return false;
    dfw_settings_update(pdest + 9, pHead->len - 1, false);
    return true;
}

static bool DevConf_GetAndVaild(CONF_FILE_TYPE_ENUM type)
{
    msg_packet_t pHead;
    return DevConf_GetAndVaildGetHead(type, &pHead);
}




void dev_conf_save_profile(void)
{

#if 0
    //要检测
    DevConf_IsVerify(&dev_settings, true, false);
    //读所有的配置到Flash当中。
    if (sg_sDevSaveInfo.pData)
    {
        APP_FREE(sg_sDevSaveInfo.pData);
    }
    sg_sDevSaveInfo.pData = (uint8_t *)APP_MALLOC(PROTO_READ_CONFIG_SETTING, MAX_PARAM);
    if (sg_sDevSaveInfo.pData == NULL)
    {
        return;
    }
    memset(sg_sDevSaveInfo.pData, 0, MAX_PARAM);
    uint32_t len = dev_config_read(NULL, 0, sg_sDevSaveInfo.pData + 9, MAX_PARAM);
    sg_sDevSaveInfo.pData[8] = COMMAND_ID_CONFIG;


    len = ProtoPacking(NULL, sg_sDevSaveInfo.pData + 8, len, 0, sg_sDevSaveInfo.pData);
    HalParam_Write(CONF_FILE_NORMAL, (uint32_t *)sg_sDevSaveInfo.pData, MAX_PARAM);
#endif
    sg_sDevSaveInfo.isNewData = true;
    //DevSave_Start();
}


static void dev_conf_update_from_factory_default(void)
{
    DevConf_IsVerify(&dev_settings, true, true);
}




void dev_conf_reset_to_factory(void)
{
    if (DevConf_GetAndVaild(CONF_FILE_FACTORY))
    {

    }
    else
    {
        //恢复默认设置
        dev_conf_update_from_factory_default();
    }
    dev_conf_save_profile();
}


void WriteCompleted(CONF_FILE_TYPE_ENUM type, int32_t reslut)
{
    switch (type)
    {
    case CONF_FILE_NORMAL:
        {
            sg_sDevSaveInfo.isSaveCompleted = true;
            sg_sDevSaveInfo.ret = reslut;
            break;
        }
    case CONF_FILE_BACKUP:
        {
            sg_sDevSaveInfo.isSaveCompleted = true;
            sg_sDevSaveInfo.ret = reslut;
            break;
        }
    }


}





void dev_conf_init(dev_conf_init_t *p_conf_init)
{
    if (p_conf_init == NULL)
    {
        return;
    }


    m_dev_cb = p_conf_init->cb;

    HalParam_Init(WriteCompleted);


    if (DevConf_GetAndVaild(CONF_FILE_NORMAL))
    {
        //DEV_LOG("CONF_FILE_NORMAL Get ok\r\n");
    }

    else if (DevConf_GetAndVaild(CONF_FILE_BACKUP))
    {

        //DEV_LOG("CONF_FILE_BACKUP Get ok\r\n");
    }
    else if (DevConf_GetAndVaild(CONF_FILE_FACTORY))
    {

       //DEV_LOG("CONF_FILE_FACTORY Get ok\r\n");
    }
    else
    {
        //恢复默认设置
        dev_conf_update_from_factory_default();
    }
    //要检测
    DevConf_IsVerify(&dev_settings, true, false);

    alarms_init_t alarms_obj;
    alarms_obj.cb        = p_conf_init->clock_active_handle;
    alarms_obj.clocks[0] = &dev_settings.system.alarms[0];
    alarms_obj.clocks[1] = &dev_settings.system.alarms[1];
    alarms_obj.clocks[2] = &dev_settings.system.alarms[2];
    alarms_obj.clocks[3] = &dev_settings.system.alarms[3];
    alarmclock_init(&alarms_obj);

    //更新所有KEY的参数。
    for (int i = 0; i < 255; i++)
    {
        m_dev_cb(i, NULL);
    }
}






bool dev_conf_volume_speaker_inc(void)
{
    uint32_t old = dev_settings.system.volume.speaker;
    if (old == SYS_VOLUME_SPEADKER_MAX) return false;
    old += 10;
    if (old > SYS_VOLUME_SPEADKER_MAX)
    {
        old = SYS_VOLUME_SPEADKER_MAX;
    }
    if (old != dev_settings.system.volume.speaker)
    {
        dev_settings.system.volume.speaker = old;
        return true;
    }
    return false;
}



bool dev_conf_volume_speaker_dec(void)
{
    if (dev_settings.system.volume.speaker == 0) return false;

    if (dev_settings.system.volume.speaker < 10)
    {
        dev_settings.system.volume.speaker = 0;
    }
    else dev_settings.system.volume.speaker -= 10;

    return true;
}


bool dev_conf_volume_mic_inc(void)
{
    uint32_t old = dev_settings.system.volume.mic;
    if (old == SYS_VOLUME_MIC_MAX) return false;
    old += 1;
    if (old > SYS_VOLUME_MIC_MAX)
    {
        old = SYS_VOLUME_MIC_MAX;
    }
    if (old != dev_settings.system.volume.mic)
    {
        dev_settings.system.volume.mic = old;
        return true;
    }
    return false;
}

bool dev_conf_volume_mic_dec(void)
{

    uint32_t old = dev_settings.system.volume.mic;

    if (old == 0) return false;

    if (old <= 0)
    {
        old = 0;
    }
    else old -= 1;
    if (old != dev_settings.system.volume.mic)
    {
        dev_settings.system.volume.mic = old;
        return true;
    }
    return false;
}








