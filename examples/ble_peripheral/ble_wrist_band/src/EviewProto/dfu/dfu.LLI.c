
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "dfu.h"
#include "LibHeap.h"
#include "proto.pb.h"
#include "DateTime.h"
//#include "flash_config.h"
#include "app_flash_drv.h"
#include "app_fw_info.h"
#include "HalDelay.h"
/**
 * DFU的状态
 * 
 * @author hzhmc (2018/5/6)
 */
//dfu 超时时间。
static uint32_t  		m_dfu_timeout;

static objDFU_t *pObj;


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#if 1
  #define DFU_LOG       NRF_LOG_INFO
#else
  #define DFU_LOG(...)
#endif

/**
 * DFU复位要满足两个条件， 
 * 1.外部请求复位。 
 * 2.Setting 参数写完。 
 * 
 * @author hzhmcu (2018/5/9)
 * 
 * @return int32_t 
 */
static int32_t DFU_ResetDevice(objDFU_t *obj)
{
  if (obj->isReqReset == true
      && obj->isSettingUpdated == true)
  {
    return obj->cbDeviceReset();
  }
  return -1;
}

/**@brief DFU配置写完成回调,  如果为升级完成请求复位，在此复位（需要保证固件升级配置完整才能复位）
 * @param evt --flash操作事件
 * @param result --flash操作结果
 * @return null
 * @return
*/
static void on_dfu_setting_write_completed(uint32_t reson)
{
  //if dfu settting write completed, set device reset
  pObj->isSettingUpdated = true;
  DFU_ResetDevice(pObj);
}





/**
 * 初始化DFU参数及回调函数
 * 
 * @author hzhmcu (2018/5/9)
 * 
 * @param DeviceReset_CallBack 
 * 
 * @return int32_t 
 */
static int32_t LLI_Init(objDFU_t *obj, int32_t(*DeviceReset_CallBack)())
{
  pObj = obj;
  obj->write_offset = 0;
  obj->state = DFU_STA_INIT_COMMAND;
  m_dfu_timeout = 0;
  return 0;
}

/**@brief 请求升级
 * @param [in] p_dfu_req 请求升级使用的初始化信息
 * @param [out] p_dfu_res 请求升级初始化结果
 * @return
 * @return
*/
static int32_t dfu_req_init(objDFU_t *obj, dfu_req_t *p_dfu_req, dfu_res_t *p_dfu_res)
{

  //(void) ble_conn_params_change_conn_params( (ble_gap_conn_params_t*)& dfu_conn_params );


  p_dfu_res->p_data[0] = FIRM_KEY_REQ_UPDATE;
  p_dfu_res->p_data[1] = EPB_SUCCESS;
  p_dfu_res->size 		= 2;

  //
  pb_create_command_t req_init = { 0 };


  obj->encrypt_crc32 = 0;


  //对req_init参数进行初始化
  if (DFUInitCmd_Decode(p_dfu_req->p_data, p_dfu_req->size, &req_init))
  {

    obj->encrypt_crc32 = req_init.init.encrypt_crc32;

    APP_FREE(obj->init_cmd);
    obj->init_cmd = (uint8_t *)APP_MALLOC(DFU_REQ_INIT_COMMAND, p_dfu_req->size);
    
    if (obj->init_cmd == NULL)
    {
      p_dfu_res->p_data[1] = EPB_ERR_NO_MEM;
      return EPB_ERR_NO_MEM;
    }
    memcpy(obj->init_cmd, p_dfu_req->p_data, p_dfu_req->size);
    obj->init_cmd_len = p_dfu_req->size;

    obj->crc32 = req_init.init.crc32;
    obj->size = req_init.init.app_size;
    obj->write_offset = 0;

    obj->state = DFU_STA_WRITE_FIRMWARE;
    obj->firmware_start_addr = FIRMWARE_START_ADDR;
     
    obj->req_size = 0;
    obj->receive_size = 0;

    //dfu
    dfu_flash_init();

  } else
  {
    p_dfu_res->p_data[1] = EPB_ERR_INVALID_PARAMETER;
    return EPB_ERR_INVALID_PARAMETER;
  }
  return EPB_SUCCESS;
}
/**@brief 请求写固件内容
 * @param p_dfu_req 固件命令 内容
 * @param p_dfu_res 固件内容写结果
 * @return
 * @return
*/
static int32_t dfu_req_write(objDFU_t *obj, dfu_req_t *p_dfu_req, dfu_res_t *p_dfu_res)
{
  p_dfu_res->p_data[0] = FIRM_KEY_XFER_CONTENT;
  p_dfu_res->p_data[1] = EPB_SUCCESS;
  p_dfu_res->size 		= 2;

  if (obj->state != DFU_STA_WRITE_FIRMWARE)
  {
    p_dfu_res->p_data[1] = EPB_ERR_INVALID_STATE;
    return EPB_ERR_INVALID_STATE;
  }

  if (p_dfu_req->size > 0 && (p_dfu_req->size & 0x03) == 0 && (p_dfu_req->size > 7))
  {
    uint32_t offset = uint32_decode(p_dfu_req->p_data);
    if (obj->write_offset != offset)
    {
      p_dfu_res->p_data[1] = EPB_ERR_INVALID_PARAMETER;
      return EPB_ERR_INVALID_PARAMETER;
    }

    if (dfu_flash_write_update(offset + obj->firmware_start_addr, p_dfu_req->p_data + 4, p_dfu_req->size - 4))
    {

      obj->write_offset += (p_dfu_req->size - 4);
      //DFU_LOG("OFF SET:0x%x\r\n", obj->write_offset);
      if (obj->write_offset >= obj->size) // firmware receive completed
      {
        obj->state = DFU_STA_EXECUTE_VERIFY;
      } else
      {
        obj->receive_size += p_dfu_req->size - 4;

        // 累加成功数据，超过回复设置时，才发送回复包
        if (obj->receive_size > obj->req_size || obj->req_size == 0)
        {
          if (obj->req_size > 0) obj->receive_size -= obj->req_size;

          p_dfu_res->size = 5;
          uint32_encode(obj->write_offset, p_dfu_res->p_data + 1);
          return -1;
        } else
        {
          p_dfu_res->size = 0; // 不发送回复包
        }
      }
    } else
    {
      p_dfu_res->p_data[1] = EPB_ERR_NO_MEM;
      return EPB_ERR_NO_MEM;
    }
  } else
  {
    p_dfu_res->p_data[1] = EPB_ERR_DATA_SIZE_ERROR;
    return EPB_ERR_DATA_SIZE_ERROR;
  }
  return EPB_SUCCESS;

}


/**@brief 固件校验
 * @param p_dfu_res 校验结果及返回信息
 * @param null
 * @return
 * @return
*/
static int32_t dfu_req_validate(objDFU_t *obj, dfu_res_t *p_dfu_res)
{
  p_dfu_res->size = 2;
  p_dfu_res->p_data[0] = FIRM_KEY_VALIDATE;
  p_dfu_res->p_data[1] = EPB_SUCCESS;

  if (obj->state != DFU_STA_EXECUTE_VERIFY)
  {

    p_dfu_res->p_data[1] = EPB_ERR_INVALID_STATE;

    return EPB_ERR_INVALID_STATE;
  }
  uint32_t crc = dfu_flash_crc_compute(obj->firmware_start_addr, obj->size);
  if (crc != obj->encrypt_crc32)
  {    
    p_dfu_res->p_data[1] = EPB_ERR_CHECK_SUM;
    obj->state = DFU_STA_INIT_COMMAND;
    LIB_FREE(obj->init_cmd);
    obj->init_cmd_len = 0;
    return EPB_ERR_CHECK_SUM;
  }
  uint32_t err_code = 0;
  err_code = err_code;
  
  DFU_LOG("read flash crc :%x  recived encrypt_crc:%x obj_size:%x", crc, obj->encrypt_crc32, obj->size);

  if (obj->p_dfu_settings == NULL)
  {
     DFU_LOG("p_dfu_settings=NULL 1"); 
    obj->p_dfu_settings = (dfu_settings_t *)APP_MALLOC(DFU_SAVE_SETTING_BUFFER, sizeof(dfu_settings_t));
    if (obj->p_dfu_settings == NULL)
    {
      DFU_LOG("p_dfu_settings=NULL 2"); 
      p_dfu_res->p_data[1] = EPB_ERR_NO_MEM;
      return EPB_ERR_NO_MEM;
    }
  }
  err_code = DFUSetting_Save(obj->p_dfu_settings, obj->init_cmd, obj->init_cmd_len, on_dfu_setting_write_completed);
  
  DFU_LOG("DFUSetting_Save reslut: %d " ,err_code); 
  //释放资源
  LIB_FREE(obj->init_cmd);
  obj->init_cmd_len = 0;

  return EPB_SUCCESS;
}
/**@brief 固件升级 状态查询（查询当前状态决定需要发送的命令）
 * @param p_dfu_res 状态查询返回结果
 * @param null
 * @return
 * @return
*/
static int32_t dfu_req_query_state(objDFU_t *obj, dfu_res_t *p_dfu_res)
{
  p_dfu_res->p_data[0] = FIRM_KEY_QUERY;
  switch (obj->state)
  {
      case DFU_STA_INIT_COMMAND:
        p_dfu_res->size = 2;
        p_dfu_res->p_data[1] = FIRM_KEY_REQ_UPDATE;
        break;
      case DFU_STA_WRITE_FIRMWARE:
        p_dfu_res->size = 6;
        p_dfu_res->p_data[1] = FIRM_KEY_XFER_CONTENT;
        uint32_encode(obj->write_offset, p_dfu_res->p_data + 2);
        break;
      case DFU_STA_EXECUTE_VERIFY:
        p_dfu_res->size = 2;
        p_dfu_res->p_data[1] = FIRM_KEY_VALIDATE;
        break;
      default:
        obj->state = DFU_STA_INIT_COMMAND;
        p_dfu_res->size = 2;
        p_dfu_res->p_data[1] =  FIRM_KEY_UNKNOW_STATE;
        break;
  }

  //由DFU 格式发送。
  return -1;
}


static void DFU_Reset_device(objDFU_t *obj)
{

  obj->isReqReset = true;
  DFU_ResetDevice(obj);
}

/**@brief 复位设备请求
 * @param p_dfu_res 请求返回结果
 * @param null
 * @return
 * @return
*/
static int32_t dfu_req_reset_device(objDFU_t *obj, dfu_res_t *p_dfu_res)
{
  p_dfu_res->size = 2;
  p_dfu_res->p_data[0] = FIRM_KEY_RESET_DEVICE;
  p_dfu_res->p_data[1] = EPB_SUCCESS;
  DFU_Reset_device(obj);
  return EPB_SUCCESS;
}

/**@brief DFU回复包设置
 * @param 
 * @param null
 * @return
 * @return
*/
static int32_t dfu_req_packet_max_len_resp(objDFU_t *obj, dfu_req_t *p_dfu_req, dfu_res_t *p_dfu_res)
{
  uint16_t maxLen = (EPB_PAYLOAD_DOWN_MAX_LEN & 0xff00);
  p_dfu_res->size = 3;
  p_dfu_res->p_data[0] = FIRM_KEY_REQ_PACKET_MAX_LEN;
  p_dfu_res->p_data[1] = maxLen & 0x00;
  p_dfu_res->p_data[2] = maxLen >> 8;
  return -1;
}

/**@brief DFU回复包设置
 * @param 
 * @param null
 * @return
 * @return
*/
static int32_t dfu_req_packets_resp(objDFU_t *obj, dfu_req_t *p_dfu_req, dfu_res_t *p_dfu_res)
{
  p_dfu_res->size = 2;
  p_dfu_res->p_data[0] = FIRM_KEY_RESP_PACKET;
  p_dfu_res->p_data[1] = EPB_SUCCESS;
  if (p_dfu_req->size != 4)
  {
    p_dfu_res->p_data[1] = EPB_ERR_DATA_SIZE_ERROR;
    return EPB_ERR_DATA_SIZE_ERROR;
  }
  // 设置回复包阈值
  obj->req_size = uint16_decode(p_dfu_req->p_data);
  return EPB_SUCCESS;
}


/**
 * 固件升级 命令解码及处理
 * 
 * @author hzhmcu (2018/5/9)
 * 
 * @param p_dfu_req [in]命令请求
 * @param p_dfu_res [out]命令处理结果
 * 
 * @return bool 
 */
static int32_t PutReq(objDFU_t *obj, dfu_req_t *p_dfu_req, dfu_res_t *p_dfu_res)
{
  m_dfu_timeout = RunTime_GetValue();

  int32_t success = 0;

  switch (p_dfu_req->cmd)
  {
    //请求升级，把*.dat 下发下来。
    //对信息进行解释，并对m_dfu_app进行赋值。
      case FIRM_KEY_REQ_UPDATE:
        success = dfu_req_init(obj, p_dfu_req, p_dfu_res);
        break;

        //对flash进行写内容。
      case FIRM_KEY_XFER_CONTENT:
        //DFU_LOG("[DFU]: Write Flash\r\n");
        success = dfu_req_write(obj, p_dfu_req, p_dfu_res);
        break;


        //固件校验，只作长度检查，并到*.dat写到内部Flash当中。
      case FIRM_KEY_VALIDATE:
        success = dfu_req_validate(obj, p_dfu_res);
        if (success == EPB_SUCCESS)
        {
          DFU_LOG("****Reset Device****\r\n");

          DFU_Reset_device(obj);
        }
        break;

        //查询状态。
      case FIRM_KEY_QUERY:
        success = dfu_req_query_state(obj, p_dfu_res);
        //DFU_LOG("---FIRM_KEY_QUERY---\r\n");
        break;

        //请求复位设备。
      case FIRM_KEY_RESET_DEVICE:
        success = dfu_req_reset_device(obj, p_dfu_res);
        //DFU_LOG("---FIRM_KEY_RESET_DEVICE---\r\n")
        break;

        //设置回复包阈值
      case FIRM_KEY_RESP_PACKET:
        success = dfu_req_packets_resp(obj, p_dfu_req, p_dfu_res);
        break;

        //设置回复包阈值
      case FIRM_KEY_REQ_PACKET_MAX_LEN:
        success = dfu_req_packet_max_len_resp(obj, p_dfu_req, p_dfu_res);
        break;

      default:
        success = EPB_ERR_INVALID_FORMAT;
        break;
  }
  return success;
}




/**
 * 查询DFU当前的状态
 * 
 * @author hzhmcu (2018/5/9)
 * 
 * @return DFU_State_t 
 */
static DFU_State_t GetState(objDFU_t *obj)
{
  return obj->state;
}









/**@brief 固件升级过程处理，轮询方法
 * @param null
 * @param null
 * @return
 * @return
*/

static int32_t Execute(objDFU_t *obj)
{
  if (obj->state != DFU_STA_INIT_COMMAND)
  {

    //这时Flash操作，不应该在这里出来。
//#warning "flash 的操作不应该在这里。"
    dfu_flash_process();

    if ((m_dfu_timeout + (10 * 60)) < RunTime_GetValue())
    {

      obj->state = DFU_STA_INIT_COMMAND;
      APP_FREE(obj->init_cmd);
      APP_FREE(obj->p_dfu_settings);
    }
    return 1;
  }
  return 0;
}


const ApiDFU_t scg_sApiLLIDFU = {
  .LLI_Init = LLI_Init,
  .PutReq = PutReq,
  .GetState = GetState,
  .SetIdleState = NULL,
  .Execute = Execute,
};

