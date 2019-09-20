
#include "LibConfig.h"
#include "EpbProto.h"
#include "LibHeap.h"
#include "crc16.h"
#include "proto.pb.h"
#if 1
#define PB_LOG				NRF_LOG_INFO
#else
#define PB_LOG(...)
#endif



/**
 * 初始化把对象进行初始化
 * 
 * @author hzhmc (2018/5/5)
 * 
 * @param obj 对象
 * @param cmds 子层cmd的回调函数结构体
 * @param cmdSize 子层cmd的大小。
 */
void _EpbProto_Init(objEpbProto_t *obj, const EpbProtoCmd_t *cmds, uint8_t cmdSize)
{
    obj->cmds = cmds;
    obj->cmdSize = cmdSize;
    for (int i = 0; i < cmdSize; i++)
    {
        if (cmds[i].keys != NULL && cmds[i].pKeySize == NULL)
        {
            //长度赋值
            while (1);
        }
    }

}










/**
 * 发送包数据。
 * 
 * @author hzhmc (2018/5/5)
 * 
 * @param obj 
 * @param flagType 
 * @param data 
 * @param len 
 * 
 * @return int32_t 
 */
int32_t _EpbProto_Resp(objEpbProto_t *obj, uint8_t flagType,
                       uint8_t *data, uint16_t len)
{
    obj->isSendResp = false;
    if (obj->resp_cb == NULL)
    {
        return EPB_ERR_INVALID_PARAMETER;
    }

    uint8_t *data_buff = (uint8_t *)APP_MALLOC(PROTO_PACK_DATA_PACK, len + 8);
    if (data_buff == NULL) //no more heap to alloc
    {
        PB_LOG("[PACK]: no more buffer \r\n ");
        return EPB_ERR_NO_MEM;
    }


    msg_packet_t 	respHead;
    respHead.magic	= MAGIC_NUMBER;
    respHead.len	= len;

    respHead.val	= flagType;
    respHead.id	= obj->pMsg->id;
    //Mark 不清楚这个参数有何作用？
    obj->sendId	= obj->pMsg->id;



    if (len > 0)
    {
        uint16_t crc16 = crc16_compute(data, len, 0);
        respHead.crc = crc16;
    }
    else
    {
        respHead.crc = 0;
    }

    memcpy(data_buff, (uint8_t *)&respHead, 8);
    memcpy(data_buff + 8, data, len);

    if (obj->resp_cb != NULL)
    {
        obj->resp_cb(data_buff, len + 8);
    }
    else
    {
        APP_FREE(data_buff);
        PB_LOG("[PACK]E: callback is NULL!!\r\n");
    }
    return EPB_SUCCESS;
}

#if 0

/**
 * 返回应答包，错误码
 * 
 * @author hzhmc (2018/5/5)
 * 
 * @param obj 
 * @param code 错误码
 * @return int32_t 
 */
int32_t _EpbProto_RespNegative(objEpbProto_t *obj,uint8_t code)
{
    uint8_t 	data[2];
    data[0] = 	COMMAND_ID_NEGATIVE;
    data[1] = 	code;

    return _EpbProto_Resp(obj,  FLAG_ERROR_RESP, data, 2);
}

/**
 * 返回应答包，ACK包
 * 
 * @author hzhmc (2018/5/5)
 * 
 * @param obj 
 * @return int32_t 
 */
int32_t _EpbProto_RespAck( objEpbProto_t *obj)
{
    return _EpbProto_Resp(obj, FLAG_ACK_RESP, NULL, 0);
}
#endif
static int  _EpbProto_Disperse(objEpbProto_t *obj, uint8_t cmdIndex, uint8_t *data, int32_t len)
{
    int32_t reset_len = len;
    uint8_t key = 0;
    int32_t key_len = 0;
    uint8_t *p = data;
    int i = 0;
    uint8_t cnt = 0;
    bool isFirst = true;
    bool key_len_zero = false;
    while (reset_len > 0)
    {
        key_len = p[0];

        key = p[1];
        if (isFirst == true && key_len == 0)
        {
            key_len = len - 1;
            key_len_zero = true;
        }
        else if (reset_len < key_len + 1) //长度不足
        {
            return EPB_ERR_INVALID_PARAMETER;
        }
        
        // PB_LOG("reset=%d, key len= %d,key=0x%X\r\n", reset_len, key_len, key);
        for (i = 0; i < obj->cmds[cmdIndex].pKeySize[0]; i++)
        {
            if (obj->cmds[cmdIndex].keys[i].key == key)
            {
                cnt++;
                if (obj->cmds[cmdIndex].keys[i].handle)
                {
                    
                    if (obj->cmds[cmdIndex].keys[i].handle(obj, key, p + 2, key_len - 1) != EPB_SUCCESS)
                    {
                        //进行其他操作。
                    }
                    if(key_len_zero == true)
                    {
                      return 0;
                    }
                }

                break;
            }
        }

        //不支持此命令。
        reset_len -= key_len + 1;
        p += key_len + 1;
        isFirst = false;
    }
    return 0;
}

/**
 * 通信协议接收到数据，统一入口。
 * 
 * @author hzhmcu (2018/5/4)
 * 
 * @param p_msg 需要把数据进行封包
 * @param evt_handle 响应回调函数 
 * @return int  
 */
int _EpbProto_PutPacket(objEpbProto_t *obj, msg_packet_t *p_msg, resp_callback_t resp_cb)
{
    uint16_t crc16;
    uint8_t i;
    uint8_t cmd;
    if (p_msg == NULL || p_msg->payload == NULL || resp_cb == NULL)
    {
        PB_LOG("[PB]:NULL!!\r\n");
        return EPB_ERR_INVALID_PARAMETER;
    }

    crc16 = crc16_compute(p_msg->payload, p_msg->len, 0);

    if (crc16 != p_msg->crc)
    {
        PB_LOG("[PB]:CRC!!\r\n");
        return EPB_ERR_CHECK_SUM;
    }

    //对象进行初始化
    obj->isSendResp = true;
    obj->resp_cb = resp_cb;
    obj->pMsg = p_msg;
    if (obj->resp_cb == NULL)
    {
        PB_LOG("[PACK1]: callback is NULL!!\r\n");
    }
    cmd = p_msg->payload[COMMAND_OFFSET];
    //查找处理函数
    for (i = 0; i < obj->cmdSize; i++)
    {
        if (cmd == obj->cmds[i].cmd)
        {
            if (obj->cmds[i].pKeySize[0] != 0 && obj->cmds[i].keys != NULL)
            {
                // p_msg->len - 1
                _EpbProto_Disperse(obj, i, p_msg->payload + DATA_OFFSET, p_msg->len - DATA_OFFSET);
            }
            else if (obj->cmds[i].handle != NULL)
            {
                obj->cmds[i].handle(obj, p_msg->payload, p_msg->len);
            }

            break;
        }
    }

    LibStack_Debug();
    
    if (obj->resp_cb == NULL)
    {
        PB_LOG("[PB]: callback is NULL!!\r\n");
    }
    // if need ack, send an ack response when no response has been sent
    if (p_msg->flag.ack > 0 && obj->isSendResp == true)
    {

        uint8_t 	data[3];
        data[0] = 	COMMAND_ID_NEGATIVE;
        data[1] = 	1;
        data[2] = 	EPB_SUCCESS;
        //ack resp
        _EpbProto_Resp(obj, FLAG_NORMAL_RESP, data, sizeof(data));

    }
    return EPB_SUCCESS;
}


