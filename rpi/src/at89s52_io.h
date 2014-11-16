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

#ifndef __AT89S52_IO_H__
#define __AT89S52_IO_H__

#include <stdint.h>
#include <pthread.h>

#include <string>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//               Definition of macros
/////////////////////////////////////////////////////////////////////////////
// Return codes
#define AT89S52_SUCCESS      0
#define AT89S52_FILE_ERROR  -1
#define AT89S52_SPI_ERROR   -2
#define AT89S52_ADDR_ERROR  -3
#define AT89S52_TIME_ERROR  -4

// Chip definitions
#define AT89S52_SIGNATURE  0x001e5206

#define AT89S52_FLASH_MEMORY_SIZE  8192

#define AT89S52_RESET_ACTIVE_MIN_TIME  0.001  // RESET must be high at least
                                              // 64 "system clocks" according to
                                              // to datasheet. Trial and error using
                                              // values in range 0.1-1ms was ok.

/////////////////////////////////////////////////////////////////////////////
//               Class support types
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Definition of classes
/////////////////////////////////////////////////////////////////////////////

class at89s52_io {

 public:
  at89s52_io(string spi_dev, uint32_t speed);
  ~at89s52_io(void);

  long initialize(void);
  long finalize(void);

  long enable_programming(void);

  long read_signature(uint32_t &value);

  long chip_erase(void);

  long read_flash(uint16_t addr,
		  uint8_t *data,
		  uint16_t nbytes);

  long write_flash(uint16_t addr,
		   const uint8_t *data,
		   uint16_t nbytes);
 private:
  string          m_spi_dev;
  uint32_t        m_spi_speed;
  int             m_spi_fd;
  pthread_mutex_t m_xfer_mutex;

  void init_members(void);

  long spi_xfer(const void *tx_buf,
		void *rx_buf,
		uint32_t nbytes);

  bool check_valid_access(uint16_t addr,
			  uint16_t bytes_to_access);
};

#endif // __AT89S52_IO_H__
