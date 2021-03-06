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
#include <stdint.h>

#include "delay.h"
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
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
#define TEST_LED_PIN  P2_0
#define TEST_BUT_PIN  P2_1

#define TEST_LED_ON   (TEST_LED_PIN = 1)
#define TEST_LED_OFF  (TEST_LED_PIN = 0)

#define DELAY_TIME_US  50000 // Microseconds, 0..65535

////////////////////////////////////////////////////////////////

void main(void)
{
  int led_cnt = 10;

  // Initialize hardware
  TEST_BUT_PIN = 1; // Input
  TEST_LED_ON;
  delay_initialize();
  rs232_initialize();
  TEST_LED_OFF;

  // Blink LED
  while (led_cnt--) {
    TEST_LED_ON;
    delay_usleep(DELAY_TIME_US);
    TEST_LED_OFF;
    delay_usleep(DELAY_TIME_US);
  }

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
    delay_usleep(DELAY_TIME_US);
    rs232_put_char('L');
    delay_usleep(DELAY_TIME_US);
    rs232_put_char('A');
    delay_usleep(DELAY_TIME_US);
    rs232_put_char('R');
    delay_usleep(DELAY_TIME_US);
    rs232_put_char('A');
    delay_usleep(DELAY_TIME_US);
    rs232_put_char('\n');
    delay_usleep(DELAY_TIME_US);

    rs232_putln_string("BROLIN");
    delay_usleep(DELAY_TIME_US);
  }
}
