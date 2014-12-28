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

#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
// Return codes
#define SPI_SUCCESS       0
#define SPI_FAILURE      -1

/////////////////////////////////////////////////////////////////////////////
//               Definition of types
/////////////////////////////////////////////////////////////////////////////
typedef enum {SPI_MODE_0,
	      SPI_MODE_1,
	      SPI_MODE_2,
	      SPI_MODE_3} SPI_MODE;

/////////////////////////////////////////////////////////////////////////////
//               Definition of exported functions
/////////////////////////////////////////////////////////////////////////////
extern int spi_initialize(SPI_MODE mode);

extern int spi_xfer(const uint8_t *tx_buf,
		    uint8_t *rx_buf,
		    uint16_t nbytes);

#endif // __SPI_H__
