// ************************************************************************
// *                                                                      *
// * Copyright (C) 2014 Bonden i Nol (hakanbrolin@hotmail.com)            *
// *                                                                      *
// * This program is free software; you can redistribute it and/or modify *
// * it under the terms of the GNU General Public License as published by *
// * the Free Software Foundation; either version 2 of the License, or    *
// * (at your option) any later version.                                  *
// *                                                                      *
// ************************************************************************

#include <stdio.h>

#include "at89s52_isp.h"
#include "at89s52_io.h"
#include "xmodem.h"
#include "user_io.h"
#include "delay.h"

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
#define XMODEM_RECV_FLAGS  (XMODEM_FLAG_CRC)
//#define XMODEM_RECV_FLAGS 0

/////////////////////////////////////////////////////////////////////////////
//               Function prototypes
/////////////////////////////////////////////////////////////////////////////
static int enable_isp(void);
static void disable_isp(void);
static int isp_probe(void);
static int isp_erase(void);
static int isp_read_bin(void);
static int isp_write_bin(void);
static int isp_verify_bin(void);

// Callback functions
static int cb_write_xmodem_data(const XMODEM_PACKET *xpack);
static int cb_verify_xmodem_data(const XMODEM_PACKET *xpack);

static void print_xmodem_packet(const XMODEM_PACKET *xpack);

/////////////////////////////////////////////////////////////////////////////
//               Global variables
/////////////////////////////////////////////////////////////////////////////
static uint16_t g_chip_addr; // Keep track of current chip address

static uint8_t g_chip_data[XMODEM_PACKET_DATA_BYTES];

/////////////////////////////////////////////////////////////////////////////
//               Public functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

int at89s52_isp_probe(void)
{
  int rc;

  rc = enable_isp();  // Activate RESET pin
  if (rc != AT89S52_ISP_SUCCESS) goto at89s52_isp_probe_exit;
  rc = isp_probe();

 at89s52_isp_probe_exit:
  disable_isp();     // Deactivate RESET pin
  return rc;
}

////////////////////////////////////////////////////////////////

int at89s52_isp_erase(void)
{
  int rc;

  rc = enable_isp();  // Activate RESET pin
  if (rc != AT89S52_ISP_SUCCESS) goto at89s52_isp_erase_exit;
  rc = isp_erase();

 at89s52_isp_erase_exit:
  disable_isp();      // Deactivate RESET pin
  return rc;
}

////////////////////////////////////////////////////////////////

int at89s52_isp_read_bin(void)
{
  int rc;

  rc = enable_isp();  // Activate RESET pin
  if (rc != AT89S52_ISP_SUCCESS) goto at89s52_isp_read_bin_exit;
  rc = isp_read_bin();

 at89s52_isp_read_bin_exit:
  disable_isp();     // Deactivate RESET pin
  return rc;
}

////////////////////////////////////////////////////////////////

int at89s52_isp_write_bin(void)
{
  int rc;

  rc = enable_isp();  // Activate RESET pin
  if (rc != AT89S52_ISP_SUCCESS) goto at89s52_isp_write_bin_exit;
  rc = isp_erase();
  if (rc != AT89S52_ISP_SUCCESS) goto at89s52_isp_write_bin_exit;
  rc = isp_write_bin();

 at89s52_isp_write_bin_exit:
  disable_isp();     // Deactivate RESET pin
  return rc;
}

////////////////////////////////////////////////////////////////

int at89s52_isp_verify_bin(void)
{
  int rc;

  rc = enable_isp();  // Activate RESET pin
  if (rc != AT89S52_ISP_SUCCESS) goto at89s52_isp_verify_bin_exit;
  rc = isp_verify_bin();

 at89s52_isp_verify_bin_exit:
  disable_isp();     // Deactivate RESET pin
  return rc;
}

/////////////////////////////////////////////////////////////////////////////
//               Private functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

static int enable_isp(void)
{
  int rc;

  // Activate RESET pin

  // RESET must be active for a while, before any command is sent
  delay_ms(AT89S52_RESET_ACTIVE_MIN_TIME_MS);

  // Enable serial programming
  rc = at89s52_io_enable_programming();
  if (rc != AT89S52_SUCCESS) {
    return AT89S52_ISP_FAILURE;
  }
  return AT89S52_ISP_SUCCESS;
}

////////////////////////////////////////////////////////////////

static void disable_isp(void)
{
  // Deactivate RESET pin
}

////////////////////////////////////////////////////////////////

static int isp_probe(void)
{
  uint32_t signature;

  if (at89s52_io_read_signature(&signature) != AT89S52_SUCCESS) {
    return AT89S52_ISP_FAILURE;
  }
  if (signature == AT89S52_SIGNATURE) {
    user_io_new_line();
    user_io_put_line("Found AT89S52", 13);
  }
  else {
    return AT89S52_ISP_FAILURE;
  }
  return AT89S52_ISP_SUCCESS;
}

////////////////////////////////////////////////////////////////

static int isp_erase(void)
{
  if (at89s52_io_chip_erase() != AT89S52_SUCCESS) {
    return AT89S52_ISP_FAILURE;
  }
  return AT89S52_ISP_SUCCESS;
}

////////////////////////////////////////////////////////////////

static int isp_read_bin(void)
{
  return AT89S52_ISP_SUCCESS;
}

////////////////////////////////////////////////////////////////

static int isp_write_bin(void)
{
  int rc;

  user_io_new_line();
  user_io_put_line("XMODEM send now", 15);

  // Receive a file, let callback handle each packet
  // and write packet data to flash.
  g_chip_addr = 0;
  rc = xmodem_recv_file(cb_write_xmodem_data,
			XMODEM_RECV_FLAGS);
  if (rc != XMODEM_SUCCESS) {
    return AT89S52_ISP_FAILURE;
  }

  return AT89S52_ISP_SUCCESS;
}

////////////////////////////////////////////////////////////////

static int isp_verify_bin(void)
{
  int rc;

  user_io_new_line();
  user_io_put_line("XMODEM send now", 15);

  // Receive a file, let callback handle each packet
  // and compare packet data with data read from flash.
  g_chip_addr = 0;
  rc = xmodem_recv_file(cb_verify_xmodem_data,
			XMODEM_RECV_FLAGS);
  if (rc != XMODEM_SUCCESS) {
    return AT89S52_ISP_FAILURE;
  }

  return AT89S52_ISP_SUCCESS;
}

////////////////////////////////////////////////////////////////

static int cb_write_xmodem_data(const XMODEM_PACKET *xpack)
{
  int rc;

  print_xmodem_packet(xpack);

  // Write packet data to chip
  rc = at89s52_io_write_flash(g_chip_addr,
			      xpack->data,
			      XMODEM_PACKET_DATA_BYTES);
  if (rc != AT89S52_SUCCESS) {    
    return 1;
  }

  g_chip_addr += XMODEM_PACKET_DATA_BYTES; // Next chip address

  return 0;
}

////////////////////////////////////////////////////////////////

static int cb_verify_xmodem_data(const XMODEM_PACKET *xpack)
{
  int rc;

  print_xmodem_packet(xpack);

  // Read data from chip
  rc = at89s52_io_read_flash(g_chip_addr,
			     g_chip_data,
			     XMODEM_PACKET_DATA_BYTES);
  if (rc != AT89S52_SUCCESS) {        
    return 1;
  }

  // Compare data read from chip and data recived from XMODEM
  for (int i=0; i < XMODEM_PACKET_DATA_BYTES; i++) {
    if (g_chip_data[i] != xpack->data[i]) {
      return 1;
    }
  }

  g_chip_addr += XMODEM_PACKET_DATA_BYTES; // Next chip address

  return 0;
}

////////////////////////////////////////////////////////////////

static void print_xmodem_packet(const XMODEM_PACKET *xpack)
{
  // Print packet information
  printf("---- PACKET : %d\n", xpack->seq);
  printf("  header   = 0x%02x\n", xpack->header);
  printf("  seq      = 0x%02x\n", xpack->seq);
  printf("  cmpl_seq = 0x%02x\n", xpack->cmpl_seq);
  printf("  data[0]  = 0x%02x\n", xpack->data[0]);
  printf("  data[1]  = 0x%02x\n", xpack->data[1]);
  printf("  data[126]= 0x%02x\n", xpack->data[126]);
  printf("  data[127]= 0x%02x\n", xpack->data[127]);
  printf("  csum[0]  = 0x%02x\n", xpack->csum[0]);
  printf("  csum[1]  = 0x%02x\n", xpack->csum[1]);
}
