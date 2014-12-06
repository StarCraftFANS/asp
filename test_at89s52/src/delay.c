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

#include "delay.h"

// Implementation notes:
// 1. General
//    Assumes AT89S52@12MHz.
//    Using TIMER0 as 16-bit timer to implement delay (no interrupts).
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

void delay_initialize(void)
{
  TR0 = 0; // Turn off TIMER0
  TF0 = 0; // Clear TIMER0 overflow flag
}

////////////////////////////////////////////////////////////////

void delay_usleep(uint16_t usec)
{
  uint16_t timer_val;

  // Set TIMER0 as 16-bit timer (mode 1)
  TMOD = (TMOD & 0xf0) | 0x01;

  // Set intial timer value
  timer_val = 0xffff - usec;
  TH0 = timer_val >> 8;
  TL0 = timer_val & 0x0f;

  TR0 = 1;       // Turn on TIMER0
  while (!TF0) ; // Wait for TIMER0 overflow

  TR0 = 0; // Turn off TIMER0
  TF0 = 0; // Clear TIMER0 overflow flag
}
