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

#include "rs232.h"
#include "delay.h"

// Implementation notes:
// 1. PIC18F2510 datasheet, Document: DS39636D
//    http://www.microchip.com/
//
// 2. Assumes external clock, Fosc=20MHz.
//
// 3. Baudrate set to 9600bps, BRGH=1, BRG16=0:
//    Baudrate = Fosc / (16 x [SPBRG + 1] )
//
//    Fosc  : Frequency external clock
//    SPBRG : Baudrate Generator Register
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

int rs232_initialize(void)
{
  DDRC |= (_RC7 | _RC6);  // TRISC<7> = TRISC<6> = 1

  RCSTA = _SPEN | _CREN;  // Serial port enable, Receive enable
  TXSTA = _TXEN | _BRGH;  // Transmit enable, BRGH=1  

  BAUDCON = 0;            // BRG16=0

  SPBRGH = 0x00;
  SPBRG  = 129;           // 9600bps@20MHz, BRGH=1, BRG16=0

  return RS232_SUCCESS;
}

////////////////////////////////////////////////////////////////

int rs232_finalize(void)
{
  return RS232_SUCCESS;
}

////////////////////////////////////////////////////////////////

int rs232_recv(uint8_t *data)
{
  if (PIR1bits.RCIF) {     // Check if reception complete
    *data = RCREG;         // Read data (clear RCIF)
    return RS232_SUCCESS;
  }
  else {
    if (RCSTAbits.OERR) {    // Check if overrun error
        RCSTAbits.CREN = 0;  // Clear error
        RCSTAbits.CREN = 1;
    }
    return RS232_WOULD_BLOCK;
  }
}

////////////////////////////////////////////////////////////////

int rs232_recv_timeout(uint8_t *data,
		       unsigned timeout_s)
{
  int rc;
  uint32_t timeout_cnt;

  // Define timeout for receive operation
  timeout_cnt = (uint32_t)timeout_s * (uint32_t)10000; // 100us

  // Receive one character
  do {
    rc = rs232_recv(data);
    if ( timeout_s && (rc == RS232_WOULD_BLOCK) ) {
      delay_100us(1); // 100us
      timeout_cnt--;
    }
    else {
      break;
    }
  } while (timeout_cnt);

  return rc;
}

////////////////////////////////////////////////////////////////

int rs232_send(uint8_t data)
{
  while (!TXSTAbits.TRMT) { ; }  // Wait until TSR empty
  TXREG = data;                  // Send data

  return RS232_SUCCESS;
}

////////////////////////////////////////////////////////////////

int rs232_send_bytes(const uint8_t *data,
		     uint8_t nbytes)
{
  int rc;
  uint8_t i;

  for (i=0; i < nbytes; i++) {
    rc = rs232_send(data[i]);
    if (rc != RS232_SUCCESS) {
      return rc;
    }
  }
  return RS232_SUCCESS;
}

////////////////////////////////////////////////////////////////

int rs232_purge_receiver(void)
{
  volatile uint8_t data;
  int timeout_cnt;


  timeout_cnt = 10; // 1000ms = 1s

  while (timeout_cnt > 0) { 
    data = RCREG;  // Flush any data on rx line
    delay_ms(100); // 100ms
    timeout_cnt--;
  }

  if (RCSTAbits.OERR) {  // Check if overrun error
    RCSTAbits.CREN = 0;  // Clear error
    RCSTAbits.CREN = 1;
  }

  return RS232_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//               Private functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
