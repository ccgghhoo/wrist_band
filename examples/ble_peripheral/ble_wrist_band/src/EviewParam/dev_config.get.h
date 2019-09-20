

#ifndef DEV_CONFIG_GET_H__
#define DEV_CONFIG_GET_H__


#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "app_util.h"

#include "dev_conf_defines.h"
#include "compiler_abstraction.h"

uint32_t dev_config_get_module_id(void);
//const sys_mode_t * dev_config_get_sys_mode(void);

char* dev_config_get_IMEI(void);
char* dev_config_get_IMSI(void);
char* dev_config_get_ICCID(void);
char* dev_config_get_fw_ver_string(void);
uint32_t dev_config_get_module(void);

uint8_t dev_config_get_sys_mode_mode(void);
uint32_t dev_config_get_sys_mode_time(void);
int8_t dev_config_get_timezone_offset(void);


bool dev_config_get_no_disturb_is_enable();
uint16_t dev_config_get_no_disturb_start_time(void);
uint16_t dev_config_get_no_disturb_stop_time(void);

bool dev_config_get_password_enable(void);
uint32_t dev_config_get_password(void);

bool dev_config_get_leds_switch_on(void);
bool dev_config_get_buzzer_switch(void);
bool dev_config_get_motor_switch(void);
bool dev_config_get_wifi_loc_switch(void);
bool dev_config_get_sos_speaker_switch(void);
bool dev_config_get_x_speaker_switch(void);
bool dev_config_get_ble_often_connect_switch(void);
bool dev_config_get_gsm_loc_switch(void);

bool dev_config_get_ble_loc_switch(void);



bool dev_config_get_auto_update_switch(void);
bool dev_config_get_agps_switch(void);

uint8_t dev_config_get_ringtone_level(void);
uint8_t dev_config_get_mic_level(void);
uint8_t dev_config_get_speaker_level(void);
uint32_t dev_config_music_switch(void);
bool dev_config_get_ble_whitelist(uint8_t const mac[]);

char* dev_config_gps_url(void);
char* dev_config_wifi_url(void);




extern  button_operation_t const* dev_config_get_buttons_operation(ButtonsIdex_en idx);

const gprs_apn_t* dev_config_get_apn(void);
bool dev_config_get_apn_enable(void);
const char* dev_config_get_apn_apn(void);
const char* dev_config_get_apn_usrname(void);
const char* dev_config_get_apn_password(void);


const gprs_server_t* dev_config_get_server(void);
bool dev_config_get_server_enable(void);
const char* dev_config_get_server_ip(void);
uint16_t dev_config_get_server_port(void);
bool  dev_config_get_server_protocol_is_udp(void);

const gprs_time_t* dev_config_get_gprs_time(void);
bool dev_config_get_gprs_heartbeat_enable(void);
uint32_t dev_config_get_gprs_heartbeat_time(void);
uint32_t dev_config_get_gprs_upload_interval(void);
uint32_t dev_config_get_gprs_lazy_upload_interval(void);

uint32_t dev_config_get_continue_loc_interval(void);
uint32_t dev_config_get_continue_loc_time(void);

uint8_t dev_config_user_id_len(void);
uint8_t* dev_config_user_id(void);


const phone_contact_t* dev_config_get_contact_by(uint8_t index);
bool dev_config_get_contact_at_index_by(char *number, uint8_t *ret_index);
bool dev_config_get_contact_enable_at(uint8_t index);
bool dev_config_get_contact_sms_at(uint8_t index);
bool dev_config_get_contact_phone_at(uint8_t index);
bool dev_config_get_contact_no_card_at(uint8_t index);
const char* dev_config_get_contact_number_at(uint8_t index);
uint8_t dev_config_get_contact_phone_enable_from(uint8_t index);
uint8_t dev_config_get_contact_sms_enable_from(uint8_t index);


const char* dev_config_get_sms_prefix_text(void);
bool dev_config_get_sms_prefix_text_enable(void);
uint8_t dev_config_get_sms_prefix_text_size(void);

uint16_t dev_config_get_sos_holding_time(void);
uint8_t dev_config_get_sos_ring_time(void);
uint8_t dev_config_get_sos_loops(void);
const phone_sos_t* dev_config_get_phone_sos(void);

bool dev_config_get_auto_answer_enable(void);
uint8_t dev_config_get_auto_answer_rings_need(void);
bool dev_config_get_call_only_by_auth_number(void);
bool dev_config_get_hangup_manual_enable(void);
bool dev_config_get_monitor_environment_enable(void);
bool dev_config_get_sms_set_only_by_auth_number(void);

bool dev_config_get_alert_power_on(void);
bool dev_config_get_alert_power_off(void);
bool dev_config_get_alert_power_low(void);
uint8_t dev_config_get_alert_power_low_threshold(void);
bool dev_config_get_alert_power_low_voice(void);
uint8_t dev_config_get_alert_power_low_voice_threshold(void);


const alert_geo_t* dev_config_get_geo_by(uint8_t index);
bool dev_config_get_geo_enable_at(uint8_t index);
bool dev_config_get_geo_in_fence_type(uint8_t index);
uint8_t dev_config_get_geo_points_count(uint8_t index);
int32_t* dev_config_get_geo_points_addr(uint8_t index);
bool dev_config_get_geo_dir_inside_at(uint8_t index);
uint16_t dev_config_get_geo_radius_at(uint8_t index);
int32_t dev_config_get_geo_longitude_at(uint8_t index);
int32_t dev_config_get_geo_latitude_at(uint8_t index);

const alert_motion_t* dev_config_get_alert_motion(void);
bool dev_config_get_alert_motion_enable(void);
bool dev_config_get_alert_motion_is_dial(void);
uint16_t dev_config_get_alert_motion_setup_time(void);

uint16_t dev_config_get_alert_motion_action_time(void);
bool dev_config_get_alert_actionless_enable(void);
bool dev_config_get_alert_actionless_is_dial(void);
uint32_t dev_config_get_alert_actionless_threshold(void);


bool dev_config_get_alert_speed_enable(void);
uint32_t dev_config_get_alert_speed_threshold(void);

bool dev_config_get_alert_tilt_enable(void);
bool dev_config_get_alert_tilt_is_dial(void);
uint8_t dev_config_get_alert_tilt_angle(void);
uint8_t dev_config_get_alert_tilt_threshold(void);
bool dev_config_get_alert_falldown_enable(void);
bool dev_config_get_alert_falldown_is_dial(void);
bool dev_config_get_alert_falldown_call(void);
uint8_t dev_config_get_alert_falldown_level(void);

//ble
uint16_t dev_config_get_ble_scan_time(void);
uint16_t dev_config_get_ble_sleep_time(void);
uint16_t dev_config_get_ble_scan_beacon_time(void);

#endif

