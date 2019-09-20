#include "halble.h"
#include "halble.h"
#include "DateTime.h"


typedef struct
{
    uint32_t lat;
    uint32_t lon;
    uint32_t timestamp;
}ChargeBaseLoc_t;

static ChargeBaseLoc_t sg_sChargeBaseLoc;

int HalBLE_SetChargeBaseLoc(uint32_t lat, uint32_t lon)
{
    sg_sChargeBaseLoc.lat = lat;
    sg_sChargeBaseLoc.lat = lon;
    sg_sChargeBaseLoc.timestamp = RunTime_GetValue();
    //Advertising_SetChargeBase(lat, lon);
    return 0;
}




int HalBLE_SecondLoop()
{
    if (sg_sChargeBaseLoc.timestamp != 0)
    {
        if (RunTime_InRange(sg_sChargeBaseLoc.timestamp, (2 * 60)) == false)
        {
            sg_sChargeBaseLoc.timestamp = 0;
          //  Advertising_SetChargeBaseDisable();
        }
    }
    return 0;
}

