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
  DDRB &= ~(_RB4);                // Target reset pin 

  DDRB &= ~(_RB7 | _RB6 | _RB5);  // LED control pins
                                  // TRISB<7> = 0 (output)
                                  // TRISB<6> = 0 (output)
                                  // TRISB<5> = 0 (output)

  ACTIV_LED_OFF;                  // Turn off all LEDs
  HOST_ERR_LED_OFF;
  TARGET_ERR_LED_OFF;

  rs232_initialize();             // Initialize RS232
  spi_initialize(SPI_MODE_0);     // Initialize SPI, mode (0,0)

  ACTIV_LED_ON;                   // Signal alive
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
