
#include "proto.config.h"
#include "dev_conf.pb.h"
#include "LibHeap.h"
#include "proto.h"
#include "dev_config.read.h"
#include "dev_config.write.h"

/**
 * Config 处理函数
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
int ProtoConfig_Handle(struct objEpbProto *obj,
                       const uint8_t *p_data, uint16_t len)
{
  PB_LOG("ProtoConfig_Handle\r\n");
  if (len < 3) // command/len/key as least
  {
    Proto_RespNegative(obj, EPB_ERR_INVALID_KEY_LENGTH);
    return EPB_ERR_INVALID_KEY_LENGTH;
  }

  uint8_t field_len = p_data[LENGTH_OFFSET];
  uint8_t field_type = p_data[KEY_OFFSET];

  /**
   * 读所有的配置。
   */
  if (field_type == DEV_CONF_READ_SETTINGS)
  {
    uint8_t *p_read_settings = (uint8_t *)APP_MALLOC(PROTO_READ_CONFIG_SETTING,  dev_config_read_max_size() + 1);
    if (p_read_settings == NULL)
    {
      Proto_RespNegative(obj, EPB_ERR_NO_MEM);
      return EPB_ERR_NO_MEM;
    }

    if (field_len == 0)
    {
      Proto_RespNegative(obj, EPB_ERR_INVALID_KEY_LENGTH);
      return EPB_ERR_INVALID_KEY_LENGTH;
    }

    p_read_settings[0] = COMMAND_ID_CONFIG;

    uint16_t ret_len = dev_config_read(p_data + VALUE_OFFSET, field_len - 1, p_read_settings + 1, dev_config_read_max_size() + 1);

    if (ret_len > 0)
    {
      Proto_Resp(obj, FLAG_NORMAL_RESP, p_read_settings,  ret_len + 1);
      PB_LOG("dev_config_read len %d\r\n", ret_len);
     
    } else
    {
      Proto_RespNegative(obj, EPB_ERR_SUB_FUNCTION_NOT_SUPPORTED);
    }
    APP_FREE(p_read_settings);
  }
  else
  {
    bool success = dfw_settings_update(p_data + LENGTH_OFFSET, len - 1,true);
    if (!success)
    {
      Proto_RespNegative(obj, EPB_ERR_SUB_FUNCTION_NOT_SUPPORTED);
    } else
    {
      Proto_RespNegative(obj, EPB_SUCCESS); 
    }
  }
  return EPB_SUCCESS;
}



