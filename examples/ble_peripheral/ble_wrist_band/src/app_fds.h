#ifndef _APP_FDS_H_
#define _APP_FDS_H_

#include <stdint.h>
#include <stdbool.h>
#include <fds.h>

#define  APP_FILE_ID_BASE           0x0001
#define  APP_REC_KEY_ID_BASE        0x0001

#define  APP_REC_KEY_ID_SPORT_LVL       0x0001
#define  APP_REC_KEY_ID_HR              0x0002
#define  APP_REC_KEY_ID_STEP            0x0003
//......


#define  APP_FILE_ID_END            (0xC000-1) //file id that peer manager used  started  at 0xc000 
#define  APP_REC_KEY_ID_END         (0xC000-1)

#define  MAX_FLASH_SAVE_DAY          3


typedef struct 
{
    fds_find_token_t    ftoken;
    fds_record_desc_t   fdesc;
    uint16_t            file_id;
    uint8_t             read_day_index;
    uint8_t             retry_cnt;
    uint32_t            delay_s;
    bool                b_find_begin;
    bool                b_find_end;
}find_file_t;


void  app_fds_init(void);
void  app_fds_new_day_handle(void);
void  sport_level_data_save(uint32_t *pdata, uint16_t len);
void  sport_level_data_read(uint32_t *readDataBuff, uint16_t  *ReadLength, uint8_t whichDay);
bool  del_current_sport_record(void);
bool  find_read_sport_record(uint32_t *readDataBuff, uint16_t  *ReadLength, uint8_t whichDay);

#endif

