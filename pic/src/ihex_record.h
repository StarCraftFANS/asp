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

#ifndef __IHEX_RECORD_H__
#define __IHEX_RECORD_H__

#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
#define IHEX_RECORD_MAX_BYTES  32

// Supported Intel HEX standard record types
#define IHEX_REC_DATA  0x00
#define IHEX_REC_EOF   0x01

/////////////////////////////////////////////////////////////////////////////
//               Definition of types
/////////////////////////////////////////////////////////////////////////////
// Intel HEX file record
typedef struct {
  uint8_t  nbytes;
  uint16_t addr;
  uint8_t  type;
  uint8_t  data[IHEX_RECORD_MAX_BYTES];
  uint8_t  chksum;
} IHEX_RECORD;

#endif // __IHEX_RECORD_H__
