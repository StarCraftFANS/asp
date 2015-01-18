// ************************************************************************
// *                                                                      *
// * Copyright (C) 2015 Bonden i Nol (hakanbrolin@hotmail.com)            *
// *                                                                      *
// * This program is free software; you can redistribute it and/or modify *
// * it under the terms of the GNU General Public License as published by *
// * the Free Software Foundation; either version 2 of the License, or    *
// * (at your option) any later version.                                  *
// *                                                                      *
// ************************************************************************

#ifndef __IHEX_PRIVATE_H__
#define __IHEX_PRIVATE_H__

#include <stdint.h>

#include "ihex_record.h"

/////////////////////////////////////////////////////////////////////////////
//               Definition of types
/////////////////////////////////////////////////////////////////////////////
// States for the internal state machine
typedef enum {IHEX_STATE_INIT,
	      IHEX_STATE_NBYTES,
	      IHEX_STATE_ADDR,
	      IHEX_STATE_TYPE,
	      IHEX_STATE_DATA,
	      IHEX_STATE_CHKSUM} IHEX_STATE;

// The opaque type
typedef struct {
  IHEX_RECORD ihex_rec;
  IHEX_STATE  current_state;
  uint8_t     bytes_received;
  char        hex_str[4];
} IHEX;

#endif // __IHEX_PRIVATE_H__
