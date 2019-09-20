#include "LibConfig.h"
#include "dfu.h"
#include "app_fw_info.h"
#include "dfu.init_command.h"
#include "dfu.types.h"





/**
 * 对为固件描述文件 进行解释，并保存到req_cmd当中
 *  
 * 并对软件是否适合这个硬件进行管理。 
 *  
 * @author hzhmc (2018/5/6)
 * 
 * @param p_data 来的数据。
 * @param len 
 * @param [out]req_cmd 
 * 
 * @return bool 
 */
bool DFUInitCmd_Decode(const uint8_t *p_data, uint16_t len, pb_create_command_t *req_cmd)
{
  if (len > PB_CREATE_COMMAND_SIZE) return NRF_ERROR_DATA_SIZE;

//	pb_create_command_t	req_cmd;
//	req_cmd->init.sd_req_count = 0;
  DFU_LOG("[DFUinit]: data len =%d\r\n ", len);
  uint16_t index = 0;

  while (index < len)
  {
    uint8_t field_len = p_data[index];
    uint8_t field_type = p_data[index + 1];
    DFU_LOG("[DFUinit]: len=%d,type =0x%x\r\n ", field_len, field_type);
    if ((field_len + index) > len) break;

    switch (field_type)
    {
        case PB_INIT_SIGN:
          req_cmd->has_signed_command = true;
          break;
        case PB_INIT_HASH:
          req_cmd->init.has_hash = true;
          break;
        case PB_INIT_CRC32:
          if (field_len == 5)
          {
            req_cmd->init.has_crc = true; //save this crc32
            req_cmd->init.crc32 = uint32_decode(p_data + index + 2);
          } else
          {
            return false;
          }
          break;

        case PB_INIT_ENCRYT_CRC32:
          if (field_len == 5)
          {
            req_cmd->init.has_encrypt_crc = true; //save this crc32
            req_cmd->init.encrypt_crc32 = uint32_decode(p_data + index + 2);
          } else
          {
            return false;
          }
          break;

        case PB_INIT_DFU_TYPE:
          if (field_len == 5)
          {
            req_cmd->init.has_type = true; //save this type
            req_cmd->init.type = (dfu_fw_type_t)uint32_decode(p_data + index + 2);
          } else
          {
            return false;
          }
          break;
        case PB_INIT_APP_SIZE:
          if (field_len == 5)
          {
            req_cmd->init.has_app_size = true; //save this size
            req_cmd->init.app_size = uint32_decode(p_data + index + 2);
          } else
          {
            return false;
          }
          break;
//			case PB_INIT_SD_SIZE:
//				req_cmd->init.has_sd_size = true; // error
//				break;
//			case PB_INIT_BL_SIZE:
//				req_cmd->init.has_bl_size = true;
//				break;
        case PB_INIT_FW_VER:
          if (field_len == 5)
          {
            req_cmd->init.has_fw_version = true;
            req_cmd->init.fw_version = uint32_decode(p_data + index + 2);
          } else
          {
            return false;
          }
          break;
        case PB_INIT_HW_VER:
          if (field_len == 5)
          {
            req_cmd->init.has_hw_version = true;
            req_cmd->init.hw_version = uint32_decode(p_data + index + 2);
          } else
          {
            return false;
          }
          break;
        case PB_INIT_SD_FWID:
          if (field_len == 5)
          {
            if (req_cmd->init.sd_req_count < 4)
            {
              req_cmd->init.sd_req[req_cmd->init.sd_req_count] = uint32_decode(p_data + index + 2);
              req_cmd->init.sd_req_count++;
            } else
            {
              return false;
            }
          }
          break;
        default:
          break;
    }

    index += field_len + 1;
  }

// start to check
  if (!req_cmd->has_signed_command) return false;

  if (!req_cmd->init.has_hash) return false;

  if (!req_cmd->init.has_crc) return false;

  if (!req_cmd->init.has_type) return false;

  if (req_cmd->init.type != DFU_FW_TYPE_APPLICATION) return false;

  if (!req_cmd->init.has_app_size) return false;
  if (req_cmd->init.app_size == 0 || req_cmd->init.app_size > DFU_APP_MAX_SIZE) return false;

  //if (req_cmd->init.fw_version < FW_REVISION) return false;

  if (req_cmd->init.hw_version != APP_FW_HW_VER) return false;


  if (req_cmd->init.sd_req_count == 0) return false;

  if (req_cmd->init.has_encrypt_crc == false) return false; 

  DFU_LOG("[DFUinit]:app size 0x%x,FW =%x,HW=%x\r\n ",
          req_cmd->init.app_size,
          req_cmd->init.fw_version,
          req_cmd->init.hw_version);

  for (uint32_t i = 0; i < req_cmd->init.sd_req_count; i++)
  {
    if (req_cmd->init.sd_req[i] == APP_FW_SD_FW_ID)
    {
      return true;
    }
  }

  return false;
}




