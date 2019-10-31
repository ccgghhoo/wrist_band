#ifndef _APP_INIT_H_
#define _APP_INIT_H_

//add all .h to here , then other .c files  only include  app_init.h

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "boards.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "crc16.h"

#include "nrf_log_default_backends.h"
#include "nrf_log.h"

#include "proto.h"
#include "dfu.h"
#include "nrf_delay.h"
#include "app_evt.h"
#include "DateTime.h"
#include "app_motion_detect.h"
#include "peripheral_role.h"
#include "batt_adc_detect.h"   
#include "app_led_ind.h"
#include "fds.h"
#include "app_fds.h"
#include "app_wdt.h"
#include "app_nus.h"
#include "indicator.h"
#include "sport_degree_count.h"










void  app_init(void);


#endif

