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

#ifndef __AT89S52_IO_H__
#define __AT89S52_IO_H__

#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
// Return codes
#define AT89S52_SUCCESS      0
#define AT89S52_SPI_ERROR   -1
#define AT89S52_ADDR_ERROR  -2

// Chip definitions
#define AT89S52_SIGNATURE  0x001e5206

#define AT89S52_FLASH_MEMORY_SIZE  8192

#define AT89S52_RESET_ACTIVE_MIN_TIME_MS  1  // RESET must be high at least
                                              // 64 "system clocks" according to
                                              // to datasheet. Trial and error using
                                              // values in range 0.1-1ms was ok.

/////////////////////////////////////////////////////////////////////////////
//               Definition of exported functions
/////////////////////////////////////////////////////////////////////////////
extern int at89s52_io_enable_programming(void);

extern int at89s52_io_read_signature(uint32_t *value);

extern int at89s52_io_chip_erase(void);

extern int at89s52_io_read_flash(uint16_t addr,
				 uint8_t *data,
				 uint16_t nbytes);

extern int at89s52_io_write_flash(uint16_t addr,
				  const uint8_t *data,
				  uint16_t nbytes);

#endif // __AT89S52_IO_H__
