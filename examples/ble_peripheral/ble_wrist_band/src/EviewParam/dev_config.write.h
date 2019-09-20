

#ifndef DEV_CONF_WRITE_H__
#define DEV_CONF_WRITE_H__
#include "dev_conf_defines.h"

void dfw_property_imei_update( char * imei ); 
void dfw_property_iccid_update( char* iccid ); 
void dfw_property_gsm_module_update(char *module);

// System 
bool dfw_system_mode_update(uint8_t mode, uint32_t time );
bool dfw_system_timezone_update( int8_t offset);
bool dfw_system_password_update(bool enable, uint32_t value);
bool dfw_system_password_protect(bool enable); 

bool dfw_system_switch_leds( bool on ); 
bool dfw_system_switch_motor( bool on );
bool dfw_system_switch_buzzer( bool on );
bool dfw_system_switch_agps(bool on );

bool dfw_system_switch_wifi_loc(bool on );
bool dfw_system_switch_ble_loc(bool on );
bool dfw_system_switch_gsm_loc(bool on );
bool dfw_system_switch_sos_speaker(bool on);
bool dfw_system_switches_update( uint32_t value );
bool dfw_system_switch_BLEOftenConnection(bool on);

bool dfw_system_volume_ringtone_update( uint8_t level );
bool dfw_system_volume_mic_update( uint8_t level );
bool dfw_system_volume_speaker_update( uint8_t level );
bool dfw_system_dev_name_update( const uint8_t * p_data, uint8_t len);

bool dfw_system_alarmclock_update(alarm_clock_t *alarm);
bool dfw_system_do_not_disturb_update(uint8_t flag, uint8_t start_hour, uint8_t start_minute,
                                      uint8_t stop_hour, uint8_t stop_minute);
//button
bool dfw_alert_sos_button_mode(uint32_t input);
bool dfw_alert_sos_button_time(uint32_t time);
bool dfw_alert_sos_button(uint32_t input, uint16_t time);
bool dfw_buttons_side_button_dial_no( uint8_t contact ); 

bool dfw_gprs_apn_enable_set( bool enable );
bool dfw_gprs_apn_apn_update( const char* apnStr );
bool dfw_gprs_apn_user_name_update( const char* usrStr );
bool dfw_gprs_apn_password_update( const char* pwdStr );
bool dfw_gprs_cf_connect(uint32_t input);
bool dfw_gprs_apn_update( const char* apn, const char* usrname, const char* password); 
bool dfw_gprs_apn_write( const uint8_t * p_data, uint8_t len );

bool dfw_gprs_set_connect(bool enable);
bool dfw_gprs_server_update(bool enable,bool isUDP,const char *ip, uint16_t port);
bool dfw_gprs_server_write( const uint8_t * p_data, uint8_t len );

bool dfw_gprs_heartbeat_update(bool enable, uint32_t interval );
bool dfw_gprs_upload_interval_update( uint32_t interval );
bool dfw_gprs_upload_lazy_interval_update( uint32_t interval );
bool dfw_gprs_continue_loc_time_update(uint16_t interval,uint16_t time);
bool dfw_gprs_user_id_update(uint8_t *data, uint16_t len);

bool dfw_phone_contact_delete_at( uint8_t no );
bool dfw_phone_contact_update(uint8_t no, bool sms, bool phone, const char* number);
bool dfw_phone_contact_write( const uint8_t * p_data, uint8_t len );

bool dfw_phone_sms_prefix_update( bool enable, const char* prefix_text); 
bool dfw_phone_sms_prefix_write( const uint8_t * p_data, uint8_t len );

bool dfw_phone_sos_holding_time_update( uint16_t holding_time); 
bool dfw_phone_sos_ring_time_update( uint8_t ring_time);
bool dfw_phone_sos_loops_update( uint8_t loops ); 
bool dfw_phone_sos_write( const uint8_t * p_data, uint8_t len );

bool dfw_phone_switch_auto_answer_update( bool enable, uint8_t rings);
bool dfw_phone_switch_intercept_update( uint8_t enable );
bool dfw_phone_switch_hangup_update( uint8_t enable );
bool dfw_phone_switch_monitor_update( uint8_t enable );
bool dfw_phone_switch_set_sms_update( uint8_t enable );
bool dfw_phone_switches_update( const uint8_t * p_data, uint8_t len );
bool dfw_buttons_set_side_button(uint8_t auth, uint8_t feeback, uint8_t time);
bool dfw_buttons_set_side_button_disable();

bool dfw_alert_power_low_update( uint8_t enable, uint8_t threshold );
bool dfw_alert_power_on_update( uint8_t enable );
bool dfw_alert_power_off_update( uint8_t enable );
bool dfw_alert_power_update( const uint8_t * p_data, uint8_t len);

bool dfw_alert_geo_delete_at( uint8_t no ); 
bool dfw_alert_geo_update( uint8_t no, uint8_t enable, uint8_t dir, uint16_t radius, int32_t longitude, int32_t latitude ); 
bool dfw_alert_geo_write( const uint8_t * p_data, uint8_t len );

bool dfw_alert_motion_update( bool enable,bool isDial, uint16_t setup_time, uint16_t action_time);
bool dfw_alert_motion_write( const uint8_t * p_data, uint8_t len );

bool dfw_alert_actionless_update( bool enable,bool isDial,  uint32_t threshold);
bool dfw_alert_actionless_write( const uint8_t * p_data, uint8_t len);

bool dfw_alert_speed_update( bool enable, uint32_t threshold);
bool dfw_alert_speed_write( const uint8_t * p_data, uint8_t len);

bool dfw_alert_tilt_update( bool enable, bool isDial,uint8_t angle, uint16_t time);
bool dfw_alert_tilt_write( const uint8_t * p_data, uint8_t len );

bool dfw_alert_falldown_update(bool enable, uint8_t level, bool dial);
bool dfw_alert_falldown_write( const uint8_t * p_data, uint8_t len );

bool dfw_ble_scan_time_write(const uint8_t *p_data, uint8_t len);
bool dfw_ble_scan_beacon_time_write(const uint8_t *p_data, uint8_t len);

bool dfw_settings_update(const uint8_t *p_data, uint16_t size, bool isSave);

#endif 

