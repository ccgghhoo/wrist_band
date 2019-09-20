

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
 * Ӧ��ص�������
 * 
 * @author hzhmc (2018/5/5)
 */
typedef void (*resp_callback_t)(uint8_t *data, uint16_t len);

/**
 * cmd�ϲ㴦����
 * 
 * @author hzhmc (2018/5/5)
 */
typedef int (*pfnEpbProtoCmdHandle_t)(struct objEpbProto *obj, const uint8_t *p_data, uint16_t len);




/** 
 *  
 * cmd�ϲ�ṹ���Ӧ����
 * 
 * @author hzhmc (2018/5/5)
 */
typedef struct {
  uint8_t cmd;
  pfnEpbProtoCmdHandle_t handle;
  //playLoad�е�Type key�ٷ�ɢʵ�� 
  const EpbProtoKey_t *keys;//��Ӧ��keys��
  const uint8_t* pKeySize;//keys�Ĵ�С��
}EpbProtoCmd_t;






/**
 * obj ����
 * 
 * @author hzhmc (2018/5/5)
 */
struct objEpbProto
{
  msg_packet_t *pMsg; //��Ϣ��
  resp_callback_t resp_cb; //Ӧ��ص�����

  const	EpbProtoCmd_t *cmds; //���е�����
  uint8_t cmdSize; //����Ĵ�С

  bool isSendResp; //�Ƿ���Ҫ�ⲿӦ��

  uint16_t sendId; //
};


/**
 * ��ʼ���Ѷ�����г�ʼ��
 * 
 * @author hzhmc (2018/5/5)
 * 
 * @param obj ����
 * @param cmds �Ӳ�cmd�Ļص������ṹ��
 * @param cmdSize �Ӳ�cmd�Ĵ�С��
 */
void _EpbProto_Init(objEpbProto_t *obj, const EpbProtoCmd_t *cmds, uint8_t cmdSize);

/**
 * ͨ��Э����յ����ݣ�ͳһ��ڡ�
 * 
 * @author hzhmcu (2018/5/4)
 * 
 * @param p_msg ��Ҫ�����ݽ��з��
 * @param evt_handle ��Ӧ�ص����� 
 * @return int  
 */
int _EpbProto_PutPacket(objEpbProto_t *obj, msg_packet_t *p_msg, resp_callback_t resp_cb);



/**
 * ���Ͱ����ݡ�
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
 * �������
 * 
 * @author hzhmc (2018/5/5)
 */
#define EpbProto_Define(name)	objEpbProto_t g_objEpbProto_##name={0}

/**
 * ��������
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


