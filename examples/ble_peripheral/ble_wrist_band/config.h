#ifndef __CONFIG_H
#define __CONFIG_H
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
//#include "log.h"


#define APP_FW_HW_VER			(0x90)//

// <h> Property
// <o> Device Module Name
// <i> define this project module code
#ifndef PROPERTY_MODULE_NAME
#define PROPERTY_MODULE_NAME	0x20190916
#endif

#define VER_H (1)
#define VER_M (0)
#define VER_L (7)
#define VER_ML (1)

// <o> Firmware Version
#ifndef PROPERTY_FIRMWARE_VER
#define PROPERTY_FIRMWARE_VER	((VER_H<<24)|(VER_M<<16)|(VER_L<<8)|(VER_ML))
#endif

// <s> Firmware Version String
#ifndef DEVICE_NAME
#define DEVICE_NAME						"eview_wrist_band"
#endif

#define DEGREE 1
#define  TILT_VERTICAL_ACCEL_S        TILT_VERTICAL_ACCEL_POINT_X
  
#ifndef DEV_VERSION
#define DEV_VERSION "V0.0.0"
#endif


#endif
