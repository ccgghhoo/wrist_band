#ifndef __PROTO_SERVICE_H
#define __PROTO_SERVICE_H
#include "proto.h"

extern const EpbProtoKey_t scg_sProtoSeviceKeys[];
extern const uint8_t s_ucProtoSevicKeySize; 
/**
 * firmware ������
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
int ProtoService_Handle(struct objEpbProto *obj,
                        const uint8_t *p_data, uint16_t len);
#endif



