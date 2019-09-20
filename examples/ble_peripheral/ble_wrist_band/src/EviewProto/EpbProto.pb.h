

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
//ֻ��TCP download
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
 * ������ 
 *  
 */
typedef enum {
  EPB_SUCCESS = 0,

  //Э�����
  EPB_ERR_INVALID_VERSION = 0x11, //Э��汾��֧��
  EPB_ERR_INVALID_ENCRYPTION, //���ܷ�ʽ��֧��
  EPB_ERR_LENGTH, //Э�鳤�ȴ���
  EPB_ERR_CHECK_SUM, //У�����
  EPB_ERR_INVALID_CMD, //���֧��
  EPB_ERR_INVALID_KEY,
  EPB_ERR_INVALID_KEY_LENGTH,



  EPB_ERR_INVALID_FORMAT = 0x21, //����Խ��
  EPB_ERR_DATA_SIZE_ERROR, //�����ֽڳ��ȴ���
  EPB_ERR_INVALID_STATE, //״̬����
  EPB_ERR_INVALID_PARAMETER, //��������
  EPB_ERR_NO_MEM, //�ڴ治��

  EPB_ERR_SUB_FUNCTION_NOT_SUPPORTED, //���ܲ�֧��
  EPB_ERR_GPS_IS_NOT_READY, //GPS�޶�λ
  EPB_ERR_ADDR_RESP, //��ַ����

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
 * ͨ�Ÿ�ʽ�� 
 *  
 * ---------
 * |magic |  
 * 
 * @author hzhmc (2018/5/5)
 */
typedef struct {

  uint8_t  magic; //magic �˴�Ϊ0xAB
  union
  {
    struct
    {
      uint8_t version:	4; //Э��汾�ţ�һ��Ϊ0
      uint8_t ack:		1; //�Ƿ���ҪӦ��
      uint8_t error:		1; //�Ƿ��Ǵ����
      uint8_t encrypt:	2; //���ּ���
    }flag;
    uint8_t val;
  };
  uint16_t len; //����
  uint16_t crc; //����crc����Ҫ��payload��CRC
  uint16_t id; //���豸��

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
