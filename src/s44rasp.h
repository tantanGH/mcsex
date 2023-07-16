#ifndef __H_S44RASP__
#define __H_S44RASP__

#include <stdio.h>
#include <stdint.h>
#include "uart.h"

#define S44RASP_OK       (0)
#define S44RASP_EXIT     (1)
#define S44RASP_QUIT     (2)
#define S44RASP_TIMEOUT  (3)

typedef struct {
  int16_t use_high_memory;
} S44RASP;

int32_t s44rasp_open(S44RASP* s44rasp);
void s44rasp_close(S44RASP* s44rasp);

int32_t s44rasp_head(S44RASP* s44rasp, const char* pcm_path, UART* uart, uint32_t* pcm_file_size);
int32_t s44rasp_play(S44RASP* s44rasp, const char* pcm_path, UART* uart);
int32_t s44rasp_stop(S44RASP* s44rasp, UART* uart);

#endif