

#ifndef __EPB_PROTO_H
#define __EPB_PROTO_H

#include "epbproto.pb.h"
typedef struct objEpbProto objEpbProto_t;
typedef int (*pfnEpbProtoKeyHandle_t)(objEpbProto_t *obj, uint8_t key, const uint8_t *p_data, uint16_t len);

typedef struct {
  uint8_t key;
  pfnEpbProtoKeyHandle_t handle;
}EpbProtoKey_t;




/**
 * 应答回调函数。
 * 
 * @author hzhmc (2018/5/5)
 */
typedef void (*resp_callback_t)(uint8_t *data, uint16_t len);

/**
 * cmd上层处理函数
 * 
 * @author hzhmc (2018/5/5)
 */
typedef int (*pfnEpbProtoCmdHandle_t)(struct objEpbProto *obj, const uint8_t *p_data, uint16_t len);




/** 
 *  
 * cmd上层结构体对应函数
 * 
 * @author hzhmc (2018/5/5)
 */
typedef struct {
  uint8_t cmd;
  pfnEpbProtoCmdHandle_t handle;
  //playLoad中的Type key再分散实现 
  const EpbProtoKey_t *keys;//相应的keys表
  const uint8_t* pKeySize;//keys的大小。
}EpbProtoCmd_t;






/**
 * obj 对象
 * 
 * @author hzhmc (2018/5/5)
 */
struct objEpbProto
{
  msg_packet_t *pMsg; //信息包
  resp_callback_t resp_cb; //应答回调函数

  const	EpbProtoCmd_t *cmds; //所有的命令
  uint8_t cmdSize; //命令的大小

  bool isSendResp; //是否需要外部应答。

  uint16_t sendId; //
};


/**
 * 初始化把对象进行初始化
 * 
 * @author hzhmc (2018/5/5)
 * 
 * @param obj 对象
 * @param cmds 子层cmd的回调函数结构体
 * @param cmdSize 子层cmd的大小。
 */
void _EpbProto_Init(objEpbProto_t *obj, const EpbProtoCmd_t *cmds, uint8_t cmdSize);

/**
 * 通信协议接收到数据，统一入口。
 * 
 * @author hzhmcu (2018/5/4)
 * 
 * @param p_msg 需要把数据进行封包
 * @param evt_handle 响应回调函数 
 * @return int  
 */
int _EpbProto_PutPacket(objEpbProto_t *obj, msg_packet_t *p_msg, resp_callback_t resp_cb);



/**
 * 发送包数据。
 * 
 * @author hzhmc (2018/5/5)
 * 
 * @param obj 
 * @param flagType 
 * @param data 
 * @param len 
 * 
 * @return int32_t 
 */
int32_t _EpbProto_Resp(objEpbProto_t *obj, uint8_t flagType,
                       uint8_t *data, uint16_t len);


/**
 * 定义对象
 * 
 * @author hzhmc (2018/5/5)
 */
#define EpbProto_Define(name)	objEpbProto_t g_objEpbProto_##name={0}

/**
 * 声明对象
 * 
 * @author hzhmc (2018/5/5)
 */
#define EpbProto_Extern(name)	extern objEpbProto_t g_objEpbProto_##name

#define EpbProto_Init(name,cmds,cmdSize) \
		_EpbProto_Init(&g_objEpbProto_##name, cmds, cmdSize)

#define EpbProto_PutPacket(name,p_msg,resp_cb) \
		_EpbProto_PutPacket(&g_objEpbProto_##name, p_msg,  resp_cb)


#define EpbProto_Resp(name,flagType,data,len) _EpbProto_Resp(&g_objEpbProto_##name,flagType,data, len)





#endif


