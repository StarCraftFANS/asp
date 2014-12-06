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

#include <mcs51/at89x52.h> //Alternative : #include <mcs51/8052.h>
#include <string.h>

#include "rs232.h"

// Implementation notes:
// 1. General
//    Assumes AT89S52@12MHz.
//
// 2. AT89S52 datasheet, Document: 1919D-MICRO-6/08
//    http://www.atmel.com/
//
// 3. Atmel 8051 Microcontrollers Hardware Manual, Document: 4316E–8051–01/07
//    http://www.atmel.com/
//

/////////////////////////////////////////////////////////////////////////////
//               Public functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

void rs232_initialize(void)
{
  SCON = 0x50;  // serial (mode 1) : 8 data bit, 1 start bit, 1 stop bit
  TMOD = 0x20;  // timer1 (mode 2) : 8-bit, auto reload
  TH1 = 0xF3;   // baud rate 2400
  TL1 = 0xF3;   // baud rate 2400
  PCON |= SMOD; // double baud rate => 4800
  TR1 = 1;      // enable timer
  TI = 1;       // enable transmitting
  RI = 0;       // waiting to receive
}

////////////////////////////////////////////////////////////////

char rs232_get_char(void)
{
  char c;

  while (!RI); // wait to receive
  c = SBUF;    // receive from serial
  RI = 0;
  return c;
}

////////////////////////////////////////////////////////////////

void rs232_put_char(char c)
{
  while (!TI); // wait end of last transmission
  TI = 0;
  SBUF = c;    // transmit to serial
}

////////////////////////////////////////////////////////////////

void rs232_put_string(const char *s)
{
  int i;

  for (i=0; i< strlen(s); i++) {
    rs232_put_char(s[i]);
  }
}

////////////////////////////////////////////////////////////////

void rs232_putln_string(const char *s)
{
  rs232_put_string(s);
  rs232_put_char(10); // Line feed
  rs232_put_char(13); // Carriage return
}
