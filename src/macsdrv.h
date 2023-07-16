#ifndef __H_MACSDRV__
#define __H_MACSDRV__

#include <stdint.h>

int32_t macsdrv_play(void* data_addr, int16_t mute);
int32_t macsdrv_status(void);
int32_t macsdrv_abort(void);
int32_t macsdrv_version(void);
int32_t macsdrv_version_ext(void);
int32_t macsdrv_register(uint8_t* app_name);
int32_t macsdrv_unregister(uint8_t* app_name);
int32_t macsdrv_data_version(void* data_addr);
int32_t macsdrv_isavailable(void);

#endif