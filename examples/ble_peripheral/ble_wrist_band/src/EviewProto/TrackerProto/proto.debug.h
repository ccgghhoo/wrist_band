#ifndef __PROTO_DEBUG_H
#define __PROTO_DEBUG_H
#include "proto.h"

extern const EpbProtoKey_t scg_sProtoDebugKeys[];
extern const uint8_t s_ucProtoDebugKeySize ;

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


/**
 * �Ƿ��GPS LOG
 * 
 * @author hzhmcu (2018/8/30)
 * 
 * @return bool 
 */
bool ProtoDebug_IsGPSLog();
/**
 * �Ƿ��GSM LOG
 * 
 * @author hzhmcu (2018/8/30)
 * 
 * @return bool 
 */
bool ProtoDebug_IsGSMLog();
bool ProtoDebug_IsLog();
bool ProtoDebug_IsDebugLog(uint32_t flag);

#endif

