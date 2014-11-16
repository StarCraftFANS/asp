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

#include <strings.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#include "at89s52_io.h"
#include "delay.h"

// Implementation notes:
// 1. General
//    https://www.kernel.org/doc/Documentation/spi/spidev
//    https://www.kernel.org/doc/Documentation/spi/spi-summary
//
// 2. Raspberry PI kernel modules:
//    a) Protocol driver   (spidev.ko)
//    b) Controller driver (spi-bcm2708.ko)
//    See .../drivers/spi/spidev.c
//        .../drivers/spi/spi-bcm2708.c
//
// 3. Protocol driver has a character device interface.
//    /dev/spidevB.C	  (B=Bus, C=Chip select)
//    /dev/spidev0.0      (Chip select #0)
//    /dev/spidev0.1      (Chip select #1)
//    
// 4. User space API: /usr/include/linux/spi/spidev.h
//
// 5. AT89S52 datasheet, Document: 1919D-MICRO-6/08
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
#define AT89S52_WRITE_CYCLE_TIME  0.001  // Seconds, including safety factor x2

// Erase instruction cycle time is 500ms
#define AT89S52_ERASE_CYCLE_TIME  0.750  // Seconds, including safety factor x1.5

// Addresses, signature bytes
#define AT89S52_ADDR_SIGNATURE_1  0x0000  // 0x1e
#define AT89S52_ADDR_SIGNATURE_2  0x0100  // 0x52
#define AT89S52_ADDR_SIGNATURE_3  0x0200  // 0x06

/////////////////////////////////////////////////////////////////////////////
//               Definition of constants
/////////////////////////////////////////////////////////////////////////////
// Internal exceptions
const int internal_spi_error = 1;

/////////////////////////////////////////////////////////////////////////////
//               Public member functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

at89s52_io::at89s52_io(string spi_dev, uint32_t speed)
{
  m_spi_dev = spi_dev;
  m_spi_speed = speed;

  pthread_mutex_init(&m_xfer_mutex, NULL); // Use default mutex attributes

  init_members();
}

////////////////////////////////////////////////////////////////

at89s52_io::~at89s52_io(void)
{
  pthread_mutex_destroy(&m_xfer_mutex);
}

////////////////////////////////////////////////////////////////

long at89s52_io::initialize(void)
{
  int rc;
  const uint8_t spi_mode = SPI_MODE_0; // SPI mode (0,0)
  const uint8_t spi_bpw  = 8;

  // Open SPI device
  rc = open(m_spi_dev.c_str(), O_RDWR);
  if (rc == -1) {
    return AT89S52_FILE_ERROR;
  }
  m_spi_fd = rc;

  try {
    // Set SPI mode
    if ( ioctl(m_spi_fd, SPI_IOC_WR_MODE, &spi_mode) < 0 ) {
      throw internal_spi_error;
    }
    
    // Set SPI word length (bits per word)
    if ( ioctl(m_spi_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bpw) < 0 ) {
      throw internal_spi_error;
    }
    
    // Set max speed (Hz)
    if ( ioctl(m_spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &m_spi_speed) < 0 ) {
      throw internal_spi_error;
    }
  }
  catch (...) {
    close(m_spi_fd);
    return AT89S52_SPI_ERROR;
  }

  return AT89S52_SUCCESS;
}

////////////////////////////////////////////////////////////////

long at89s52_io::finalize(void)
{
  // Close SPI device
  if ( close(m_spi_fd) == -1 ) {
    return AT89S52_FILE_ERROR;
  }

  init_members();

  return AT89S52_SUCCESS;
}

////////////////////////////////////////////////////////////////

long at89s52_io::enable_programming(void)
{
  long rc;

  uint8_t tx_buf[4];
  uint8_t rx_buf[4];

  tx_buf[0] = (AT89S52_PROGRAMMING_ENABLE & 0xff00) >> 8;
  tx_buf[1] = AT89S52_PROGRAMMING_ENABLE & 0x00ff;
  tx_buf[2] = 0; // Don't care
  tx_buf[3] = 0; // Don't care

  // Transfer is done using 8-bit segments (x4)
  rc = spi_xfer((const void *)tx_buf,
		(void *)rx_buf,
		4);
  if (rc != AT89S52_SUCCESS) {
    return rc;
  }

  // Response is now in receive buffer
  if (rx_buf[3] != AT89S52_PE_RESPONSE_OK) {
    return AT89S52_SPI_ERROR;
  }

  return AT89S52_SUCCESS;
}

////////////////////////////////////////////////////////////////

long at89s52_io::read_signature(uint32_t &value)
{
  long rc;

  uint8_t tx_buf[4];
  uint8_t rx_buf[4];

  value = 0;  // Shall return 0x001e5206

  ///////////////////////////////
  // Read signature byte 1 / 3
  ///////////////////////////////
  tx_buf[0] = (AT89S52_READ_SIGNATURE & 0xff00) >> 8;
  tx_buf[1] = (AT89S52_ADDR_SIGNATURE_1 & 0xff00) >> 8;
  tx_buf[2] = 0;
  tx_buf[3] = 0; // Don't care

  // Transfer is done using 8-bit segments (x4)
  rc = spi_xfer((const void *)tx_buf,
		(void *)rx_buf,
		4);
  if (rc != AT89S52_SUCCESS) {
    return rc;
  }

  // Response is now in receive buffer
  value = ( (uint32_t)(rx_buf[3]) << 16 );

  ///////////////////////////////
  // Read signature byte 2 / 3
  ///////////////////////////////
  tx_buf[1] = (AT89S52_ADDR_SIGNATURE_2 & 0xff00) >> 8;

  // Transfer is done using 8-bit segments (x4)
  rc = spi_xfer((const void *)tx_buf,
		(void *)rx_buf,
		4);
  if (rc != AT89S52_SUCCESS) {
    return rc;
  }

  // Response is now in receive buffer
  value |= ( (uint32_t)(rx_buf[3]) << 8 );

  ///////////////////////////////
  // Read signature byte 3 / 3
  ///////////////////////////////
  tx_buf[1] = (AT89S52_ADDR_SIGNATURE_3 & 0xff00) >> 8;

  // Transfer is done using 8-bit segments (x4)
  rc = spi_xfer((const void *)tx_buf,
		(void *)rx_buf,
		4);
  if (rc != AT89S52_SUCCESS) {
    return rc;
  }

  // Response is now in receive buffer
  value |= ( (uint32_t)(rx_buf[3]) );

  return AT89S52_SUCCESS;
}

////////////////////////////////////////////////////////////////

long at89s52_io::chip_erase(void)
{
  long rc;

  uint8_t tx_buf[4];
  uint8_t rx_buf[4];

  tx_buf[0] = (AT89S52_CHIP_ERASE & 0xff00) >> 8;
  tx_buf[1] = AT89S52_CHIP_ERASE & 0x00ff;
  tx_buf[2] = 0; // Don't care
  tx_buf[3] = 0; // Don't care

  // Transfer is done using 8-bit segments (x4)
  rc = spi_xfer((const void *)tx_buf,
		(void *)rx_buf,
		4);
  if (rc != AT89S52_SUCCESS) {
    return rc;
  }

  // Wait for erase to complete
  if (delay(AT89S52_ERASE_CYCLE_TIME) != DELAY_SUCCESS) {
    return AT89S52_TIME_ERROR;
  }    

  return AT89S52_SUCCESS;
}

////////////////////////////////////////////////////////////////

long at89s52_io::read_flash(uint16_t addr,
			    uint8_t *data,
			    uint16_t nbytes)
{
  long rc;

  uint8_t tx_buf[4];
  uint8_t rx_buf[4]; 

  // Check that requested is valid
  if (!check_valid_access(addr, nbytes)) {
    return AT89S52_ADDR_ERROR;
  }

  // Read program memory (byte mode)
  tx_buf[0] = (AT89S52_READ_PM_BYTES & 0xff00) >> 8;
  tx_buf[3] = 0; // Don't care

  for (unsigned i=0; i < nbytes; i++) {

    tx_buf[1] = ( (addr+i) & 0xff00 ) >> 8;
    tx_buf[2] = ( (addr+i) & 0x00ff );

    // Transfer is done using 8-bit segments (x4)
    rc = spi_xfer((const void *)tx_buf,
		  (void *)rx_buf,
		  4);
    if (rc != AT89S52_SUCCESS) {
      return rc;
    }

    // Response is now in receive buffer
    data[i] = rx_buf[3];
  }

  return AT89S52_SUCCESS;
}

////////////////////////////////////////////////////////////////

long at89s52_io::write_flash(uint16_t addr,
			     const uint8_t *data,
			     uint16_t nbytes)
{
  long rc;

  uint8_t tx_buf[4];
  uint8_t rx_buf[4]; 

  // Check that requested is valid
  if (!check_valid_access(addr, nbytes)) {
    return AT89S52_ADDR_ERROR;
  }

  // Read program memory (byte mode)
  tx_buf[0] = (AT89S52_WRITE_PM_BYTES & 0xff00) >> 8;

  for (unsigned i=0; i < nbytes; i++) {

    tx_buf[1] = ( (addr+i) & 0xff00 ) >> 8;
    tx_buf[2] = ( (addr+i) & 0x00ff );
    tx_buf[3] = data[i];

    // Transfer is done using 8-bit segments (x4)
    rc = spi_xfer((const void *)tx_buf,
		  (void *)rx_buf,
		  4);
    if (rc != AT89S52_SUCCESS) {
      return rc;
    }

    // Wait for write to complete
    // JOE: This could be improved by using data polling
    if (delay(AT89S52_WRITE_CYCLE_TIME) != DELAY_SUCCESS) {
      return AT89S52_TIME_ERROR;
    }
  }

  return AT89S52_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//               Private member functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

void at89s52_io::init_members(void)
{
  m_spi_fd = 0;
}

////////////////////////////////////////////////////////////////

long at89s52_io::spi_xfer(const void *tx_buf,
			  void *rx_buf,
			  uint32_t nbytes)
{
  struct spi_ioc_transfer spi_transfer;

  // Clear receive buffer
  bzero(rx_buf, nbytes);

  // Clear SPI transfer
  bzero((void *) &spi_transfer, sizeof(spi_transfer));

  // Change SPI settings specific for this transfer
  spi_transfer.tx_buf = (uint64_t) tx_buf;
  spi_transfer.rx_buf = (uint64_t) rx_buf;
  spi_transfer.len = nbytes;

  try {
    pthread_mutex_lock(&m_xfer_mutex); // Lockdown SPI operation
    
    // Do SPI transfer
    if ( ioctl(m_spi_fd, SPI_IOC_MESSAGE(1), &spi_transfer) < 0 ) {
      throw internal_spi_error;
    }

    pthread_mutex_unlock(&m_xfer_mutex); // Lockup SPI operation
  }
  catch (...) {
    pthread_mutex_unlock(&m_xfer_mutex);
    return AT89S52_SPI_ERROR;
  }

  return AT89S52_SUCCESS;
}

////////////////////////////////////////////////////////////////

bool at89s52_io::check_valid_access(uint16_t addr,
				    uint16_t bytes_to_access)
{
  // Check that chip is large enough
  if (bytes_to_access > AT89S52_FLASH_MEMORY_SIZE) {
    return false;
  }

  // Check that address is valid for specified access
  if ( addr > (AT89S52_FLASH_MEMORY_SIZE-bytes_to_access) ) {
    return false;
  }

  return true;
}
