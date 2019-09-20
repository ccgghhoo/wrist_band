#include "proto.pb.h"
#include "proto.memory.h"
#include "LibHeap.h"
//#include "FM25Q.h"

const EpbProtoKey_t scg_sProtoMemoryKeys[] =
{
  {
    .key = MEMO_KEY_INNER_RAM,
    .handle = NULL,
  },

  {
    .key = MEMO_KEY_EXTERNAL_FLASH,
    .handle = NULL,
  },

  {
    .key = MEMO_KEY_MEMS,
    .handle = NULL,
  },
};
const uint8_t s_ucProtoMemoryKeySize = sizeof(scg_sProtoMemoryKeys) / sizeof(scg_sProtoMemoryKeys[0]);





static void memory_reader_process(objEpbProto_t *obj, uint8_t field_type, const uint8_t *p_data, uint16_t len)
{
  int32_t ret = 0;
  uint32_t addr = uint32_decode(p_data);
  uint16_t size = uint16_decode(p_data + 4);

  if (size > 512 || size == 0) return;

  uint8_t *buff = APP_MALLOC(PROTO_PACK_MEMORY_READER, size + 9);
  if (buff == NULL) return;

  buff[0] = COMMAND_ID_MEMORY_READER;
  buff[1] = 7;
  buff[2] = field_type;

  uint32_encode(addr, buff + 3);
  uint16_encode(size, buff + 7);

  switch (field_type)
  {
      case MEMO_KEY_INNER_RAM:
        if (addr > 0x2000000  && addr < 0x20010000)
        {
          memcpy(buff + 8, (uint8_t *)addr, size);
        } else if (addr >= 0x70000 && addr < 0x80000)
        {
          memcpy(buff + 8, (uint8_t *)addr, size);
        }
        break;

      case MEMO_KEY_EXTERNAL_FLASH:

        if (addr < FLASH_END_ADDR)
        {
          flash_release_power_down_mode();
          flash_read_data(addr, buff + 8, size);
          flash_enter_power_down_mode();
        }

        break;
      case MEMO_KEY_MEMS:
      default:
        ret  = -1;
        break;
  }
  /**
   * 没实现的，也发送，应该有问题的。 
   *  
   */
  PB_LOG("Memory size %d\r\n", size + 8);
  if (ret == 0)
  {
    Proto_Resp(obj, FLAG_NORMAL_RESP, buff, size + 8);
  }
  APP_FREE(buff);
}
/**
 * memory 处理函数
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
int ProtoMemory_Handle(struct objEpbProto *obj,
                       const uint8_t *p_data, uint16_t len)
{
  PB_LOG("ProtoMemory_Handle\r\n");
  uint8_t field_len = p_data[LENGTH_OFFSET];
  uint8_t field_type = p_data[KEY_OFFSET];
  if (field_len != 7)
  {
    Proto_RespNegative(obj, EPB_ERR_INVALID_KEY_LENGTH);
    return EPB_ERR_INVALID_KEY_LENGTH;
  }
  memory_reader_process(obj, field_type, p_data + VALUE_OFFSET, field_len - 1);

  return EPB_SUCCESS;
}

