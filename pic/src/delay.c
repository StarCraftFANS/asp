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

#include "platform.h"

#include "delay.h"

// Implementation notes:
// 1. PIC18F2510 datasheet, Document: DS39636D
//    http://www.microchip.com/
//
// 2. Assumes external clock, Fosc=20MHz.
//
// 3. Implementing delay using Timer0:
//    F = Fosc / (4 x Ps x [256 - TMR0] )
//
//    Fosc : Frequency external clock
//    F    : Frequency Timer0
//    Ps   : Timer0 prescaler value
//    TMR0 : Timer0 initial value
//

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Function prototypes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Global variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Public functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

void delay_100us(uint16_t n)
{
  T0CONbits.TMR0ON = 0;  // Stop Timer0
  INTCONbits.TMR0IF = 0; // Clear any interrupt

  // Each loop is 100us delay:
  // 250 x ( 1/Fosc x 4 x 2 )
  while (n) {
    TMR0 = 6;           // 256-6=250
    T0CON = 0b11000000; // TMR0ON = 1 (Start timer)
                        // T08BIT = 1 (8-bit timer)
                        // T0CS   = 0 (Internal instruction cycle)
                        // PSA    = 0 (Prescaler assigned)
                        // Prescaler = 1:2

    while (!INTCONbits.TMR0IF) { ; } // Wait for interrupt
    INTCONbits.TMR0IF = 0;           // Clear interrupt
    
    n--;
  }

}

////////////////////////////////////////////////////////////////

void delay_ms(uint16_t msec)
{
  T0CONbits.TMR0ON = 0;  // Stop Timer0
  INTCONbits.TMR0IF = 0; // Clear any interrupt

  // Each loop is 1ms delay:
  // 156 x ( 1/Fosc x 4 x 32 )
  while (msec) {
    TMR0 = 100;         // 256-100=156
    T0CON = 0b11000100; // TMR0ON = 1 (Start timer)
                        // T08BIT = 1 (8-bit timer)
                        // T0CS   = 0 (Internal instruction cycle)
                        // PSA    = 0 (Prescaler assigned)
                        // Prescaler = 1:32

    while (!INTCONbits.TMR0IF) { ; } // Wait for interrupt
    INTCONbits.TMR0IF = 0;           // Clear interrupt
    
    msec--;
  }
}

////////////////////////////////////////////////////////////////

void delay_s(uint16_t sec)
{
  uint8_t cnt;

  T0CONbits.TMR0ON = 0;  // Stop Timer0
  INTCONbits.TMR0IF = 0; // Clear any interrupt

  // Each loop is 1s delay:
  // 100 x 196 x ( 1/Fosc x 4 x 256 )
  while (sec) {
    cnt = 100;
    while (cnt) {
      TMR0 = 60;          // 256-60=196
      T0CON = 0b11000111; // TMR0ON = 1 (Start timer)
                          // T08BIT = 1 (8-bit timer)
                          // T0CS   = 0 (Internal instruction cycle)
                          // PSA    = 0 (Prescaler assigned)
                          // Prescaler = 1:256

      while (!INTCONbits.TMR0IF) { ; } // Wait for interrupt
      INTCONbits.TMR0IF = 0;           // Clear interrupt

      cnt--;
    }
    sec--;
  }
}

/////////////////////////////////////////////////////////////////////////////
//               Private functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
