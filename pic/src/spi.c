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

#include "spi.h"

// Implementation notes:
// 1. PIC18F2510 datasheet, Document: DS39636D
//    http://www.microchip.com/
//
// 2. Assumes external clock, Fosc=20MHz.
//
// 3. SPI clock set to Fosc/64 = 312.5KHz
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

int spi_initialize(SPI_MODE mode)
{
  uint8_t ckp = 0;
  uint8_t cke = 0;

  switch(mode) {
  case SPI_MODE_0:
    ckp = 0;
    cke = 1;
    break;
  case SPI_MODE_1:
    ckp = 0;
    cke = 0;
    break;
  case SPI_MODE_2:
    ckp = 1;
    cke = 1;
    break;
  case SPI_MODE_3:
    ckp = 1;
    cke = 0;
    break;
  }

  // Set pin direction
  DDRC &= ~(_RC5 | _RC3); // TRISC<5> = TRISC<3> = 0
  DDRC |= (_RC4);         // TRISC<4> = 1

  // SSPSTAT
  SSPSTATbits.SMP = 0;    // SMP = 0 => Input data sample middle 
  SSPSTATbits.CKE = cke;

  // SSPCON1
  SSPCON1 = 0b00100010;   // SSPEN = 1
                          // SSPM3-SSPM0 = 0010 => Fosc/64
  SSPCON1bits.CKP = ckp;

  return SPI_SUCCESS;
}

////////////////////////////////////////////////////////////////

int spi_xfer(const uint8_t *tx_buf,
	     uint8_t *rx_buf,
	     uint16_t nbytes)
{
  uint8_t data;
  uint16_t i;

  // Transfer all bytes
  for (i=0; i < nbytes; i++) {    

    if (tx_buf) {                 // Send data
      SSPBUF = tx_buf[i];
    }
    else {
      SSPBUF = 0;
    }
    
    while (!SSPSTATbits.BF) { ; } // Wait until receive complete
    data = SSPBUF;                // Consume received data
    if (rx_buf) {
      rx_buf[i] = data;
    }
  }

  return SPI_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//               Private functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
