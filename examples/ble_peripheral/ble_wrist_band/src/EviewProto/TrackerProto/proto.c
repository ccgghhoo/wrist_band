#include "proto.h"
#include "proto.service.h"
#include "proto.memory.h"
#include "proto.record.h"
#include "proto.negative.h"
#include "proto.control.h"
#include "proto.debug.h"
#include "proto.factory.h"
#include "proto.blebase.h"
const uint8_t key = 3;
#define KEY_SIZE(keys) (sizeof(keys)/sizeof(keys[0]))
/**
 * epb proto cmd处理函数处理表。 
 * 
 * @author hzhmc (2018/5/5)
 */
static const EpbProtoCmd_t scg_sCmds[] =
{
/*  
    {
        .cmd = COMMAND_ID_NEGATIVE,
        .handle = ProtoNegative_Handle,
        .keys = NULL,
        .pKeySize = NULL,
    },
    {
        //0x04
        .cmd = COMMAND_ID_SYS_CONTROL,
        .handle = NULL,
        .keys = scg_sProtoControlKeys,
        .pKeySize = &s_ucProtoControlKeySize,
    },
*/
    {
        //0x01
        .cmd = COMMAND_ID_DATA,
        .handle = NULL,
        .keys = scg_sProtoRecordKeys,
        .pKeySize = &s_ucProtoRecodeKeySize,
    },

    {
        //0x7E  dfu
        .cmd = COMMAND_ID_FIRMWARE,
        .handle = ProtoFirmware_Handle,
        .keys = NULL,
        .pKeySize = NULL,
    },
/*    
    {
        //cmd 0x02;
        .cmd = COMMAND_ID_CONFIG,
        .handle = ProtoConfig_Handle,
        .keys = NULL,
        .pKeySize = NULL,
    },
    {
        //0x03
        .cmd = COMMAND_ID_SERVICES,
        .handle = NULL,
        .keys = scg_sProtoSeviceKeys,
        .pKeySize = &s_ucProtoSevicKeySize,
    },

    {
        //0x05
        .cmd = COMMAND_ID_MEMORY_READER,
        .handle = ProtoMemory_Handle,
        .keys = NULL,
        .pKeySize = NULL,
    },

    {
        //0x05
        .cmd = COMMAND_ID_DEBUG,
        .handle = NULL,
        .keys = scg_sProtoDebugKeys,
        .pKeySize = &s_ucProtoDebugKeySize,
    },
    {
        .cmd = COMMAND_ID_FACOTRY_TEST,
        .handle = NULL,
        .keys = scg_sProtoFactroyKeys,
        .pKeySize = &s_sProtoFactroyKeysSize,
    },

    {
        .cmd = COMMAND_ID_BLE_BASE,
        .handle = NULL,
        .keys = scg_sProtoBLEBaseKeys,
        .pKeySize = &s_ucProtoBLEBaseKeySize,
    },*/

};

static uint8_t scg_ucCmdSize = sizeof(scg_sCmds) / sizeof(scg_sCmds[0]);


EpbProto_Define(objProto);



/**
 * 初始化协议，把上层协议的对象进行初始化
 * 
 * @author hzhmc (2018/5/5)
 */
void Proto_Init()
{
    EpbProto_Init(objProto, scg_sCmds, scg_ucCmdSize);
}




/**
 * 底层向上层传送包的接口。
 * 
 * @author hzhmc (2018/5/5)
 * 
 * @param p_msg 消息包
 * @param resp_cb 应答回调函数
 * 
 * @return int 
 */
int32_t Proto_PutPack(msg_packet_t *p_msg, resp_callback_t resp_cb)
{
    return EpbProto_PutPacket(objProto, p_msg, resp_cb);
}
/**
 *  返回应答包，并反馈数据。
 * 
 * @author hzhmc (2018/5/5)
 * 
 * @param flag 标志
 * @param data 数据
 * @param len 长度
 * 
 * @return int32_t 
 */
int32_t Proto_Resp(objEpbProto_t *obj, uint8_t flag, uint8_t *data, uint16_t len)
{
    //PB_LOG("Send Resp len %d\r\n",len);
    return _EpbProto_Resp(obj, flag, data, len);
}


/**
 * 回应答包。
 * 
 * @author hzhmc (2018/5/5)
 * 
 * @return int 
 */
int32_t Proto_RespAck(objEpbProto_t *obj)
{
    return Proto_Resp(obj, FLAG_ACK_RESP, NULL, 0);
}
/**
 * 返回应答包，正确包
 * 
 * @author hzhmc (2018/5/5)
 *  
 * @param code 错误码
 * 
 * @return int32_t 
 */
int32_t Proto_RespPositive(objEpbProto_t *obj, uint8_t cmd, uint8_t key)
{
    uint8_t     data[3];
    data[0] = cmd;
    data[1] = 1;
    data[2] = key;

    return Proto_Resp(obj, FLAG_NORMAL_RESP, data, 3);
}

/**
 * 返回应答包，错误码
 * 
 * @author hzhmc (2018/5/5)
 *  
 * @param code 错误码
 * 
 * @return int32_t 
 */
int32_t Proto_RespNegative(objEpbProto_t *obj, uint8_t code)
{
    uint8_t 	data[3];
    data[0] = 	COMMAND_ID_NEGATIVE;
    data[1] = 	1;
    data[2] = 	code;
    return Proto_Resp(obj, FLAG_NORMAL_RESP, data, 3);
}







