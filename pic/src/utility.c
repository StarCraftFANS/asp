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

#include "utility.h"

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Function prototypes
/////////////////////////////////////////////////////////////////////////////
static void byte_to_hex_str(uint8_t data,
			    char *str);

/////////////////////////////////////////////////////////////////////////////
//               Global variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Public functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

void uint32_to_hex_str(uint32_t data,
		       char *str)
{
  int i;

  for (i=4; i > 0; i--) {
    byte_to_hex_str((uint8_t) (data >> ((i-1) * 8)),
		    &str[(4-i)*2]);
  }
}

////////////////////////////////////////////////////////////////

void uint16_to_hex_str(uint16_t data,
		       char *str)
{
  int i;

  for (i=2; i > 0; i--) {
    byte_to_hex_str((uint8_t) (data >> ((i-1) * 8)),
		    &str[(2-i)*2]);
  }
}

////////////////////////////////////////////////////////////////

void uint8_to_hex_str(uint8_t data,
		       char *str)
{
  byte_to_hex_str(data,
		  str);
}

/////////////////////////////////////////////////////////////////////////////
//               Private functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

static void byte_to_hex_str(uint8_t data,
			    char *str)
{
  const char hex_table[] = "0123456789abcdef";

  str[0] = hex_table[data >> 4];
  str[1] = hex_table[data & 0x0f];
}
