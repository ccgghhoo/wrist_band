
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
 * DFU��״̬
 * 
 * @author hzhmc (2018/5/6)
 */
//dfu ��ʱʱ�䡣
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
 * DFU��λҪ�������������� 
 * 1.�ⲿ����λ�� 
 * 2.Setting ����д�ꡣ 
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

/**@brief DFU����д��ɻص�,  ���Ϊ�����������λ���ڴ˸�λ����Ҫ��֤�̼����������������ܸ�λ��
 * @param evt --flash�����¼�
 * @param result --flash�������
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
 * ��ʼ��DFU�������ص�����
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

/**@brief ��������
 * @param [in] p_dfu_req ��������ʹ�õĳ�ʼ����Ϣ
 * @param [out] p_dfu_res ����������ʼ�����
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


  //��req_init�������г�ʼ��
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
/**@brief ����д�̼�����
 * @param p_dfu_req �̼����� ����
 * @param p_dfu_res �̼�����д���
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

        // �ۼӳɹ����ݣ������ظ�����ʱ���ŷ��ͻظ���
        if (obj->receive_size > obj->req_size || obj->req_size == 0)
        {
          if (obj->req_size > 0) obj->receive_size -= obj->req_size;

          p_dfu_res->size = 5;
          uint32_encode(obj->write_offset, p_dfu_res->p_data + 1);
          return -1;
        } else
        {
          p_dfu_res->size = 0; // �����ͻظ���
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


/**@brief �̼�У��
 * @param p_dfu_res У������������Ϣ
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
  //�ͷ���Դ
  LIB_FREE(obj->init_cmd);
  obj->init_cmd_len = 0;

  return EPB_SUCCESS;
}
/**@brief �̼����� ״̬��ѯ����ѯ��ǰ״̬������Ҫ���͵����
 * @param p_dfu_res ״̬��ѯ���ؽ��
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

  //��DFU ��ʽ���͡�
  return -1;
}


static void DFU_Reset_device(objDFU_t *obj)
{

  obj->isReqReset = true;
  DFU_ResetDevice(obj);
}

/**@brief ��λ�豸����
 * @param p_dfu_res ���󷵻ؽ��
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

/**@brief DFU�ظ�������
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

/**@brief DFU�ظ�������
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
  // ���ûظ�����ֵ
  obj->req_size = uint16_decode(p_dfu_req->p_data);
  return EPB_SUCCESS;
}


/**
 * �̼����� ������뼰����
 * 
 * @author hzhmcu (2018/5/9)
 * 
 * @param p_dfu_req [in]��������
 * @param p_dfu_res [out]�������
 * 
 * @return bool 
 */
static int32_t PutReq(objDFU_t *obj, dfu_req_t *p_dfu_req, dfu_res_t *p_dfu_res)
{
  m_dfu_timeout = RunTime_GetValue();

  int32_t success = 0;

  switch (p_dfu_req->cmd)
  {
    //������������*.dat �·�������
    //����Ϣ���н��ͣ�����m_dfu_app���и�ֵ��
      case FIRM_KEY_REQ_UPDATE:
        success = dfu_req_init(obj, p_dfu_req, p_dfu_res);
        break;

        //��flash����д���ݡ�
      case FIRM_KEY_XFER_CONTENT:
        //DFU_LOG("[DFU]: Write Flash\r\n");
        success = dfu_req_write(obj, p_dfu_req, p_dfu_res);
        break;


        //�̼�У�飬ֻ�����ȼ�飬����*.datд���ڲ�Flash���С�
      case FIRM_KEY_VALIDATE:
        success = dfu_req_validate(obj, p_dfu_res);
        if (success == EPB_SUCCESS)
        {
          DFU_LOG("****Reset Device****\r\n");

          DFU_Reset_device(obj);
        }
        break;

        //��ѯ״̬��
      case FIRM_KEY_QUERY:
        success = dfu_req_query_state(obj, p_dfu_res);
        //DFU_LOG("---FIRM_KEY_QUERY---\r\n");
        break;

        //����λ�豸��
      case FIRM_KEY_RESET_DEVICE:
        success = dfu_req_reset_device(obj, p_dfu_res);
        //DFU_LOG("---FIRM_KEY_RESET_DEVICE---\r\n")
        break;

        //���ûظ�����ֵ
      case FIRM_KEY_RESP_PACKET:
        success = dfu_req_packets_resp(obj, p_dfu_req, p_dfu_res);
        break;

        //���ûظ�����ֵ
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
 * ��ѯDFU��ǰ��״̬
 * 
 * @author hzhmcu (2018/5/9)
 * 
 * @return DFU_State_t 
 */
static DFU_State_t GetState(objDFU_t *obj)
{
  return obj->state;
}









/**@brief �̼��������̴�����ѯ����
 * @param null
 * @param null
 * @return
 * @return
*/

static int32_t Execute(objDFU_t *obj)
{
  if (obj->state != DFU_STA_INIT_COMMAND)
  {

    //��ʱFlash��������Ӧ�������������
//#warning "flash �Ĳ�����Ӧ�������"
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

