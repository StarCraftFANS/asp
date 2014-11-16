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

//Alternative : #include <mcs51/8052.h>
#include <mcs51/at89x52.h>

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
#define TEST_LED_PIN  P2_0
#define TEST_BUT_PIN  P2_1

#define TEST_LED_ON   (TEST_LED_PIN = 1)
#define TEST_LED_OFF  (TEST_LED_PIN = 0)

/////////////////////////////////////////////////////////////////////////////
//               Function prototypes
/////////////////////////////////////////////////////////////////////////////
static void delay(int value);
static char rs232_get_char(void);
static void rs232_put_char(char c);
static void rs232_init(void);
static void rs232_init_joe(void);

////////////////////////////////////////////////////////////////

static void delay(int value)
{
  unsigned char k;
  while (value--) {
    for (k=0; k < 255; k++);
  }
}

////////////////////////////////////////////////////////////////

static char rs232_get_char(void)
{
  char c;

  while (!RI); // wait to receive
  c = SBUF;    // receive from serial
  RI = 0;
  return c;
}

////////////////////////////////////////////////////////////////

static void rs232_put_char(char c)
{
  while (!TI); // wait end of last transmission
  TI = 0;
  SBUF = c;    // transmit to serial
}

////////////////////////////////////////////////////////////////

static void rs232_init(void)
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

static void rs232_init_joe(void)
{
  SCON = 0x50;  // serial (mode 1) : 8 data bit, 1 start bit, 1 stop bit
  TMOD = 0x20;  // timer1 (mode 2) : 8-bit, auto reload
  TH1 = 0xFA;   // baud rate 4800
  TR1 = 1;      // enable timer
  TI = 1;       // enable transmitting
}

////////////////////////////////////////////////////////////////

void main(void)
{
  // Initialize
  TEST_BUT_PIN = 1; // Input
  TEST_LED_ON;
  rs232_init();
  TEST_LED_OFF;

  // Main loop
  while (1) {
    // Check button
    if (TEST_BUT_PIN == 1) {
      TEST_LED_ON;
    }
    else {
      TEST_LED_OFF;
    }

    // Print message
    rs232_put_char('K');
    delay(255);
    rs232_put_char('L');
    delay(255);
    rs232_put_char('A');
    delay(255);
    rs232_put_char('R');
    delay(255);
    rs232_put_char('A');
    delay(255);
    rs232_put_char('\n');
    delay(255);
  }
}
