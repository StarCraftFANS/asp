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

#include "at89s52_isp.h"
#include "at89s52_io.h"
#include "xmodem.h"
#include "user_io.h"
#include "delay.h"
#include "asp_hw.h"
#include "utility.h"

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

static void signal_xmodem_error(int rc);
static void signal_activity(void);

// Callback functions
static int cb_write_xmodem_data(const XMODEM_PACKET *xpack);
static int cb_verify_xmodem_data(const XMODEM_PACKET *xpack);

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

  TARGET_RESET_PIN = 1; // Activate RESET pin

  // RESET must be active for a while, before any command is sent
  delay_ms(AT89S52_RESET_ACTIVE_MIN_TIME_MS);

  // Enable serial programming
  rc = at89s52_io_enable_programming();
  if (rc != AT89S52_SUCCESS) {
    TARGET_ERR_LED_ON; // Signal error
    return AT89S52_ISP_FAILURE;
  }
  return AT89S52_ISP_SUCCESS;
}

////////////////////////////////////////////////////////////////

static void disable_isp(void)
{
  TARGET_RESET_PIN = 0; // Deactivate RESET pin
}

////////////////////////////////////////////////////////////////

static int isp_probe(void)
{
  uint32_t signature;
  char signature_str[8];

  if (at89s52_io_read_signature(&signature) != AT89S52_SUCCESS) {
    TARGET_ERR_LED_ON; // Signal error
    return AT89S52_ISP_FAILURE;
  }
  if (signature == AT89S52_SIGNATURE) {
    user_io_new_line();
    user_io_put_line("Found AT89S52", 13);
  }
  else {
    user_io_new_line();
    uint32_to_hex_str(signature, signature_str);
    user_io_put("Bad signature=0x", 16);
    user_io_put_line(signature_str, 8);
    TARGET_ERR_LED_ON; // Signal error
    return AT89S52_ISP_FAILURE;
  }
  return AT89S52_ISP_SUCCESS;
}

////////////////////////////////////////////////////////////////

static int isp_erase(void)
{
  if (at89s52_io_chip_erase() != AT89S52_SUCCESS) {
    TARGET_ERR_LED_ON; // Signal error
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
  ACTIV_LED_ON;

  // Receive a file, let callback handle each packet
  // and write packet data to flash.
  g_chip_addr = 0;
  rc = xmodem_recv_file(cb_write_xmodem_data,
			XMODEM_RECV_FLAGS);
  ACTIV_LED_OFF;
  if (rc != XMODEM_SUCCESS) {
    signal_xmodem_error(rc);
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
  ACTIV_LED_ON;

  // Receive a file, let callback handle each packet
  // and compare packet data with data read from flash.
  g_chip_addr = 0;
  rc = xmodem_recv_file(cb_verify_xmodem_data,
			XMODEM_RECV_FLAGS);
  ACTIV_LED_OFF;
  if (rc != XMODEM_SUCCESS) {
    signal_xmodem_error(rc);
    return AT89S52_ISP_FAILURE;
  }

  return AT89S52_ISP_SUCCESS;
}

////////////////////////////////////////////////////////////////

static void signal_xmodem_error(int rc)
{
  // Signal host related (Xmodem) error
  // on selected error codes
  switch(rc) {
  case XMODEM_MEDIA_ERROR:
  case XMODEM_TIMEOUT:
  case XMODEM_END_OF_TRANSFER:
  case XMODEM_BAD_PACKET:
    HOST_ERR_LED_ON; // Signal error
    break;
  }
}

////////////////////////////////////////////////////////////////

static void signal_activity(void)
{
  static uint8_t activity_led_on = 1;

  // Every second Xmodem transfer will toggle activity LED
  if ( (g_chip_addr % (XMODEM_PACKET_DATA_BYTES*2)) == 0 ) {
    if (activity_led_on) {
      ACTIV_LED_ON;
      activity_led_on = 0;
      
    }
    else {
      ACTIV_LED_OFF;
      activity_led_on = 1;
    }
  }
}

////////////////////////////////////////////////////////////////

static int cb_write_xmodem_data(const XMODEM_PACKET *xpack)
{
  int rc;

  signal_activity();

  // Write packet data to chip
  rc = at89s52_io_write_flash(g_chip_addr,
			      xpack->data,
			      XMODEM_PACKET_DATA_BYTES);
  if (rc != AT89S52_SUCCESS) {    
    TARGET_ERR_LED_ON; // Signal error
    return 1;
  }

  g_chip_addr += XMODEM_PACKET_DATA_BYTES; // Next chip address

  return 0;
}

////////////////////////////////////////////////////////////////

static int cb_verify_xmodem_data(const XMODEM_PACKET *xpack)
{
  int rc;
  int i;

  signal_activity();

  // Read data from chip
  rc = at89s52_io_read_flash(g_chip_addr,
			     g_chip_data,
			     XMODEM_PACKET_DATA_BYTES);
  if (rc != AT89S52_SUCCESS) {        
    TARGET_ERR_LED_ON; // Signal error
    return 1;
  }

  // Compare data read from chip and data recived from XMODEM
  for (i=0; i < XMODEM_PACKET_DATA_BYTES; i++) {
    if (g_chip_data[i] != xpack->data[i]) {
      TARGET_ERR_LED_ON; // Signal error
      return 1;
    }
  }

  g_chip_addr += XMODEM_PACKET_DATA_BYTES; // Next chip address

  return 0;
}
