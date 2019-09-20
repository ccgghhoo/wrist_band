#include "proto.firmware.h"
#include "dfu.h"

/**
 *  
 * cmd len key value 
 * 
 * @author hzhmcu (2018/5/31)
 * 
 * @param obj 
 * @param p_data 
 * @param len 
 */
static void Duf_Handle(objEpbProto_t *obj,  const uint8_t *p_data, uint16_t len)
{
    dfu_req_t	dfu_req;
    dfu_req.cmd = p_data[2];
    dfu_req.p_data = (uint8_t *)(p_data + 3);
    dfu_req.size = len - 3;


    uint8_t 	temp[10];
    dfu_res_t 	dfu_res;
    dfu_res.p_data = temp + 2;
    dfu_res.size = 0;
    temp[0] = COMMAND_ID_FIRMWARE;
    temp[1] = 0;
    int32_t success = pApiDFU->PutReq(objDFU, &dfu_req, &dfu_res);
    if (success < 0)
    //由DFU的格式发送。
    {
        //temp[ 1 ] = dfu_res.size;
        if (dfu_res.size > 0)
        {
            Proto_Resp(obj, FLAG_NORMAL_RESP, temp, dfu_res.size + 2);
        }
    }
    else
    //错误码格式发送。
    {
        Proto_RespNegative(obj, success);
    }
}


/**
 * firmware 处理函数
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
int ProtoFirmware_Handle(struct objEpbProto *obj,
                         const uint8_t *p_data, uint16_t len)
{

    // cmd - key - value ( no length of this command )
//	uint8_t field_len = p_data[ LENGTH_OFFSET ];


    //长度不足，返回错误码。
    if (len < 2)
    {
        Proto_RespNegative(obj, EPB_ERR_INVALID_KEY_LENGTH);
        PB_LOG("[PFirmware] EPB_ERR_INVALID_KEY_LENGTH !!\r\n");
        return EPB_ERR_INVALID_KEY_LENGTH;
    }
//	uint8_t field_type = p_data[1];

    // command id / field len / keyword / value
    Duf_Handle(obj,  p_data, len);
    


    return EPB_SUCCESS;
}

