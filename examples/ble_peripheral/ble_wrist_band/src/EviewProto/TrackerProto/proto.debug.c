
#include "proto.debug.h"
#include "proto.pb.h"
//#include "mode.h"
//#include "record.h"
//#include "dev_conf.h"
//#include "dev_conf.pb.h"
#include "LibHeap.h"
//#include "storage.h"
//#include "Ls_api.h"
#include "DateTime.h"
#include "dev_config.get.h"
//#include "loc_gsm.h"
//#include "loc.mode.h"
//#include "loc_gps.util.h"
//#include "smsproto.h"
//#include "FM25q.h"
//#include "loc_gps.agps.h"
//#include "step_gsm.DownMusicFile.h"
//#include "flash_config.h"
static int ProtoDebug_Save(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);

static int ProtoDebug_Record(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);

static int ProtoDebug_RecordRam(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoDebug_Stack(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoDebug_Heap(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoDebug_FlashLogControl(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoDebug_FlashLog(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoDebug_Status(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoDebug_SMS(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoDebug_SimGPS(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoDebug_OnlyATMode(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoDebug_LogDebug(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);

static int ProtoDebug_AgpsWrite(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoDebug_Alert(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoDebug_ServerParseAddress(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoDebug_LogSwitch(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoDebug_LogTx(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);

static int ProtoDebug_DownSim800Start(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoDebug_DownSim800File(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoDebug_DownSim800End(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);


//FS
//DIR
static int ProtoDebug_FSDirRead(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoDebug_FSMkdir(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
//FILE
static int ProtoDebug_FSFileOpen(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoDebug_FSFileClose(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoDebug_FSFileRead(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoDebug_FSFileWrite(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoDebug_FSFileState(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoDebug_FSFileSize(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);
static int ProtoDebug_FSRemove(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);

static int ProtoDebug_FSTraverse(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len);


const EpbProtoKey_t scg_sProtoDebugKeys[] =
{
    {
        .key = DEBUG_KEY_SAVE,
        .handle = ProtoDebug_Save,
    },
    {
        .key = DEBUG_KEY_SIMGPS,
        .handle = ProtoDebug_SimGPS,
    },
    {
        .key = DEBUG_KEY_RECORD,
        .handle = ProtoDebug_Record,
    },
    {
        .key = DEBUG_KEY_RECORD_RAM,
        .handle = ProtoDebug_RecordRam,
    },
    {
        .key = DEBUG_KEY_STACK,
        .handle = ProtoDebug_Stack,
    },
    {
        .key = DEBUG_KEY_HEAP,
        .handle = ProtoDebug_Heap,
    },
    {
        .key = DEBUG_KEY_FLASH_LOG,
        .handle = ProtoDebug_FlashLog,
    },
    {
        .key = DEBUG_KEY_FLASH_LOG_CONTROL,
        .handle = ProtoDebug_FlashLogControl,
    },
    {
        .key = DEBUG_KEY_SERVER_PARSE_ADDRESS,
        .handle = ProtoDebug_ServerParseAddress,
    },

    {
        .key = DEBUG_KEY_STATUS,
        .handle = ProtoDebug_Status,
    },

    {
        .key = DEBUG_KEY_AGPS_WRITE,
        .handle = ProtoDebug_AgpsWrite,
    },

    {
        .key = DEBUG_KEY_ALERT,
        .handle = ProtoDebug_Alert,
    },

    {
        .key = DEBUG_KEY_LOG_SWITCH,
        .handle = ProtoDebug_LogSwitch,
    },
    {
        .key = DEBUG_KEY_LOG_SWITCH_DEBUG,
        .handle = ProtoDebug_LogDebug,
    },
    {
        .key = DEBUG_KEY_LOG_GSM_TX,
        .handle = ProtoDebug_LogTx,
    },

    {
        .key = DEBUG_KEY_SMS,
        .handle = ProtoDebug_SMS,
    },

    {
        .key = DEBUG_KEY_ONLY_AT,
        .handle = ProtoDebug_OnlyATMode,
    },

    {
        .key = DEBUG_KEY_DOWN_SIM800_START,
        .handle = ProtoDebug_DownSim800Start,
    },

    {
        .key = DEBUG_KEY_DOWN_SIM800_FILE,
        .handle = ProtoDebug_DownSim800File,
    },

    {
        .key = DEBUG_KEY_DOWN_SIM800_END,
        .handle = ProtoDebug_DownSim800End,
    },


    //FS
    {
        .key = DEBUG_KEY_FS_OPEN,
        .handle = ProtoDebug_FSFileOpen,
    },
    {
        .key = DEBUG_KEY_FS_CLOSE,
        .handle = ProtoDebug_FSFileClose,
    },
    {
        .key = DEBUG_KEY_FS_WRITE,
        .handle = ProtoDebug_FSFileWrite,
    },
    {
        .key = DEBUG_KEY_FS_READ,
        .handle = ProtoDebug_FSFileRead,
    },
    {
        .key = DEBUG_KEY_FS_SEEK,
        .handle = NULL,
    },
    {
        .key = DEBUG_KEY_FS_SIZE,
        .handle = ProtoDebug_FSFileSize,
    },

    {
        .key = DEBUG_KEY_FS_FILE_STAT,
        .handle = ProtoDebug_FSFileState,
    },
    {
        .key = DEBUG_KEY_FS_REMOVE,
        .handle = ProtoDebug_FSRemove,
    },

    {
        .key = DEBUG_KEY_FS_DIR_READ,
        .handle = ProtoDebug_FSDirRead,
    },

    {
        .key = DEBUG_KEY_FS_MK_DIR,
        .handle = ProtoDebug_FSMkdir,
    },
    {
        .key = DEBUG_KEY_FS_TRAVERSE,
        .handle = ProtoDebug_FSTraverse,
    },
};


const uint8_t s_ucProtoDebugKeySize = sizeof(scg_sProtoDebugKeys) / sizeof(scg_sProtoDebugKeys[0]);


/**
 * 地址解释
 * 
 * @author hzhmcu (2018/5/10)
 * 
 * @param obj 
 * @param key 
 * @param p_data 
 * @param len 
 * 
 * @return int 
 */
static int ProtoDebug_Save(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{

//  loc_gsm_task_setup_locator();
    PB_LOG("ProtoDebug_Save\r\n");



    Proto_RespNegative(obj, EPB_SUCCESS);
    return EPB_SUCCESS;
}


static int ProtoDebug_RecordRam(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    uint8_t buff[40];
    int index = 0;
    buff[index++] = COMMAND_ID_DEBUG;
    buff[index++] = 13;
    buff[index++] = key;


    uint32_encode(Recod_GetCurPiont(), buff + index);
    index += 4;
    uint32_encode(Record_GetCurDataRp(), buff + index);
    index += 4;
    uint32_encode(Record_GetCurDataWp(), buff + index);
    index += 4;


    Proto_Resp(obj, FLAG_NORMAL_RESP, buff, index);

    return 0;
}





static int ProtoDebug_Stack(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    uint8_t buff[200];
    uint8_t *pLen;
    int index = 0;
    buff[index++] = COMMAND_ID_DEBUG;
    pLen = buff + index; index++;
    buff[index++] = key;
#include "LibStack.h"
    LibStack_t *pStack = LibStack_Get();
/* 
  typedef struct
{
  uint32_t start;//SP开始
  uint32_t maxSize;//监控中，最大使用值
  uint32_t line;//监控中，最大使用的行数
  uint8_t func[MAX_FUNC_SIZE];//监控中，最大使用的函数名。
}LibStack_t;
 */

    uint32_encode(pStack->start, buff + index);
    index += 4;
    uint32_encode(pStack->maxSize, buff + index);
    index += 4;
    uint32_encode(pStack->line, buff + index);
    index += 4;
    int slen = strlen((char const *)pStack->func);
    if (slen > 100)
    {
        slen = 100;
    }
    memcpy(buff + index, pStack->func, slen);
    index += slen;
    *pLen = index - 2;
    Proto_Resp(obj, FLAG_NORMAL_RESP, buff, index);

    return 0;
}

static int ProtoDebug_Heap(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    int32_t cnt = LibHeap_GetCount();
    uint8_t buff[3 + cnt * (sizeof(LibHeapItem_t) + 2)];
    int index = 0;
    buff[index++] = COMMAND_ID_DEBUG;
    uint8_t *pLen;
    LibHeapItem_t item;
    PB_LOG("Cnt =%d", cnt);
    if (cnt == 0)
    {
        pLen = &buff[index]; index++;;
        buff[index++] = key;
        PB_LOG("index =%d", index);
    }
    else
    {


        for (int i=0;i<MAX_HEAP_ITEM&& cnt>0; i++)
        {
            if (LibHeap_GetItem(&item, i) > 0)
            {
                pLen = &buff[index]; index++; *pLen = 1;
                buff[index++] = key;
                uint32_encode(item.addr, buff + index);
                index += 4; *pLen += 4;
                uint16_encode(item.size, buff + index);
                index += 2; *pLen += 2;
                uint32_encode(item.times, buff + index);
                index += 4; *pLen += 4;
                uint32_encode(item.interval, buff + index);
                index += 4; *pLen += 4;
                uint32_encode(item.line, buff + index);
                index += 4; *pLen += 4;
                memcpy(buff + index, item.func, MAX_HEAP_FUNC_SIZE);
                index += MAX_HEAP_FUNC_SIZE; *pLen += MAX_HEAP_FUNC_SIZE;
                cnt--;
            }
        }
        PB_LOG("2 index =%d", index);
    }
    *pLen = index -2;
    Proto_Resp(obj, FLAG_NORMAL_RESP, buff, index);

    return 0;
}

static int ProtoDebug_Record(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    int32_t ret = 0;
    uint32_t addr = uint32_decode(p_data);
    uint16_t size = uint16_decode(p_data + 4);

    if (size > 512 || size == 0) return -1;

    uint8_t *buff = APP_MALLOC(PROTO_PACK_MEMORY_READER, size + 9);
    if (buff == NULL) return -1;
    int index = 0;
    buff[index++] = COMMAND_ID_DEBUG;
    buff[index++] = 0;
    buff[index++] = key;

    uint32_encode(addr, buff + index);
    index += 4;
    uint16_encode(size, buff + index);
    index += 2;


    if (addr < FLASH_END_ADDR)
    {
        flash_release_power_down_mode();
        flash_enter_power_down_mode();
        index += size;
    }

    /**
     * 没实现的，也发送，应该有问题的。 
     *  
     */
    PB_LOG("Flash 0x%X\r\n", addr);
    Proto_Resp(obj, FLAG_NORMAL_RESP, buff, index);
    APP_FREE(buff);
    return ret;
}

static int ProtoDebug_FlashLogControl(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    return 0;
}
static int ProtoDebug_FlashLog(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    int32_t ret = 0;
    uint32_t addr = uint32_decode(p_data);
    uint16_t size = uint16_decode(p_data + 4);

    if (size > 512 || size == 0) return -1;

    uint8_t *buff = APP_MALLOC(PROTO_PACK_MEMORY_READER, size + 9);
    if (buff == NULL) return -1;
    int index = 0;
    buff[index++] = COMMAND_ID_DEBUG;
    buff[index++] = 0;
    buff[index++] = key;

    uint32_encode(addr, buff + index);
    index += 4;
    uint16_encode(size, buff + index);
    index += 2;


    if (addr < FLASH_END_ADDR)
    {
        flash_release_power_down_mode();
        flash_read_data(addr, buff + index, size);
        flash_enter_power_down_mode();
        index += size;
    }

    /**
     * 没实现的，也发送，应该有问题的。 
     *  
     */
    PB_LOG("Flash 0x%X\r\n", addr);
    Proto_Resp(obj, FLAG_NORMAL_RESP, buff, index);
    APP_FREE(buff);
    return ret;
}


static int ProtoDebug_Status(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    uint8_t buff[100] = { 0 };

    PB_LOG("State \r\n");
    uint8_t *pLen = buff + 1;
    int index = 0;
    buff[index++] = COMMAND_ID_DEBUG;
    buff[index++] = 0; //len
    buff[index++] = key;

    //gsm state
    uint32_encode(ls_api_get_gsm_state(), buff + index);
    index += 4;
    uint32_encode(ls_api_get_gsm_task(), buff + index);
    index += 4;

    uint32_encode(DebugTime_GetUpdateTime(0), buff + index);
    index += 4;
    uint32_encode(DebugTime_GetLiveTime(0), buff + index);
    index += 4;
    buff[index++] = DebugTime_GetWay();


    uint32_t GPS_GetSuccessCnt();
    uint32_t GPS_GetFailCnt();
    //GPS 成功与失败的次数
    uint32_encode(GPS_GetSuccessCnt(), buff + index);
    index += 4;
    uint32_encode(GPS_GetFailCnt(), buff + index);
    index += 4;

    //3DH状态
    buff[index++] = loc_mode_get_motion();
    buff[index++] = GetGEOStatus(0);
    buff[index++] = GetGEOStatus(1);
    buff[index++] = GetGEOStatus(2);
    buff[index++] = GetGEOStatus(3);


    //AGPS time
    //AGPS utc
    uint32_encode(AGPS_GetDownUTC(), buff + index);
    index += 4;
    //AGPS  write utc
    uint32_encode(AGPS_GetWriteUTC(), buff + index);
    index += 4;

    //CALL STATE
    uint32_encode(locate_services_get_call_status(), buff + index);
    index += 4;


    *pLen = index - 2;
    Proto_Resp(obj, FLAG_NORMAL_RESP, buff, index);
    // Proto_RespNegative(obj, EPB_SUCCESS);
    return 0;
}


#include "loc_gsm.task.h"
#include "step_gsm.tcp.h"
static int ProtoDebug_ServerParseAddress(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    PB_LOG("ServerParseAddress \r\n");
    loc_gsm_task_setup_tcp_upload(TCPIP_UPLOAD_ADDR_PARSE);
    return 0;
}
int ProtoDebug_SendParseAddress(struct objEpbProto *obj, const uint8_t *p_data, uint16_t len)
{
    uint8_t *buff = APP_MALLOC(PROTO_PACK_MEMORY_READER, len + 9);
    if (buff == NULL) return -1;
    int index = 0;
    buff[index++] = COMMAND_ID_DEBUG;
    buff[index++] = len + 1;
    buff[index++] = DEBUG_KEY_SERVER_PARSE_ADDRESS;
    memcpy(buff, p_data, len);
    index++;
    Proto_Resp(obj, FLAG_NORMAL_RESP, buff, index);
    APP_FREE(buff);
    return 0;

}


int ProtoDebug_AgpsWrite(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    //loc_agps_force_write_agps_set(true);
    Proto_RespNegative(obj, EPB_SUCCESS);
    return 0;

}



int ProtoDebug_Alert(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    uint8_t ret = 0;
    switch (p_data[0])
    {
    case DEV_CONF_ALERT_POWER:
        ls_api_event_input(LS_EVT_ALERT_LOW_POWER);
        break;
    case DEV_CONF_ALERT_GEO:
        ls_api_event_input(LS_EVT_ALERT_GEO_FENCE_2);
        break;
    case DEV_CONF_ALERT_MOTION:
        ls_api_event_input(LS_EVT_ALERT_PRE_MOTION);
        break;
    case DEV_CONF_ALERT_NO_MOTION:
        ls_api_event_input(LS_EVT_ALERT_PRE_STATIC);
        break;
    case DEV_CONF_ALERT_OVER_SPEED:
        ls_api_event_input(LS_EVT_ALERT_OVERSPEED);
        break;
    case DEV_CONF_ALERT_TILT:
        ls_api_event_input(LS_EVT_ALERT_PRE_TILT);
        break;
    case DEV_CONF_ALERT_FALLDOWN:
        ls_api_event_input(LS_EVT_ALERT_PRE_FALLDOWN);
        break;
    default:
        ret = -1;
        break;
    }
    if (ret == 0)
    {
        Proto_RespNegative(obj, EPB_SUCCESS);
    }
    else
    {
        Proto_RespNegative(obj, EPB_ERR_SUB_FUNCTION_NOT_SUPPORTED);
    }
    return 0;
}


typedef struct
{
    union {
        struct {
            uint32_t gps:1;
            uint32_t gsm:1;
            uint32_t rvs:29;
        };
        uint32_t value;
    };
    uint32_t timestamp;
    uint32_t debug;
}LogSwitch_t;

LogSwitch_t sg_sLogSwitch = { .value = 0, .debug = 0 };


/**
 * 是否打开GPS LOG
 * 
 * @author hzhmcu (2018/8/30)
 * 
 * @return bool 
 */
bool ProtoDebug_IsGPSLog()
{
    return sg_sLogSwitch.gps;
}

void ProtoDebu_SetGPSLog()
{
    sg_sLogSwitch.value = 0;
    sg_sLogSwitch.gps = true;
}


/**
 * 是否打开GSM LOG
 * 
 * @author hzhmcu (2018/8/30)
 * 
 * @return bool 
 */
bool ProtoDebug_IsGSMLog()
{
    return sg_sLogSwitch.gsm;
}

bool ProtoDebug_IsDebugLog(uint32_t flag)
{
    return (sg_sLogSwitch.debug & flag)>0;
}

bool ProtoDebug_IsLog()
{
    return (sg_sLogSwitch.value > 0) || (sg_sLogSwitch.debug > 0);
}

static int ProtoDebug_LogSwitch(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    sg_sLogSwitch.timestamp = RunTime_GetValue();

    sg_sLogSwitch.value = uint32_decode(p_data);
    Proto_RespNegative(obj, EPB_SUCCESS);
    return 0;
}



static int ProtoDebug_LogDebug(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    sg_sLogSwitch.timestamp = RunTime_GetValue();
    sg_sLogSwitch.debug = uint32_decode(p_data);
    Proto_RespNegative(obj, EPB_SUCCESS);
    return 0;
}



char sg_ucGSMTx[200];
char* GetGSMTx()
{
    return sg_ucGSMTx;
}

static int ProtoDebug_LogTx(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    if (false == GSMDownOnlyAT_Put(p_data, len))
    {
        memcpy(sg_ucGSMTx, p_data, len);
        sg_ucGSMTx[len] = 0;
        gsm_task_setup_DebugAT();
    }
    return 0;
}
static int ProtoDebug_SimGPS(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    uint32_t lat = uint32_decode(p_data);
    uint32_t lon = uint32_decode(p_data + 4);
    LocGPS_CheckGEO(lat, lon);
    return 0;
}

static int ProtoDebug_SMS(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    memcpy(sg_ucGSMTx, p_data, len);
    sg_ucGSMTx[len] = 0;
    //把要回复的内容回复
    uint8_t buff[500] = { 0 };

    uint8_t *pLen = buff + 1;
    int index = 0;
    buff[index++] = COMMAND_ID_DEBUG;
    buff[index++] = 0; //len
    buff[index++] = key;
    SMSProto_Start("13534964174");
    if (false ==  SMSProto_ConsumeText(sg_ucGSMTx))
    {
        strcpy((char *)(buff + index), "SMS Error\n");
        index += strlen("SMS Error\n");
    }
    else
    {
        ArrayData_t arry;
        SMSProto_GetResponeData(&arry);
        memcpy(buff + index, arry.data, arry.len);
        index += arry.len;

    }




    *pLen = index - 2;
    Proto_Resp(obj, FLAG_NORMAL_RESP, buff, index);
    return 0;
}
static int ProtoDebug_OnlyATMode(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    GSMDownOnlyAT_Start();
    Proto_RespNegative(obj, EPB_SUCCESS);
    return EPB_SUCCESS;
}

struct objEpbProto  *pDownMusic = NULL;


static int ProtoDebug_DownSim800Start(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    pDownMusic = obj;
    GSM_DownMusic_Start();
    Proto_RespNegative(obj, EPB_SUCCESS);
    return EPB_SUCCESS;
}



static int ProtoDebug_DownSim800File(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    obj->isSendResp = false;

    GSM_DownMusic_Put((char *)p_data, len);
    return 0;
}



static int ProtoDebug_DownSim800End(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    pDownMusic = NULL;
    GSM_DownMusic_End();
    Proto_RespNegative(obj, EPB_SUCCESS);
    return EPB_SUCCESS;
}


void DownSim800Music_GSMRx(char *data, int len)
{
    GSMDown_Rx(data, len);
}
#define TEST_MAX_TIME (1*60*60)

void ProtoDebug_Second()
{
    if (RunTime_InRange(sg_sLogSwitch.timestamp, TEST_MAX_TIME) == false)
    {
        sg_sLogSwitch.value = 0;
    }
}

#include "Halfs.h"
#define DEBUG_FS
FILE_t file;
bool isOpenFile = false;
#define MAX_LEN 1024
static int ProtoDebug_FSFileOpen(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{

    char fileName[100] = { 0 };

    uint8_t buff[100] = { 0 };

    int index = 0;
    buff[index++] = COMMAND_ID_DEBUG;
    buff[index++] = 5;
    buff[index++] = key;

    int flag =  uint32_decode(p_data);
    memcpy(fileName, p_data + 4, len - 4);
    if (isOpenFile == true)
    {
        HalFS_CloseFile(&file);
        isOpenFile = false;
    }


    isOpenFile = true;

    int ret = HalFS_OpenFile(&file, fileName, flag);
    buff[index++] = ret;
    buff[index++] = ret >> 8;
    buff[index++] = ret >> 16;
    buff[index++] = ret >> 24;
    Proto_Resp(obj, FLAG_NORMAL_RESP, buff, index);
    return EPB_SUCCESS;
}

static int ProtoDebug_FSFileClose(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{


    uint8_t buff[100] = { 0 };

    int index = 0;
    buff[index++] = COMMAND_ID_DEBUG;
    buff[index++] = 5;
    buff[index++] = key;


    int ret = HalFS_CloseFile(&file);
    buff[index++] = ret;
    buff[index++] = ret >> 8;
    buff[index++] = ret >> 16;
    buff[index++] = ret >> 24;
    isOpenFile = false;
    Proto_Resp(obj, FLAG_NORMAL_RESP, buff, index);
    return EPB_SUCCESS;
}


static int ProtoDebug_FSFileRead(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    int offset =  uint32_decode(p_data);
    int count =  uint32_decode(p_data + 4);
    if (count > 200)
    {
        count = 200;
    }

    uint8_t buff[MAX_LEN] = { 0 };
    int index = 0;
    buff[index++] = COMMAND_ID_DEBUG;
    uint8_t *pLen = NULL;

    if (offset > 0)
    {
        if (HalFS_SetFileSeek(&file, offset, LFS_SEEK_SET) < 0)
        {
            buff[index++] = 1;
            buff[index++] = key;
        }
    }

    while (count > 0)
    {
        pLen = buff + index;
        index++;
        buff[index++] = key;
        int readCount = count < 200 ? count : 200;
        int err = HalFS_Read(&file, (char *)(buff + index), readCount);
        if (err == 0)
        {
            *pLen = err + 1;
            break;
        }
        else if (err > 0)
        {
            index += err;
            *pLen = err + 1;
            break;
        }
        else
        {
            index = 1;
            buff[index++] = 1;
            buff[index++] = key;
            break;

        }
//        count -= err;

    }
    Proto_Resp(obj, FLAG_NORMAL_RESP, buff, index);
    return EPB_SUCCESS;
}

static int ProtoDebug_FSFileWrite(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{

    uint8_t buff[100] = { 0 };
    int index = 0;
    buff[index++] = COMMAND_ID_DEBUG;
    buff[index++] = 5;
    buff[index++] = key;
    int ret = HalFS_Write(&file, (char *)p_data, len);
    buff[index++] = ret;
    buff[index++] = ret >> 8;
    buff[index++] = ret >> 16;
    buff[index++] = ret >> 24;
    Proto_Resp(obj, FLAG_NORMAL_RESP, buff, index);
    return EPB_SUCCESS;
}


static int ProtoDebug_FSFileSize(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{

    uint8_t buff[100] = { 0 };
    int index = 0;
    buff[index++] = COMMAND_ID_DEBUG;
    buff[index++] = 5;
    buff[index++] = key;
    if (isOpenFile == false)
    {
        Proto_RespNegative(obj, EPB_ERR_INVALID_STATE);
        return EPB_SUCCESS;
    }
    int size =  HalFS_GetFileSize(&file);
    buff[index++] = size;
    buff[index++] = size >> 8;
    buff[index++] = size >> 16;
    buff[index++] = size >> 24;
    Proto_Resp(obj, FLAG_NORMAL_RESP, buff, index);
    return EPB_SUCCESS;
}


static int ProtoDebug_FSFileState(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    return EPB_SUCCESS;
}


static int ProtoDebug_FSRemove(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    char pathName[50] = { 0 };
    memcpy(pathName, p_data, len);
    uint8_t buff[100] = { 0 };
    int index = 0;
    buff[index++] = COMMAND_ID_DEBUG;
    buff[index++] = 5;
    buff[index++] = key;
    int ret = HalFS_Remove(pathName);
    buff[index++] = ret;
    buff[index++] = ret >> 8;
    buff[index++] = ret >> 16;
    buff[index++] = ret >> 24;
    Proto_Resp(obj, FLAG_NORMAL_RESP, buff, index);
    return EPB_SUCCESS;
}


static int ProtoDebug_FSTraverse(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    char pathName[50] = { 0 };
    memcpy(pathName, p_data, len);
    uint8_t buff[100] = { 0 };
    int index = 0;
    buff[index++] = COMMAND_ID_DEBUG;
    buff[index++] = 5 + 8;
    buff[index++] = key;

    lfs_size_t in_use = 0;
    int err = HalFS_Traverse(&in_use);
    if (err)
    {
        Proto_RespNegative(obj, EPB_ERR_INVALID_STATE);
        return EPB_ERR_INVALID_STATE;
    }
    uint32_t bsize  = FLASH_SECTOR_SIZE;
    uint32_t bcount = HAL_FS_SIZE;


    buff[index++] = bsize;
    buff[index++] = bsize >> 8;
    buff[index++] = bsize >> 16;
    buff[index++] = bsize >> 24;

    buff[index++] = bcount;
    buff[index++] = bcount >> 8;
    buff[index++] = bcount >> 16;
    buff[index++] = bcount >> 24;

    buff[index++] = in_use;
    buff[index++] = in_use >> 8;
    buff[index++] = in_use >> 16;
    buff[index++] = in_use >> 24;

    Proto_Resp(obj, FLAG_NORMAL_RESP, buff, index);
    return EPB_SUCCESS;
}
static int ProtoDebug_FSMkdir(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{
    char pathName[50] = { 0 };
    memcpy(pathName, p_data, len);
    uint8_t buff[100] = { 0 };
    int index = 0;
    buff[index++] = COMMAND_ID_DEBUG;
    buff[index++] = 5;
    buff[index++] = key;
    int size =  HalFS_Mkdir(pathName);
    buff[index++] = size;
    buff[index++] = size >> 8;
    buff[index++] = size >> 16;
    buff[index++] = size >> 24;
    Proto_Resp(obj, FLAG_NORMAL_RESP, buff, index);
    return EPB_SUCCESS;
}

static int ProtoDebug_FSDirRead(struct objEpbProto *obj, uint8_t key, const uint8_t *p_data, uint16_t len)
{

    DIR_t dir;
    FSInfo_t info;
    char pathName[50] = { 0 };

    memcpy(pathName, p_data, len);

    //把要回复的内容回复
    uint8_t buff[MAX_LEN] = { 0 };

    uint8_t *pLen = buff + 1;
    int index = 0;
    int itemIndex = 0;
    buff[index++] = COMMAND_ID_DEBUG;
    if (HalFS_DirOpen(&dir, pathName) < 0)
    {
        return EPB_SUCCESS;
    }
    do
    {
        HalFS_DirRead(&dir, &info);
        if (info.type > 0)
        {

            itemIndex = 0;
            pLen = buff + index + itemIndex;
            buff[index + itemIndex++] = 0; //len
            buff[index + itemIndex++] = key;
            buff[index + itemIndex++] = info.type;
            buff[index + itemIndex++] = info.size;
            buff[index + itemIndex++] = info.size >> 8;
            buff[index + itemIndex++] = info.size >> 16;
            buff[index + itemIndex++] = info.size >> 24;


            strcpy((char *)(buff + index + itemIndex), info.name);
            itemIndex +=  strlen(info.name);
            *pLen = itemIndex - 1;
            index += itemIndex;
        }
    }
    while (info.type > 0);
    Proto_Resp(obj, FLAG_NORMAL_RESP, buff, index);
    HalFS_DirClose(&dir);
    return EPB_SUCCESS;
}




