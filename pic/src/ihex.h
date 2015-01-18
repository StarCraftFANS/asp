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

#ifndef __IHEX_H__
#define __IHEX_H__

#include <stdint.h>

#include "ihex_record.h"
#include "ihex_private.h"

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
// Return codes
#define IHEX_SUCCESS        0
#define IHEX_RECORD_READY  -1
#define IHEX_FORMAT_ERROR  -2
#define IHEX_BAD_REC_TYPE  -3
#define IHEX_CHKSUM_ERROR  -4

/////////////////////////////////////////////////////////////////////////////
//               Definition of types
/////////////////////////////////////////////////////////////////////////////
// The opaque pointer type
typedef IHEX *IHEX_HANDLE;

/////////////////////////////////////////////////////////////////////////////
//               Definition of exported functions
/////////////////////////////////////////////////////////////////////////////
int ihex_initialize(IHEX_HANDLE handle);

int ihex_parse_record(IHEX_HANDLE handle,
		      uint8_t data);

int ihex_get_record(IHEX_HANDLE handle,
		    IHEX_RECORD **rec);

#endif // __IHEX_H__
