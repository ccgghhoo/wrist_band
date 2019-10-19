#ifndef _APP_FDS_H_
#define _APP_FDS_H_

#include <stdint.h>
#include <stdbool.h>

#define  APP_FILE_ID_BASE           0x0001
#define  APP_REC_KEY_ID_BASE        0x0001

#define  APP_FILE_ID_END            (0xC000-1) //file id that peer manager used  started  at 0xc000 
#define  APP_REC_KEY_ID_END         (0xC000-1)

#define  MAX_FLASH_SAVE_DAY          7


void  app_fds_init(void);
void  app_fds_new_day_handle(void);
void  history_data_save(void);

#endif

