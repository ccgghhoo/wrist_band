#include "scan_adv.h"
#include "app_timer.h"

typedef enum
{
  SCAN_ADV_STEP_INIT,
  SCAN_ADV_STEP_START,
  SCAN_ADV_STEP_SLEEP,
  SCAN_ADV_STEP_STOP,
}ScanAdv_Step_en;
typedef struct {
  bool enable;
  ScanAdv_Step_en step;
  uint32_t scan_time; //扫描的时间
  uint32_t scan_priod; //扫描周期。
  void (*start_scan)();
  void (*stop_scan)();
}Scan_adv_obj;

static Scan_adv_obj sg_objScanAdv;
#define objScanAdv (&sg_objScanAdv)


APP_TIMER_DEF(timer_scan_adv);


static void scan_adv_timer_handler(void *p_context);


int32_t ScanAdv_Init(uint32_t scan_time, uint32_t scan_priod,
                     void (*start_scan)(), void (*stop_scan)())
{
  if (start_scan == NULL || stop_scan == NULL || scan_priod <= scan_time)
  {
    return -1;
  }
  objScanAdv->enable = false;
  objScanAdv->start_scan = start_scan;
  objScanAdv->stop_scan = stop_scan;
  objScanAdv->scan_time = scan_time;
  objScanAdv->scan_priod = scan_priod;
  objScanAdv->step = SCAN_ADV_STEP_INIT;
  uint32_t err_code = app_timer_create(&timer_scan_adv, APP_TIMER_MODE_SINGLE_SHOT, scan_adv_timer_handler);
  if (err_code != NRF_SUCCESS)
  {
    return err_code;
  }
  return 0;
}

int32_t ScanAdv_SetTime(uint32_t scan_time, uint32_t scan_priod)
{
  if (scan_priod <= scan_time) return -1;
  objScanAdv->scan_time = scan_time;
  objScanAdv->scan_priod = scan_priod;
  return 0;
}



int32_t ScanAdv_Start()
{
  objScanAdv->enable = true;
  objScanAdv->step = SCAN_ADV_STEP_START;
  uint32_t err_code = app_timer_start(timer_scan_adv, APP_TIMER_TICKS(objScanAdv->scan_time), NULL);
  if (err_code != NRF_SUCCESS)
  {
    APP_ERROR_CHECK(err_code);
  }
  objScanAdv->start_scan();
  return 0;
}

int32_t ScanAdv_Stop()
{
  objScanAdv->enable = false;
  objScanAdv->step = SCAN_ADV_STEP_STOP;
  uint32_t err_code = app_timer_stop(timer_scan_adv);
  if (err_code != NRF_SUCCESS)
  {
    APP_ERROR_CHECK(err_code);
  }
  objScanAdv->stop_scan();
   return 0;
}



static int32_t ScanAdv_Sleep()
{
  objScanAdv->enable = true;
  objScanAdv->step = SCAN_ADV_STEP_SLEEP;
  uint32_t err_code = app_timer_start(timer_scan_adv,
                                      APP_TIMER_TICKS(objScanAdv->scan_priod - objScanAdv->scan_time), NULL);
  if (err_code != NRF_SUCCESS)
  {
    APP_ERROR_CHECK(err_code);
  }
  objScanAdv->stop_scan();
   return 0;
}


static void scan_adv_timer_handler(void *p_context)
{
  if (objScanAdv->enable == false)
  {
    ScanAdv_Stop();
  } else
  {
    if (objScanAdv->step == SCAN_ADV_STEP_START)
    {
      ScanAdv_Sleep();
    } else
    {
      ScanAdv_Start();
    }
  }
}


int32_t ScanAdv_CallBack(ble_gap_evt_adv_report_t const *pAdv)
{

  return 0;
}










