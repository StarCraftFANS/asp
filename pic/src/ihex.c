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

#include "ihex.h"
#include "utility.h"

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Definition of types
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Function prototypes
/////////////////////////////////////////////////////////////////////////////
static int ihex_checksum_check(const IHEX_RECORD *rec);

/////////////////////////////////////////////////////////////////////////////
//               Global variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Public functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

int ihex_initialize(IHEX_HANDLE handle)
{
  handle->current_state = IHEX_STATE_INIT;
  handle->bytes_received = 0;

  return IHEX_SUCCESS;
}

////////////////////////////////////////////////////////////////

int ihex_parse_record(IHEX_HANDLE handle,
		      uint8_t data)
{
  int rc = IHEX_SUCCESS;
  IHEX_STATE next_state = handle->current_state; 

  // Save data in string for later conversion to unsigned value
  if (handle->current_state != IHEX_STATE_DATA) { 
    handle->hex_str[handle->bytes_received++ % 4] = (char)data;
  }
  else {
    handle->hex_str[handle->bytes_received++ % 2] = (char)data;
  }

  switch (handle->current_state) {
    ///////////////////////////////////
  case IHEX_STATE_INIT:
    ///////////////////////////////////
    handle->bytes_received = 0;
    if (data == (uint8_t)':') {
      next_state = IHEX_STATE_NBYTES;
    }
    break;
    ///////////////////////////////////
  case IHEX_STATE_NBYTES:
    ///////////////////////////////////    
    if (handle->bytes_received == 2) {
      if (hex_str_to_uint8(handle->hex_str,
			   &handle->ihex_rec.nbytes) != UTILITY_SUCCESS) {
	next_state = IHEX_STATE_INIT;
	rc = IHEX_FORMAT_ERROR;
      }
      handle->bytes_received = 0;
      if (rc != IHEX_FORMAT_ERROR) {
	next_state = IHEX_STATE_ADDR;
      }
    }
    break;
    ///////////////////////////////////
  case IHEX_STATE_ADDR:
    ///////////////////////////////////
    if (handle->bytes_received == 4) {
      if (hex_str_to_uint16(handle->hex_str,
			    &handle->ihex_rec.addr) != UTILITY_SUCCESS) {
	next_state = IHEX_STATE_INIT;
	rc = IHEX_FORMAT_ERROR;
      }
      handle->bytes_received = 0;
      if (rc != IHEX_FORMAT_ERROR) {
	next_state = IHEX_STATE_TYPE;
      }
    }
    break;
    ///////////////////////////////////
  case IHEX_STATE_TYPE:
    ///////////////////////////////////
    if (handle->bytes_received == 2) {
      if (hex_str_to_uint8(handle->hex_str,
			   &handle->ihex_rec.type) != UTILITY_SUCCESS) {
	next_state = IHEX_STATE_INIT;
	rc = IHEX_FORMAT_ERROR;
      }
      handle->bytes_received = 0;
      if (rc != IHEX_FORMAT_ERROR) {
	if (handle->ihex_rec.type == IHEX_REC_DATA) {
	  next_state = IHEX_STATE_DATA;
	}
	else if (handle->ihex_rec.type == IHEX_REC_EOF) {
	  next_state = IHEX_STATE_CHKSUM; // No data, go direct to checksum
	}
	else {
	  rc = IHEX_BAD_REC_TYPE;
	  next_state = IHEX_STATE_INIT;
	}
      }
    }
    break;
    ///////////////////////////////////
  case IHEX_STATE_DATA:
    ///////////////////////////////////
    if (handle->bytes_received % 2 == 0) {
      if (hex_str_to_uint8(handle->hex_str,
			   &handle->ihex_rec.data[handle->bytes_received/2-1]) != UTILITY_SUCCESS) {
	next_state = IHEX_STATE_INIT;
	rc = IHEX_FORMAT_ERROR;
      }
    }
    if (rc != IHEX_FORMAT_ERROR) {
      if (handle->bytes_received == handle->ihex_rec.nbytes*2) {
	handle->bytes_received = 0;
	next_state = IHEX_STATE_CHKSUM; // All data bytes handled
      }
    }
    break;
    ///////////////////////////////////
  case IHEX_STATE_CHKSUM:
    ///////////////////////////////////
    if (handle->bytes_received == 2) {
      if (hex_str_to_uint8(handle->hex_str,
			   &handle->ihex_rec.chksum) != UTILITY_SUCCESS) {
	rc = IHEX_FORMAT_ERROR;
      }
      handle->bytes_received = 0;
      next_state = IHEX_STATE_INIT;
      if (rc != IHEX_FORMAT_ERROR) {
	rc = ihex_checksum_check(&handle->ihex_rec);
	if (rc == IHEX_SUCCESS) {
	  rc = IHEX_RECORD_READY; // Signal record complete
	}	
      }
    }
    break;
  }

  handle->current_state = next_state;

  return rc;
}

////////////////////////////////////////////////////////////////

int ihex_get_record(IHEX_HANDLE handle,
		    IHEX_RECORD **ihex_rec)
{
  *ihex_rec = &handle->ihex_rec;
  return IHEX_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//               Private functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

static int ihex_checksum_check(const IHEX_RECORD *rec)
{
  uint8_t *data_p;
  uint8_t sum;
  uint8_t i;

  /* The checksum value is the two's complement of the least
   * significant byte (LSB) of the sum of all decoded byte
   * values in the record that precede the checksum.
   */

  // Calculate the sum of all decoded bye values
  sum = rec->nbytes;

  data_p = (uint8_t *)&rec->addr;
  sum += *data_p++;
  sum += *data_p;

  sum += rec->type;

  data_p = (uint8_t *)rec->data;
  for (i=0; i < rec->nbytes; i++) {
    sum += *data_p++;
  }

  // Two's complement
  sum = ~sum + 1;

  if (sum != rec->chksum) {
    return IHEX_CHKSUM_ERROR;
  }
  
  return IHEX_SUCCESS;
}
