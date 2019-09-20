#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "dfu.h"
#include "dfu.LLI.h"
#include "Libheap.h"
static void InitParam(objDFU_t *obj)
{
  obj->isReqReset = false;
  obj->isSettingUpdated = false;
  obj->init_cmd_len = 0;
  obj->state = DFU_STA_INIT_COMMAND;
  LIB_FREE(obj->p_dfu_settings);
  LIB_FREE(obj->init_cmd);
  
}


static __INLINE int32_t Init(objDFU_t *obj, int32_t(*DeviceReset_CallBack)())
{
  if (obj == NULL) return ERR_OBJ_NULL;

  //底层有实现就调用底层函数，
  if (obj->pLLI != NULL)
  {
    if (obj->pLLI->LLI_Init)
    {
      obj->pLLI->LLI_Init(obj, DeviceReset_CallBack);
    }
    if (obj->pLLI->Init)
    {
      return obj->pLLI->Init(obj, DeviceReset_CallBack);
    }
  }

  if (DeviceReset_CallBack == NULL)
  {
    return -1;
  }
  InitParam(obj);
  obj->cbDeviceReset = DeviceReset_CallBack;

  return SUCCESS;

}


static __INLINE int32_t PutReq(objDFU_t *obj, dfu_req_t *p_dfu_req, dfu_res_t *p_dfu_res)
{
  if (obj == NULL || obj->pLLI == NULL) return ERR_LLI_NULL;
  return obj->pLLI->PutReq(obj, p_dfu_req, p_dfu_res);
}

static __INLINE DFU_State_t GetState(objDFU_t *obj)
{
  if (obj == NULL) return DFU_STA_UNDEFINED;

  //底层有实现就调用底层函数，
  if (obj->pLLI != NULL ||  obj->pLLI->GetState)
  {
    return obj->pLLI->GetState(obj);
  }


  return obj->state;
}


static __INLINE DFU_State_t  SetIdleState(objDFU_t *obj)
{
  if (obj == NULL) return DFU_STA_UNDEFINED;

  //底层有实现就调用底层函数，
  if (obj->pLLI != NULL ||  obj->pLLI->GetState)
  {
    return obj->pLLI->SetIdleState(obj);
  }



  if (obj->init_cmd_len > 0 || obj->init_cmd != NULL)
  {
    LIB_FREE(obj->init_cmd);
    obj->init_cmd_len = 0;
  }

  InitParam(obj);
  return obj->state;
}
static __INLINE int32_t Execute(objDFU_t *obj)
{
  if (obj == NULL || obj->pLLI == NULL) return ERR_LLI_NULL;



  return obj->pLLI->Execute(obj);
}

const ApiDFU_t scg_sApiDFU = {
  .Init = Init,
  .PutReq = PutReq,
  .GetState = GetState,
  .SetIdleState = SetIdleState,
  .Execute = Execute,
};


objDFU_t sg_objDFU = {
  .pLLI = &scg_sApiLLIDFU,
  .write_offset = 0,
 
};


