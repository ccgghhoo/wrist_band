

#ifndef DFU_TYPES_H__
#define DFU_TYPES_H__


#include "HalDFUSetting.h"





#define NRF_DFU_BANK_INVALID     0x00 /**< Invalid image. */
#define NRF_DFU_BANK_VALID_APP   0x01 /**< Valid application. */
#define NRF_DFU_BANK_READY_APP	 0x02 /**< Ready application. */
#define NRF_DFU_BANK_READY_EXAPP 0x03 /**< Ready Application in Extenal Flash */
#define NRF_DFU_BANK_VALID_SD    0xA5 /**< Valid SoftDevice. */
#define NRF_DFU_BANK_VALID_BL    0xAA /**< Valid bootloader. */
#define NRF_DFU_BANK_VALID_SD_BL 0xAC /**< Valid SoftDevice and bootloader. */


#pragma pack(4)

typedef struct
{
	uint32_t                image_size;         /**< Size of the image in the bank. */
	uint32_t                image_crc;          /**< CRC of the image. If set to 0, the CRC is ignored. */
	uint32_t                bank_code;          /**< Identifier code for the bank. */
} dfu_bank_t;

/**@brief DFU progress.
 *
 * Be aware of the difference between objects and firmware images. A firmware image consists of multiple objects, each of a maximum size @ref DATA_OBJECT_MAX_SIZE.
 */
typedef struct
{
	uint32_t command_size;              /**< The size of the current init command stored in the DFU settings. */
	uint32_t command_offset;            /**< The offset of the currently received init command data. The offset will increase as the init command is received. */
	uint32_t command_crc;               /**< The calculated CRC of the init command (calculated after the transfer is completed). */

	uint32_t data_object_size;          /**< The size of the last object created. Note that this size is not the size of the whole firmware image.*/

	uint32_t firmware_image_crc;        /**< CRC value of the current firmware (continuously calculated as data is received). */
	uint32_t firmware_image_crc_last;   /**< The CRC of the last executed object. */
	uint32_t firmware_image_offset;     /**< The offset of the current firmware image being transferred. Note that this offset is the offset in the entire firmware image and not only the current object. */
	uint32_t firmware_image_offset_last; /**< The offset of the last executed object from the start of the firmware image. */
} dfu_progress_t;

typedef struct {
	uint32_t            crc;                /**< CRC for the stored DFU settings, not including the CRC itself. If 0xFFFFFFF, the CRC has never been calculated. */
	uint32_t            settings_version;   /**< Version of the currect dfu settings struct layout.*/
	uint32_t            app_version;        /**< Version of the last stored application. */
	uint32_t            bootloader_version; /**< Version of the last stored bootloader. */

	uint32_t            bank_layout;        /**< Bank layout: single bank or dual bank. This value can change. */
	uint32_t            bank_current;       /**< The bank that is currently used. */

	dfu_bank_t      	bank_0;             /**< Bank 0. */
	dfu_bank_t      	bank_1;             /**< Bank 1. */

	uint32_t            write_offset;       /**< Write offset for the current operation. */
	uint32_t            sd_size;            /**< SoftDevice size (if combined BL and SD). */

	dfu_progress_t      progress;           /**< Current DFU progress. */

	uint32_t            enter_buttonless_dfu;
	uint8_t             init_command[INIT_COMMAND_MAX_SIZE];  /**< Buffer for storing the init command. */
}dfu_settings_t;

#pragma pack()

#endif

