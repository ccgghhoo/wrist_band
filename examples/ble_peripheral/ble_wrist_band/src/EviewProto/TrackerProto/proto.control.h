#ifndef __PROTO_CONTROL_H
#define __PROTO_CONTROL_H
#include "proto.h"

extern const EpbProtoKey_t scg_sProtoControlKeys[];
extern const uint8_t s_ucProtoControlKeySize ;

/**
 * control ������
 * 
 * @author hzhmc (2018/5/5)
 * 
 * @param obj �����������Ҫ�ײ�Ӧ�𣬾� 
 * 				obj->isSendResp = false;
 * @param p_data 
 * @param len 
 * 
 * @return int 
 */
int ProtoControl_Handle(struct objEpbProto *obj,
												  const uint8_t *p_data, uint16_t len);

#endif

