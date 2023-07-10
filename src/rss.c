#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "himem.h"
#include "uart.h"
#include "rss.h"

// static buffers for RSSN APIs
static uint8_t request_buf[ 512 ];
static uint8_t response_buf[ 1024 * 128 ];
static uint8_t body_size_buf[ 16 ];

//
//  open rss
//
int32_t rss_open(RSS* rss) {

  // use high memory if we can
  rss->use_high_memory = himem_isavailable() ? 1 : 0;

  return 0;
}

//
//  close rss
//
void rss_close(RSS* rss) {
}

//
//  check pcm remotely
//
int32_t rss_head_pcm(RSS* rss, const char* pcm_path, UART* uart, uint32_t* pcm_file_size) {

  // default return code
  int32_t rc = -1;

  // request
  strcpy(request_buf, ">|        /pcmhead?path=");
  strcat(request_buf, pcm_path);
  size_t request_size = strlen(request_buf);
  sprintf(body_size_buf, "%08x", request_size - 10);
  memcpy(request_buf + 2, body_size_buf, 8);
  if (uart_write(uart, request_buf, request_size) != 0) {
    //printf("error: request write error.\n");
    goto exit;
  }

  // response
  int32_t uart_result = uart_read(uart, response_buf, 14);
  if (uart_result != UART_OK) {
    rc = uart_result;
    goto exit;
  }
  if (memcmp(response_buf, "<|", 2) == 0) {
    uint8_t response_code[] = "    ";
    memcpy(response_code, response_buf + 2, 4);
    rc = atoi(response_code);
  }

  size_t response_size;
  sscanf(response_buf + 6, "%08x", &response_size);
  if (response_size > 1024 * 128 - 16) {
    //printf("error: too large response.\n");
    goto exit;
  }
  if (response_size > 0) {
    uart_result = uart_read(uart, response_buf + 14, response_size);
    if (uart_result != UART_OK) {
      rc = uart_result;
      goto exit;
    }

    if (rc == 200) {
      response_buf[ 14 + response_size ] = '\0';
      *pcm_file_size = atoi(response_buf + 14);
    }
  }

exit:

  return rc;
}

//
//  play pcm remotely
//
int32_t rss_play_pcm(RSS* rss, const char* pcm_path, UART* uart) {

  // default return code
  int32_t rc = -1;

  // request
  strcpy(request_buf, ">|        /pcmplay?path=");
  strcat(request_buf, pcm_path);
  size_t request_size = strlen(request_buf);
  sprintf(body_size_buf, "%08x", request_size - 10);
  memcpy(request_buf + 2, body_size_buf, 8);
  if (uart_write(uart, request_buf, request_size) != 0) {
    //printf("error: request write error.\n");
    goto exit;
  }

  // response
  int32_t uart_result = uart_read(uart, response_buf, 14);
  if (uart_result != UART_OK) {
    rc = uart_result;
    goto exit;
  }
  if (memcmp(response_buf, "<|", 2) == 0) {
    uint8_t response_code[] = "    ";
    memcpy(response_code, response_buf + 2, 4);
    rc = atoi(response_code);
  }

  size_t response_size;
  sscanf(response_buf + 6, "%08x", &response_size);
  if (response_size > 1024 * 128 - 16) {
    //printf("error: too large response.\n");
    goto exit;
  }
  if (response_size > 0) {
    uart_result = uart_read(uart, response_buf + 14, response_size);
    if (uart_result != UART_OK) {
      rc = uart_result;
      goto exit;
    }
  }

exit:

  return rc;
}

//
//  play pcm remotely
//
int32_t rss_stop_pcm(RSS* rss, UART* uart) {

  // default return code
  int32_t rc = -1;

  // request
  strcpy(request_buf, ">|        /pcmstop");
  size_t request_size = strlen(request_buf);
  sprintf(body_size_buf, "%08x", request_size - 10);
  memcpy(request_buf + 2, body_size_buf, 8);
  if (uart_write(uart, request_buf, request_size) != 0) {
    //printf("error: request write error.\n");
    goto exit;
  }

  // response
  int32_t uart_result = uart_read(uart, response_buf, 14);
  if (uart_result != UART_OK) {
    rc = uart_result;
    goto exit;
  }
  if (memcmp(response_buf, "<|0200", 6) != 0) {
    //printf("error: unexpected error code.\n");
    goto exit;
  }

  size_t response_size;
  sscanf(response_buf + 6, "%08x", &response_size);
  if (response_size > 1024 * 128 - 16) {
    //printf("error: too large response.\n");
    goto exit;
  }
  uart_result = uart_read(uart, response_buf + 14, response_size);
  if (uart_result != UART_OK) {
    rc = uart_result;
    goto exit;
  }

  rc = 0;

exit:

  return rc;
}
