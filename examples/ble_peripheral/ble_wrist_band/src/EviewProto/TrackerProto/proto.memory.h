#ifndef __PROTO_MEMORY_H
#define __PROTO_MEMORY_H
#include "proto.h"


/**
 * memory ������
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
int ProtoMemory_Handle(struct objEpbProto *obj,
												  const uint8_t *p_data, uint16_t len);





extern const EpbProtoKey_t scg_sProtoMemoryKeys[];
extern const uint8_t s_ucProtoMemoryKeySize ;
#endif


