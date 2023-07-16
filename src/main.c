// base
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// dos/iocs call
#include <doslib.h>
#include <iocslib.h>

// devices
//#include "keyboard.h"
#include "himem.h"

// uart
#include "uart.h"
#include "s44rasp.h"

// drivers
#include "pcm8a.h"
#include "pcm8pp.h"
#include "macsdrv.h"

// application
#include "mcsex.h"

// mcs file buffer (high memory)
static uint8_t* mcs_file_buffer = NULL;

// abort vector handler
static void abort_application() {
  
  // reclaim high memory
  if (mcs_file_buffer != NULL) {
    himem_free(mcs_file_buffer, 1);
    mcs_file_buffer = NULL;
  }

  // flush key buffer
  KFLUSHIO(0xff);

  // cursor on
  C_CURON();

  // abort exit
  EXIT2(1);
}

//
//  show help message
//
static void show_help_message() {
  printf("usage: mcsex [options] <filename.mcs>\n");
  printf("options:\n");
  printf("       -l<n> ... loop count (0=endless, default:no loop)\n");
  printf("       -b<n> ... bulk load chunk size in MB (1-32, default:16)\n");
  printf("       -h    ... show help message\n");
  printf("\n");
  printf("       -r <remote-mcs-path> ... remote mcs/pcm path (for s44rasp)\n");
}

//
//  main
//
int32_t main(int32_t argc, uint8_t* argv[]) {

  // default return code
  int32_t rc = -1;

  // loop count
  int32_t loop_count = 1;

  // chunk size
  int32_t chunk_size = 16 * 1024 * 1024;

  // pcm driver
  int16_t use_pcm8a = 0;
  int16_t use_pcm8pp = 0;

  // mcs file name
  uint8_t* mcs_file_name = NULL;

  // remote mcs use
  int16_t use_remote_mcs = 0;

  // remote mcs path
  uint8_t* remote_mcs_path = NULL;

  // baud rate (uart)
  int32_t baud_rate = 38400;

  // timeout (uart)
  int32_t timeout = 60;

  // mcs file handle
  FILE* fp = NULL;

  // uart instance
  UART uart = { 0 };

  // s44rasp instance
  S44RASP s44rasp = { 0 };

  // set abort vectors
  uint32_t abort_vector1 = INTVCS(0xFFF1, (int8_t*)abort_application);
  uint32_t abort_vector2 = INTVCS(0xFFF2, (int8_t*)abort_application);  

  // credit
  printf("MCSEX.X - Yet another high memory MACS loader version " PROGRAM_VERSION " by tantan\n");

  // parse command lines
  for (int16_t i = 1; i < argc; i++) {
    if (argv[i][0] == '-' && strlen(argv[i]) >= 2) {
      if (argv[i][1] == 'b') {
        int32_t b = atoi(argv[i]+2);
        if (b < 1 || b > 32) {
          show_help_message();
          goto exit;
        }
        chunk_size = b * 1024 * 1024;
      } else if (argv[i][1] == 'l') {
        if (strlen(argv[i]) == 2) {
          loop_count = 0;
        } else {
          loop_count = atoi(argv[i]+2);
        }
      } else if (argv[i][1] == 'h') {
        show_help_message();
        goto exit;
      } else if (argv[i][1] == 'r' && i+1 < argc) {
        use_remote_mcs = 1;
        remote_mcs_path = argv[i+1];
        i++;      
      } else {
        show_help_message();
        goto exit;
      }
    } else {
      mcs_file_name = argv[i];
    }
  }

  // input file
  if (mcs_file_name == NULL) {
    show_help_message();
    goto exit;
  }

  // HIMEM check
  if (!himem_isavailable()) {
    printf("error: High memory driver is not available.\n");
    goto exit;
  }

  // pcm8a.x / pcm8pp.x check
  if (pcm8a_isavailable()) {
    use_pcm8a = 1;
  } else if (pcm8pp_isavailable()) {
    use_pcm8pp = 1;
  }
  if (!use_pcm8a && !use_pcm8pp) {
    printf("error: PCM8A/PCM8PP is not available.\n");
    goto exit;
  }

  // macsdrv.x check
  if (!macsdrv_isavailable()) {
    printf("error: MACSDRV is not available.\n");
    goto exit;
  }

  if (use_remote_mcs) {
    // open uart  
    if (uart_open(&uart, baud_rate, timeout) != 0) {
      goto exit;
    }
    // open s44rasp
    if (s44rasp_open(&s44rasp) != 0) {
      goto exit;
    }
  }

  // cursor off
  C_CUROFF();

  // get file attrib
  struct FILBUF filbuf;
  if (FILES(&filbuf, mcs_file_name, 0x21) < 0) {
    printf("error: MACS file open error.\n");
    goto exit;
  }

  // allocate high memory buffer
  size_t mcs_file_len = filbuf.filelen;
  mcs_file_buffer = himem_malloc(mcs_file_len, 1);
  if (mcs_file_buffer == NULL) {
    printf("error: high memory allocation error. (out of memory)\n");
    goto exit;
  }

  if (use_remote_mcs) {
    // check remote mcs existence and get size
    uint32_t remote_mcs_size = 0;
    int32_t s44rasp_result = s44rasp_head(&s44rasp, remote_mcs_path, &uart, &remote_mcs_size);
    // check communication result
    if (s44rasp_result == UART_QUIT || s44rasp_result == UART_EXIT) {
      printf("\rerror: canceled.\n");
      goto exit;
    } else if (s44rasp_result == UART_TIMEOUT) { 
      printf("\rerror: timeout.\n");
      goto exit;
    } else if (s44rasp_result == 404) { 
      printf("\rerror: not found.\n");
      goto exit;
    } else if (s44rasp_result != 200) {
      printf("\rerror: communication error. (%d)\n", s44rasp_result);
      goto exit;
    }
  }

  // open file
  fp = fopen(mcs_file_name, "rb");
  if (fp == NULL) {
    printf("error: MACS file open error.\n");
    goto exit;
  }

  // read header
  size_t read_len = fread(mcs_file_buffer, 1, 512, fp);
  if (read_len < 64 || memcmp(mcs_file_buffer, "MACSDATA", 8) != 0) {
    printf("error: not MACS data.\n");
    goto exit;
  }
  printf("\n");
  printf("File name    : %s\n", mcs_file_name);
  printf("File size    : %d bytes\n", mcs_file_len);
  printf("PCM driver   : %s\n", use_remote_mcs ? "S44RASP" :
                                use_pcm8a ? "PCM8A" :
                                use_pcm8pp ? "PCM8PP" : "???");
  size_t header_ofs = 8;
  while (header_ofs < read_len) {
    if (memcmp(mcs_file_buffer + header_ofs, "DUALPCM/PCM8PP:", 15) == 0 ||
        memcmp(mcs_file_buffer + header_ofs, "PCM8PP:", 7) == 0 ||
        memcmp(mcs_file_buffer + header_ofs, "ADPCM:", 6) == 0) {
      printf("PCM format   : %s\n", mcs_file_buffer + header_ofs);
      header_ofs += strlen(mcs_file_buffer + header_ofs);
    }
    if (memcmp(mcs_file_buffer + header_ofs, "TITLE:", 6) == 0) {
      printf("MACS title   : %s\n", mcs_file_buffer + header_ofs + 6);
      header_ofs += strlen(mcs_file_buffer + header_ofs);
    }
    if (memcmp(mcs_file_buffer + header_ofs, "COMMENT:", 8) == 0) {
      printf("MACS comment : %s\n", mcs_file_buffer + header_ofs + 8);
      header_ofs += strlen(mcs_file_buffer + header_ofs);
    }
    header_ofs++;
  }

  // read body
  printf("\nNow loading ... Press [SHIFT] to cancel.\n");
  uint32_t t0 = ONTIME();
  do {
    size_t remain = mcs_file_len - read_len;
    size_t len = fread(mcs_file_buffer + read_len, 1, chunk_size < remain ? chunk_size : remain, fp);
    if (len == 0) break;
    read_len += len;
    printf("\r%d/%d (%4.2f%%)", read_len, mcs_file_len, read_len * 100.0 / mcs_file_len);
    if (B_SFTSNS() & 0x01) {
      printf("\nCanceled.\n");
      goto exit;
    }
  } while (read_len < mcs_file_len);

  // read check
  if (read_len < mcs_file_len) {
    printf("\nerror: MACS file read error.\n");
    goto exit;
  } else {
    uint32_t t1 = ONTIME();
    printf("\r\x1b[KLoaded %d bytes in %2.2f secs.\n", read_len, (t1 - t0 + 1) / 100.0);
  }

loop:

  // play remote MCS
  if (use_remote_mcs) {
    s44rasp_play(&s44rasp, remote_mcs_path, &uart);
  }

  // play
  int32_t rc_macs = macsdrv_play(mcs_file_buffer, use_remote_mcs);

  // stop remote MCS
  if (use_remote_mcs) {
    s44rasp_stop(&s44rasp, &uart);
  }

  if (rc_macs == -4) {
    printf("Aborted.\n");
    goto exit;
  } else if (rc_macs < 0) {
    printf("error: MACS call returned error code %d.\n", rc_macs);
    goto exit;
  }

  // loop check
  if (loop_count == 0 || --loop_count > 0) goto loop;

exit:

  // close file handle
  if (fp != NULL) {
    fclose(fp);
    fp = NULL;
  }

  // reclaim high memory
  if (mcs_file_buffer != NULL) {
    himem_free(mcs_file_buffer, 1);
    mcs_file_buffer = NULL;
  }

  if (use_remote_mcs) {
    // stop pcm
    s44rasp_stop(&s44rasp, &uart);
    // close s44rasp
    s44rasp_close(&s44rasp);
    // close uart
    uart_close(&uart);
  }

  // flush key buffer
  KFLUSHIO(0xff);

  // graphic on & palette clear
  G_CLR_ON();

  // cursor on
  C_CURON();

  // resume abort vectors
  INTVCS(0xFFF1, (int8_t*)abort_vector1);
  INTVCS(0xFFF2, (int8_t*)abort_vector2);  

  return rc;
}