#ifndef __PROTO_CONFIG_H
#define __PROTO_CONFIG_H
#include "proto.h"


/**
 * Config 处理函数
 * 
 * @author hzhmc (2018/5/5)
 * 
 * @param obj 对象，如果不需要底层应答，就 
 * 				obj->isSendResp = false;
 * @param p_data 
 * @param len 
 * 
 * @return int 
 */
int ProtoConfig_Handle(struct objEpbProto *obj,
												  const uint8_t *p_data, uint16_t len);
#endif


