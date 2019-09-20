

#ifndef DEV_DATA_PACK_H__
#define DEV_DATA_PACK_H__

#include <stdint.h>
#include <stdbool.h>

#include "app_util.h"

#include "proto.pb.h"
extern bool m_resp_sent;



void proto_pack_get_ble_loc(msg_packet_t *reqHead);
void proto_pack_get_ble_base_info(msg_packet_t *reqHead);
void proto_pack_send_ble_music_play(msg_packet_t *reqHead, int music, uint8_t loop);

uint8_t* __send_pack_msg(msg_packet_t *reqHead, uint8_t *data, uint16_t len, uint8_t flag);

bool proto_pack_gps_latest_data(void);
/**
 * GPS Rx 的数据放在Log 打印
 * 
 * @author hzhmcu (2018/9/4)
 * 
 * @param p_data 接收到的数据，最好是一段数据。
 * @param length 数据的长度
 */
void GPSLog_Rx(uint8_t *p_data, uint16_t length);


/**
 * GSM Rx 的数据放在Log 打印
 * 
 * @author hzhmcu (2018/9/4)
 * 
 * @param p_data  接收到的数据，最好是一段数据。
 * @param length 数据的长度
 */
void GSMLog_Rx(uint8_t *p_data, uint16_t length);


/**
 * GSM Rx 的数据放在Log 打印
 * 
 * @author hzhmcu (2018/9/4)
 * 
 * @param p_data  接收到的数据，最好是一段数据。
 * @param length 数据的长度
 */
void GSMLog_Tx(uint8_t *p_data, uint16_t length);

// GSM active data
void proto_pack_get_heart_beat_msg(PacketInfo_t *targ);
void proto_pack_get_time(PacketInfo_t *targ);
void proto_pack_get_gps_curr_data_msg(PacketInfo_t *targ);
void proto_pack_get_alert_msg(PacketInfo_t *targ);
void proto_pack_req_server_parse_lat_long(PacketInfo_t *targ);
void proto_pack_get_historical_data_msg(PacketInfo_t *targ);
void proto_pack_get_activity_data_msg(PacketInfo_t *targ);
void proto_pack_get_historical_tx_completed_msg(PacketInfo_t *targ);




void long_lan_parsed_from_server(const uint8_t *addr, uint16_t len);


/**
 * 组包函数
 * 
 * @author hzhmcu (2018/8/28)
 * 
 * @param reqHead 请求包头，返回组包后的包头
 * @param data  内容数据
 * @param len   内容数据的长度
 * @param flag  标志位
 * @param data_buff 组包后的数据
 * 
 * @return uint32_t 组包后的数据长度
 */
uint32_t  ProtoPacking(msg_packet_t *reqHead, uint8_t *data, uint16_t len, uint8_t flag, uint8_t *data_buff);
uint32_t  ProtoPacking_WithSn(msg_packet_t *reqHead, uint8_t *data, uint16_t len, uint8_t flag, uint8_t *data_buff,uint16_t sn);

/**
 * 
 * 
 * @author hzhmcu (2018/9/4)
 * 
 * @param p_data  接收到的数据，最好是一段数据。
 * @param length 数据的长度
 */
void GSMDown_Rx(uint8_t *p_data, uint16_t length);
#endif
