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

#include "asp_hw.h"
#include "rs232.h"
#include "spi.h"
#include "delay.h"

// Implementation notes:
// 1. PIC18F2510 datasheet, Document: DS39636D
//    http://www.microchip.com/
//
// 2. Assumes external clock, Fosc=20MHz.
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

void asp_hw_initialize(void)
{
  // Special handling needed when using PORTA for digital I/O
  PORTA = 0;                      // Clear output data latches

  CMCON = 0x7;                    // Turn off comparators

  ADCON0bits.ADON = 0;            // Disable A/D converter
  ADCON1 |= 0b00001111;           // Configure pins for digital I/O
                                  // PCFG3:0 = 1111

  // Define direction for port pins
  DDRA |= (_RA5);                 // Button input pin
                                  // TRISA<5> = 1 (input)

  DDRA &= ~(_RA3 | _RA2 | _RA1);  // LED control pins
                                  // TRISA<3> = 0 (output)
                                  // TRISA<2> = 0 (output)
                                  // TRISA<1> = 0 (output)

  DDRA &= ~(_RA0);                // Debug LED control pin
                                  // TRISA<0> = 0 (output)

  DDRC &= ~(_RC2);                // Target reset pin
                                  // TRISC<2> = 0 (output)

  // Initialization sequence
  DBG_LED_OFF;                    // Turn off all LEDs
  ACTIV_LED_OFF;
  HOST_ERR_LED_OFF;
  TARGET_ERR_LED_OFF;

  rs232_initialize();             // Initialize RS232
  spi_initialize(SPI_MODE_0);     // Initialize SPI, mode (0,0)

  DBG_LED_ON;                     // Signal alive
  delay_ms(300);
  DBG_LED_OFF;
  ACTIV_LED_ON;
  delay_ms(300);
  ACTIV_LED_OFF;
  HOST_ERR_LED_ON;
  delay_ms(300);
  HOST_ERR_LED_OFF;
  TARGET_ERR_LED_ON;
  delay_ms(300);
  TARGET_ERR_LED_OFF;
}

/////////////////////////////////////////////////////////////////////////////
//               Private functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
