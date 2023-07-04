#ifndef __H_MACSDRV__
#define __H_MACSDRV__

#include <stdint.h>

int32_t macs_play(void* data_addr);
int32_t macs_status(void);
int32_t macs_abort(void);
int32_t macs_version(void);
int32_t macs_register(uint8_t* app_name);
int32_t macs_unregister(uint8_t* app_name);
int32_t macs_data_version(void* data_addr);
int32_t macs_isavailable(void);

#endif