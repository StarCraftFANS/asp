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

#define DEBUG_X86  // Undefine for no printouts

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

static int xmodem_initialize_send_file(int *use_crc);

static int xmodem_finalize_send_file(void);

static int xmodem_send(int use_crc,
		       uint8_t packet_seq,
		       XMODEM_PACKET *xpack);

static int xmodem_do_send(int use_crc,
			  uint8_t packet_seq,
			  XMODEM_PACKET *xpack);

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

int xmodem_send_file(XMODEM_PACKET_CALLBACK_SEND packet_callback,
		     uint16_t packets,
		     int flags)
{
  int rc = XMODEM_SUCCESS;
  int handle_packets = 1;
  int use_crc = 0;
  uint8_t packet_seq = 1;

  // Check flags
  if (flags & XMODEM_FLAG_CRC) {
    use_crc = 1;  // 16-bit CRC is supported
  }
  else {
    use_crc = 0;  // Old school, no CRC
  }

  DEBUG_PRINTF("SEND_FILE: crc=%d\n", use_crc);

  // Wait for receiver to send NAK or CRC request
  rc = xmodem_initialize_send_file(&use_crc);
  if (rc != XMODEM_SUCCESS) {
    return rc;
  }

  // Send entire file, packet by packet
  // For each packet, let callback handle packet
  while (handle_packets) {
    if (packet_callback) {
      if (packet_callback(&g_xpack) != 0) {
	xmodem_cancel_transfer(); // Callback failed, cancel transfer
	rc = XMODEM_CALLBACK_ERROR;
	handle_packets = 0;
	continue;
      }
    }    
    rc = xmodem_send(use_crc,
		     packet_seq,
		     &g_xpack);
    if (rc == XMODEM_SUCCESS) {
      packet_seq++;
      packets--;
      if (packets == 0) {
	handle_packets = 0;
      }
    }
    else {
      handle_packets = 0;
    }
  }

  if (rc != XMODEM_SUCCESS) {
    return rc;
  }

  // End of transfer
  rc = xmodem_finalize_send_file();
  return rc;
}

////////////////////////////////////////////////////////////////

int xmodem_recv_file(XMODEM_PACKET_CALLBACK_RECV packet_callback,
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

static int xmodem_initialize_send_file(int *use_crc)
{
  int rc;
  int i;
  uint8_t data;

  // Wait for receiver to send NAK or CRC request
  // Use 1-minute high-level-timeout
  for (i=0; i < 10; i++) {
    rc = rs232_recv_timeout(&data, 6);
    if (rc == RS232_SUCCESS) {
      if ( (data == XMODEM_CTRL_CRC) && *use_crc ) {
	DEBUG_PRINTF("CRC REQUEST\n");
	return XMODEM_SUCCESS; // CRC request
      }
      if (data == XMODEM_CTRL_NAK) {
	DEBUG_PRINTF("NAK REQUEST\n");
	*use_crc = 0;
	return XMODEM_SUCCESS; // NAK request
      }
    }
    else if (rc == RS232_WOULD_BLOCK) {
      DEBUG_PRINTF("WAITING FOR REQUEST\n");
      continue;
    }
    else {
      return XMODEM_MEDIA_ERROR;
    }
  }
  return XMODEM_TIMEOUT;
}

////////////////////////////////////////////////////////////////

static int xmodem_finalize_send_file(void)
{
  int rc;
  int i;
  uint8_t data; 

  // Signal End Of Transfer
  // Wait for receiver to send ACK
  // Use 1-minute high-level-timeout
  for (i=0; i < 10; i++) {

    rc = xmodem_send_ctrl(XMODEM_CTRL_EOT);
    if (rc != RS232_SUCCESS) {
      return XMODEM_MEDIA_ERROR;
    }

    rc = rs232_recv_timeout(&data, 6);
    if (rc == RS232_SUCCESS) {
      if (data == XMODEM_CTRL_ACK) {
	return XMODEM_SUCCESS; // ACK received
      }
    }
  }
  return XMODEM_TIMEOUT;
}

////////////////////////////////////////////////////////////////

static int xmodem_send(int use_crc,
		       uint8_t packet_seq,
		       XMODEM_PACKET *xpack)
{
  int rc = XMODEM_SUCCESS;
  int retries = 10;
  int do_send = 1;
  uint8_t data;

  while (retries > 0) {
    if (do_send) {
      rc = xmodem_do_send(use_crc, packet_seq, xpack);
      if (rc != XMODEM_SUCCESS) {
	break;
      }
    }

    // Wait for receiver to respond
    rc = rs232_recv_timeout(&data, 6);
    if (rc == RS232_SUCCESS) {
      if (data == XMODEM_CTRL_ACK) {
	DEBUG_PRINTF("ACK RECEIVED : seq=%u, retry=%d\n",
		     packet_seq, retries);
	rc = XMODEM_SUCCESS;     // Receiver acked, we are done
	break;
      }
      else if (data == XMODEM_CTRL_NAK) {
	rc = XMODEM_TIMEOUT;     // Send again, possible timeout
	do_send = 1;
	DEBUG_PRINTF("NAK RECEIVED : seq=%u, retry=%d\n",
		     packet_seq, retries);
      }
      else if (data == XMODEM_CTRL_CAN) {
	rc = XMODEM_PEER_CANCEL; // Receiver canceled, we are done
	DEBUG_PRINTF("CAN RECEIVED\n");
	break;
      }
      else {
	rc = XMODEM_TIMEOUT;    // Wait again, possible timeout
	do_send = 0;            // Unexpected character received
	DEBUG_PRINTF("UNEXPECTED RECEIVED, data=%u\n", data);
      }
    }
    else if (rc == RS232_WOULD_BLOCK) {
      rc = XMODEM_TIMEOUT;      // Wait again, possible timeout
      do_send = 0;              // No character received
      DEBUG_PRINTF("NOTHING RECEIVED : seq=%u, retry=%d\n",
		   packet_seq, retries);
    }
    else {
      rc = XMODEM_MEDIA_ERROR;  // Unexpected error, we are done
      break;
    }
    retries--;
  }

  return rc;
}

////////////////////////////////////////////////////////////////

static int xmodem_do_send(int use_crc,
			  uint8_t packet_seq,
			  XMODEM_PACKET *xpack)
{
  int rc;
  uint8_t bytes_to_send;
  uint8_t sum;
  uint16_t crc;
  unsigned i;

  if (use_crc) {
    bytes_to_send = XMODEM_PACKET_DATA_BYTES + 5;
  }
  else {
    bytes_to_send = XMODEM_PACKET_DATA_BYTES + 4;
  }

  // Create packet metadata
  xpack->header = XMODEM_CTRL_SOH;
  xpack->seq = packet_seq;
  xpack->cmpl_seq = ((uint8_t)255-packet_seq);
  
  // Calculate packet checksum
  if (use_crc) {
    crc = 0;
    for (i=0; i < XMODEM_PACKET_DATA_BYTES; i++) {
      crc = xmodem_crc_update(crc, xpack->data[i]);
    }
    xpack->csum[0] = (crc & 0xff00) >> 8;
    xpack->csum[1] = crc & 0x00ff;
  }
  else {
    sum = 0;
    for (i=0; i < XMODEM_PACKET_DATA_BYTES; i++) {
      sum += xpack->data[i];
    }
    xpack->csum[0] = sum;
  }

  // Send entire packet
  rc = rs232_send_bytes((uint8_t *)xpack, bytes_to_send);
  if (rc == RS232_SUCCESS) {
    rc = XMODEM_SUCCESS;
  }
  else {
    rc = XMODEM_MEDIA_ERROR;
  }
  return rc;
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
