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
static uint8_t ihex_checksum_get(const IHEX_RECORD *rec);

static int ihex_checksum_check(const IHEX_RECORD *rec);

/////////////////////////////////////////////////////////////////////////////
//               Global variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Public functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

int ihex_get_file_size(uint8_t record_data_bytes,
		       uint16_t records,
		       uint32_t *nbytes)
{
  // Each line in an Intel HEX file contains one record according to
  // :llaaaatt[dd...]cc\n        ( \n = 0x0a, line feed )

  // Last line in an Intel HEX file contains one EOF record
  // :00000001FF\n               ( \n = 0x0a, line feed )

  *nbytes = (12 + record_data_bytes * 2) * records + 12;

  return IHEX_SUCCESS;
}

////////////////////////////////////////////////////////////////

int ihex_initialize(IHEX_HANDLE handle)
{
  handle->current_state = IHEX_STATE_INIT;
  handle->bytes_received = 0;

  handle->available_bytes = 0;
  handle->current_index = 0;

  return IHEX_SUCCESS;
}

////////////////////////////////////////////////////////////////

int ihex_ascii_to_bin_record(IHEX_HANDLE handle,
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

int ihex_get_bin_record(IHEX_HANDLE handle,
			IHEX_RECORD **ihex_rec)
{
  *ihex_rec = &handle->ihex_rec;
  return IHEX_SUCCESS;
}

////////////////////////////////////////////////////////////////

int ihex_bin_to_ascii_record(IHEX_HANDLE handle,
			     uint16_t addr,
			     uint8_t type,
			     const uint8_t *data,
			     uint8_t nbytes)
{
  uint8_t index = 0;
  uint8_t i, j;
  char ascii_str[4];

  // Create binary record ...
  handle->ihex_rec.nbytes = nbytes;
  handle->ihex_rec.addr   = addr;
  handle->ihex_rec.type   = type;
  for (i=0; i < nbytes; i++) {
    handle->ihex_rec.data[i] = data[i];
  }
  // ... and calculate checksum
  handle->ihex_rec.chksum = ihex_checksum_get(&handle->ihex_rec);

  // Create ASCII record
  handle->ihex_ascii_rec[index++] = ':';

  uint8_to_hex_str(handle->ihex_rec.nbytes, ascii_str);
  for (i=0; i < 2; i++) {
    handle->ihex_ascii_rec[index++] = ascii_str[i];
  }

  uint16_to_hex_str(handle->ihex_rec.addr, ascii_str);
  for (i=0; i < 4; i++) {
    handle->ihex_ascii_rec[index++] = ascii_str[i];
  }

  uint8_to_hex_str(handle->ihex_rec.type, ascii_str);
  for (i=0; i < 2; i++) {
    handle->ihex_ascii_rec[index++] = ascii_str[i];
  }

  for (i=0; i < handle->ihex_rec.nbytes; i++) {
    uint8_to_hex_str(handle->ihex_rec.data[i], ascii_str);
    for (j=0; j < 2; j++) {
      handle->ihex_ascii_rec[index++] = ascii_str[j];
    }
  }

  uint8_to_hex_str(handle->ihex_rec.chksum, ascii_str);
  for (i=0; i < 2; i++) {
    handle->ihex_ascii_rec[index++] = ascii_str[i];
  }

  handle->ihex_ascii_rec[index++] = 0x0a; // line feed

  // One new ASCII record is created and available
  handle->available_bytes = index;
  handle->current_index = 0;

  return IHEX_SUCCESS;
}

////////////////////////////////////////////////////////////////

int ihex_get_ascii_record(IHEX_HANDLE handle,
			  uint8_t requested_bytes,
			  uint8_t *actual_bytes,
			  uint8_t **data)
{
  if (handle->available_bytes) {
    // Limit amount of data that can be fetched
    if (requested_bytes > handle->available_bytes) {
      *actual_bytes = handle->available_bytes;
    }
    else {
      *actual_bytes = requested_bytes;
    }
    *data = &handle->ihex_ascii_rec[handle->current_index]; 

    // Update for next fetch
    handle->available_bytes -= *actual_bytes;
    handle->current_index += *actual_bytes;
  }
  else {
    *actual_bytes = 0;
    *data = 0;
  }

  return IHEX_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//               Private functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

static uint8_t ihex_checksum_get(const IHEX_RECORD *rec)
{
  uint8_t *data_p;
  uint8_t sum;
  uint8_t i;

  /* The checksum value is the two's complement of the least
   * significant byte (LSB) of the sum of all decoded byte
   * values in the record that precede the checksum.
   */

  // Calculate the sum of all decoded byte values
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

  return sum;
}

////////////////////////////////////////////////////////////////

static int ihex_checksum_check(const IHEX_RECORD *rec)
{
  uint8_t sum;

  sum = ihex_checksum_get(rec);

  if (sum != rec->chksum) {
    return IHEX_CHKSUM_ERROR;
  }
  
  return IHEX_SUCCESS;
}
