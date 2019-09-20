

#ifndef DEV_CONF_H__
#define DEV_CONF_H__

#include <stdint.h>
#include <stdbool.h>

#include "dev_conf_defines.h"
#include "dev_config.write.h"
#include "dev_config.read.h"
#include "dev_config.get.h"
#include "dev_conf.pb.h"
#include "alarm_clock.h"

typedef struct {
    clock_activate_handle_t             clock_active_handle;
    dev_settings_update_handler_t		cb;
}dev_conf_init_t;





void dev_conf_reset_to_factory(void);

void dev_conf_save_profile(void);

bool dev_conf_volume_speaker_inc(void);
bool dev_conf_volume_speaker_dec(void);

bool dev_conf_volume_mic_inc(void);
bool dev_conf_volume_mic_dec(void);

void dev_conf_init(dev_conf_init_t *p_conf_init);
int32_t DevSave_Loop();

#endif

