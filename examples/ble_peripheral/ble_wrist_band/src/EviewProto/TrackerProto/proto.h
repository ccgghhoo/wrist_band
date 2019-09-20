

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
 * 初始化协议，把上层协议的对象进行初始化
 * 
 * @author hzhmc (2018/5/5)
 */
void Proto_Init(void);
/**
 * 底层向上层传送包的接口。
 * 
 * @author hzhmc (2018/5/5)
 * 
 * @param p_msg 消息包
 * @param resp_cb 应答回调函数
 * 
 * @return int 
 */
int32_t Proto_PutPack( msg_packet_t *p_msg, resp_callback_t resp_cb);

/**
 *  返回应答包，并反馈数据。
 * 
 * @author hzhmc (2018/5/5)
 * 
 * @param flag 标志
 * @param data 数据
 * @param len 长度
 * 
 * @return int32_t 
 */
int32_t Proto_Resp(objEpbProto_t *obj, uint8_t flag,uint8_t *data, uint16_t len);

/**
 * 回应答包。
 * 
 * @author hzhmc (2018/5/5)
 * 
 * @return int 
 */
int32_t Proto_RespAck( objEpbProto_t *obj);

/**
 * 返回应答包，错误码
 * 
 * @author hzhmc (2018/5/5)
 *  
 * @param code 错误码
 * 
 * @return int32_t 
 */
int32_t Proto_RespNegative( objEpbProto_t *obj,uint8_t code);


/**
 * 返回应答包，正确包
 * 
 * @author hzhmc (2018/5/5)
 *  
 * @param code 错误码
 * 
 * @return int32_t 
 */
int32_t Proto_RespPositive(objEpbProto_t *obj,uint8_t cmd, uint8_t key);
#endif
