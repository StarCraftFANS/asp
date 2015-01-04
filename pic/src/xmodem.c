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

//#define DEBUG_X86  // Undefine for no printouts

#ifdef DEBUG_X86
#include <stdio.h> // Debug printouts
#endif

#include "xmodem.h"
#include "rs232.h"

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG_X86
#define DEBUG_PRINTF(fmt, args...) printf(fmt, ##args);
#else
#define DEBUG_PRINTF(fmt, args...) 
#endif

/////////////////////////////////////////////////////////////////////////////
//               Function prototypes
/////////////////////////////////////////////////////////////////////////////
static int xmodem_send_ctrl(uint8_t ctrl);

static int xmodem_cancel_transfer(void);

static int xmodem_recv(int retries,
		       int timeout_s,
		       int use_crc,
		       uint8_t expected_seq,
		       XMODEM_PACKET *packet);

static int xmodem_do_recv(int timeout_s,
			  int use_crc,
			  uint8_t expected_seq,
			  XMODEM_PACKET *xpack);

static uint16_t xmodem_crc_update(uint16_t crc,
				  uint8_t data);

/////////////////////////////////////////////////////////////////////////////
//               Global variables
/////////////////////////////////////////////////////////////////////////////
XMODEM_PACKET g_xpack;

/////////////////////////////////////////////////////////////////////////////
//               Public functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

int xmodem_recv_file(XMODEM_PACKET_CALLBACK packet_callback,
		     int flags)
{
  int rc = XMODEM_SUCCESS;
  int handle_packets = 1;
  int retries;
  int timeout_s;
  int use_crc = 0;
  uint8_t packet_seq = 1;

  // Check flags
  if (flags & XMODEM_FLAG_CRC) {
    use_crc = 1;
  }

  // Begin transmission
  if (use_crc) {
    retries = 3;
    timeout_s = 3;
    xmodem_send_ctrl(XMODEM_CTRL_CRC); // Use 16-bit CRC    
  }
  else {
    retries = 10;
    timeout_s = 10;
    xmodem_send_ctrl(XMODEM_CTRL_NAK); // Old school, no CRC
  }

  DEBUG_PRINTF("RECV_FILE: crc=%d, retries=%d, timeout=%d\n",
	       use_crc, retries, timeout_s);

  // Receive entire file, packet by packet
  // For each packet, let callback handle packet
  while (handle_packets) {
    rc = xmodem_recv(retries,
		     timeout_s,
		     use_crc,
		     packet_seq,
		     &g_xpack);
    if (rc == XMODEM_SUCCESS) {
      if (packet_callback) {
	if (packet_callback(&g_xpack) != 0) {
	  xmodem_cancel_transfer(); // Callback failed, cancel transfer
	  rc = XMODEM_CALLBACK_ERROR;
	  handle_packets = 0;
	}
	else {
	  xmodem_send_ctrl(XMODEM_CTRL_ACK); // Callback OK
        }
      }
      else {
	xmodem_send_ctrl(XMODEM_CTRL_ACK); // No callback
      }
      timeout_s = 1; // Switch to 1s timeout once a packet is received
      retries = 10;  // Switch to 10 retries once a packet is received
      packet_seq++;
    }
    else if (rc == XMODEM_END_OF_TRANSFER) {
      xmodem_send_ctrl(XMODEM_CTRL_ACK);
      rc = XMODEM_SUCCESS;
      handle_packets = 0;
    }
    else {
      if (use_crc &&
	  (packet_seq == 1) &&
	  (rc == XMODEM_TIMEOUT) ) {
	use_crc = 0; // Switch back to original Xmodem
	retries = 10;
	timeout_s = 10;
	DEBUG_PRINTF("RECV_FILE: crc=%d, retries=%d, timeout=%d\n",
		     use_crc, retries, timeout_s);
      }
      else {
	handle_packets = 0;
      }
    }
  }
  return rc;
}

/////////////////////////////////////////////////////////////////////////////
//               Private functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

static int xmodem_send_ctrl(uint8_t ctrl)
{
  if (rs232_send(ctrl) != RS232_SUCCESS) {
    return XMODEM_MEDIA_ERROR;
  }
  return XMODEM_SUCCESS;
}

////////////////////////////////////////////////////////////////

static int xmodem_cancel_transfer(void)
{
  int rc;

  // A doube CAN will cancel the transfer
  rc = xmodem_send_ctrl(XMODEM_CTRL_CAN);
  if (rc != XMODEM_SUCCESS) {
    return rc;
  }
  rc = xmodem_send_ctrl(XMODEM_CTRL_CAN);
  if (rc != XMODEM_SUCCESS) {
    return rc;
  }

  // Flush any received bytes
  // still being sent by peer
  rs232_purge_receiver();

  return XMODEM_SUCCESS;
}

////////////////////////////////////////////////////////////////

static int xmodem_recv(int retries,
		       int timeout_s,
		       int use_crc,
		       uint8_t expected_seq,
		       XMODEM_PACKET *xpack)
{
  int rc = XMODEM_SUCCESS;

  while (retries > 0) {
    rc = xmodem_do_recv(timeout_s, use_crc, expected_seq, xpack);
    if ( (rc == XMODEM_SUCCESS) ||
	 (rc == XMODEM_END_OF_TRANSFER) ) {
      break;
    }
    else {
      if ( use_crc &&
	   (expected_seq == 1) &&
	   (retries > 1) ) {
	xmodem_send_ctrl(XMODEM_CTRL_CRC);
      }
      else {
	xmodem_send_ctrl(XMODEM_CTRL_NAK);
      }
    }
    retries--;
  }
  
  // Cancel transfer on any error
  if ( (rc != XMODEM_SUCCESS) &&
       (rc != XMODEM_END_OF_TRANSFER) ) {
    xmodem_cancel_transfer();
  }

  return rc;
}

////////////////////////////////////////////////////////////////

static int xmodem_do_recv(int timeout_s,
			  int use_crc,
			  uint8_t expected_seq,
			  XMODEM_PACKET *xpack)
{
  int rc;
  unsigned bytes_to_receive;
  uint8_t *packet_p = (uint8_t *)xpack;
  uint8_t sum;
  uint16_t crc;
  uint16_t expected_crc;
  unsigned i;

  if (use_crc) {
    bytes_to_receive = XMODEM_PACKET_DATA_BYTES + 5;
  }
  else {
    bytes_to_receive = XMODEM_PACKET_DATA_BYTES + 4;
  }

  // Receive entire packet, character by character
  for (i=0; i < bytes_to_receive; i++) {    

    // Receive one character in packet
    rc = rs232_recv_timeout(&packet_p[i], timeout_s);
    switch (rc) {
    case RS232_SUCCESS:
      break;
    case RS232_WOULD_BLOCK:
      DEBUG_PRINTF("TIMEOUT ERROR\n");
      rs232_purge_receiver();
      return XMODEM_TIMEOUT;
      break;
    case RS232_FAILURE:
    default:
      DEBUG_PRINTF("MEDIA ERROR\n");
      return XMODEM_MEDIA_ERROR;
    }

    // Check packet components
    if ( (i==0) && (packet_p[i] != XMODEM_CTRL_SOH) ) {
      if (packet_p[i] == XMODEM_CTRL_EOT) {
	DEBUG_PRINTF("EOT\n");
	return XMODEM_END_OF_TRANSFER;
      }
      else {
	DEBUG_PRINTF("UNEXPECTED HEADER : i=%d, h=%d\n",
		     i, packet_p[i]);
	rs232_purge_receiver();
	return XMODEM_BAD_PACKET;
      }
    }
    if ( (i==1) && (packet_p[i] != expected_seq) ) {
      DEBUG_PRINTF("UNEXPECTED SEQ : i=%d, %d / %d\n",
		   i, packet_p[i], expected_seq);
      rs232_purge_receiver();
      return XMODEM_BAD_PACKET;
    }
    if ( (i==2) && (packet_p[i] != (255-expected_seq)) ) {
      DEBUG_PRINTF("UNEXPECTED COMPL SEQ\n");
      rs232_purge_receiver();
      return XMODEM_BAD_PACKET;
    }
  }
  
  // Calculate packet checksum
  if (use_crc) {
    expected_crc = xpack->csum[0] << 8;
    expected_crc |= xpack->csum[1];
    crc = 0;
    for (i=0; i < XMODEM_PACKET_DATA_BYTES; i++) {
      crc = xmodem_crc_update(crc, xpack->data[i]);
    }
    if (crc != expected_crc) {
      DEBUG_PRINTF("BAD CHECKSUM (CRC)\n");
      rs232_purge_receiver();
      return XMODEM_BAD_PACKET;
    }
  }
  else {
    sum = 0;
    for (i=0; i < XMODEM_PACKET_DATA_BYTES; i++) {
      sum += xpack->data[i];
    }
    if (sum != xpack->csum[0]) {
      DEBUG_PRINTF("BAD CHECKSUM (SUM)\n");
      rs232_purge_receiver();
      return XMODEM_BAD_PACKET;
    }
  }

  return XMODEM_SUCCESS;
}

////////////////////////////////////////////////////////////////

static uint16_t xmodem_crc_update(uint16_t crc,
				  uint8_t data)
{
  int i;

  crc = crc ^ ((uint16_t)data << 8);

  for (i=0; i<8; i++) {
    if (crc & 0x8000) {
      crc = (crc << 1) ^ 0x1021;
    }
    else {
      crc <<= 1;
    }
  }
  return crc;
}
