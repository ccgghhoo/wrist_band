
#include "proto.h"
#include "proto.pb.h"
#include "proto.pack.h"
#include "proto.service.h"
//#include "indicator.h"
#include "DateTime.h"
#include "loc_gsm.task.h"
static int ProtoService_AddrParse(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoService_ConnectService(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoService_DistanceAlert(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoService_GetTimestarmp(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);



const EpbProtoKey_t scg_sProtoSeviceKeys[] =
{
  {
    .key = SERV_KEY_ADDR_PARSE,
    .handle = ProtoService_AddrParse,
  },

  {
    .key = SERV_KEY_GET_TIMESTARMP,
    .handle = ProtoService_GetTimestarmp,
  },


  {
    .key = SERV_KEY_CONN_SERVER,
    .handle = ProtoService_ConnectService,
  },
  {
    .key = SERV_KEY_DISTANCE_ALERT,
    .handle = ProtoService_DistanceAlert,
  },

};
const uint8_t s_ucProtoSevicKeySize = sizeof(scg_sProtoSeviceKeys) / sizeof(scg_sProtoSeviceKeys[0]);



static  bool loc_gps_util_enable_movement(uint32_t threshold)
{
  return true;
}

/**
 * 地址解释
 * 
 * @author hzhmcu (2018/5/10)
 * 
 * @param obj 
 * @param key 
 * @param p_data 
 * @param len 
 * 
 * @return int 
 */
static int ProtoService_AddrParse(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
  if (len < 8)
  {
    Proto_RespNegative(obj, EPB_ERR_INVALID_FORMAT);
    return -EPB_ERR_INVALID_FORMAT;
  }
  PB_LOG("$$$$$$$$$$\r\n");
  // uint8_t const *str = &p_data[8];
  long_lan_parsed_from_server(p_data + 8, len - 8);
  Proto_RespNegative(obj, EPB_SUCCESS);
  return EPB_SUCCESS;
}

/**
 * 地址解释
 * 
 * @author hzhmcu (2018/5/10)
 * 
 * @param obj 
 * @param key 
 * @param p_data 
 * @param len 
 * 
 * @return int 
 */
static int ProtoService_GetTimestarmp(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{

  if (len != 4)
  {
    Proto_RespNegative(obj, EPB_ERR_INVALID_FORMAT);
    return -EPB_ERR_INVALID_FORMAT;
  }
  uint32_t utc = uint32_decode(p_data);
  datetime_t datatime = DateTime_UTC2DateTime(utc);
  DateTime_Update(datatime, UPDATE_TIME_BY_GPRS);
  return 0;
}






/**
 * 连接服务器。
 * 
 * @author hzhmcu (2018/5/10)
 * 
 * @param obj 
 * @param key 
 * @param p_data 
 * @param len 
 * 
 * @return int 
 */
static int ProtoService_ConnectService(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
  PB_LOG("ConnectService\r\n");
  if (len != 0)
  {
    Proto_RespNegative(obj, EPB_ERR_INVALID_FORMAT);
    return -EPB_ERR_INVALID_FORMAT;
  }
  loc_gsm_task_setup_connect_tcp();
  Proto_RespNegative(obj, EPB_SUCCESS);
  return EPB_SUCCESS;
}

/**
 * 距离报警。
 * 
 * @author hzhmcu (2018/5/10)
 * 
 * @param obj 
 * @param key 
 * @param p_data 
 * @param len 
 * 
 * @return int 
 */
static int ProtoService_DistanceAlert(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
  PB_LOG("DistanceAlert\r\n");
  if (len != 5)
  {
    Proto_RespNegative(obj, EPB_ERR_INVALID_FORMAT);
    return -EPB_ERR_INVALID_FORMAT;
  }

  uint32_t threshold = uint32_decode(p_data);
  if (!loc_gps_util_enable_movement(threshold))
  {
    Proto_RespNegative(obj, EPB_ERR_INVALID_FORMAT);
    return EPB_ERR_INVALID_FORMAT;
  }
  Proto_RespNegative(obj, EPB_SUCCESS);
  return EPB_SUCCESS;
}

/**
 * firmware 处理函数
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
int ProtoService_Handle(struct objEpbProto *obj,
                        const uint8_t *p_data, uint16_t len)
{
  PB_LOG("ProtoService_Handle\r\n");

  uint8_t field_len = p_data[LENGTH_OFFSET];
  uint8_t field_type = p_data[KEY_OFFSET];
  if ((field_len + 1 + 1) > len)
  {
    Proto_RespNegative(obj, EPB_ERR_INVALID_KEY_LENGTH);
    return EPB_ERR_INVALID_KEY_LENGTH;
  }

  switch (field_type)
  {

      case SERV_KEY_ADDR_PARSE:
        if (field_len < 2)
        {
          Proto_RespNegative(obj, EPB_ERR_INVALID_FORMAT);
          break;
        }
        long_lan_parsed_from_server(p_data + VALUE_OFFSET, field_len - 1);
        break;


      case SERV_KEY_CONN_SERVER:
        if (field_len != 1)
        {
          Proto_RespNegative(obj, EPB_ERR_INVALID_FORMAT);
          break;
        }
        loc_gsm_task_setup_connect_tcp();
        break;


      case SERV_KEY_DISTANCE_ALERT:
        {
          if (field_len == 5)
          {
            uint32_t threshold = uint32_decode(p_data + VALUE_OFFSET);
            if (!loc_gps_util_enable_movement(threshold))
            {
              Proto_RespNegative(obj, EPB_ERR_INVALID_FORMAT);
            }
          }
          break;
        }




      default:
        break;
  }
  return 0;
}
