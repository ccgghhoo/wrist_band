

#ifndef __PROTO_H
#define __PROTO_H

#include "proto.pb.h"
// payload sequence
#define COMMAND_OFFSET			0
#define LENGTH_OFFSET			1
#define KEY_OFFSET				2
#define VALUE_OFFSET			3

#include "EpbProto.h"


#include "proto.config.h"
#include "proto.control.h"
#include "proto.factory.h"
#include "proto.firmware.h"
#include "proto.memory.h"
#include "proto.record.h"
#include "proto.service.h"



#if 1
#define PB_LOG NRF_LOG_INFO
#else
#define PB_LOG(...)
#endif



/**
 * ��ʼ��Э�飬���ϲ�Э��Ķ�����г�ʼ��
 * 
 * @author hzhmc (2018/5/5)
 */
void Proto_Init(void);
/**
 * �ײ����ϲ㴫�Ͱ��Ľӿڡ�
 * 
 * @author hzhmc (2018/5/5)
 * 
 * @param p_msg ��Ϣ��
 * @param resp_cb Ӧ��ص�����
 * 
 * @return int 
 */
int32_t Proto_PutPack( msg_packet_t *p_msg, resp_callback_t resp_cb);

/**
 *  ����Ӧ��������������ݡ�
 * 
 * @author hzhmc (2018/5/5)
 * 
 * @param flag ��־
 * @param data ����
 * @param len ����
 * 
 * @return int32_t 
 */
int32_t Proto_Resp(objEpbProto_t *obj, uint8_t flag,uint8_t *data, uint16_t len);

/**
 * ��Ӧ�����
 * 
 * @author hzhmc (2018/5/5)
 * 
 * @return int 
 */
int32_t Proto_RespAck( objEpbProto_t *obj);

/**
 * ����Ӧ�����������
 * 
 * @author hzhmc (2018/5/5)
 *  
 * @param code ������
 * 
 * @return int32_t 
 */
int32_t Proto_RespNegative( objEpbProto_t *obj,uint8_t code);


/**
 * ����Ӧ�������ȷ��
 * 
 * @author hzhmc (2018/5/5)
 *  
 * @param code ������
 * 
 * @return int32_t 
 */
int32_t Proto_RespPositive(objEpbProto_t *obj,uint8_t cmd, uint8_t key);
#endif
