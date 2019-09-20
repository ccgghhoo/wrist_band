

#ifndef __DFU_H
#define __DFU_H

#include <stdint.h>
#include <stdbool.h>
#include "LibConfig.h"

#include "dfu.init_command.h"

#include "dfu.h"
#include "dfu.types.h"
#include "dfu.flash.h"
#include "dfu.settings.h"


typedef struct objDFU objDFU_t;

/*


typedef enum {
  DFU_RES_SUCCESS = 0,
  DFU_RES_ERROR_INVALID_PARAM,
  DFU_RES_ERROR_NO_MEM,
  DFU_RES_ERROR_DATA_SIZE,
  DFU_RES_ERROR_ADDR,
  DFU_RES_ERROR_STATE,
  DFU_RES_ERROR_CRC,
  DFU_RES_UNKNOW_STATE,
}dfu_res_ext_code_t;
*/


typedef enum { //epb_key_of_firmware_t

  FIRM_KEY_REQ_UPDATE		= 0x10,
  FIRM_KEY_XFER_CONTENT, //0X11
  FIRM_KEY_VALIDATE, //0X12
  FIRM_KEY_QUERY, //0X13 ��ѯ��ǰ��״̬��
  FIRM_KEY_RESET_DEVICE, //0X14
  FIRM_KEY_REQ_PACKET_MAX_LEN,
  FIRM_KEY_RESP_PACKET = 0x20,


  FIRM_KEY_UNKNOW_STATE = 0XFF,
}epb_key_of_firmware_t;



typedef enum
{

  DFU_STA_INIT_COMMAND = 0, //DFU����״̬�����Խ����ʼ������״̬��
  DFU_STA_WRITE_FIRMWARE, //DFU��д��FLASH״̬
  DFU_STA_EXECUTE_VERIFY, //DFU����֤�׶�
  DFU_STA_COMPLETED, //DFU���
  DFU_STA_UNDEFINED,
}DFU_State_t;


/**
 * dfu �����ʽ��
 */
typedef struct {
  uint8_t 	cmd;
  uint8_t *p_data;
  uint16_t 	size;
}dfu_req_t;

/**
 * DFU��Ӧ��
 */
typedef struct {
  uint8_t *p_data;
  uint16_t 	size;
}dfu_res_t;




typedef struct {
  /**
 * ��ʼ��DFU�������ص�����
 * 
 * @author hzhmcu (2018/5/9)
 * 
 * @param DeviceReset_CallBack 
 * 
 * @return int32_t 
 */
  int32_t (*Init)(objDFU_t *obj, int32_t(*DeviceReset_CallBack)());

  /**
*LLI ��ʼ��DFU�������ص�����
* 
* @author hzhmcu (2018/5/9)
* 
* @param DeviceReset_CallBack 
* 
* @return int32_t 
*/
  int32_t (*LLI_Init)(objDFU_t *obj, int32_t(*DeviceReset_CallBack)());
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
  int32_t (*PutReq)(objDFU_t *obj, dfu_req_t *p_dfu_req, dfu_res_t *p_dfu_res);
  /**
 * ��ѯDFU��ǰ��״̬
 * 
 * @author hzhmcu (2018/5/9)
 * 
 * @return DFU_State_t 
 */
  DFU_State_t (*GetState)(objDFU_t *obj);
  /**
 * ȡ�������� 
 * �ⲿֻ�����ô�״̬�� 
 * �����е���Դ���ͷš� 
 *  
 * @author hzhmcu (2018/5/9)
 * 
 * @return DFU_State_t 
 */
  DFU_State_t (*SetIdleState)(objDFU_t *obj);
  /**
* ȡ�������� 
* �ⲿֻ�����ô�״̬�� 
* �����е���Դ���ͷš� 
*  
* @author hzhmcu (2018/5/9)
* 
* @return DFU_State_t 
*/
  int32_t (*Execute)(objDFU_t *obj);
}ApiDFU_t;


extern const ApiDFU_t scg_sApiDFU;


#define pApiDFU (&scg_sApiDFU)


struct objDFU
{
  uint32_t encrypt_crc32;


  uint32_t crc32; //app crc32
  uint32_t size;  //app �Ĵ�С
  uint32_t write_offset; //д���ĵ�ַ

  DFU_State_t state; //DFU״̬

  uint8_t *init_cmd;
  uint32_t init_cmd_len; //

  uint32_t firmware_start_addr; //�̼���ʼ��ַ

  /**
   * ֻ�е�������ͬʱΪ��ʱ����DFU��λ��
   * 
   * @author hzhmc (2018/5/6)
   */
  bool isSettingUpdated; //Setting��Ϣ�Ƿ�д��
  bool isReqReset; //�Ƿ�����λ


  //
  uint32_t req_size; //���յ����ٸ����ݣ��ͻ�һ��Ӧ��
  uint32_t receive_size; //��ǰ���յ����ݵĳ��ȡ�


  /**
   * �豸��λ�ص����� 
   */

  int32_t (*cbDeviceReset)();

  //���涯̬���ɵ�p_dfu_settings
  dfu_settings_t *p_dfu_settings;


  const ApiDFU_t *pLLI; //Lower Layer Interface �ײ�ʵ�ֽӿڡ�

};

extern  objDFU_t sg_objDFU;
#define objDFU (&sg_objDFU)


#if 1
  #define DFU_LOG(...)
#else
  #define DFU_LOG(...)
#endif

#endif

