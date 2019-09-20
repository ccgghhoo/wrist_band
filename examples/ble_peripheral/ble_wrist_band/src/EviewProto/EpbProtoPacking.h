#ifndef __EPB_PROTO_PACKING_H
#define __EPB_PROTO_PACKING_H

#include "CircularBuf.h"
typedef struct
{
  bool isGetData;
  CBUF_STRUCT *pCBuf;
  void (*response_cb)(uint8_t *data, uint16_t len);
}objEpbProtoPacking_t;
#define EPBProtoPacking_Define(name,size) CBUF_Define(sg_s##name##Buf,(size))\
                                          static objEpbProtoPacking_t obj_##name ={.pCBuf= &sg_s##name##Buf,};
/**
 * ��__EPBProtoPacking_Init ���г�ʼ��
 * 
 * @author hzhmcu (2018/8/23)
 * 
 * @param obj ����
 * @param response_cb Ӧ��ص��ص�
 * @param data 
 * @param len 
 * 
 * @return int 
 */
int __EPBProtoPacking_Init(objEpbProtoPacking_t *obj,
                           void (*response_cb)(uint8_t *data, uint16_t len));
/**
 * ���յ������ݣ����������ʱ��š�
 * 
 * @author hzhmcu (2018/8/23)
 * 
 * @param obj 
 * @param buf 
 * @param len 
 * 
 * @return int 
 */
int __EPBProtoPacking_Put(objEpbProtoPacking_t *obj, uint8_t buf[], int len);
/**
 * Loop �Դ��ڵ����ݽ��н���� 
 *  
 * 
 * @author hzhmcu (2018/8/23)
 * 
 * @param obj 
 * 
 * @return int 
 */
int __EPBProtoPacking_Loop(objEpbProtoPacking_t *obj);




#define EPBProtoPacking_init(name,response_cb) __EPBProtoPacking_Init(&obj_##name,response_cb)
#define EPBProtoPacking_Put(name,buf,len)      __EPBProtoPacking_Put(&obj_##name,buf,len)
#define EPBProtoPacking_Loop(name)             __EPBProtoPacking_Loop(&obj_##name)
#endif
