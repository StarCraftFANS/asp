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

#include "at89s52_io.h"
#include "spi.h"
#include "delay.h"

// Implementation notes:
// 1. AT89S52 datasheet, Document: 1919D-MICRO-6/08
//    http://www.atmel.com/
//

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
// AT89S52 serial programming instructions
#define AT89S52_PROGRAMMING_ENABLE  0xac53
#define AT89S52_CHIP_ERASE          0xac80
#define AT89S52_READ_PM_BYTES       0x2000
#define AT89S52_WRITE_PM_BYTES      0x4000
#define AT89S52_WRITE_LOCK_BITS     0xace0
#define AT89S52_READ_LOCK_BITS      0x2400
#define AT89S52_READ_SIGNATURE      0x2800
#define AT89S52_READ_PM_PAGES       0x3000
#define AT89S52_WRITE_PM_PAGES      0x5000

// Expected response, programming enable accepted
#define AT89S52_PE_RESPONSE_OK  0x69

// Write cycle time is typical < 0.5ms
#define AT89S52_WRITE_CYCLE_TIME_MS  1    // ms, including safety factor x2

// Erase instruction cycle time is 500ms
#define AT89S52_ERASE_CYCLE_TIME_MS  750  // ms, including safety factor x1.5

// Addresses, signature bytes
#define AT89S52_ADDR_SIGNATURE_1  0x0000  // 0x1e
#define AT89S52_ADDR_SIGNATURE_2  0x0100  // 0x52
#define AT89S52_ADDR_SIGNATURE_3  0x0200  // 0x06

/////////////////////////////////////////////////////////////////////////////
//               Function prototypes
/////////////////////////////////////////////////////////////////////////////
static uint8_t check_valid_access(uint16_t addr,
				  uint16_t bytes_to_access);

/////////////////////////////////////////////////////////////////////////////
//               Global variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Public functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

int at89s52_io_enable_programming(void)
{
  int rc;

  uint8_t tx_buf[4];
  uint8_t rx_buf[4];

  tx_buf[0] = (AT89S52_PROGRAMMING_ENABLE & 0xff00) >> 8;
  tx_buf[1] = AT89S52_PROGRAMMING_ENABLE & 0x00ff;
  tx_buf[2] = 0; // Don't care
  tx_buf[3] = 0; // Don't care

  // Transfer is done using 8-bit segments (x4)
  rc = spi_xfer(tx_buf,
		rx_buf,
		4);
  if (rc != SPI_SUCCESS) {
    return AT89S52_SPI_ERROR;
  }

  // Response is now in receive buffer
  if (rx_buf[3] != AT89S52_PE_RESPONSE_OK) {
    return AT89S52_SPI_ERROR;
  }

  return AT89S52_SUCCESS;
}

////////////////////////////////////////////////////////////////

int at89s52_io_read_signature(uint32_t *value)
{
  int rc;

  uint8_t tx_buf[4];
  uint8_t rx_buf[4];

  *value = 0;  // Shall return 0x001e5206

  ///////////////////////////////
  // Read signature byte 1 / 3
  ///////////////////////////////
  tx_buf[0] = (AT89S52_READ_SIGNATURE & 0xff00) >> 8;
  tx_buf[1] = (AT89S52_ADDR_SIGNATURE_1 & 0xff00) >> 8;
  tx_buf[2] = 0;
  tx_buf[3] = 0; // Don't care

  // Transfer is done using 8-bit segments (x4)
  rc = spi_xfer(tx_buf,
		rx_buf,
		4);
  if (rc != SPI_SUCCESS) {
    return AT89S52_SPI_ERROR;
  }

  // Response is now in receive buffer
  *value = ( (uint32_t)(rx_buf[3]) << 16 );

  ///////////////////////////////
  // Read signature byte 2 / 3
  ///////////////////////////////
  tx_buf[1] = (AT89S52_ADDR_SIGNATURE_2 & 0xff00) >> 8;

  // Transfer is done using 8-bit segments (x4)
  rc = spi_xfer(tx_buf,
		rx_buf,
		4);
  if (rc != SPI_SUCCESS) {
    return AT89S52_SPI_ERROR;
  }

  // Response is now in receive buffer
  *value |= ( (uint32_t)(rx_buf[3]) << 8 );

  ///////////////////////////////
  // Read signature byte 3 / 3
  ///////////////////////////////
  tx_buf[1] = (AT89S52_ADDR_SIGNATURE_3 & 0xff00) >> 8;

  // Transfer is done using 8-bit segments (x4)
  rc = spi_xfer(tx_buf,
		rx_buf,
		4);
  if (rc != SPI_SUCCESS) {
    return AT89S52_SPI_ERROR;
  }

  // Response is now in receive buffer
  *value |= ( (uint32_t)(rx_buf[3]) );

  return AT89S52_SUCCESS;
}

////////////////////////////////////////////////////////////////

int at89s52_io_chip_erase(void)
{
  int rc;

  uint8_t tx_buf[4];
  uint8_t rx_buf[4];

  tx_buf[0] = (AT89S52_CHIP_ERASE & 0xff00) >> 8;
  tx_buf[1] = AT89S52_CHIP_ERASE & 0x00ff;
  tx_buf[2] = 0; // Don't care
  tx_buf[3] = 0; // Don't care

  // Transfer is done using 8-bit segments (x4)
  rc = spi_xfer(tx_buf,
		rx_buf,
		4);
  if (rc != SPI_SUCCESS) {
    return AT89S52_SPI_ERROR;
  }

  // Wait for erase to complete
  delay_ms(AT89S52_ERASE_CYCLE_TIME_MS);

  return AT89S52_SUCCESS;
}

////////////////////////////////////////////////////////////////

int at89s52_io_read_flash(uint16_t addr,
			  uint8_t *data,
			  uint16_t nbytes)
{
  int rc;
  uint16_t i;

  uint8_t tx_buf[4];
  uint8_t rx_buf[4]; 

  // Check that requested is valid
  if (!check_valid_access(addr, nbytes)) {
    return AT89S52_ADDR_ERROR;
  }

  // Read program memory (byte mode)
  tx_buf[0] = (AT89S52_READ_PM_BYTES & 0xff00) >> 8;
  tx_buf[3] = 0; // Don't care

  for (i=0; i < nbytes; i++) {

    tx_buf[1] = ( (addr+i) & 0xff00 ) >> 8;
    tx_buf[2] = ( (addr+i) & 0x00ff );

    // Transfer is done using 8-bit segments (x4)
    rc = spi_xfer(tx_buf,
		  rx_buf,
		  4);
    if (rc != SPI_SUCCESS) {
      return AT89S52_SPI_ERROR;
    }

    // Response is now in receive buffer
    data[i] = rx_buf[3];
  }

  return AT89S52_SUCCESS;
}

////////////////////////////////////////////////////////////////

int at89s52_io_write_flash(uint16_t addr,
			   const uint8_t *data,
			   uint16_t nbytes)
{
  int rc;
  uint16_t i;

  uint8_t tx_buf[4];
  uint8_t rx_buf[4]; 

  // Check that requested is valid
  if (!check_valid_access(addr, nbytes)) {
    return AT89S52_ADDR_ERROR;
  }

  // Write program memory (byte mode)
  tx_buf[0] = (AT89S52_WRITE_PM_BYTES & 0xff00) >> 8;

  for (i=0; i < nbytes; i++) {

    tx_buf[1] = ( (addr+i) & 0xff00 ) >> 8;
    tx_buf[2] = ( (addr+i) & 0x00ff );
    tx_buf[3] = data[i];

    // Transfer is done using 8-bit segments (x4)
    rc = spi_xfer(tx_buf,
		  rx_buf,
		  4);
    if (rc != AT89S52_SUCCESS) {
      return rc;
    }

    // Wait for write to complete
    // JOE: This could be improved by using data polling
    delay_ms(AT89S52_WRITE_CYCLE_TIME_MS);
  }

  return AT89S52_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//               Private functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

static uint8_t check_valid_access(uint16_t addr,
				  uint16_t bytes_to_access)
{
  // Check that chip is large enough
  if (bytes_to_access > AT89S52_FLASH_MEMORY_SIZE) {
    return 0;
  }

  // Check that address is valid for specified access
  if ( addr > (AT89S52_FLASH_MEMORY_SIZE-bytes_to_access) ) {
    return 0;
  }

  return 1;
}

