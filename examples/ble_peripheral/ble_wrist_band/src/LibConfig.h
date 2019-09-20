#ifndef __LIB_CONFIG_H
#define __LIB_CONFIG_H
#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"
#include "string.h"
#include "LibErr.h"


//#include "nrf52.h"
#include "nrf.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "config.h"


#include "app_util_platform.h"

#include "LibStack.h"

//¥Ú”°LOG
#ifdef LibLOG
	#undef LibLOG
#endif

#define LibLOG NRF_LOG_INFO


//#include "log.h"

#define CRITICAL_Enter()	CRITICAL_REGION_ENTER()
#define CRITICAL_Exit()		CRITICAL_REGION_EXIT()



#ifndef __assert__
#define __assert__(exp)  extern char __assert_data__[((int)(exp)) ? 1:-1];
#endif

#ifndef __ASSERT__
#define __ASSERT__(exp)  __assert__(exp)
#endif




#define UTIL_SIZEOF_CBUF_SIZE_T 2
#define __SIZEOF_CPU_OP_UNIT___ 4


#endif
