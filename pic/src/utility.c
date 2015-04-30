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

static int hex_str_to_uintx(const char *str,
			    uint32_t *data,
			    uint8_t nbytes); // nbytes : 1,2,4

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

////////////////////////////////////////////////////////////////

int hex_str_to_uint8(const char *str,
		     uint8_t *data)
{
  int rc;
  uint32_t tmp_data;

  rc = hex_str_to_uintx(str, &tmp_data, 1);
  if (rc != UTILITY_SUCCESS) {
    return rc;
  }
  *data = (uint8_t)tmp_data;

  return UTILITY_SUCCESS;
}

////////////////////////////////////////////////////////////////

int hex_str_to_uint16(const char *str,
		      uint16_t *data)
{
  int rc;
  uint32_t tmp_data;

  rc = hex_str_to_uintx(str, &tmp_data, 2);
  if (rc != UTILITY_SUCCESS) {
    return rc;
  }
  *data = (uint16_t)tmp_data;

  return UTILITY_SUCCESS;
}

////////////////////////////////////////////////////////////////

int hex_str_to_uint32(const char *str,
		      uint32_t *data)
{
  int rc;
  uint32_t tmp_data;

  rc = hex_str_to_uintx(str, &tmp_data, 4);
  if (rc != UTILITY_SUCCESS) {
    return rc;
  }
  *data = tmp_data;

  return UTILITY_SUCCESS;
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

////////////////////////////////////////////////////////////////

static int hex_str_to_uintx(const char *str,
			    uint32_t *data,
			    uint8_t nbytes) // nbytes : 1,2,4
{
  int i;
  uint8_t tmp;
  uint8_t exp;

  *data = 0;

  exp = 4 * (nbytes*2 - 1);
  for (i=0; i < (nbytes * 2); i++) {
    if ( !( (str[i] >= '0') && (str[i] <= '9') ) &&
         !( (str[i] >= 'A') && (str[i] <= 'F') ) &&
         !( (str[i] >= 'a') && (str[i] <= 'f') ) ) {
      return UTILITY_FAILURE;
    }
    if (str[i] >= '0' && (str[i] <= '9')) {
      tmp = (uint8_t)str[i] - 0x30;      // 0-9
    }
    else if (str[i] >= 'A' && (str[i] <= 'F')) {
      tmp = (uint8_t)str[i] - 0x41 + 10; // A-F
    }
    else {
      tmp = (uint8_t)str[i] - 0x61 + 10; // a-f
    }
    *data += (uint32_t)((uint32_t)1 << exp) * (uint32_t)tmp;
    exp -= 4;
  }

  return UTILITY_SUCCESS;
}
