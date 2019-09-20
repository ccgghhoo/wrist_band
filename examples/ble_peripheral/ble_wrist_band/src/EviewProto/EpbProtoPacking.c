#include "EpbProtoPacking.h"
#include "EpbProto.pb.h"
#include "proto.h"
#include "crc16.h"

#include "nrf_log.h"
#define PACKING_LOG(...)
#define PACKING_LOG_RAW(...)
/**
 * 对__EPBProtoPacking_Init 进行初始化
 * 
 * @author hzhmcu (2018/8/23)
 * 
 * @param obj 对你
 * @param response_cb 应答回调回调
 * @param data 
 * @param len 
 * 
 * @return int 
 */
int __EPBProtoPacking_Init(objEpbProtoPacking_t *obj,
                           void (*response_cb)(uint8_t *data, uint16_t len))
{
  obj->response_cb = response_cb;
  return 0;
}


/**
 * 接收到的数据，向这个函数时存放。
 * 
 * @author hzhmcu (2018/8/23)
 * 
 * @param obj 
 * @param buf 
 * @param len 
 * 
 * @return int 
 */
int __EPBProtoPacking_Put(objEpbProtoPacking_t *obj, uint8_t buf[], int len)
{
  obj->isGetData = true;
  CBUF_Put(obj->pCBuf, buf, len);
  len = CBUF_Query(obj->pCBuf);
  PACKING_LOG("put len =%d\r\n", len);
  return 0;
}

/**
 * Loop 对存在的数据进行解包。 
 *  
 * 
 * @author hzhmcu (2018/8/23)
 * 
 * @param obj 
 * 
 * @return int 
 */
int __EPBProtoPacking_Loop(objEpbProtoPacking_t *obj)
{
  if (obj->isGetData == false) return false;
  obj->isGetData = false;
  uint16_t len = 0;
  while (1)
  {
    len = CBUF_Query(obj->pCBuf);
    PACKING_LOG("Get len =%d\r\n", len);
    if (len < EPB_HEAD_DATA_SIZE) break; //头部的长度不足，不能向下走

    //查找magic
    if (len > 60)
    {
      len = 60;
    }
    uint8_t buf[60];
    int i = 0;


    CBUF_PreGet(obj->pCBuf, buf, len);
    for (i = 0; i < len; i++)
    {
      if (buf[i] == MAGIC_NUMBER)
      {

        break;
      }
    }
    if (i > 0)
    {
      CBUF_Discard(obj->pCBuf, i - 1);
    }
    PACKING_LOG("MAGIC_NUMBER index =%d\r\n", i);

    len = CBUF_Query(obj->pCBuf);
    PACKING_LOG("xx len =%d\r\n", len);
    if (len < EPB_HEAD_DATA_SIZE) break; //头部的长度不足，不能向下走

    CBUF_PreGet(obj->pCBuf, buf, EPB_HEAD_DATA_SIZE);
    msg_packet_t *p = (msg_packet_t *)buf;
    if (p->len > EPB_PAYLOAD_MAX_LEN)
    {
      CBUF_Discard(obj->pCBuf, 1);
      continue;
    }

    len = CBUF_Query(obj->pCBuf);
    PACKING_LOG("x2 len =%d\r\n", len);
    if (len < (EPB_HEAD_DATA_SIZE + p->len)) break; //还没等到足够的长度

    //取内容
    uint8_t data[EPB_HEAD_DATA_SIZE + p->len];
    PACKING_LOG("len = %d \r\n", EPB_HEAD_DATA_SIZE + p->len); 
    CBUF_PreGet(obj->pCBuf, data, EPB_HEAD_DATA_SIZE + p->len);
    p = (msg_packet_t *)data;
    msg_packet_t p_msg = *p;
    p_msg.payload = data + EPB_HEAD_DATA_SIZE;
    uint16_t crc16 = crc16_compute(p_msg.payload, p_msg.len, 0);
    PACKING_LOG("crc 0x%X crc2 = 0x%X \r\n", crc16, p_msg.crc);
    if (crc16 != p_msg.crc)
    {
      CBUF_Discard(obj->pCBuf, 1);
      continue;
    }
    
    Proto_PutPack(&p_msg, obj->response_cb);
    CBUF_Discard(obj->pCBuf, EPB_HEAD_DATA_SIZE + p->len);

    return 0;
  }
  return 0;
}




