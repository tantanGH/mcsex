#include <stdint.h>
#include <string.h>
#include <doslib.h>
#include <iocslib.h>
#include "macsdrv.h"

//
//  MACS play
//
int32_t macsdrv_play(void* data_addr, int16_t mute) {

    struct REGS in_regs = { 0 };
    struct REGS out_regs = { 0 };

    in_regs.d0 = 0xD0;
    in_regs.d1 = 0;
    in_regs.d2 = -1;
    in_regs.d4 = mute ? 2 : 0;
    in_regs.a1 = (uint32_t)data_addr;

    TRAP15(&in_regs, &out_regs);

    return out_regs.d0;
}

//
//  MACS status
//
int32_t macsdrv_status() {

    struct REGS in_regs = { 0 };
    struct REGS out_regs = { 0 };

    in_regs.d0 = 0xD0;
    in_regs.d1 = 1;
    in_regs.d2 = -1;

    TRAP15(&in_regs, &out_regs);

    return out_regs.d0;
}

//
//  MACS abort
//
int32_t macsdrv_abort() {

    struct REGS in_regs = { 0 };
    struct REGS out_regs = { 0 };

    in_regs.d0 = 0xD0;
    in_regs.d1 = 2;
    in_regs.d2 = -1;
    in_regs.d3 = -1;

    TRAP15(&in_regs, &out_regs);

    return out_regs.d0;
}

//
//  MACS version
//
int32_t macsdrv_version() {

    struct REGS in_regs = { 0 };
    struct REGS out_regs = { 0 };

    in_regs.d0 = 0xD0;
    in_regs.d1 = 3;

    TRAP15(&in_regs, &out_regs);

    return out_regs.d0;
}

//
//  MACS register
//
int32_t macsdrv_register(uint8_t* app_name) {

    struct REGS in_regs = { 0 };
    struct REGS out_regs = { 0 };

    in_regs.d0 = 0xD0;
    in_regs.d1 = 4;
    in_regs.a1 = (uint32_t)app_name;

    TRAP15(&in_regs, &out_regs);

    return out_regs.d0;
}

//
//  MACS unregister
//
int32_t macsdrv_unregister(uint8_t* app_name) {

    struct REGS in_regs = { 0 };
    struct REGS out_regs = { 0 };

    in_regs.d0 = 0xD0;
    in_regs.d1 = 5;
    in_regs.a1 = (uint32_t)app_name;

    TRAP15(&in_regs, &out_regs);

    return out_regs.d0;
}

//
//  MACS data version
//
int32_t macsdrv_data_version(void* data_addr) {

    struct REGS in_regs = { 0 };
    struct REGS out_regs = { 0 };

    in_regs.d0 = 0xD0;
    in_regs.d1 = 6;
    in_regs.a1 = (uint32_t)data_addr;

    TRAP15(&in_regs, &out_regs);

    return out_regs.d0;
}

//
//  MACS version (extended)
//
int32_t macsdrv_version_ext() {

    struct REGS in_regs = { 0 };
    struct REGS out_regs = { 0 };

    in_regs.d0 = 0xD0;
    in_regs.d1 = 17;

    TRAP15(&in_regs, &out_regs);

    return out_regs.d0;
}

//
//  macsdrv keep check
//
int32_t macsdrv_isavailable() {

  uint32_t eye_catch_addr = INTVCG(0x1d0) + 2;

  uint8_t eye_catch[8];
  for (int16_t i = 0; i < 8; i++) {
    eye_catch[i] = B_BPEEK((uint8_t*)(eye_catch_addr + i));
  }

  return (memcmp(eye_catch, "MACSIOCS", 8) == 0) ? 1 : 0;
}