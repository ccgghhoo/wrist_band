#include "proto.pb.h"
#include "proto.LOG.h"
#include "app_timer.h"
#if defined(__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined(__ICCARM__)
  #pragma language=extended
#elif defined(__GNUC__)
// anonymous unions are enabled by default
#endif

typedef union {
  struct {
    uint32_t err:1; //´íÎó
    uint32_t rvs:30;
    uint32_t enable:1;
  };
  uint32_t value;
}LogInfo_t;

#if defined(__CC_ARM)
  #pragma pop
#elif defined(__ICCARM__)
// leave anonymous unions enabled
#elif defined(__GNUC__)
// anonymous unions are enabled by default
#endif

static int ProtoLog_Control(objEpbProto_t *obj, uint8_t field_type, const uint8_t *p_data, uint16_t len);


static LogInfo_t sg_sLogInfo = { .enable = false };


bool BLELog_IsEnable()
{
  return sg_sLogInfo.enable;
}


const EpbProtoKey_t scg_sProtoLOGKeys[] =
{
  {
    .key = LOG_KEY_CONTROL,
    .handle = ProtoLog_Control,
  },



};
const uint8_t s_ucProtoLOGKeySize = sizeof(scg_sProtoLOGKeys) / sizeof(scg_sProtoLOGKeys[0]);

bool ProtoLog_IsEnable()
{
  return sg_sLogInfo.enable;
}
static bool isInitTimer = false;
static int ProtoLog_Control(objEpbProto_t *obj, uint8_t field_type, const uint8_t *p_data, uint16_t len)
{

  sg_sLogInfo.value = uint32_decode(p_data);
  if (isInitTimer == false)
  {
    isInitTimer = true;
   
  }

  if (sg_sLogInfo.enable == true)
  {

  }
  Proto_RespNegative(obj, EPB_SUCCESS);
  return 0;
}








