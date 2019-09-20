

#ifndef EPB_PROTO_PB_H__
#define EPB_PROTO_PB_H__

#include "LibConfig.h"

#define MAGIC_NUMBER			0xAB
#define EPB_VER					0x00

#define FLAG_ACK				0x10
#define FLAG_ERROR				0x20
#define FLAG_VERSION			EPB_VER
#define FLAG_CRYPTO_NONE		0x00

#define FLAG_NORMAL_RESP		(0x00 | FLAG_VERSION )
#define FLAG_ERROR_RESP			(0x00 | FLAG_ERROR | FLAG_VERSION )
#define FLAG_ACK_RESP			(0x00 | FLAG_ACK | FLAG_VERSION )

#define EPB_PAYLOAD_MAX_LEN		(900+80)//4200 	//
#define EPB_HEAD_DATA_SIZE		8
#define EPB_DATA_SIZE_MAX		(EPB_PAYLOAD_MAX_LEN + 8 + 8 )

//ONLY FOR down firmware
//只是TCP download
#define EPB_PAYLOAD_DOWN_MAX_LEN	(1024*4 +16)//(512 +16)	//


#define NEW_EPB_PAYLOAD_MAX_LEN 	(EPB_PAYLOAD_MAX_LEN)
#define EPB_HEART_BEAT_MAGIC_CODE	0x5A




typedef enum {
  EPB_STEP_MAGIC  = 0,
  EPB_STEP_FLAG,
  EPB_STEP_LEN_LSB,
  EPB_STEP_LEN_MSB,
  EPB_STEP_CRC_LSB,
  EPB_STEP_CRC_MSB,
  EPB_STEP_TID_LSB,
  EPB_STEP_TID_MSB,
  EPB_STEP_PAYLOAD,
  EPB_STEP_COMPLETED,
}EPB_STEP_ENUM;


/**
 * 错误码 
 *  
 */
typedef enum {
  EPB_SUCCESS = 0,

  //协议错误
  EPB_ERR_INVALID_VERSION = 0x11, //协议版本不支持
  EPB_ERR_INVALID_ENCRYPTION, //加密方式不支持
  EPB_ERR_LENGTH, //协议长度错误
  EPB_ERR_CHECK_SUM, //校验错误
  EPB_ERR_INVALID_CMD, //命令不支持
  EPB_ERR_INVALID_KEY,
  EPB_ERR_INVALID_KEY_LENGTH,



  EPB_ERR_INVALID_FORMAT = 0x21, //数据越界
  EPB_ERR_DATA_SIZE_ERROR, //数据字节长度错误。
  EPB_ERR_INVALID_STATE, //状态错误
  EPB_ERR_INVALID_PARAMETER, //参数错误
  EPB_ERR_NO_MEM, //内存不足

  EPB_ERR_SUB_FUNCTION_NOT_SUPPORTED, //功能不支持
  EPB_ERR_GPS_IS_NOT_READY, //GPS无定位
  EPB_ERR_ADDR_RESP, //地址错误

  EPB_BATTERY_POWER_LOW = 0XF0,

}epb_errcode_t;





#if defined(__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined(__ICCARM__)
  #pragma language=extended
#elif defined(__GNUC__)
// anonymous unions are enabled by default
#endif

/**
 * 通信格式： 
 *  
 * ---------
 * |magic |  
 * 
 * @author hzhmc (2018/5/5)
 */
typedef struct {

  uint8_t  magic; //magic 此处为0xAB
  union
  {
    struct
    {
      uint8_t version:	4; //协议版本号，一般为0
      uint8_t ack:		1; //是否需要应答
      uint8_t error:		1; //是否是错误包
      uint8_t encrypt:	2; //哪种加密
    }flag;
    uint8_t val;
  };
  uint16_t len; //长度
  uint16_t crc; //数据crc。主要是payload的CRC
  uint16_t id; //主设备的

  uint8_t *payload;
}msg_packet_t;


#if defined(__CC_ARM)
  #pragma pop
#elif defined(__ICCARM__)
// leave anonymous unions enabled
#elif defined(__GNUC__)
// anonymous unions are enabled by default
#endif

// payload sequence
#define COMMAND_OFFSET			0
#define DATA_OFFSET			    1



#endif
