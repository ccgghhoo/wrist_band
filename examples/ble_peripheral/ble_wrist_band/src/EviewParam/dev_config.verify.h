#ifndef __DEV_CONFIG_VERIFY_H
#define __DEV_CONFIG_VERIFY_H
#include "config.h"
#include "dev_config.factory.h"
#include "Libconfig.h"
#include "dev_conf_defines.h"

/**
 * System
 */
///initMileage
#define SYS_INIT_MILEAGE_DEF 0

static  bool DevCfg_Verify_InitMileage(uint32_t *pValue)
{
    if (pValue == NULL) return false;
    return true;
}
static  bool DevCfg_Verify_InitMileage_DEF(uint32_t *pValue)
{
    if (pValue == NULL) return false;
    *pValue = SYS_INIT_MILEAGE_DEF;
    return true;
}

//Work Mode
#define SYS_MODES_MODE_MIN  1
#define SYS_MODES_MODE_MAX  5

#ifndef SYSTEM_MODE_MODE
#define SYS_MODES_MODE_DEF  2
#else
#define SYS_MODES_MODE_DEF SYSTEM_MODE_MODE
#endif

#define SYS_MODES_4_TIME_MIN (1*60)
#define SYS_MODES_4_TIME_MAX (7*24*60*60)
#define SYS_MODES_4_TIME_DEF (60*60)

#define SYS_MODES_5_TIME_MIN (20*60)
#define SYS_MODES_5_TIME_MAX (7*24*60*60)

static  bool DevCfg_Verify_Mode(sys_mode_t *pValue)
{
    if (pValue == NULL) return false;
    if (pValue->mode > SYS_MODES_MODE_MAX || pValue->mode < SYS_MODES_MODE_MIN)
    {
        return false;
    }
    if (pValue->time == 0)
    {
        return true;
    }
    if (pValue->mode == 4)
    {

        if (pValue->time > SYS_MODES_4_TIME_MAX ||
            pValue->time < SYS_MODES_4_TIME_MIN)
        {
            if (pValue->time != 0)
            {
                return false;
            }
        }
    }
    else if (pValue->mode == 5)
    {

        if (pValue->time > SYS_MODES_5_TIME_MAX ||
            pValue->time < SYS_MODES_5_TIME_MIN)
        {
            if (pValue->time != 0)
            {
                return false;
            }
        }
    }
    return true;
}


static  bool DevCfg_Verify_Mode_DEF(sys_mode_t *pValue)
{
    if (pValue == NULL) return false;
    pValue->mode = SYS_MODES_MODE_DEF;
    pValue->time = 0;
    return true;
}


///Alarm Clock
#define SYS_ALARM_ENABLE_DEF false
#define SYS_ALARM_HOUR_DEF  (0)
#define SYS_ALARM_MIN_DEF  (0)
#define SYS_ALARM_WORK_DAY_DEF  (0)

#define SYS_ALARM_INDEX_MIN 0
#define SYS_ALARM_INDEX_MAX 3



#define SYS_ALARM_TIME_MIN 1
#define SYS_ALARM_TIME_MAX 120
#define SYS_ALARM_TIME_DEF 30


#define SYS_ALARM_RING_MIN 1
#define SYS_ALARM_RING_MAX 10
#define SYS_ALARM_RING_DEF 1

static  bool DevCfg_Verify_Alarm(alarm_clock_t *pValue)
{
    if (pValue == NULL) return false;
    if (pValue->index > SYS_ALARM_INDEX_MAX //  ||      pValue->index  < SYS_ALARM_INDEX_MIN
       ) return false;
    if (pValue->hour > 24) return false;
    if (pValue->minute > 60) return false;
    if (pValue->ring_time > SYS_ALARM_TIME_MAX || pValue->ring_time  < SYS_ALARM_TIME_MIN) return false;
    if (pValue->ring_type > SYS_ALARM_RING_MAX || pValue->ring_type  < SYS_ALARM_RING_MIN) return false;
    return true;
}
static  bool DevCfg_Verify_Alarm_DEF(alarm_clock_t *pValue, int32_t index)
{
    if (pValue == NULL) return false;
    if (index > SYS_ALARM_INDEX_MAX || index < SYS_ALARM_INDEX_MIN) return false;
    pValue->index = index;
    pValue->enable = SYS_ALARM_ENABLE_DEF;
    pValue->hour = SYS_ALARM_HOUR_DEF;
    pValue->minute = SYS_ALARM_MIN_DEF;
    pValue->ring_time = SYS_ALARM_TIME_DEF;
    pValue->ring_type = SYS_ALARM_RING_DEF;
    pValue->weekday = SYS_ALARM_WORK_DAY_DEF;
    return true;
}
//NO Disturb
#define SYS_NO_DISTURB_ENABLE_DEF SYSTEM_NO_DISTURB_TIME_ENABLE




static  bool DevCfg_Verify_NoDisturb(system_no_disturb_t *pValue)
{
    if (pValue == NULL) return false;
    if (pValue->start_hour > 24) return false;
    if (pValue->end_hour > 24) return false;
    if (pValue->start_minute > 60) return false;
    if (pValue->end_minute > 60) return false;

    return true;
}
static  bool DevCfg_Verify_NoDisturb_DEF(system_no_disturb_t *pValue)
{
    if (pValue == NULL) return false;

    pValue->enable = SYS_NO_DISTURB_ENABLE_DEF;
    pValue->start_hour = SYSTEM_NO_DISTURB_TIME_START_HOUR;
    pValue->start_minute = SYSTEM_NO_DISTURB_TIME_START_MINUTE;
    pValue->end_hour = SYSTEM_NO_DISTURB_TIME_END_HOUR;
    pValue->start_minute = SYSTEM_NO_DISTURB_TIME_END_MINUTE;

    return true;
}

//password
#define SYS_PASS_WORD_ENABLE_DEF SYSTEM_PASSWORD_ENABLE

#define SYS_PASS_WORD_MIN 0
#define SYS_PASS_WORD_MAX 999999
#define SYS_PASS_WORD_DEF SYSTEM_PASSWORD_VALUE

static  bool DevCfg_Verify_Password(sys_password_t *pValue)
{
    if (pValue == NULL) return false;
    if (pValue->number > SYS_PASS_WORD_MAX
       //|| pValue->number < SYS_PASS_WORD_MIN
       ) return false;

    return true;
}
static  bool DevCfg_Verify_Password_DEF(sys_password_t *pValue)
{
    if (pValue == NULL) return false;

    pValue->enable = SYS_PASS_WORD_ENABLE_DEF;
    pValue->number = SYS_PASS_WORD_DEF;
    return true;
}


//time Zone
#define SYS_TIMEZONE_OFFSET_DEF  SYSTEM_TIME_ZONE_VALUE
#define SYS_TIMEZONE_OFFSET_MAX 	(56)
#define SYS_TIMEZONE_OFFSET_MIN  -(48)

static  bool DevCfg_Verify_Timezone(sys_time_zone_t *pValue)
{
    if (pValue == NULL) return false;

    if (pValue->offset > SYS_TIMEZONE_OFFSET_MAX || pValue->offset < SYS_TIMEZONE_OFFSET_MIN) return false;

    return true;
}
static  bool DevCfg_Verify_Timezone_DEF(sys_time_zone_t *pValue)
{
    if (pValue == NULL) return false;

    pValue->offset = SYS_TIMEZONE_OFFSET_DEF;
    return true;
}


//Enable Control
#define SYS_ENABLE_CONTROL_ENABLE_APGS_DEF SYSTEM_AGPS_ENABLE
#define SYS_ENABLE_CONTROL_ENABLE_AUTO_UPDATE_DEF SYSTEM_AUTO_UPDATE_ENABLE
#define SYS_ENABLE_CONTROL_ENABLE_WIFI_DEF SYSTEM_WIFI_ENABLE
#define SYS_ENABLE_CONTROL_ENABLE_BLE_LOC_DEF SYSTEM_BLE_LOC_ENABLE
#define SYS_ENABLE_CONTROL_ENABLE_SOS_SPEAKER_DEF SYSTEM_SOS_SPEAKER_ENABLE
#define SYS_ENABLE_CONTROL_ENABLE_X_SPEAKER_DEF SYSTEM_X_SPEAKER_ENABLE
#define SYS_ENABLE_CONTROL_ENABLE_GSM_DEF SYSTEM_LBS_ENABLE
#define SYS_ENABLE_CONTROL_ENABLE_MOTO_DEF SYSTEM_MOTOR_ENABLE
#define SYS_ENABLE_CONTROL_ENABLE_BEEP_DEF SYSTEM_BUZZER_ENABLE
#define SYS_ENABLE_CONTROL_ENABLE_LED_DEF SYSTEM_LEDS_ENABLE



static  bool DevCfg_Verify_Switchs(sys_switchs_t *pValue)
{
    if (pValue == NULL) return false;
    return true;
}
static  bool DevCfg_Verify_Switchs_DEF(sys_switchs_t *pValue)
{
    if (pValue == NULL) return false;
    pValue->AGPS = SYS_ENABLE_CONTROL_ENABLE_APGS_DEF;
    pValue->autoUpdate = SYS_ENABLE_CONTROL_ENABLE_AUTO_UPDATE_DEF;

    pValue->GSMLoc = SYS_ENABLE_CONTROL_ENABLE_GSM_DEF;
    pValue->WifiLoc = SYS_ENABLE_CONTROL_ENABLE_WIFI_DEF;
    pValue->SosSpeaker = SYS_ENABLE_CONTROL_ENABLE_SOS_SPEAKER_DEF;
    pValue->XSpeaker = SYS_ENABLE_CONTROL_ENABLE_X_SPEAKER_DEF;
    pValue->buzzer = SYS_ENABLE_CONTROL_ENABLE_BEEP_DEF;
    pValue->leds = SYS_ENABLE_CONTROL_ENABLE_LED_DEF;
    pValue->motor = SYS_ENABLE_CONTROL_ENABLE_MOTO_DEF;
    pValue->BLEOftenConnection = SYSTEM_BLE_LONG_CONNTION_ENABLE;
    pValue->BLELoc = SYS_ENABLE_CONTROL_ENABLE_BLE_LOC_DEF;

    return true;
}

//Ring-Tone Volume
#define SYS_VOLUME_RING_TONE_MAX  SYSTEM_VOLUME_RINGTONE_MAX
#define SYS_VOLUME_RING_TONE_MIN  SYSTEM_VOLUME_RINGTONE_MIN
#define SYS_VOLUME_RING_TONE_DEF  SYSTEM_VOLUME_RINGTONE_LEVEL

static  bool DevCfg_Verify_RingToneVolume(uint8_t *pValue)
{
    if (pValue == NULL) return false;
#if 0
    if (
        //*pValue < SYS_VOLUME_RING_TONE_MIN ||
        *pValue > SYS_VOLUME_RING_TONE_MAX) return false;
#else
    if (*pValue > SYS_VOLUME_RING_TONE_MAX)
    {
        *pValue = SYS_VOLUME_RING_TONE_MAX;
    }
#endif
    return true;
}
static  bool DevCfg_Verify_RingToneVolume_DEF(uint8_t *pValue)
{
    if (pValue == NULL) return false;

    *pValue = SYS_VOLUME_RING_TONE_DEF;
    return true;
}

//Mic Volume
#define SYS_VOLUME_MIC_MIN  SYSTEM_VOLUME_MIC_MIN
#define SYS_VOLUME_MIC_MAX  SYSTEM_VOLUME_MIC_MAX
#define SYS_VOLUME_MIC_DEF  SYSTEM_VOLUME_MIC_LEVEL
static  bool DevCfg_Verify_MicVolume(uint8_t *pValue)
{
    if (pValue == NULL) return false;
    if (
        //*pValue < SYS_VOLUME_MIC_MIN ||
        *pValue > SYS_VOLUME_MIC_MAX) return false;

    return true;
}
static  bool DevCfg_Verify_MicVolume_DEF(uint8_t *pValue)
{
    if (pValue == NULL) return false;

    *pValue = SYS_VOLUME_MIC_DEF;
    return true;
}

//Speak Volume
#define SYS_VOLUME_SPEADKER_MIN  SYSTEM_VOLUME_SPEAKER_MIN
#define SYS_VOLUME_SPEADKER_MAX  SYSTEM_VOLUME_SPEAKER_MAX
#define SYS_VOLUME_SPEADKER_DEF  SYSTEM_VOLUME_SPEAKER_LEVEL


static  bool DevCfg_Verify_SpeakerVolume(uint8_t *pValue)
{
    if (pValue == NULL) return false;
#if 0

    if (
        //*pValue < SYS_VOLUME_SPEADKER_MIN ||
        *pValue > SYS_VOLUME_SPEADKER_MAX) return false;
#else
    if (*pValue > SYS_VOLUME_SPEADKER_MAX)
    {
        *pValue = SYS_VOLUME_SPEADKER_MAX;
    }
#endif

    return true;
}
static  bool DevCfg_Verify_SpeakerVolume_DEF(uint8_t *pValue)
{
    if (pValue == NULL) return false;

    *pValue = SYS_VOLUME_SPEADKER_DEF;
    return true;
}
#define SYS_MUSIC_ONOFF_DEF  SYSTEM_MUSIC_SWITCH


static  bool DevCfg_Verify_MusicSwitch_DEF(sys_musicswitch_t *pValue)
{
    pValue->value =  SYS_MUSIC_ONOFF_DEF;
    return true;
}





/**
//BUTTON
*/
//BUTTON
#define SYS_BUTTON_FEEDBACK_MIN  (0)
#define SYS_BUTTON_FEEDBACK_MAX  (3)
#define SYS_BUTTON_FEEDBACK_DEF  3



#ifndef SYS_BUTTON_TIME_MIN
#define SYS_BUTTON_TIME_MIN  (10)
#define SYS_BUTTON_TIME_MAX  (100)
#define SYS_BUTTON_TIME_DEF   20
#endif

#define SYS_BUTTON_TASK_MIN  (0)
#define SYS_BUTTON_TASK_MAX  (15)
#define SYS_BUTTON_TASK_DEF  15

#define SYS_BUTTON_MODE_MIN  (0)
#define SYS_BUTTON_MODE_MAX 	(1)
#define SYS_BUTTON_MODE_DEF  0


#define SYS_BUTTON_ENABLE_DEF true
static  bool DevCfg_Verify_Button(button_operation_t *pValue)
{
    if (pValue == NULL) return false;
    if (pValue->triggerMode > SYS_BUTTON_MODE_MAX
       //|| pValue->triggerMode < SYS_BUTTON_MODE_MIN
       ) return false;
    // if (pValue->task > SYS_BUTTON_TASK_MAX || pValue->triggerMode < SYS_BUTTON_TASK_MIN) return false;
    if (pValue->task != SYS_BUTTON_TASK_MAX && pValue->task > 10) return false;
    if (pValue->time > SYS_BUTTON_TIME_MAX
       //|| pValue->time < SYS_BUTTON_TIME_MIN
       ) return false;
    if (pValue->feedback > SYS_BUTTON_FEEDBACK_MAX
       // || pValue->feedback < SYS_BUTTON_FEEDBACK_MIN
       ) return false;

    return true;
}
static  bool DevCfg_Verify_Button_DEF(button_operation_t *pValue, uint8_t index)
{
    if (pValue == NULL) return false;
    switch (index)
    {
    case 0:
        {
            pValue->enable = BUTTONS_SOS_ENABLE;
            pValue->triggerMode = BUTTONS_SOS_TRIGGER_MODE;
            pValue->task = BUTTONS_SOS_TASK;
            pValue->time = BUTTONS_SOS_TIME;
            pValue->feedback = BUTTONS_SOS_FEEDBACK;
            break;
        }
    case 1:
        {
            pValue->enable = BUTTONS_CALL_1_ENABLE;
            pValue->triggerMode = BUTTONS_CALL_1_TRIGGER_MODE;
            pValue->task = BUTTONS_CALL_1_TASK;
            pValue->time = BUTTONS_CALL_1_TIME;
            pValue->feedback = BUTTONS_CALL_1_FEEDBACK;
            break;
        }
    case 2:
        {
            pValue->enable = BUTTONS_CALL_2_ENABLE;
            pValue->triggerMode = BUTTONS_CALL_2_TRIGGER_MODE;
            pValue->task = BUTTONS_CALL_2_TASK;
            pValue->time = BUTTONS_CALL_2_TIME;
            pValue->feedback = BUTTONS_CALL_2_FEEDBACK;
            break;
        }
    }
    return true;
}





/**?
//phone
*/

//以前是从1开始。
#define SYS_PHONE_INDEX_MIN  (0)
#define SYS_PHONE_INDEX_MAX 	(9)
#define SYS_PHONE_INDEX_DEF  0

#define SYS_PHONE_NUMBER_LEN_MAX 19
#define SYS_PHONE_NUMBER_LEN_MIN 1


#define PHONE_CONTACTS_NUM			(SYS_PHONE_INDEX_MAX+1)
#define PHONE_CONTACT_NO_MIN			SYS_PHONE_INDEX_MIN
#define PHONE_CONTACT_NO_MAX			SYS_PHONE_INDEX_MAX
#define PHONE_CONTACT_LEN_MIN			SYS_PHONE_NUMBER_LEN_MIN
#define PHONE_CONTACT_LEN_MAX 			SYS_PHONE_NUMBER_LEN_MAX

static bool DevCfg_Verify_PhoneNumber(char const *number)
{
    if (number == NULL)
    {
        return false;
    }
    uint8_t len = strlen((char const *)number);

    if (len > SYS_PHONE_NUMBER_LEN_MAX)
    {
        return false;
    }
    for (int i = 0; i < len; i++)
    {
        if (i == 0)
        {
            if (number[i] == '+')
            {
                continue;
            }
        }
        if (number[i] > '9' || number[i] < '0')
        {
            return false;
        }
    }
    return true;
}

static bool DevCfg_Verify_PhoneContact(phone_contact_t *pValue, uint8_t index)
{
    if (pValue == NULL) return false;
    if ( //pValue->idx < SYS_PHONE_INDEX_MIN ||
        pValue->idx > SYS_PHONE_INDEX_MAX) return false;
    if (pValue->idx != index) return false;
    return DevCfg_Verify_PhoneNumber(pValue->number);
}


static  bool DevCfg_Verify_PhoneContact_DEF(phone_contact_t *pValue, uint8_t index)
{
    if (pValue == NULL) return false;

    if (
        //index < SYS_PHONE_INDEX_MIN ||
        index > SYS_PHONE_INDEX_MAX) return false;

    pValue->idx = index;
    memset(pValue->number, 0, SYS_PHONE_NUMBER_LEN_MAX);
    pValue->enable = false;
    pValue->phone = 0;
    pValue->sms = 0;
    pValue->no_card = 0;
    switch (index)
    {
    case 0:
        {
            pValue->enable = AUTH_CONTACT_1ST_ENABLE;
#if AUTH_CONTACT_1ST_ENABLE
            pValue->phone = AUTH_CONTACT_1ST_ACCEPT_PHONE;
            pValue->sms = AUTH_CONTACT_1ST_ACCEPT_SMS;
            pValue->no_card = AUTH_CONTACT_1ST_NO_CARD;
            memcpy(pValue->number, AUTH_CONTACT_1ST_PHONE_NUMBER, strlen(AUTH_CONTACT_1ST_PHONE_NUMBER));
#endif
            break;
        }
    case 1:
        {
            pValue->enable = AUTH_CONTACT_2ND_ENABLE;
#if AUTH_CONTACT_2ND_ENABLE
            pValue->phone = AUTH_CONTACT_2ND_ACCEPT_PHONE;
            pValue->sms = AUTH_CONTACT_2ND_ACCEPT_SMS;
            pValue->no_card = AUTH_CONTACT_2ND_NO_CARD;
            memcpy(pValue->number, AUTH_CONTACT_2ND_PHONE_NUMBER, strlen(AUTH_CONTACT_2ND_PHONE_NUMBER));
#endif
            break;
        }

    default:

        break;
    }

    return true;
}

//SMS Repley Prefix Text
#define SYS_PHONE_SMS_REPLY_LEN_MAX  (PREFIX_MAX)

#if SMS_PREFIX_TEXT_ENABLE
#define SYS_PHONE_SMS_REPLY_DEF   SMS_PREFIX_TEXT_STRING
#else
#define SYS_PHONE_SMS_REPLY_DEF ""
#endif

#define PHONE_SMS_PREFIX_LEN_MAX		SYS_PHONE_SMS_REPLY_LEN_MAX

static  bool DevCfg_Verify_PhoneSMSRepleyPrefix(phone_sms_prefix_t *pValue)
{
    if (pValue == NULL) return false;
    if (pValue->size > SYS_PHONE_SMS_REPLY_LEN_MAX)
    {
        return false;
    }
    return true;
}

static  bool DevCfg_Verify_PhoneSMSRepleyPrefix_DEF(phone_sms_prefix_t *pValue)
{
    if (pValue == NULL) return false;
    pValue->enable = SMS_PREFIX_TEXT_ENABLE;
    uint8_t len = strlen(SYS_PHONE_SMS_REPLY_DEF);
    if (len > SYS_PHONE_SMS_REPLY_LEN_MAX) return false;
    pValue->size =  len;
    memset(pValue->text, 0, SYS_PHONE_SMS_REPLY_LEN_MAX);
    memcpy(pValue->text, SYS_PHONE_SMS_REPLY_DEF, len);
    return true;
}
//SOS
#define SYS_PHONE_HOND_TIME_MIN  (0)
#define SYS_PHONE_HOND_TIME_MAX 	(0XFFFF)
#define SYS_PHONE_HOND_TIME_DEF  SOS_HOLDING_TIME_ALLOW

#define SYS_PHONE_RING_TIME_MIN  (1)
#define SYS_PHONE_RING_TIME_MAX 	(60)
#define SYS_PHONE_RING_TIME_DEF  SOS_RING_TIME_ALLOW


#define SYS_PHONE_CYCLE_MIN  (0)
#define SYS_PHONE_CYCLE_MAX 	(10)
#define SYS_PHONE_CYCLE_DEF  SOS_CALL_LOOP_ALLOW



#define PHONE_SOS_DATA_SIZE				4
#define PHONE_SOS_HOLDING_TIME_MIN		SYS_PHONE_HOND_TIME_MIN //unit: second
#define PHONE_SOS_HOLDING_TIME_MAX 		(SYS_PHONE_HOND_TIME_MAX)// 5 hours
#define PHONE_SOS_RING_TIME_MIN			SYS_PHONE_RING_TIME_MIN
#define PHONE_SOS_RING_TIME_MAX 		SYS_PHONE_RING_TIME_MAX
#define PHONE_SOS_LOOP_TIMES_MAX		SYS_PHONE_CYCLE_MAX


static  bool DevCfg_Verify_PhoneSOSOption(phone_sos_t *pValue)
{
    if (pValue == NULL) return false;
    //if (pValue->holding_time > SYS_PHONE_HOND_TIME_MAX ||pValue->holding_time < SYS_PHONE_HOND_TIME_MIN )return false;

    if (pValue->rings_time > SYS_PHONE_RING_TIME_MAX || pValue->rings_time < SYS_PHONE_RING_TIME_MIN) return false;
    if (pValue->loops > SYS_PHONE_CYCLE_MAX // || pValue->loops < SYS_PHONE_CYCLE_MIN
       ) return false;

    return true;
}

static  bool DevCfg_Verify_PhoneSOSOption_DEF(phone_sos_t *pValue)
{
    if (pValue == NULL) return false;
    pValue->holding_time = SYS_PHONE_HOND_TIME_DEF;
    pValue->loops = SYS_PHONE_CYCLE_DEF;
    pValue->rings_time = SYS_PHONE_RING_TIME_DEF;

    return true;
}


#define SYS_PHONE_SWITCH_ONLY_AUTH_SMS_DEF    SMS_COMMAND_ONLY_AUTH_CONTACT
#define SYS_PHONE_SWITCH_LISTEN_IN_DEF        MONITOR_ENVIRONMENT_ENABLE
#define SYS_PHONE_SWITCH_HANG_UP_DEF          HANGUP_PHONE_CALL_ENABLE
#define SYS_PHONE_SWITCH_ONLY_AUTH_CALL_DEF   INTERCEPT_PHONE_CALL_ENABLE
#define SYS_PHONE_SWITCH_AUTO_ANSWER_DEF      AUTO_ANWER_INCOMING_CALL_ENABLE

#define SYS_PHONE_SWITCH_ANSWER_AFTER_MIN    (0)
#define SYS_PHONE_SWITCH_ANSWER_AFTER_MAX    (25)
#define SYS_PHONE_SWITCH_ANSWER_AFTER_DEF     AUTO_ANSWER_RING_COUNTS


#define PHONE_SWITCHES_DATA_SIZE 		4
#define PHONE_AUTO_ANSWER_RINGS_MAX 	SYS_PHONE_SWITCH_ANSWER_AFTER_MAX


static  bool DevCfg_Verify_PhoneSwitch(phone_swithes_t *pValue)
{
    if (pValue == NULL) return false;

    if (pValue->auto_answer_rings_delay > SYS_PHONE_SWITCH_ANSWER_AFTER_MAX
       // || pValue->auto_answer_rings_delay< SYS_PHONE_SWITCH_ANSWER_AFTER_MIN)
       ) return false;

    return true;
}
static  bool DevCfg_Verify_PhoneSwitch_DEF(phone_swithes_t *pValue)
{
    if (pValue == NULL) return false;


    pValue->auto_answer_enable = SYS_PHONE_SWITCH_AUTO_ANSWER_DEF;
    pValue->auto_answer_rings_delay = SYS_PHONE_SWITCH_ANSWER_AFTER_DEF;
    pValue->hangup_manual_not_allow = SYS_PHONE_SWITCH_HANG_UP_DEF;
    pValue->monitor_environment_enable = SYS_PHONE_SWITCH_LISTEN_IN_DEF;
    pValue->sms_set_only_by_auth_number = SYS_PHONE_SWITCH_ONLY_AUTH_SMS_DEF;
    pValue->call_set_only_by_auth_number = SYS_PHONE_SWITCH_ONLY_AUTH_CALL_DEF;
    return true;
}


/**
GPRS
*/
#define SYS_GPRS_APN_LEN_MAX 31
#define SYS_GPRS_APN_USERNAME_LEN_MAX 15
#define SYS_GPRS_APN_PASSWORD_LEN_MAX 15
#define SYS_GPRS_APN_ENABLE_DEF GPRS_APN_ENABLE



#define GPRS_APN_APN_LEN_MAX	       SYS_GPRS_APN_LEN_MAX
#define GPRS_APN_USERNAME_LEN_MAX    SYS_GPRS_APN_USERNAME_LEN_MAX
#define GPRS_APN_PASSWORD_LEN_MAX    SYS_GPRS_APN_PASSWORD_LEN_MAX


static  bool DevCfg_Verify_APN(gprs_apn_t *pValue)
{
    if (pValue == NULL) return false;
    if (pValue->apnLen > SYS_GPRS_APN_LEN_MAX) return false;
    if (pValue->passwordLen > SYS_GPRS_APN_PASSWORD_LEN_MAX) return false;
    if (pValue->userNameLen > SYS_GPRS_APN_USERNAME_LEN_MAX) return false;

    return true;
}
static  bool DevCfg_Verify_APN_DEF(gprs_apn_t *pValue)
{
    if (pValue == NULL) return false;

    pValue->enable = SYS_GPRS_APN_ENABLE_DEF;
    pValue->apnLen = 0;
    memset(pValue->apns, 0, SYS_GPRS_APN_LEN_MAX);
    memcpy(pValue->apns, GPRS_APN_NAME_STRING, strlen(GPRS_APN_NAME_STRING));
    pValue->apnLen = strlen(pValue->apns);

    pValue->passwordLen = 0;
    memset(pValue->password, 0, SYS_GPRS_APN_PASSWORD_LEN_MAX);
    memcpy(pValue->password, GPRS_APN_PASSWORD_STRING, strlen(GPRS_APN_PASSWORD_STRING));
    pValue->passwordLen = strlen(pValue->password);

    pValue->userNameLen = 0;
    memset(pValue->username, 0, SYS_GPRS_APN_USERNAME_LEN_MAX);
    memcpy(pValue->username, GPRS_APN_USER_NAME_STRING, strlen(GPRS_APN_USER_NAME_STRING));
    pValue->userNameLen = strlen(pValue->username);

    return true;
}

//server
#ifdef GPRS_ENABLE
#define SYS_GPRS_SERVER_ENABLE_DEF GPRS_ENABLE
#else
#define SYS_GPRS_SERVER_ENABLE_DEF false
#endif
#define SYS_GPRS_SERVER_IS_UDP_DEF false


#define SYS_GPRS_SERVER_PORT_MIN  (0)
#define SYS_GPRS_SERVER_PORT_MAX 	(0XFFFF)
#define SYS_GPRS_SERVER_PORT_DEF  GPRS_SERVER_PORT

#define SYS_GPRS_IP_LEN_MAX 29
#define SYS_GPRS_IP_DEF GPRS_SERVER_IP_STRING


#define GPRS_SERVER_IP_LEN_MIN 3
#define GPRS_SERVER_IP_LEN_MAX (SYS_GPRS_IP_LEN_MAX+GPRS_SERVER_IP_LEN_MIN)

static  bool DevCfg_Verify_GPRS_Server(gprs_server_t *pValue)
{
    if (pValue == NULL) return false;


    return true;
}
static  bool DevCfg_Verify_GPRS_Server_DEF(gprs_server_t *pValue)
{
    if (pValue == NULL) return false;
    uint32_t len = strlen(SYS_GPRS_IP_DEF);
    if (len > SYS_GPRS_IP_LEN_MAX) return false;

    pValue->enable = SYS_GPRS_SERVER_ENABLE_DEF;
    pValue->isUDP = SYS_GPRS_SERVER_IS_UDP_DEF;


    pValue->port = SYS_GPRS_SERVER_PORT_DEF;

    memset(pValue->ip, 0, SYS_GPRS_IP_LEN_MAX);
    memcpy(pValue->ip, SYS_GPRS_IP_DEF, len);
    return true;
}


#define SYS_GPRS_HEART_BEAT_ENABLE_DEF GPRS_HEARTBEAT_ENABLE
#define SYS_GPRS_HEART_BEAT_MIN  (10)
#define SYS_GPRS_HEART_BEAT_MAX 	(3600*24)
#define SYS_GPRS_HEART_BEAT_DEF  GPRS_HEARTBEAT_INTERVAL

#define SYS_GPRS_AUTO_UPLOAD_MIN  (10)
#define SYS_GPRS_AUTO_UPLOAD_MAX 	(0XFFFF)
#define SYS_GPRS_AUTO_UPLOAD_DEF  GPRS_UPLOAD_INTERVAL

#define SYS_GPRS_AUTO_UPLOAD_LAZY_MIN  (10)
#define SYS_GPRS_AUTO_UPLOAD_LAZY_MAX 	(0XFFFF)
#define SYS_GPRS_AUTO_UPLOAD_LAZY_DEF  GPRS_LAZY_UPLOAD_INTERVAL




#define GPRS_HEARTBEAT_INTERVAL_MIN		SYS_GPRS_HEART_BEAT_MIN // unit: second
#define GPRS_HEARTBEAT_INTERVAL_MAX 	(SYS_GPRS_HEART_BEAT_MAX) // half an  hour
#define GPRS_UPLOAD_INTERVAL_MIN		SYS_GPRS_AUTO_UPLOAD_MIN // unit: second
#define GPRS_UPLOAD_INTERVAL_MAX 		(SYS_GPRS_AUTO_UPLOAD_MAX) // 24 hour
#define GPRS_UPLOAD_LAZY_MIN			(SYS_GPRS_AUTO_UPLOAD_LAZY_MIN) // 10minute
#define GPRS_UPLOAD_LAZY_MAX 			(SYS_GPRS_AUTO_UPLOAD_LAZY_MAX)



static  bool DevCfg_Verify_GPRS_Time(gprs_time_t *pValue)
{
    if (pValue == NULL) return false;
    if (pValue->heartbeat > SYS_GPRS_HEART_BEAT_MAX
        || pValue->heartbeat < SYS_GPRS_HEART_BEAT_MIN) return false;
    if (pValue->upload > SYS_GPRS_AUTO_UPLOAD_MAX
        || pValue->upload < SYS_GPRS_AUTO_UPLOAD_MIN) return false;
    if (pValue->lazy_upload > SYS_GPRS_AUTO_UPLOAD_LAZY_MAX
        || pValue->lazy_upload < SYS_GPRS_AUTO_UPLOAD_LAZY_MIN) return false;
    return true;
}
static  bool DevCfg_Verify_GPRS_Time_DEF(gprs_time_t *pValue)
{
    if (pValue == NULL) return false;
    pValue->hb_enable = SYS_GPRS_HEART_BEAT_ENABLE_DEF;
    pValue->heartbeat = SYS_GPRS_HEART_BEAT_DEF;
    pValue->upload = SYS_GPRS_AUTO_UPLOAD_DEF;
    pValue->lazy_upload = SYS_GPRS_AUTO_UPLOAD_LAZY_DEF;
    return true;
}

#define SYS_GPRS_CONTINUE_LOC_INTERVAL_MIN  (10)
#define SYS_GPRS_CONTINUE_LOC_INTERVAL_MAX 	(10*60)
#define SYS_GPRS_CONTINUE_LOC_INTERVAL_DEF  (10)

#define SYS_GPRS_CONTINUE_LOC_TIME_MIN  (1*60)
#define SYS_GPRS_CONTINUE_LOC_TIME_MAX 	(30*60)
#define SYS_GPRS_CONTINUE_LOC_TIME_DEF  (10*60)


static  bool DevCfg_Verify_ContinueLoc_Time(gprs_continue_loc_t *pValue)
{
    if (pValue == NULL) return false;
    if (pValue->interval > SYS_GPRS_CONTINUE_LOC_INTERVAL_MAX
        || pValue->interval < SYS_GPRS_CONTINUE_LOC_INTERVAL_MIN) return false;
    if (pValue->time > SYS_GPRS_CONTINUE_LOC_TIME_MAX
        || pValue->time < SYS_GPRS_CONTINUE_LOC_TIME_MIN) return false;

    return true;
}
static  bool DevCfg_Verify_ContinueLoc_Time_DEF(gprs_continue_loc_t *pValue)
{
    if (pValue == NULL) return false;
    pValue->interval = SYS_GPRS_CONTINUE_LOC_INTERVAL_DEF;
    pValue->time = SYS_GPRS_CONTINUE_LOC_TIME_DEF;
    return true;
}


#define SYS_GPRS_GPRS_USER_ID          GPRS_USER_ID
#define SYS_GPRS_GPRS_USER_ID_LEN_DEF  (strlen(SYS_GPRS_GPRS_USER_ID))


static  bool DevCfg_Verify_GPRS_UserID(gprs_user_id_t *pValue)
{
    if (pValue == NULL) return false;
    if (pValue->len > SYS_GPRS_GPRS_USER_ID_MAX) return false;
    return true;
}
static  bool DevCfg_Verify_GPRS_UserID_DEF(gprs_user_id_t *pValue)
{
    if (pValue == NULL) return false;
    pValue->len = SYS_GPRS_GPRS_USER_ID_LEN_DEF;
    if (pValue->len > SYS_GPRS_GPRS_USER_ID_MAX) return false;
    memcpy(pValue->data, SYS_GPRS_GPRS_USER_ID, pValue->len);
    return true;
}


//ALERT
//POWER
#define SYS_ALERT_POWER_OFF_ALERT_DEF   POWER_OFF_ALERT_ENABLE
#define SYS_ALERT_POWER_ON_ALERT_DEF   	POWER_ON_ALERT_ENABLE
#define SYS_ALERT_POWER_LOW_DEF 	POWER_LOW_ALERT_ENABLE

#define SYS_ALERT_POWER_THRES_MIN  (10)
#define SYS_ALERT_POWER_THRES_MAX 	(100)
#define SYS_ALERT_POWER_THRES_DEF  POWER_LOW_ALERT_THRESHOLD

#define SYS_ALERT_POWER_LOW_VOICE_DEF 	POWER_LOW_VOICE_ENABLE
#define SYS_ALERT_POWER_VOICE_THRES_DEF  POWER_LOW_VOICE_THRESHOLD

#define ALERT_POWER_DATA_SIZE 			4
#define ALERT_POWER_LOW_LEVEL_MIN     SYS_ALERT_POWER_THRES_MIN
#define ALERT_POWER_LOW_LEVEL_MAX 	SYS_ALERT_POWER_THRES_MAX



static  bool DevCfg_Verify_AlertPower(alert_power_t *pValue)
{
    if (pValue == NULL) return false;
    if (pValue->low_voice_threshold < SYS_ALERT_POWER_THRES_MIN || pValue->low_voice_threshold > SYS_ALERT_POWER_THRES_MAX) return false;

    if (pValue->threshold < SYS_ALERT_POWER_THRES_MIN || pValue->threshold > SYS_ALERT_POWER_THRES_MAX) return false;

    return true;
}
static  bool DevCfg_Verify_AlertPower_DEF(alert_power_t *pValue)
{
    if (pValue == NULL) return false;
    pValue->on = SYS_ALERT_POWER_ON_ALERT_DEF;
    pValue->off = SYS_ALERT_POWER_OFF_ALERT_DEF;
    pValue->low_voice = SYS_ALERT_POWER_LOW_VOICE_DEF;
    pValue->low_voice_threshold = SYS_ALERT_POWER_VOICE_THRES_DEF;
    pValue->low = SYS_ALERT_POWER_LOW_DEF;
    pValue->threshold = SYS_ALERT_POWER_THRES_DEF;
    return true;
}



//GEO
#define SYS_ALERT_GEO_IS_POLYGON true
#define SYS_ALERT_GEO_ENABLE_DEF false
#define SYS_ALERT_GEO_IS_POLYGON_DEF !(SYS_ALERT_GEO_IS_POLYGON)
#define SYS_ALERT_GEO_IN_DIR_DEF false

#define SYS_ALERT_GEO_INDEX_MIN  (0)
#define SYS_ALERT_GEO_INDEX_MAX 	(3)
#define SYS_ALERT_GEO_INDEX_DEF  0

#define ALERT_GEO_DATA_SIZE_MIN			4
#define ALERT_GEO_DATA_SIZE_MAX 		(4 + (16*4))

//CIRCLE

#define SYS_ALERT_GEO_CIRCLE_RADIUS_MIN  (100)
#define SYS_ALERT_GEO_CIRCLE_RADIUS_MAX 	(0XFFFF)
#define SYS_ALERT_GEO_CIRCLE_RADIUS_DEF  500

#define SYS_ALERT_GEO_CIRCLE_POINTS_MIN  (1)
#define SYS_ALERT_GEO_CIRCLE_POINTS_MAX 	(1)
#define SYS_ALERT_GEO_CIRCLE_POINTS_DEF  1


//POLYGON
#define SYS_ALERT_GEO_POLYGON_POINTS_MIN  (3)
#define SYS_ALERT_GEO_POLYGON_POINTS_MAX 	(8)
#define SYS_ALERT_GEO_POLYGON_POINTS_DEF  3


#define ALERT_GEO_DATA_SIZE_MIN			4
#define ALERT_GEO_DATA_SIZE_MAX 		(4 + (16*4))
#define ALERT_GEO_START_INDEX			1
#define ALERT_GEO_END_INDEX				4

#define ALERT_GEO_INDEX_MASK			3
#define ALERT_GEO_RADIUS_MAX 			(50000)	// unit: meter
#define ALERT_GEO_RADIUS_MIN 			( 100 )
#define ALERT_GEO_CIRCLE_POINTS         ( 1 )
#define ALERT_GEO_FENCE_POINTS			( 8 )



static  bool DevCfg_Verify_AlertGEO(alert_geo_t *pValue)
{
    if (pValue == NULL) return false;

    if (pValue->idx > SYS_ALERT_GEO_INDEX_MAX
       //|| pValue->idx < SYS_ALERT_GEO_INDEX_MIN
       ) return false;
    //if (pValue->enable == false) return true;

    if (pValue->type != SYS_ALERT_GEO_IS_POLYGON)
    {
        //圆
        if (pValue->count > SYS_ALERT_GEO_CIRCLE_POINTS_MAX || pValue->count < SYS_ALERT_GEO_CIRCLE_POINTS_MIN) return false;

        if (
            // pValue->radius > SYS_ALERT_GEO_CIRCLE_RADIUS_MAX ||
            pValue->radius < SYS_ALERT_GEO_CIRCLE_RADIUS_MIN) return false;



    }
    else
    {
        //多连形

        if (pValue->count > SYS_ALERT_GEO_POLYGON_POINTS_MAX || pValue->count < SYS_ALERT_GEO_POLYGON_POINTS_MIN) return false;

    }

    return true;
}

static  bool DevCfg_Verify_AlertGEO_DEF(alert_geo_t *pValue, uint8_t index)
{
    if (pValue == NULL) return false;
    if (index > SYS_ALERT_GEO_INDEX_MAX
       //|| pValue->idx < SYS_ALERT_GEO_INDEX_MIN
       ) return false;
    switch (index)
    {
    case 0:
        {
            pValue->enable = GEO_FENCE_NO_0_ALERT_ENABLE;
            pValue->type = GEO_FENCE_NO_0_ALERT_TYPE;
            pValue->count = GEO_FENCE_NO_0_ALERT_POINT_COUNT;
            pValue->dir = GEO_FENCE_NO_0_ALERT_DIR;
            pValue->radius = GEO_FENCE_NO_0_RADIUS;
            pValue->loc[0].latitude = GEO_FENCE_NO_0_LATITUDE_0;
            pValue->loc[0].longitude = GEO_FENCE_NO_0_LONGITUDE_0;
            pValue->idx = index;
            break;
        }
    default:
        pValue->enable = SYS_ALERT_GEO_ENABLE_DEF;
        pValue->type = SYS_ALERT_GEO_IS_POLYGON_DEF;
        pValue->count = 1;
        pValue->dir = SYS_ALERT_GEO_IN_DIR_DEF;
        pValue->radius = SYS_ALERT_GEO_CIRCLE_RADIUS_DEF;
        pValue->loc[0].latitude = 0;
        pValue->loc[0].longitude = 0;
        pValue->idx = index;
        break;
    }
    return true;
}

//MOTION
#define SYS_ALERT_MOTION_ENABLE_DEF MOTION_ALERT_ENABLE

#define SYS_ALERT_MOTION_MOVE_TIME_MIN  (1)
#define SYS_ALERT_MOTION_MOVE_TIME_MAX 	(60)
#define SYS_ALERT_MOTION_MOVE_TIME_DEF  MOTION_ALERT_ACTION_TIME

#define SYS_ALERT_MOTION_SETUP_MIN  (60)
#define SYS_ALERT_MOTION_SETUP_MAX 	(36000)
#define SYS_ALERT_MOTION_SETUP_DEF  MOTION_ALERT_SETUP_TIME

#define ALERT_MOTION_DATA_SIZE 			4
#define ALERT_MOTION_SETUP_TIME_MIN		SYS_ALERT_MOTION_SETUP_MIN
#define ALERT_MOTION_SETUP_TIME_MAX 	(SYS_ALERT_MOTION_SETUP_MAX) // 10 minutes
#define ALERT_MOTION_ACTION_TIME_MIN	SYS_ALERT_MOTION_MOVE_TIME_MIN
#define ALERT_MOTION_ACTION_TIME_MAX 	(SYS_ALERT_MOTION_MOVE_TIME_MAX) //


static  bool DevCfg_Verify_AlertMotion(alert_motion_t *pValue)
{
    if (pValue == NULL) return false;
    // if (pValue->enable == false) return true;
    if (pValue->action_time > SYS_ALERT_MOTION_MOVE_TIME_MAX
        || pValue->action_time < SYS_ALERT_MOTION_MOVE_TIME_MIN) return false;
    if (pValue->setup_time > SYS_ALERT_MOTION_SETUP_MAX
        || pValue->setup_time < SYS_ALERT_MOTION_SETUP_MIN) return false;

    return true;
}
static  bool DevCfg_Verify_AlertMotion_DEF(alert_motion_t *pValue)
{
    if (pValue == NULL) return false;
    pValue->enable = SYS_ALERT_MOTION_ENABLE_DEF;
    pValue->action_time = SYS_ALERT_MOTION_MOVE_TIME_DEF;
    pValue->setup_time = SYS_ALERT_MOTION_SETUP_DEF;
    return true;
}



//NO MOTION
#define SYS_ALERT_NO_MOTION_ENABLE_DEF ACTIONLESS_ALERT_ENABLE

#define SYS_ALERT_NO_MOTION_MOVE_TIME_MIN  (60)
#define SYS_ALERT_NO_MOTION_MOVE_TIME_MAX 	(36000)
#define SYS_ALERT_NO_MOTION_MOVE_TIME_DEF  ACTIONLESS_ALERT_THRESHOLD


#define ALERT_STATIC_DATA_SIZE 			4
#define ALERT_STATIC_THRESHOLD_MIN		SYS_ALERT_NO_MOTION_MOVE_TIME_MIN
#define ALERT_STATIC_THRESHOLD_MAX		(SYS_ALERT_NO_MOTION_MOVE_TIME_MAX)


static  bool DevCfg_Verify_AlertNoMotion(alert_static_t *pValue)
{
    if (pValue == NULL) return false;
    // if (pValue->enable == false) return true;
    if (pValue->threshold > SYS_ALERT_NO_MOTION_MOVE_TIME_MAX
        || pValue->threshold < SYS_ALERT_NO_MOTION_MOVE_TIME_MIN) return false;

    return true;
}
static  bool DevCfg_Verify_AlertNoMotion_DEF(alert_static_t *pValue)
{
    if (pValue == NULL) return false;
    pValue->enable = SYS_ALERT_NO_MOTION_ENABLE_DEF;
    pValue->threshold = SYS_ALERT_NO_MOTION_MOVE_TIME_DEF;
    return true;
}


//Speed

#define SYS_ALERT_SPEED_ENABLE_DEF SPEED_ALERT_ENABLE

#define SYS_ALERT_SPEED_THRESHOLD_MIN  (20)
#define SYS_ALERT_SPEED_THRESHOLD_MAX 	(400)
#define SYS_ALERT_SPEED_THRESHOLD_DEF  SPEED_ALERT_THRESHOLD




#define ALERT_SPEED_DATA_SIZE 			2
#define ALERT_SPEED_THRESHOLD_MIN		(SYS_ALERT_SPEED_THRESHOLD_MIN)//20km/h
#define ALERT_SPEED_THRESHOLD_MAX 		(SYS_ALERT_SPEED_THRESHOLD_MAX) // 400km/h

static  bool DevCfg_Verify_AlertSpeed(alert_speed_t *pValue)
{
    if (pValue == NULL) return false;
    // if (pValue->enable == false) return true;

    if (pValue->threshold > SYS_ALERT_SPEED_THRESHOLD_MAX
        || pValue->threshold < SYS_ALERT_SPEED_THRESHOLD_MIN) return false;

    return true;
}
static  bool DevCfg_Verify_AlertSpeed_DEF(alert_speed_t *pValue)
{
    if (pValue == NULL) return false;
    pValue->enable = SYS_ALERT_SPEED_ENABLE_DEF;
    pValue->threshold = SYS_ALERT_SPEED_THRESHOLD_DEF;
    return true;
}
//tilt

#define SYS_ALERT_TILT_ENABLE_DEF TILT_ALERT_ENABLE

#define SYS_ALERT_TILT_ANGLE_MIN  (30)
#define SYS_ALERT_TILT_ANGLE_MAX 	(90)
#define SYS_ALERT_TILT_ANGLE_DEF  TILT_ALERT_ANGLE_THRESHOLD


#define SYS_ALERT_TILT_TIME_MIN  (10)
#define SYS_ALERT_TILT_TIME_MAX 	(3600)
#define SYS_ALERT_TILT_TIME_DEF  TILT_ALERT_TIME_THRESHOLD


#define ALERT_TILT_DATA_SIZE 			4
#define ALERT_TILT_ANGLE_MIN			SYS_ALERT_TILT_ANGLE_MIN
#define ALERT_TILT_ANGLE_MAX 			SYS_ALERT_TILT_ANGLE_MAX
#define ALERT_TILT_TIME_MIN			SYS_ALERT_TILT_TIME_MIN
#define ALERT_TILT_TIME_MAX 			SYS_ALERT_TILT_TIME_MAX


static  bool DevCfg_Verify_AlertTilt(alert_tilt_t *pValue)
{
    if (pValue == NULL) return false;
    // if (pValue->enable == false) return true;

    if (pValue->angle > SYS_ALERT_TILT_ANGLE_MAX
        || pValue->angle < SYS_ALERT_TILT_ANGLE_MIN
       ) return false;
    if (pValue->time > SYS_ALERT_TILT_TIME_MAX
        || pValue->time < SYS_ALERT_TILT_TIME_MIN) return false;

    return true;
}
static  bool DevCfg_Verify_AlertTilt_DEF(alert_tilt_t *pValue)
{
    if (pValue == NULL) return false;
    pValue->enable = SYS_ALERT_TILT_ENABLE_DEF;
    pValue->angle = SYS_ALERT_TILT_ANGLE_DEF;
    pValue->time = SYS_ALERT_TILT_TIME_DEF;
    return true;
}
//Fall down

#define SYS_ALERT_FALL_DOWN_ENABLE_DEF FALLDOWN_ALERT_ENABLE
#define SYS_ALERT_FALL_DOWN_DIAL_DEF 	FALLDOWN_ALERT_DIAL

#define SYS_ALERT_FALL_DOWN_LEVEL_TIME_MIN  (1)
#define SYS_ALERT_FALL_DOWN_LEVEL_TIME_MAX 	(10)
#define SYS_ALERT_FALL_DOWN_LEVEL_TIME_DEF  FALLDOWN_ALERT_LEVEL



#define ALERT_FALLDOWN_DATA_SIZE 		1

#define ALERT_FALLDOWN_LEVEL_MIN		SYS_ALERT_FALL_DOWN_LEVEL_TIME_MIN
#define ALERT_FALLDOWN_LEVEL_MAX		SYS_ALERT_FALL_DOWN_LEVEL_TIME_MAX
#define ALERT_FALLDOWN_LEVEL_DEF                1

static  bool DevCfg_Verify_AlertFallDown(alert_falldown_t *pValue)
{
    if (pValue == NULL) return false;
//  if (pValue->enable == false) return true;

    if (pValue->level > SYS_ALERT_FALL_DOWN_LEVEL_TIME_MAX
        || pValue->level < SYS_ALERT_FALL_DOWN_LEVEL_TIME_MIN) return false;

    return true;
}
static  bool DevCfg_Verify_AlertFallDown_DEF(alert_falldown_t *pValue)
{
    if (pValue == NULL) return false;
    pValue->enable = SYS_ALERT_FALL_DOWN_ENABLE_DEF;
    pValue->dial = SYS_ALERT_FALL_DOWN_DIAL_DEF;
    pValue->level = SYS_ALERT_FALL_DOWN_LEVEL_TIME_DEF;

    return true;
}
#endif

