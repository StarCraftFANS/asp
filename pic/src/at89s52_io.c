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

#include "at89s52_io.h"

// Implementation notes:
// 1. AT89S52 datasheet, Document: 1919D-MICRO-6/08
//    http://www.atmel.com/
//

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
// AT89S52 serial programming instructions
#define AT89S52_PROGRAMMING_ENABLE  0xac53
#define AT89S52_CHIP_ERASE          0xac80
#define AT89S52_READ_PM_BYTES       0x2000
#define AT89S52_WRITE_PM_BYTES      0x4000
#define AT89S52_WRITE_LOCK_BITS     0xace0
#define AT89S52_READ_LOCK_BITS      0x2400
#define AT89S52_READ_SIGNATURE      0x2800
#define AT89S52_READ_PM_PAGES       0x3000
#define AT89S52_WRITE_PM_PAGES      0x5000

// Expected response, programming enable accepted
#define AT89S52_PE_RESPONSE_OK  0x69

// Write cycle time is typical < 0.5ms
#define AT89S52_WRITE_CYCLE_TIME_MS  1    // ms, including safety factor x2

// Erase instruction cycle time is 500ms
#define AT89S52_ERASE_CYCLE_TIME_MS  750  // ms, including safety factor x1.5

// Addresses, signature bytes
#define AT89S52_ADDR_SIGNATURE_1  0x0000  // 0x1e
#define AT89S52_ADDR_SIGNATURE_2  0x0100  // 0x52
#define AT89S52_ADDR_SIGNATURE_3  0x0200  // 0x06

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

int at89s52_io_enable_programming(void)
{
  return AT89S52_SUCCESS;
}

////////////////////////////////////////////////////////////////

int at89s52_io_read_signature(uint32_t *value)
{
  *value = AT89S52_SIGNATURE;
  return AT89S52_SUCCESS;
}

////////////////////////////////////////////////////////////////

int at89s52_io_chip_erase(void)
{
  return AT89S52_SUCCESS;
}

////////////////////////////////////////////////////////////////

int at89s52_io_read_flash(uint16_t addr,
			  uint8_t *data,
			  uint16_t nbytes)
{
  return AT89S52_SUCCESS;
}

////////////////////////////////////////////////////////////////

int at89s52_io_write_flash(uint16_t addr,
			   const uint8_t *data,
			   uint16_t nbytes)
{
  return AT89S52_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//               Private functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

