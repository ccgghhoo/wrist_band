#ifndef __PROTO_FIRMWARE_H
#define __PROTO_FIRMWARE_H
#include "proto.h"

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
int ProtoFirmware_Handle(struct objEpbProto *obj, const uint8_t *p_data, uint16_t len);

#endif

