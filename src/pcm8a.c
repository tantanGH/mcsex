#include <stdint.h>
#include <string.h>
#include <iocslib.h>
#include "pcm8a.h"

/*
//
//  play in normal mode ($10xx)
//
int32_t pcm8a_play(int16_t channel, uint32_t mode, uint32_t size, void* addr) {

  register uint32_t reg_d0 asm ("d0") = 0x1000 + channel;
  register uint32_t reg_d1 asm ("d1") = mode;
  register uint32_t reg_d2 asm ("d2") = size;
  register uint32_t reg_a1 asm ("a1") = (uint32_t)addr;

  asm volatile (
    "trap #2\n"         // trap #2
    : "+r"  (reg_d0)    // output (&input) operand
    : "r"   (reg_d1),   // input operand
      "r"   (reg_d2),   // input operand
      "r"   (reg_a1)    // input operand
    :                   // clobbered register
  );

  return reg_d0;
}

//
//  play in array chain mode ($11xx)
//
int32_t pcm8a_play_array_chain(int16_t channel, uint32_t mode, int16_t count, void* addr) {

  register uint32_t reg_d0 asm ("d0") = 0x1100 + channel;
  register uint32_t reg_d1 asm ("d1") = mode;
  register uint32_t reg_d2 asm ("d2") = count;
  register uint32_t reg_a1 asm ("a1") = (uint32_t)addr;

  asm volatile (
    "trap #2\n"         // trap #2
    : "+r"  (reg_d0)    // output (&input) operand
    : "r"   (reg_d1),   // input operand
      "r"   (reg_d2),   // input operand
      "r"   (reg_a1)    // input operand
    :                   // clobbered register
  );

  return reg_d0;
}

//
//  play in linked array chain mode ($12xx)
//
int32_t pcm8a_play_linked_array_chain(int16_t channel, uint32_t mode, void* addr) {

  register uint32_t reg_d0 asm ("d0") = 0x1200 + channel;
  register uint32_t reg_d1 asm ("d1") = mode;
  register uint32_t reg_a1 asm ("a1") = (uint32_t)addr;

  asm volatile (
    "trap #2\n"         // trap #2
    : "+r"  (reg_d0)    // output (&input) operand
    : "r"   (reg_d1),   // input operand
      "r"   (reg_a1)    // input operand
    :                   // clobbered register
  );

  return reg_d0;
}

//
//  set channel mode ($17xx)
//
int32_t pcm8a_set_channel_mode(int16_t channel, uint32_t mode) {

  register uint32_t reg_d0 asm ("d0") = 0x1700 + channel;
  register uint32_t reg_d1 asm ("d1") = mode;

  asm volatile (
    "trap #2\n"         // trap #2
    : "+r"  (reg_d0)    // output (&input) operand
    : "r"   (reg_d1)    // input operand
    :                   // clobbered register
  );

  return reg_d0;
}

//
//  get data length ($18xx)
//
int32_t pcm8a_get_data_length(int16_t channel) {

  register uint32_t reg_d0 asm ("d0") = 0x1800 + channel;

  asm volatile (
    "trap #2\n"         // trap #2
    : "+r"  (reg_d0)    // output (&input) operand
    :                   // input operand
    :                   // clobbered register
  );

  return reg_d0;
}

//
//  get channel mode ($19xx)
//
int32_t pcm8a_get_channel_mode(int16_t channel) {

  register uint32_t reg_d0 asm ("d0") = 0x1900 + channel;

  asm volatile (
    "trap #2\n"         // trap #2
    : "+r"  (reg_d0)    // output (&input) operand
    :                   // input operand
    :                   // clobbered register
  );

  return reg_d0;
}

//
//  get access address ($1axx)
//
void* pcm8a_get_access_address(int16_t channel) {

  register uint32_t reg_d0 asm ("d0") = 0x1a00 + channel;

  asm volatile (
    "trap #2\n"         // trap #2
    : "+r"  (reg_d0)    // output (&input) operand
    :                   // input operand
    :                   // clobbered register
  );

  return (void*)reg_d0;
}

//
//  stop all channels ($0100)
//
int32_t pcm8a_stop() {

  register uint32_t reg_d0 asm ("d0") = 0x0100;

  asm volatile (
    "trap #2\n"         // trap #2
    : "+r"  (reg_d0)    // output (&input) operand
    :                   // input operand
    :                   // clobbered register
  );

  return reg_d0;
}

//
//  pause all channels ($0101)
//
int32_t pcm8a_pause() {

  register uint32_t reg_d0 asm ("d0") = 0x0101;

  asm volatile (
    "trap #2\n"         // trap #2
    : "+r"  (reg_d0)    // output (&input) operand
    :                   // input operand
    :                   // clobbered register
  );

  return reg_d0;
}

//
//  resume all channels ($0102)
//
int32_t pcm8a_resume() {

  register uint32_t reg_d0 asm ("d0") = 0x0102;

  asm volatile (
    "trap #2\n"         // trap #2
    : "+r"  (reg_d0)    // output (&input) operand
    :                   // input operand
    :                   // clobbered register
  );

  return reg_d0;
}

//
//  set system information ($01f8)
//
int32_t pcm8a_set_system_information(uint32_t sys_info) {

  register uint32_t reg_d0 asm ("d0") = 0x01fc;
  register uint32_t reg_d1 asm ("d1") = sys_info;

  asm volatile (
    "trap #2\n"         // trap #2
    : "+r"  (reg_d0)    // output (&input) operand
    : "r"   (reg_d1)    // input operand
    :                   // clobbered register
  );

  return reg_d0;
}

//
//  set polyphonic mode ($01fc)
//
int32_t pcm8a_set_polyphonic_mode(int16_t mode) {

  register uint32_t reg_d0 asm ("d0") = 0x01fc;
  register uint32_t reg_d1 asm ("d1") = mode;

  asm volatile (
    "trap #2\n"         // trap #2
    : "+r"  (reg_d0)    // output (&input) operand
    : "r"   (reg_d1)    // input operand
    :                   // clobbered register
  );

  return reg_d0;
}
*/

//
//  pcm8a keep check
//
int32_t pcm8a_isavailable() {

  uint8_t eye_catch_addr_bytes[4];
  for (int16_t i = 0; i < 4; i++) {
    eye_catch_addr_bytes[i] = B_BPEEK((uint8_t*)(0x0088 + i));
  }
  uint8_t* eye_catch_addr = *((uint8_t**)eye_catch_addr_bytes) - 16;

  uint8_t eye_catch1[5];
  for (int16_t i = 0; i < 5; i++) {
    eye_catch1[i] = B_BPEEK(eye_catch_addr + i);
  }

  uint8_t pcm8a_version[3];
  for (int16_t i = 0; i < 3; i++) {
    pcm8a_version[i] = B_BPEEK(eye_catch_addr + i + 5);
  }

  uint8_t eye_catch2[8];
  for (int16_t i = 0; i < 8; i++) {
    eye_catch2[i] = B_BPEEK(eye_catch_addr + i + 8);
  }

  return (memcmp(eye_catch1, "PCM8A", 5) == 0 && memcmp(pcm8a_version, "100", 3) >= 0 && memcmp(eye_catch2, "PCM8/048", 8) == 0 ) ? 1 : 0;
}