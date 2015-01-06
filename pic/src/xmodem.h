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

#ifndef __XMODEM_H__
#define __XMODEM_H__

#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
// Return codes
#define XMODEM_SUCCESS           0
#define XMODEM_MEDIA_ERROR      -1
#define XMODEM_TIMEOUT          -2
#define XMODEM_END_OF_TRANSFER  -3
#define XMODEM_PEER_CANCEL      -4
#define XMODEM_BAD_PACKET       -5
#define XMODEM_CALLBACK_ERROR   -6

// Xmodem control ASCII characters
#define XMODEM_CTRL_SOH  0x01  // Start Of Header
#define XMODEM_CTRL_EOT  0x04  // End Of Transmission 
#define XMODEM_CTRL_ACK  0x06  // Acknowledge (positive)
#define XMODEM_CTRL_NAK  0x15  // Negative Acknowledge
#define XMODEM_CTRL_CAN  0x18  // Cancel transfer
#define XMODEM_CTRL_CRC  0x43  // 'C' (dec 67), use Xmodem-CRC

#define XMODEM_PACKET_DATA_BYTES  128

// Flag values (OR one or more of the following values)
#define XMODEM_FLAG_CRC  0x01  // 16-bit CRC in place of the 8-bit checksum

/////////////////////////////////////////////////////////////////////////////
//               Definition of types
/////////////////////////////////////////////////////////////////////////////
typedef struct {
  uint8_t header;
  uint8_t seq;    
  uint8_t cmpl_seq;
  uint8_t data[XMODEM_PACKET_DATA_BYTES];
  uint8_t csum[2]; // Original Xmodem = [0] : checksum 
                   // 16-bit CRC      = [0] : hi order, [1] : low order
} XMODEM_PACKET;

// Calling convention assumed on callback function:
// Returns 0 on success, otherwise 1.
typedef int (*XMODEM_PACKET_CALLBACK_SEND)(XMODEM_PACKET *xpack);
typedef int (*XMODEM_PACKET_CALLBACK_RECV)(const XMODEM_PACKET *xpack);

/////////////////////////////////////////////////////////////////////////////
//               Definition of exported functions
/////////////////////////////////////////////////////////////////////////////
extern int xmodem_send_file(XMODEM_PACKET_CALLBACK_SEND packet_callback,
			    uint16_t packets,
			    int flags);

extern int xmodem_recv_file(XMODEM_PACKET_CALLBACK_RECV packet_callback,
			    int flags);

#endif // __XMODEM_H__
