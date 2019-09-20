#include "proto.pb.h"
#include "proto.negative.h"
#include "proto.pack.h"
//#include "step_gsm.historicaldata.h"
#include "config.h"



/**
 * firmware 处理函数S
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
int ProtoNegative_Handle(struct objEpbProto *obj,
                         const uint8_t *p_data, uint16_t len)
{
  PB_LOG("ProtoNegative_Handle code 0x%X   0x%X  0x%X\r\n",p_data[0],p_data[1],p_data[2]);

  if (len <3)
  {
    return EPB_SUCCESS;
  }

  ProtoHistoricalDataAck_Put(obj->pMsg->id, p_data[2]);  
  
#ifdef DEGREE
  ProtoActivityDataAck_Put(obj->pMsg->id, p_data[2]); 
#endif
  return EPB_SUCCESS;
}
