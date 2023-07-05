// base
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// dos/iocs call
#include <doslib.h>
#include <iocslib.h>

// devices
#include "keyboard.h"
#include "himem.h"

// drivers
#include "pcm8a.h"
#include "pcm8pp.h"
#include "macsdrv.h"

// application
#include "mcsex.h"

// abort vector handler
static volatile int32_t g_abort_flag = 0;
static void abort_application() {
  g_abort_flag = 1;
}

//
//  show help message
//
static void show_help_message() {
  printf("usage: mcsex [options] <filename.mcs>\n");
  printf("options:\n");
  printf("       -l<n> ... loop count (0=endless, default:1)\n");
  printf("       -b<n> ... bulk load chunk size in MB (1-32, default:16)\n");
  printf("       -h    ... show help message\n");
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

  // mcs file name
  uint8_t* mcs_file_name = NULL;

  // mcs file buffer
  uint8_t* mcs_file_buffer = NULL;

  // mcs file handle
  FILE* fp = NULL;

  // set abort vectors
  uint32_t abort_vector1 = INTVCS(0xFFF1, (int8_t*)abort_application);
  uint32_t abort_vector2 = INTVCS(0xFFF2, (int8_t*)abort_application);  

  // credit
  printf("MCSEX.X - Yet another high memory MACS player version " PROGRAM_VERSION " by tantan\n");

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

  // macsdrv.x check
  if (!macs_isavailable()) {
    printf("error: MACSDRV is not available.\n");
    goto exit;
  }

  // pcm8a.x / pcm8pp.x check
  if (!pcm8a_isavailable() && !pcm8pp_isavailable()) {
    printf("error: PCM8A/PCM8PP is not available.\n");
    goto exit;
  }

  // HIMEM check
  if (!himem_isavailable()) {
    printf("error: High memory driver is not available.\n");
    goto exit;
  }

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
  printf("File name   : %s\n", mcs_file_name);
  printf("File size   : %d bytes\n", mcs_file_len);
  size_t header_ofs = 8;
  while (header_ofs < read_len) {
    if (memcmp(mcs_file_buffer + header_ofs, "DUALPCM/PCM8PP:", 15) == 0 ||
        memcmp(mcs_file_buffer + header_ofs, "PCM8PP:", 7) == 0 ||
        memcmp(mcs_file_buffer + header_ofs, "ADPCM:", 6) == 0) {
      printf("Audio codec : %s\n", mcs_file_buffer + header_ofs);
      header_ofs += strlen(mcs_file_buffer + header_ofs);
    }
    if (memcmp(mcs_file_buffer + header_ofs, "TITLE:", 6) == 0) {
      printf("Title       : %s\n", mcs_file_buffer + header_ofs + 6);
      header_ofs += strlen(mcs_file_buffer + header_ofs);
    }
    if (memcmp(mcs_file_buffer + header_ofs, "COMMENT:", 8) == 0) {
      printf("Comment     : %s\n", mcs_file_buffer + header_ofs + 8);
      header_ofs += strlen(mcs_file_buffer + header_ofs);
    }
    header_ofs++;
  }

  // read body
  printf("\nNow loading ... Press [ESC]/[Q] to cancel.\n");
  uint32_t t0 = ONTIME();
  g_abort_flag = 0;
  do {
    size_t remain = mcs_file_len - read_len;
    size_t len = fread(mcs_file_buffer + read_len, 1, chunk_size < remain ? chunk_size : remain, fp);
    if (len == 0) break;
    read_len += len;
    printf("\r%d/%d (%4.2f%%)", read_len, mcs_file_len, read_len * 100.0 / mcs_file_len);
    if (B_KEYSNS() != 0) {
      int16_t scan_code = B_KEYINP() >> 8;
      if (scan_code == KEY_SCAN_CODE_ESC || scan_code == KEY_SCAN_CODE_Q) {
        printf("\nCanceled.\n");
        goto exit;
      }
    }
    if (g_abort_flag) {
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

  // play
  int32_t rc_macs = macs_play(mcs_file_buffer);
  if (rc_macs == -4) {
    printf("Aborted.\n");
    goto exit;
  } else if (rc_macs == -1) {
    printf("error: not MACS data.\n");
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

  // flush key buffer
  KFLUSHIO(0xff);

  // resume abort vectors
  INTVCS(0xFFF1, (int8_t*)abort_vector1);
  INTVCS(0xFFF2, (int8_t*)abort_vector2);  

  return rc;
}