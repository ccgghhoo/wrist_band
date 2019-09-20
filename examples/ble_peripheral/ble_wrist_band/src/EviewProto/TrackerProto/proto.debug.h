#ifndef __PROTO_DEBUG_H
#define __PROTO_DEBUG_H
#include "proto.h"

extern const EpbProtoKey_t scg_sProtoDebugKeys[];
extern const uint8_t s_ucProtoDebugKeySize ;

/**
 * control 处理函数
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
int ProtoControl_Handle(struct objEpbProto *obj,
												  const uint8_t *p_data, uint16_t len);


/**
 * 是否打开GPS LOG
 * 
 * @author hzhmcu (2018/8/30)
 * 
 * @return bool 
 */
bool ProtoDebug_IsGPSLog();
/**
 * 是否打开GSM LOG
 * 
 * @author hzhmcu (2018/8/30)
 * 
 * @return bool 
 */
bool ProtoDebug_IsGSMLog();
bool ProtoDebug_IsLog();
bool ProtoDebug_IsDebugLog(uint32_t flag);

#endif

