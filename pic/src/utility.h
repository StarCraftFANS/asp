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

#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
// Return codes
#define UTILITY_SUCCESS   0
#define UTILITY_FAILURE  -1

/////////////////////////////////////////////////////////////////////////////
//               Definition of exported functions
/////////////////////////////////////////////////////////////////////////////
extern void uint32_to_hex_str(uint32_t data,
			      char *str);

extern void uint16_to_hex_str(uint16_t data,
			      char *str);

extern void uint8_to_hex_str(uint8_t data,
			     char *str);

#endif // __UTILITY_H__
