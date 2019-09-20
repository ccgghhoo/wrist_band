#ifndef _TINY_FILE_SYSTEM_
#define _TINY_FILE_SYSTEM_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TINY_FS_FILE_DATA_SIZE						(3) // the size must be: 1 3 7  
#define TINY_FS_READ_MAX_SIZE						(1024)

typedef struct
{
	uint32_t file_data[TINY_FS_FILE_DATA_SIZE];
	uint16_t file_id;
	uint16_t file_crc;
} Tinyfs_file_data_t;


bool 	 Tinyfs_init(void);
bool     Tinyfs_push_new_record(uint8_t *p_dest, uint16_t input_size);
uint16_t Tinyfs_pop_exist_record(uint8_t *p_dest, uint16_t max_size);
bool     Tingfs_queue_is_empty(void);


#ifdef __cplusplus
}
#endif

#endif
